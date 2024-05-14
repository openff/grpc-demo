#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>


#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include"testjbj.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

using T::jbj::session;
using T::jbj::jbjRegist_req;
using T::jbj::jbjReply_res;


class ServerImpl final
{
private:
    std::unique_ptr<ServerCompletionQueue> cq_;
    session::AsyncService service_;
    std::unique_ptr<Server> server_;
private:
    // 封装了服务请求所需的状态和逻辑的类
    class CallData
    {
    private:
        //与异步服务器的grpc运行通信方式
        session::AsyncService *service_;
        jbjRegist_req request_;
        jbjReply_res reply_;
        // 与客户端通信的方式
        ServerAsyncResponseWriter<jbjReply_res> responder_;
        //用于异步服务器 通知生产者-消费者队列
        ServerCompletionQueue *cq_;
        //rpc上下文
        ServerContext ctx_;

        enum CallStatus{CREATE ,PROCESS ,FINISH};
        CallStatus status_;
    public:
        CallData(session::AsyncService* service, ServerCompletionQueue* cq)
        :service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) 
        {
             Proceed();
        }
         void Proceed() {
            if (status_ == CREATE){
                //由创建 转变 运行
                status_ = PROCESS;
                service_->RequestRegist(&ctx_,&request_,&responder_,cq_,cq_,this);
            }else if (status_ == PROCESS){
                new CallData(service_,cq_);

                reply_.set_password(request_.password());
                reply_.set_user_name(request_.user_name());
                reply_.set_result_code(1);

                status_ = FINISH;
                responder_.Finish(reply_,Status::OK,this);
            }else{
                GPR_ASSERT(status_ == FINISH);
                delete this;
            }
            
        }
        ~CallData(){}
    };
    
public:
    ServerImpl(){}
    ~ServerImpl(){ }

    void Run(){

        std::string server_address("0.0.0.0:50051");
        ServerBuilder build;
        //监听
        build.AddListeningPort(server_address,grpc::InsecureServerCredentials());
        //注册
        build.RegisterService(&service_);
        //加入队列
        cq_ = build.AddCompletionQueue();
        //开启
        server_ = build.BuildAndStart();
        std::cout << "Server listening on " << server_address << std::endl;

        HandleRpcs();
    }
    void HandleRpcs() {
        // 生成一个新的 CallData 实例来为新客户端提供服务
        new CallData(&service_,cq_.get());
        void *tag;
        bool ok;
        while (true)
        {
            GPR_ASSERT(cq_->Next(&tag, &ok));
            GPR_ASSERT(ok);
            static_cast<CallData*>(tag)->Proceed();
        }
        
    }
};




int main(){
    ServerImpl server;
    server.Run();

    return 0;
}