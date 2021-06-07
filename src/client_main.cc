#include <random>
#include <string>
#include <thread>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include "vpaxos_rpc.grpc.pb.h"


std::default_random_engine random_(time(nullptr));

void Propose(std::string address, std::string value) {
    std::uniform_int_distribution<int> random_range(100, 500);
    int timeout_ms = random_range(random_);
    std::cout << "sleep " << timeout_ms << " ms" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    std::unique_ptr<vpaxos_rpc::VPaxos::Stub> stub = vpaxos_rpc::VPaxos::NewStub(channel);

    vpaxos_rpc::Propose request;
    vpaxos_rpc::ProposeReply reply;

    request.set_value(std::string(value));
    grpc::ClientContext context;
    grpc::Status s = stub->RpcPropose(&context, request, &reply);

    char buf[256];
    snprintf(buf, sizeof(buf), "err_code:%d | err_msg:%s | chosen_value:%s", reply.err_code(), reply.err_msg().c_str(), reply.chosen_value().c_str());
    LOG(INFO) << "receive: " << std::string(buf);
}

int main(int argc, char **argv) {
    FLAGS_alsologtostderr = true;
    google::InitGoogleLogging(argv[0]);

    /*
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("127.0.0.1:38000", grpc::InsecureChannelCredentials());
    std::unique_ptr<vpaxos_rpc::VPaxos::Stub> stub = vpaxos_rpc::VPaxos::NewStub(channel);

    vpaxos_rpc::Ping request;
    vpaxos_rpc::PingReply reply;
    request.set_msg("ping");
    request.set_address("haha");

    grpc::ClientContext context;
    grpc::Status s = stub->RpcPing(&context, request, &reply);

    LOG(INFO) << "receive from " << reply.address() << ": " << reply.msg();
    */

    //std::thread t3(std::bind(Propose, "127.0.0.1:38002", "2222"));
    //t3.join();


    std::thread t1(std::bind(Propose, "127.0.0.1:38000", "0000"));
    std::thread t2(std::bind(Propose, "127.0.0.1:38001", "1111"));
    std::thread t3(std::bind(Propose, "127.0.0.1:38002", "2222"));
    std::thread t4(std::bind(Propose, "127.0.0.1:38003", "3333"));
    std::thread t5(std::bind(Propose, "127.0.0.1:38004", "4444"));

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    return 0;
}
