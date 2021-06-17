#ifndef __VPAXOS_ENV_H__
#define __VPAXOS_ENV_H__

#include <chrono>
#include "timer.h"
#include "common.h"
#include "status.h"
#include "storage.h"
#include "grpc_server.h"
#include "thread_pool.h"
#include "vpaxos_rpc.grpc.pb.h"

namespace vpaxos {

class Env {
  public:
    static Env&
    GetInstance() {
        static Env instance;
        return instance;
    }

    Status Init();
    Status Start();
    Status Stop();

    // network
    Status AsyncPing(const vpaxos_rpc::Ping &request, const std::string &address, PingFinishCallBack cb);
    Status AsyncPrepare(const vpaxos_rpc::Prepare &request, const std::string &address, PrepareFinishCallBack cb);
    Status AsyncAccept(const vpaxos_rpc::Accept &request, const std::string &address, AcceptFinishCallBack cb);
    Status AsyncLearn(const vpaxos_rpc::Learn &request, const std::string &address, LearnFinishCallBack cb);
    Status AsyncProposeReply(const vpaxos_rpc::ProposeReply &reply, void *call);

    // storage
    // for proposer
    Status MaxBallot(Ballot &ballot) const;
    Status PersistMaxBallot(const Ballot &ballot);

    // for acceptor
    Status PromisedBallot(Ballot &ballot) const;
    Status PersistPromisedBallot(const Ballot &ballot);
    bool HasPromisedBallot();

    Status AcceptedBallot(Ballot &ballot) const;
    Status PersistAcceptedBallot(const Ballot &ballot);
    bool HasAcceptedBallot();

    Status AcceptedValue(std::string &accepted_value) const;
    Status PersistAcceptedValue(const std::string &accepted_value);
    bool HasAcceptedValue() const;

    // for proposer, acceptor, learner
    Status ChosenValue(std::string &chosen_value) const;
    Status PersistChosenValue(const std::string &chosen_value);
    bool HasChosenValue() const;

    ThreadPool* thread_pool() {
        return &thread_pool_;
    }

    GrpcServer* grpc_server() {
        return &grpc_server_;
    }

    Timer* timer() {
        return &timer_;
    }

  private:
    Env();
    ~Env();
    Env(const Env&) = delete;
    Env& operator=(const Env&) = delete;

    Timer timer_;
    Storage storage_;
    ThreadPool thread_pool_;
    GrpcServer grpc_server_;
};

} // namespace vpaxos

#endif
