#ifndef __VPAXOS_PROPOSER_H__
#define __VPAXOS_PROPOSER_H__

#include <map>
#include <vector>
#include <string>
#include <random>
#include "vpaxos_rpc.pb.h"
#include "jsonxx/json.hpp"
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
    bool HasAcceptedValue() const;
    std::string AcceptedValue() const;

    jsonxx::json64 ToJson() const;
    std::string ToString() const;
    std::string ToStringPretty() const;
    jsonxx::json64 ToJsonTiny() const;
    std::string ToStringTiny() const;

    bool accept() const {
        return accept_;
    }

    void set_accept() {
        accept_ = true;
    }

  private:
    bool accept_;
    int quorum_;
    Ballot promised_ballot_;
    Ballot max_accepted_ballot_; // maybe null
    std::string accepted_value_; // max_accepted_ballot_ is not null
    std::map<std::string, vpaxos_rpc::PrepareReply> votes_;
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
    std::string AcceptedValue() const;

    jsonxx::json64 ToJson() const;
    std::string ToString() const;
    std::string ToStringPretty() const;
    jsonxx::json64 ToJsonTiny() const;
    std::string ToStringTiny() const;

    bool learn() const {
        return learn_;
    }
    void set_learn() {
        learn_ = true;
    }

    const Ballot& accepted_ballot() const {
        return accepted_ballot_;
    }

  private:
    bool learn_;
    int quorum_;
    Ballot accepted_ballot_;
    std::string accepted_value_;
    std::map<std::string, vpaxos_rpc::AcceptReply> votes_;
};

class Proposer {
  public:
    Proposer();
    ~Proposer() = default;
    Proposer(const Proposer&) = delete;
    Proposer& operator=(const Proposer&) = delete;
    Status Init();

    void OnPropose(const vpaxos_rpc::Propose &request, void *async_flag);
    Status Prepare(const vpaxos_rpc::Prepare &request, const std::string &address);
    Status OnPrepareReply(const vpaxos_rpc::PrepareReply &reply);
    Status Accept(const vpaxos_rpc::Accept &request, const std::string &address);
    Status OnAcceptReply(const vpaxos_rpc::AcceptReply &reply);

  private:
    Status MaxBallot(Ballot &ballot) const;
    Status PersistMaxBallot(const Ballot &ballot);

    void NextBallot();
    void NextBallot(Ballot b);
    Status Propose(std::string value, void *flag);
    Status PrepareAll(void *flag);
    Status AcceptAll(void *flag);

  private:
    Ballot current_ballot_;
    std::string propose_value_;
    bool proposing_;
    PrepareManager prepare_manager_;
    AcceptManager accept_manager_;

  private: // for debug
    jsonxx::json64 ToJson() const;
    std::string ToString() const;
    std::string ToStringPretty() const;
    jsonxx::json64 ToJsonTiny() const;
    std::string ToStringTiny() const;

    void TraceOnPropose(const vpaxos_rpc::Propose &request) const;
    void TraceOnProposeMini(const vpaxos_rpc::Propose &request) const;
    void TraceOnProposeTiny(const vpaxos_rpc::Propose &request) const;
    void TraceOnProposeVerbose(const vpaxos_rpc::Propose &request) const;

    void TracePrepare(const vpaxos_rpc::Prepare &request, const std::string &address) const;
    void TracePrepareMini(const vpaxos_rpc::Prepare &request, const std::string &address) const;
    void TracePrepareTiny(const vpaxos_rpc::Prepare &request, const std::string &address) const;
    void TracePrepareVerbose(const vpaxos_rpc::Prepare &request, const std::string &address) const;

    void TraceOnPrepareReply(const vpaxos_rpc::PrepareReply &reply) const;
    void TraceOnPrepareReplyMini(const vpaxos_rpc::PrepareReply &reply) const;
    void TraceOnPrepareReplyTiny(const vpaxos_rpc::PrepareReply &reply) const;
    void TraceOnPrepareReplyVerbose(const vpaxos_rpc::PrepareReply &reply) const;

    void TraceAccept(const vpaxos_rpc::Accept &request, const std::string &address) const;
    void TraceAcceptMini(const vpaxos_rpc::Accept &request, const std::string &address) const;
    void TraceAcceptTiny(const vpaxos_rpc::Accept &request, const std::string &address) const;
    void TraceAcceptVerbose(const vpaxos_rpc::Accept &request, const std::string &address) const;

    void TraceOnAcceptReply(const vpaxos_rpc::AcceptReply &reply) const;
    void TraceOnAcceptReplyMini(const vpaxos_rpc::AcceptReply &reply) const;
    void TraceOnAcceptReplyTiny(const vpaxos_rpc::AcceptReply &reply) const;
    void TraceOnAcceptReplyVerbose(const vpaxos_rpc::AcceptReply &reply) const;

    void TraceChosen(const std::string &value) const;
    void TraceChosenMini(const std::string &value) const;
    void TraceChosenTiny(const std::string &value) const;
    void TraceChosenVerbose(const std::string &value) const;
};

} // namespace vpaxos

#endif
