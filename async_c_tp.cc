
#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "threadpool.h"
#include "testjbj.grpc.pb.h"

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

using T::jbj::jbjLogin_req;
using T::jbj::jbjLogin_res;
using T::jbj::session;

using namespace std::chrono;
time_t cur_time = 0;
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
class sessionClient

{
private:
    struct AsyncClinetCall
    {
        jbjLogin_res reply;
        ClientContext context;
        Status status;

        std::unique_ptr<ClientAsyncResponseReader<jbjLogin_res>> response_reader;

    public:
        AsyncClinetCall() = default;
        ~AsyncClinetCall() = default;
    };
    std::unique_ptr<session::Stub> stub_;
    CompletionQueue cq_;
    int req_num_ = 10000;
    int recv_num_ = 0;

public:
    sessionClient(std::shared_ptr<Channel> channel, int req_num)
        : stub_(session::NewStub(channel)), req_num_(req_num)
    {
    }
    int GetReqNum() { return req_num_; }

    std::string Regist(const std::string user, const std::string passwd)
    {
        jbjLogin_req request;
        request.set_user_name(user);
        request.set_password(passwd);

        // 调用对象来存储 rpc 数据
        AsyncClinetCall *call = new AsyncClinetCall;
        // 异步地发起 gRPC 客户端的 Regist 调用
        call->response_reader = (stub_->PrepareAsyncLogin(&call->context, request, &cq_));
        // 启动调用
        call->response_reader->StartCall();

        call->response_reader->Finish(&call->reply, &call->status, (void *)call);
        return "";
    }

    void AsyncCompltetRpc()
    {
        void *got_tag;
        bool ok = false;

        while (cq_.Next(&got_tag, &ok))
        {
            AsyncClinetCall *call = static_cast<AsyncClinetCall *>(got_tag);
            GPR_ASSERT(ok);
            recv_num_++;
            if ((call->status).ok())
            {
                // 获取服务端返回的数据
                if (recv_num_ % (req_num_ / 4) == 0)
                {
                    std::cout << "Greeter received: " << call->reply.user_id()
                              << " and encode " << call->reply.result_code()
                              << ", t: " << GetCurrentMillisec() - cur_time << "ms" << std::endl;
                }
            }
            else
            {
                std::cout << "RPC failed" << std::endl;
            }

            if (recv_num_ == req_num_)
            { // 接收数量和请求数量一致的时候打印测试结果
                time_t end_time = GetCurrentMillisec();
                std::cout << " need time: " << end_time - cur_time << "ms"
                          << ", tps: "
                          << req_num_ * 1000.0 / (end_time - cur_time)
                          << std::endl;
            }
            // Once we're complete, deallocate the call object.
            delete call;
        }
    }
    ~sessionClient() {}
};

int main(int argc, char **argv)
{

    int req_num = 10000;
    if (argc == 2)
    {
        req_num = atoi(argv[1]);
    }

    sessionClient greeter(grpc::CreateChannel(
                              "localhost:50051", grpc::InsecureChannelCredentials()),
                          req_num);
    // 生成无限循环的读取器线程
    std::thread thread_ =
        std::thread(&sessionClient::AsyncCompltetRpc, &greeter);

    cur_time = GetCurrentMillisec();
    for (int i = 0; i < greeter.GetReqNum(); i++)
    {
        std::string user("jbj " + std::to_string(i));
        std::string passwd("123 " + std::to_string(i));
        greeter.Regist(user, passwd);
    }

    std::cout << "等待回复，按control c退出， t: "
              << GetCurrentMillisec() - cur_time << "ms" << std::endl;
    thread_.join(); // blocks forever

    return 0;
}