#define ADDR "0.0.0.0:50051"

#include<grpcpp/grpcpp.h>
#include<grpc/support/log.h>

#include<iostream>
#include<string>
#include<thread>
#include<memory>

#include<testjbj.grpc.pb.h>
#include<testjbj.pb.h>

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

using T::jbj::session;
using T::jbj::jbjRegist_req;
using T::jbj::jbjReply_res;
using T::jbj::jbjLogin_req;
using T::jbj::jbjLogin_res;


class async_sn
{
private:
    /*
    用于异步 I/O 操作的完成队列。
    */
    std::unique_ptr<ServerCompletionQueue> cq_;
    /*
     服务端的具体实现,定义了需要处理的 RPC 方法。
    */
    session::AsyncService service_;
    /*
    整个服务端的管理者,负责监听客户端连接,并将请求分发给相应的服务实现进行处理。
    */
    std::unique_ptr<Server> server_;
public:
    void Run(){
        std::string server_addr(ADDR);
        ServerBuilder build;
        build.AddListeningPort(server_addr,grpc::InsecureServerCredentials());
        build.RegisterService(&service_);
        cq_ = build.AddCompletionQueue();
        server_ = build.BuildAndStart();

        std::cout <<"server listen to"<<server_addr<<std::endl;

        loop();
    }
     // 请求提供服务所需的状态和逻辑的类
    class CallData{
    public:
        session::AsyncService * service_;
        ServerCompletionQueue* cq_;
        ServerContext ctx_;
        enum CallStatus{CREATE,PROCESS,FINISH};
        CallStatus status_;

        CallData(session::AsyncService * service,ServerCompletionQueue* cq)
        :service_(service), cq_(cq), status_(CREATE)
        {
             std::cout << "CallData constructing, this: " << this
                    << std::endl; 
        }
        virtual ~CallData(){}
        virtual void Proceed() = 0;
    };

    class RegistCallData : public CallData{
        jbjRegist_req request_;
        jbjReply_res reply_;
        //用于异步向客户端发送响应的写入器。
        ServerAsyncResponseWriter<jbjReply_res> responder_;
    public:
        RegistCallData(session::AsyncService * service,ServerCompletionQueue* cq)
        :CallData(service,cq),responder_(&this->ctx_)
        {
            Proceed();
        }

        void Proceed()override {
            std::cout << "this: " << this
                    << " RegistCallData Proceed(), status: " << status_
                    << std::endl;
            if (status_ == CREATE)
            {
                std::cout << "this: " << this << " RegistCallData Proceed(), status: " << "CREATE" << std::endl;
                status_ = PROCESS;

                /*
                    向 gRPC 运行时发起了一个 RequestRegist 注册 RPC请求
                &ctx_ 传递了当前 RPC 的上下文信息。
                &request_ 传递了客户端发送的注册请求数据。
                &responder_ 传递了用于异步发送响应的对象。
                cq_ 传递了两次,分别作为生产者和消费者,用于接收完成事件。
                this 作为一个唯一标识符,用于标记这个特定的 RegistCallData 实例。
                */
                service_->RequestRegist(&ctx_, &request_, &responder_, cq_, cq_, this);

            }else if (status_ == FINISH)
            {
                std::cout << "this: " << this << " RegistCallData Proceed(), status: "
                            << "FINISH" << std::endl;
                GPR_ASSERT(status_ == FINISH);
                delete this;
            } else if (status_ == PROCESS)
            {
                std::cout << "this: " << this << " RegistCallData Proceed(), status: "
                            << "PROCESS" << std::endl;
                //新创一个 为了能够同时处理多个客户端的注册请求。
                new RegistCallData(service_,cq_);

                    //业务代码
                reply_.set_user_name(request_.user_name());
                reply_.set_password(request_.password());
                reply_.set_result_code(1);

                status_ = FINISH;
                //异步地向客户端发送处理完成的响应
                responder_.Finish(reply_,Status::OK,this);

            }
            
            
            
        }
    };

    class LoginCalldata : public CallData
    {
        jbjLogin_req request_;
        jbjLogin_res reply_;
        ServerAsyncResponseWriter<jbjLogin_res> responder_;
    public:
        LoginCalldata(session::AsyncService * service,ServerCompletionQueue* cq)
        :CallData(service,cq),responder_(&this->ctx_)
        {
            Proceed();
        }

        void Proceed() override{
             std::cout << "this: " << this
                    << " LoginCallData Proceed(), status: " << status_
                    << std::endl;   // darren 增加
            if(status_ == CREATE){
                  std::cout << "this: " << this << " LoginCallData Proceed(), status: "
                            << "CREATE" << std::endl;
                status_= PROCESS;

                 service_->RequestLogin(&ctx_, &request_, &responder_, cq_, cq_, this);
            }else if(status_ == FINISH){
                std::cout << "this: " << this << " LoginCallData Proceed(), status: "
                            << "FINISH" << std::endl;
                GPR_ASSERT(status_ == FINISH);
                delete this;
            }else if(status_ == PROCESS){
                std::cout << "this: " << this << " LoginCallData Proceed(), status: "
                            << "PROCESS" << std::endl;
                new LoginCalldata(service_,cq_);
                reply_.set_user_id(10);
                reply_.set_result_code(0);
                status_ = FINISH;
                responder_.Finish(reply_, Status::OK, this);
         
            }

        }
    };
    
    
    void loop(){
        //创建2个循环服务 注册和请求
        new LoginCalldata(&service_,cq_.get());
        new RegistCallData(&service_,cq_.get());

        void *tag;
        bool ok;
        while (1)   //不断地从完成队列 cq_ 中读取事件并处理。
        {
            std::cout << "before cq_->Next " << std::endl;
            GPR_ASSERT(cq_->Next(&tag,&ok));
            std::cout << "after cq_->Next " << std::endl;  // darren 增加
            GPR_ASSERT(ok);
            std::cout << "before static_cast" << std::endl;  // darren 增加
            //从完成队列读取到的事件对应的 CallData 实例的 Proceed() 方法。
            static_cast<CallData*>(tag)->Proceed();
            std::cout << "after static_cast" << std::endl;  // darren 增加
     
        }
        


    }
};  

int main(){

    async_sn server;
    server.Run();

    return 0;
}