#include "coding.h"

namespace vpaxos {

void Ballot2Pb(const Ballot &ballot, vpaxos_rpc::Ballot &pb) {
    pb.set_node_id(ballot.node_id());
    pb.set_proposal_id(ballot.proposal_id());
}

void Pb2Ballot(const vpaxos_rpc::Ballot &pb, Ballot &ballot) {
    ballot.set_node_id(pb.node_id());
    ballot.set_proposal_id(pb.proposal_id());
}

void EncodeBallot(std::string &buf, const Ballot &ballot) {
    vpaxos_rpc::Ballot pb;
    pb.set_node_id(ballot.node_id());
    pb.set_proposal_id(ballot.proposal_id());
    bool ret = pb.SerializeToString(&buf);
    assert(ret);
}

bool DecodeBallot(const std::string &buf, Ballot &ballot) {
    vpaxos_rpc::Ballot pb;
    bool ret = pb.ParseFromString(buf);
    if (ret) {
        ballot.set_node_id(pb.node_id());
        ballot.set_proposal_id(pb.proposal_id());
    }
    return ret;
}

}  // namespace vpaxos
