#define ADDR "0.0.0.0:50051"

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <signal.h>
#include <atomic>

#include <testjbj.grpc.pb.h>
#include <testjbj.pb.h>

#include "threadpool.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;

using T::jbj::jbjLogin_req;
using T::jbj::jbjLogin_res;
using T::jbj::jbjRegist_req;
using T::jbj::jbjReply_res;
using T::jbj::session;

using namespace std::chrono;

time_t GetCurrentMillisec()
{
    system_clock::time_point time_point_now =
        system_clock::now(); // 获取当前时间点
    system_clock::duration duration_since_epoch =
        time_point_now
            .time_since_epoch(); // 从1970-01-01 00:00:00到当前时间点的时长
    time_t microseconds_since_epoch =
        duration_cast<microseconds>(duration_since_epoch)
            .count(); // 将时长转换为微秒数

    time_t tm_millisec = microseconds_since_epoch / 1000; // 当前时间的毫秒数
    return tm_millisec;
}

class async_sn
{
private:
    /*
    用于异步 I/O 操作的完成队列。
    */
    // std::unique_ptr<ServerCompletionQueue> cq_;
    std::vector<std::shared_ptr<ServerCompletionQueue>> cqs_;
    // 线程池线程

    std::unique_ptr<thread_pool> pool;
    // epoll线程池
    std::unique_ptr<thread_pool> epool;
    // epoll线程数量和线程池线程数量
    size_t pool_thread_num_ = 2;
    size_t epoll_thread_num_ = 2;

    /*
     服务端的具体实现,定义了需要处理的 RPC 方法。
    */
    session::AsyncService service_;
    /*
    整个服务端的管理者,负责监听客户端连接,并将请求分发给相应的服务实现进行处理。
    */

public:
    std::unique_ptr<Server> server_;
    async_sn()
        : pool(std::make_unique<thread_pool>()), 
        epool(std::make_unique<thread_pool>())
    {
    }
    // 测试性能
    static time_t cur_time_;
    void Run(size_t epoll_thread_num, size_t pool_thread_num)
    {
        epoll_thread_num_ = epoll_thread_num;
        pool_thread_num_ = pool_thread_num;
        cur_time_ = GetCurrentMillisec();

        // 开启动态线程池
        pool->set_Mode(TPoolMode::MODE_CACHED);
        pool->set_Thread_Max(pool_thread_num); // 当任务数 < 线程数  线程关闭
        pool->start(pool_thread_num);

        epool->set_Mode(TPoolMode::MODE_CACHED);
        epool->set_Thread_Max(epoll_thread_num); // 当任务数 < 线程数  线程关闭
        epool->start(epoll_thread_num);

        std::string server_addr(ADDR);
        ServerBuilder build;
        // 设置同步关闭选项
        build.SetSyncServerOption(grpc::ServerBuilder::SyncServerOption::NUM_CQS, epoll_thread_num_);
      
        build.AddListeningPort(server_addr, grpc::InsecureServerCredentials());
        build.RegisterService(&service_);

        // cq_ = build.AddCompletionQueue();
        // 创建多个epoll
        for (size_t i = 0; i < epoll_thread_num_; i++)
        {
            std::shared_ptr<ServerCompletionQueue> cq =
                build.AddCompletionQueue();
            cqs_.push_back(cq);
        }
        server_ = build.BuildAndStart();

        std::cout << "server listen to" << server_addr << std::endl;
        // 使用线程池开启
        // loop();

        for (size_t i = 0; i < epoll_thread_num; i++)
        {
            // loop(cqs_[i]);
            std::function<void(std::shared_ptr<ServerCompletionQueue> &)> loop_func =
                std::bind(&async_sn::loop, this, std::placeholders::_1);
            epool->submitTask(loop_func, cqs_[i]);
        }
    }
    // 请求提供服务所需的状态和逻辑的类
    class CallData
    {
    public:
        session::AsyncService *service_;
        ServerCompletionQueue *cq_;
        ServerContext ctx_;
        enum CallStatus
        {
            CREATE,
            PROCESS,
            FINISH
        };
        CallStatus status_;
        std::atomic_bool is_finished_;
        CallData(session::AsyncService *service, ServerCompletionQueue *cq)
            : service_(service), cq_(cq), status_(CREATE)
        {
            //std::cout << "CallData constructing, this: " << this<< std::endl;
        }
        virtual ~CallData() {}
        virtual void Proceed() = 0;
    };

    class RegistCallData : public CallData
    {
        jbjRegist_req request_;
        jbjReply_res reply_;
        // 用于异步向客户端发送响应的写入器。
        ServerAsyncResponseWriter<jbjReply_res> responder_;

    public:
        RegistCallData(session::AsyncService *service, ServerCompletionQueue *cq)
            : CallData(service, cq), responder_(&this->ctx_)
        {
            Proceed();
        }

