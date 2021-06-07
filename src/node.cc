#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cassert>
#include "env.h"
#include "node.h"
#include "config.h"

namespace vpaxos {

std::default_random_engine random_(time(nullptr));

Status
Node::Init() {
    Status s;

    Env::GetInstance().grpc_server()->set_on_ping_cb(
        std::bind(&Node::OnPing, this, std::placeholders::_1, std::placeholders::_2)
    );

    s = proposer_.Init();
    assert(s.ok());

    s = acceptor_.Init();
    assert(s.ok());

    s = learner_.Init();
    assert(s.ok());

    return Status::OK();
}

uint64_t
Node::Id() {
    uint32_t ip = (uint32_t)inet_addr(Config::GetInstance().MyAddress()->host_.c_str());
    assert(ip != (uint32_t)-1);
    uint64_t node_id = (static_cast<uint64_t>(ip) << 32) | Config::GetInstance().MyAddress()->port_;
    return node_id;
}

void
Node::OnPing(const vpaxos_rpc::Ping &request, vpaxos_rpc::PingReply &reply) {
    LOG(INFO) << "receive from " << request.address() << ": " << request.msg();
    reply.set_address(Config::GetInstance().MyAddress()->ToString());
    if (request.msg() == "ping") {
        reply.set_msg("pang");
    } else {
        reply.set_msg("no sounds");
    }
    LOG(INFO) << "send to " << request.address() << ": " << reply.msg();
}

Status
Node::Ping(const vpaxos_rpc::Ping &request, const std::string &address) {
    LOG(INFO) << "send to " << address << ": " << request.msg();
    auto s = Env::GetInstance().AsyncPing(
                 request,
                 address,
                 std::bind(&Node::OnPingReply, this, std::placeholders::_1)
             );
    return s;
}

Status
Node::OnPingReply(const vpaxos_rpc::PingReply &reply) {
    LOG(INFO) << "receive from " << reply.address() << ": " << reply.msg();
    return Status::OK();
}

Status
Node::PingAll() {
    for (auto &hp : Config::GetInstance().address_) {
        // do not send message to myself
        if (hp->ToString() == Config::GetInstance().MyAddress()->ToString()) {
            continue;
        }

        vpaxos_rpc::Ping request;
        request.set_address(Config::GetInstance().MyAddress()->ToString());
        request.set_msg("ping");
        auto s = Ping(request, hp->ToString());
        assert(s.ok());
    }
    return Status::OK();
}

void
Node::Sleep(int min, int max) const {
    std::uniform_int_distribution<int> random_range(min, max);
    int timeout_ms = random_range(random_);
    LOG(INFO) << "sleep " << timeout_ms << " ms";
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms));
}

}  // namespace vpaxos
