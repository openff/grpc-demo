#include<iostream>
#include<memory>
#include<string>

/*
这个头文件提供了对 gRPC 服务器反射功能的支持。
服务器反射允许客户端在运行时发现服务器上可用的服务和方法。
通过包含这个头文件,你可以在服务器中启用反射功能,方便客户端与服务器交互。
*/
#include<grpcpp/ext/proto_server_reflection_plugin.h>

/*gRPC 核心功能的定义,如服务器、客户端、通道、上下文等。*/
#include<grpcpp/grpcpp.h>
/*这个头文件提供了对 gRPC 健康检查服务的支持。
健康检查服务允许客户端查询服务器的健康状态,确保服务器正常工作。
通过包含这个头文件,你可以在服务器中启用健康检查服务,方便客户端监控服务器的状态。*/
#include<grpcpp/health_check_service_interface.h>

#include"testjbj.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using T::jbj::session;
using T::jbj::jbjRegist_req;
using T::jbj::jbjReply_res;


class sessionServiceImpl final:public session::Service
{
    Status Regist(ServerContext* context ,
    const jbjRegist_req* req,jbjReply_res* res){

        std::cout<<req->user_name() <<req->password();

        res->set_user_name(req->user_name());
        res->set_password(req->password());
        res->set_result_code(1);
        return Status::OK;
    }
};
void runServer(){
    std::string addr("0.0.0.0:50051");
    sessionServiceImpl service;

    grpc::EnableDefaultHealthCheckService(true);

    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    builder.SetSyncServerOption(ServerBuilder::MIN_POLLERS,3);

    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout<<"server listen on "<<addr<<std::endl;

    server->Wait();
}

int main(){
    runServer();
    return 0;
}