        void Proceed() override
        {
            //std::cout << "this: " << this << " RegistCallData Proceed(), status: " << status_ << std::endl;
            if (status_ == CREATE)
            {
                // std::cout << "this: " << this << " RegistCallData Proceed(), status: " << "CREATE" << std::endl;
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
            }
            else if (status_ == FINISH)
            {
                // std::cout << "this: " << this << " RegistCallData Proceed(), status: "<< "FINISH" << std::endl;
                GPR_ASSERT(status_ == FINISH);
                delete this;
            }
            else if (status_ == PROCESS)
            {
                // std::cout << "this: " << this << " RegistCallData Proceed(), status: "<< "PROCESS" << std::endl;
                // 新创一个 为了能够同时处理多个客户端的注册请求。
                new RegistCallData(service_, cq_);
                // 业务代码
                reply_.set_user_name(request_.user_name());
                reply_.set_password(request_.password());
                reply_.set_result_code(1);
                status_ = FINISH;
                // 异步地向客户端发送处理完成的响应
                responder_.Finish(reply_, Status::OK, this);
            }
        }
    };

    class LoginCalldata : public CallData
    {
        jbjLogin_req request_;
        jbjLogin_res reply_;
        ServerAsyncResponseWriter<jbjLogin_res> responder_;

    public:
        LoginCalldata(session::AsyncService *service, ServerCompletionQueue *cq)
            : CallData(service, cq), responder_(&this->ctx_)
        {
            Proceed();
        }

        void Proceed() override
        {
            //std::cout << "this: " << this << " LoginCallData Proceed(), status: " << status_ << std::endl; // darren 增加
            if (status_ == CREATE)
            {
                // std::cout << "this: " << this << " LoginCallData Proceed(), status: "<< "CREATE" << std::endl;
                status_ = PROCESS;

                service_->RequestLogin(&ctx_, &request_, &responder_, cq_, cq_, this);
            }
            else if (status_ == FINISH)
            {
                // std::cout << "this: " << this << " LoginCallData Proceed(), status: "<< "FINISH" << std::endl;
                GPR_ASSERT(status_ == FINISH);
                delete this;
            }
            else if (status_ == PROCESS)
            {
                // std::cout << "this: " << this << " LoginCallData Proceed(), status: "<< "PROCESS" << std::endl;
                new LoginCalldata(service_, cq_);
                reply_.set_user_id(10);
                reply_.set_result_code(0);
                status_ = FINISH;
                responder_.Finish(reply_, Status::OK, this);
            }
        }
    };

    void loop(std::shared_ptr<ServerCompletionQueue> &cq_)
    {
        // 创建2个循环服务 注册和请求
        new LoginCalldata(&service_, cq_.get());
        new RegistCallData(&service_, cq_.get());

        void *tag;
        bool ok;
        while (1) // 不断地从完成队列 cq_ 中读取事件并处理。
        {
            // std::cout << "before cq_->Next " << std::endl;
            //从完成队列中读取下一个事件,并将事件的标签(tag)和处理结果(ok)存储在传入的指针变量中。
            GPR_ASSERT(cq_->Next(&tag, &ok));
            // std::cout << "after cq_->Next " << std::endl;  // darren 增加
            GPR_ASSERT(ok);
            //std::cout << "before static_cast" << std::endl; // darren 增加
            // 从完成队列读取到的事件对应的 CallData 实例的 Proceed() 方法。
            // static_cast<CallData*>(tag)->Proceed();
            // 交由线程池来做  std::bind (函数，对象)；

            //auto call_data = static_cast<CallData *>(tag);
            pool->submitTask(std::bind(
                static_cast<void (CallData::*)()>(&CallData::Proceed),
                static_cast<CallData *>(tag)));
            //std::cout << "Next tid:" << std::this_thread::get_id() << ", t:"<< GetCurrentMillisec() - async_sn::cur_time_ << "ms\n";
        }
    }

    ~async_sn()
    {
        // 1. 关闭 Server 实例
        server_->Shutdown();

        // 2. 关闭 ServerCompletionQueue
        for (auto& cq : cqs_) {
            cq->Shutdown();
        }

        // 3. 等待服务器关闭完成
        server_->Wait();

        // 4. 释放资源
        server_.reset();

        pool.reset();
        epool.reset();
    }

    
};

time_t async_sn::cur_time_ = 0;
std::mutex mtx;
std::condition_variable cv;
// Ctrl+C 信号处理
void signalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "Received Ctrl+C signal. Stopping the program..." << std::endl;
        cv.notify_all();
    }
}
int main(int argc, char **argv)
{
    // 注册 Ctrl+C 信号处理函数
    signal(SIGINT, signalHandler);

    int epoll_thread_num = 2;
    int pool_thread_num = 2;
    if (argc == 3)
    {
        epoll_thread_num = atoi(argv[1]);
        pool_thread_num = atoi(argv[2]);
    }

    std::cout << "epoll_thread_num: " << epoll_thread_num << std::endl;
    std::cout << "pool_thread_num: " << pool_thread_num << std::endl;

    auto server = std::make_unique<async_sn>();
    server.get()->Run(epoll_thread_num, pool_thread_num);
    

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock);
    }
    
    // 手动释放 async_sn 对象
    server.reset();

    std::cout << "Main thread is exiting..." << std::endl;
    return 0;
}