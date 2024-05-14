#include <memory>
#include <iostream>
#include <string>
#include<grpcpp/grpcpp.h>

#include"testjbj.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using T::jbj::session;
using T::jbj::jbjRegist_req;
using T::jbj::jbjReply_res;

class sessionClient{
    std::unique_ptr<session::Stub> stub_;
public:
    sessionClient(std::shared_ptr<Channel> channel)
    :stub_(session::NewStub(channel)){}
    
    std::string Sendto(const std::string user,const std::string password){
        jbjRegist_req request;
        request.set_user_name(user);
        request.set_password(password);

        jbjReply_res reply;

        ClientContext context;

        Status status = stub_->Regist(&context,request,&reply);

        if (status.ok())
        {
             // 获取服务端返回的数据
            std::cout <<"user:"<< reply.user_name()<<"  passwod:" << reply.password() <<"  encode:"<<reply.result_code() <<std::endl;
            return "RPC 成功";
        }else {
            std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
            return "RPC 失败";
        }
        
    }
    
};

int main(){
    std::string addr ("127.0.0.1:50051");
    
    sessionClient session(
        grpc::CreateChannel(addr,grpc::InsecureChannelCredentials())
    );
    std::string user("jbj");
    std::string password("123456");
    std::string res = session.Sendto(user,password);
    std::cout <<res<<std::endl;
    return 0;
}