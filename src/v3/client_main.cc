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
std::string exe_name;

void Usage() {
    std::cout << std::endl;
    std::cout << "Usage: " << std::endl;
    std::cout << exe_name << " host start_port thread_num" << std::endl;
    std::cout << exe_name << " 127.0.0.1 38000 5" << std::endl;
    std::cout << std::endl;
}

void
TracePropose(const std::string &address, const vpaxos_rpc::Propose &request, vpaxos_rpc::ProposeReply &reply) {
    std::string s;
    char buf[128];
    snprintf(buf, sizeof(buf), "tid:%ld ", gettid());
    s.append(std::string(buf));

    s.append("  send_to-");
    s.append(address);
    s.append("-");
    s.append(vpaxos::ToStringTiny(request));
    s.append("  recv_from-");
    s.append(address);
    s.append("-");
    s.append(vpaxos::ToStringTiny(reply));
    printf("%s\n", s.c_str());
    fflush(nullptr);
}

void
Propose(const std::string &address, const std::string &value) {
    std::uniform_int_distribution<int> random_range(10, 50);
    int timeout_ms = random_range(random_);
    std::cout << "sleep " << timeout_ms << " ms" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));

    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    std::unique_ptr<vpaxos_rpc::VPaxos::Stub> stub = vpaxos_rpc::VPaxos::NewStub(channel);

    vpaxos_rpc::Propose request;
    vpaxos_rpc::ProposeReply reply;

    request.set_value(std::string(value));
    grpc::ClientContext context;

    std::string str = "send to " + address + " " + vpaxos::ToString(request);
    printf("%s\n", str.c_str());
    fflush(nullptr);
    grpc::Status s = stub->RpcPropose(&context, request, &reply);
    if (s.ok()) {
        TracePropose(address, request, reply);
    } else {
        std::cout << s.error_message();
    }
}

int
main(int argc, char **argv) {
    FLAGS_alsologtostderr = true;
    exe_name = std::string(argv[0]);
    google::InitGoogleLogging(argv[0]);

    if (argc != 4) {
        Usage();
        exit(-1);
    }

    std::string host;
    int start_port, thread_num;
    host = argv[1];
    sscanf(argv[2], "%d", &start_port);
    sscanf(argv[3], "%d", &thread_num);

    std::vector<std::thread*> threads;
    for (int i = 0; i < thread_num; ++i) {
        int port = start_port + i;
        char *addr_buf = new char [128];
        char *value_buf = new char [128];
        snprintf(addr_buf, 128, "%s:%d", host.c_str(), port);
        snprintf(value_buf, 128, "value_%d", i);
        std::thread *t = new std::thread(std::bind(Propose, addr_buf, value_buf));
        threads.push_back(t);
    }

    for (auto &t : threads) {
        t->join();
    }

    return 0;
}
