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
#include "vpaxos_rpc.grpc.pb.h"

namespace vpaxos {


class Node {
  public:
    static Node&
    GetInstance() {
        static Node instance;
        return instance;
    }

    Node() = default;
    ~Node() {}
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    Status Init();
    uint64_t Id();

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

    void Sleep(int min, int max) const;

  private:
    Proposer proposer_;
    Acceptor acceptor_;
    Learner learner_;
};


}  // namespace vpaxos

#endif
