#ifndef __VPAXOS_NODE_H__
#define __VPAXOS_NODE_H__

#include <random>
#include <functional>
#include <glog/logging.h>
#include "status.h"
#include "common.h"
#include "proposer.h"
#include "acceptor.h"
#include "learner.h"
#include "nodeid.h"
#include "vpaxos_rpc.grpc.pb.h"

namespace vpaxos {

class Node {
  public:
    static Node&
    GetInstance() {
        static Node instance;
        return instance;
    }

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Status Init();
    //uint64_t Id();

    void OnPing(const vpaxos_rpc::Ping &request, vpaxos_rpc::PingReply &reply);
    Status Ping(const vpaxos_rpc::Ping &request, const std::string &address);
    Status OnPingReply(const vpaxos_rpc::PingReply &reply);
    Status PingAll();

    Proposer* proposer() {
        return &proposer_;
    }

    Acceptor* acceptor() {
        return &acceptor_;
    }

    Learner* learner() {
        return &learner_;
    }

    const NodeId &id() const {
        return id_;
    }

    void set_max_timeout_ms(int tm) {
        max_timeout_ms_ = tm;
    }

    void Sleep() const;

  private:
    Node();
    ~Node() = default;

    void TracePing(const vpaxos_rpc::Ping &request, const std::string &address) const;
    void TraceOnPing(const vpaxos_rpc::Ping &request, vpaxos_rpc::PingReply &reply) const;
    void TraceOnPingReply(const vpaxos_rpc::PingReply &reply) const;

    Proposer proposer_;
    Acceptor acceptor_;
    Learner learner_;

    NodeId id_;
    int max_timeout_ms_;
};


} // namespace vpaxos

#endif
