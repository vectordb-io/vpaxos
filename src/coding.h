#ifndef __VPAXOS_CODING_H__
#define __VPAXOS_CODING_H__

#include <string>
#include "vpaxos_rpc.pb.h"
#include "ballot.h"

namespace vpaxos {

void Ballot2Pb(const Ballot &ballot, vpaxos_rpc::Ballot &pb);
void Pb2Ballot(const vpaxos_rpc::Ballot &pb, Ballot &ballot);

void EncodeBallot(std::string &buf, const Ballot &ballot);
bool DecodeBallot(const std::string &buf, Ballot &ballot);


}  // namespace vpaxos

#endif
