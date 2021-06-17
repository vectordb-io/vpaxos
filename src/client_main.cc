#include <sys/syscall.h>
#include <cstdio>
#include <random>
#include <string>
#include <thread>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include "coding.h"
#include "vpaxos_rpc.grpc.pb.h"

#define gettid() (syscall(SYS_gettid))

std::default_random_engine random_(time(nullptr));

void
TracePropose(const std::string &address, const vpaxos_rpc::Propose &request, vpaxos_rpc::ProposeReply &reply) {
    std::string s;
    char buf[128];
    snprintf(buf, sizeof(buf), "tid:%ld ", gettid());
    s.append(std::string(buf));

    s.append("  st:");
    s.append(address);
    s.append(":");
    s.append(vpaxos::ToString(request));
    s.append("  rf:");
    s.append(address);
    s.append(":");
    s.append(vpaxos::ToString(reply));
    LOG(INFO) << s;
}

void
Propose(const std::string &address, const std::string &value) {
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

    TracePropose(address, request, reply);
}



int
main(int argc, char **argv) {
    FLAGS_alsologtostderr = true;
    google::InitGoogleLogging(argv[0]);

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
