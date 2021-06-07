#ifndef __VPAXOS_PROPOSER_H__
#define __VPAXOS_PROPOSER_H__

#include <map>
#include <vector>
#include <string>
#include <random>
#include "vpaxos_rpc.pb.h"
#include "status.h"
#include "ballot.h"

namespace vpaxos {

class PrepareManager {
  public:
    PrepareManager(int quorum);
    ~PrepareManager();
    PrepareManager(const PrepareManager&) = delete;
    PrepareManager& operator=(const PrepareManager&) = delete;

    void Vote(const vpaxos_rpc::PrepareReply &reply);
    bool Majority() const;
    void Reset(Ballot promised_ballot);

    bool accept() const;
    void set_accept();
    bool HasAcceptedValue() const;
    std::string AcceptedValue() const;
    std::string ToString() const;

  private:
    std::map<std::string, vpaxos_rpc::PrepareReply> votes_;

    bool accept_;
    int quorum_;
    Ballot promised_ballot_;
    Ballot max_accepted_ballot_; // maybe null
    std::string accepted_value_; // max_accepted_ballot_ is not null
};


class AcceptManager {
  public:
    AcceptManager(int quorum);
    ~AcceptManager();
    AcceptManager(const AcceptManager&) = delete;
    AcceptManager& operator=(const AcceptManager&) = delete;

    void Vote(const vpaxos_rpc::AcceptReply &reply);
    bool Majority() const;
    void Reset(Ballot accepted_ballot);


    bool learn() const;
    void set_learn();
    std::string AcceptedValue() const;
    std::string ToString() const;

  private:
    std::map<std::string, vpaxos_rpc::AcceptReply> votes_;

    bool learn_;
    int quorum_;
    Ballot accepted_ballot_;
    std::string accepted_value_;
};


class Proposer {
  public:
    Proposer();
    ~Proposer() = default;
    Proposer(const Proposer&) = delete;
    Proposer& operator=(const Proposer&) = delete;

    Status Init();
    Status Propose(std::string value, void *flag);
    Status PrepareAll(void *flag);
    Status AcceptAll(void *flag);

    void OnPropose(const vpaxos_rpc::Propose &request, void *async_flag);

    Status Prepare(const vpaxos_rpc::Prepare &request, const std::string &address);
    Status OnPrepareReply(const vpaxos_rpc::PrepareReply &reply);
    Status Accept(const vpaxos_rpc::Accept &request, const std::string &address);
    Status OnAcceptReply(const vpaxos_rpc::AcceptReply &reply);

  private:
    Status MaxBallot(Ballot &ballot) const;
    Status StoreMaxBallot(const Ballot &ballot);
    void NextBallot();
    void NextBallot(Ballot b);
    void DebugLog(bool is_send, std::string header, std::string address, const google::protobuf::Message *m);

    Ballot current_ballot_;
    std::string propose_value_;
    bool proposing_;
    PrepareManager prepare_manager_;
    AcceptManager accept_manager_;
};

}  // namespace vpaxos

#endif
