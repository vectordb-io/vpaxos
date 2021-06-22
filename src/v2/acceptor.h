#ifndef __VPAXOS_ACCEPTOR_H__
#define __VPAXOS_ACCEPTOR_H__

#include <random>
#include "ballot.h"
#include "status.h"
#include "vpaxos_rpc.pb.h"

namespace vpaxos {

class Acceptor {
  public:
    Acceptor();
    ~Acceptor() = default;
    Acceptor(const Acceptor&) = delete;
    Acceptor& operator=(const Acceptor&) = delete;
    Status Init();

    void OnPrepare(const vpaxos_rpc::Prepare &request, vpaxos_rpc::PrepareReply &reply);
    void OnAccept(const vpaxos_rpc::Accept &request, vpaxos_rpc::AcceptReply &reply);

  private:
    Status PromisedBallot(Ballot &ballot) const;
    Status PersistPromisedBallot(const Ballot &ballot);

    Status AcceptedBallot(Ballot &ballot) const;
    Status PersistAcceptedBallot(const Ballot &ballot);

    bool HasAcceptedValue() const;
    Status AcceptedValue(std::string &accepted_value) const;
    Status PersistAcceptedValue(const std::string &accepted_value);

  private:
    Ballot promised_ballot_;
    Ballot accepted_ballot_;
    std::string accepted_value_;


  private: // for debug
    jsonxx::json64 ToJson() const;
    std::string ToString() const;
    std::string ToStringPretty() const;
    jsonxx::json64 ToJsonTiny() const;
    std::string ToStringTiny() const;

    void TraceOnPrepare(const vpaxos_rpc::Prepare &request) const;
    void TraceOnPrepareMini(const vpaxos_rpc::Prepare &request) const;
    void TraceOnPrepareTiny(const vpaxos_rpc::Prepare &request) const;
    void TraceOnPrepareVerbose(const vpaxos_rpc::Prepare &request) const;

    void TracePrepareReply(const vpaxos_rpc::PrepareReply &reply, const std::string &address) const;
    void TracePrepareReplyMini(const vpaxos_rpc::PrepareReply &reply, const std::string &address) const;
    void TracePrepareReplyTiny(const vpaxos_rpc::PrepareReply &reply, const std::string &address) const;
    void TracePrepareReplyVerbose(const vpaxos_rpc::PrepareReply &reply, const std::string &address) const;

    void TraceOnAccept(const vpaxos_rpc::Accept &request) const;
    void TraceOnAcceptMini(const vpaxos_rpc::Accept &request) const;
    void TraceOnAcceptTiny(const vpaxos_rpc::Accept &request) const;
    void TraceOnAcceptVerbose(const vpaxos_rpc::Accept &request) const;

    void TraceAcceptReply(const vpaxos_rpc::AcceptReply &reply, const std::string &address) const;
    void TraceAcceptReplyMini(const vpaxos_rpc::AcceptReply &reply, const std::string &address) const;
    void TraceAcceptReplyTiny(const vpaxos_rpc::AcceptReply &reply, const std::string &address) const;
    void TraceAcceptReplyVerbose(const vpaxos_rpc::AcceptReply &reply, const std::string &address) const;
};

} // namespace vpaxos

#endif
