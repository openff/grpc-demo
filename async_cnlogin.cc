
#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include"testjbj.grpc.pb.h"

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

using T::jbj::session;
using T::jbj::jbjLogin_req;
using T::jbj::jbjLogin_res;


class sessionClient

{
private:
   std::unique_ptr<session::Stub> stub_;
public:
    sessionClient(std::shared_ptr<Channel> channel)
        :stub_(session::NewStub(channel))
    {}
    std::string Regist(const std::string user,const std::string passwd){
        jbjLogin_req request;
        request.set_user_name(user);
        request.set_password(passwd);

        jbjLogin_res reply;
        ClientContext context;
        CompletionQueue cq;
        Status status;
        //异步地发起 gRPC 客户端的 Regist 调用
        std::unique_ptr<ClientAsyncResponseReader<jbjLogin_res>> rpc(
            stub_->PrepareAsyncLogin(&context,request,&cq)
        );

        rpc->StartCall();

        rpc->Finish(&reply,&status,(void*)1);
        void *got_tag;
        bool ok = false;
         GPR_ASSERT(cq.Next(&got_tag, &ok));
        GPR_ASSERT(got_tag == (void*)1);
        GPR_ASSERT(ok);

         if (status.ok()) {
            // 获取服务端返回的数据
            std::cout <<"user_id:"<< reply.user_id()<<"  encode:"<<reply.result_code() <<std::endl;
            return "RPC 成功";
        } else {
            return "RPC failed";
        }
    }
    ~sessionClient(){}
};


int main(){
    sessionClient greeter(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  std::string reply = greeter.Regist("jbj","123456");  // The actual RPC call!
  std::cout << "Greeter received: " << reply << std::endl;

    return 0;
}