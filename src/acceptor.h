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

    Status PromisedBallot(Ballot &ballot) const;
    Status StorePromisedBallot(const Ballot &ballot);

    Status AcceptedBallot(Ballot &ballot) const;
    Status StoreAcceptedBallot(const Ballot &ballot);

    bool HasAcceptedValue() const;
    Status AcceptedValue(std::string &accepted_value) const;
    Status StoreAcceptedValue(const std::string &accepted_value);

    void DebugLog(bool is_send, std::string header, std::string address, const google::protobuf::Message *m);

  private:

};

}  // namespace vpaxos

#endif
