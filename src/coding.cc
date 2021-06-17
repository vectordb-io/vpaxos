#include "coding.h"

namespace vpaxos {

void
Ballot2Pb(const Ballot &ballot, vpaxos_rpc::Ballot &pb) {
    pb.set_node_id(ballot.node_id());
    pb.set_proposal_id(ballot.proposal_id());
}

void
Pb2Ballot(const vpaxos_rpc::Ballot &pb, Ballot &ballot) {
    ballot.set_node_id(pb.node_id());
    ballot.set_proposal_id(pb.proposal_id());
}

void
Ballot2Str(const Ballot &ballot, std::string &s) {
    vpaxos_rpc::Ballot pb;
    pb.set_node_id(ballot.node_id());
    pb.set_proposal_id(ballot.proposal_id());
    bool ret = pb.SerializeToString(&s);
    assert(ret);
}

bool
Str2Ballot(const std::string &s, Ballot &ballot) {
    vpaxos_rpc::Ballot pb;
    bool ret = pb.ParseFromString(s);
    if (ret) {
        ballot.set_node_id(pb.node_id());
        ballot.set_proposal_id(pb.proposal_id());
    }
    return ret;
}

jsonxx::json64
ToJson(const vpaxos_rpc::Ping &pb) {
    jsonxx::json64 j, jj;
    j["msg"] = pb.msg();
    j["address"] = pb.address();
    jj["Ping"] = j;
    return jj;
}

std::string
ToString(const vpaxos_rpc::Ping &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::Ping &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::Ping &pb) {
    jsonxx::json64 j, jj;
    j[0] = pb.msg();
    j[1] = pb.address();
    jj["Ping"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::Ping &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::PingReply &pb) {
    jsonxx::json64 j, jj;
    j["msg"] = pb.msg();
    j["address"] = pb.address();
    jj["PingReply"] = j;
    return jj;
}

std::string
ToString(const vpaxos_rpc::PingReply &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::PingReply &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::PingReply &pb) {
    jsonxx::json64 j, jj;
    j[0] = pb.msg();
    j[1] = pb.address();
    jj["PingReply"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::PingReply &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::Propose &pb) {
    jsonxx::json64 j, jj;
    j["value"] = pb.value();
    jj["Propose"] = j;
    return jj;
}

std::string
ToString(const vpaxos_rpc::Propose &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::Propose &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::Propose &pb) {
    jsonxx::json64 j, jj;
    j[0] = pb.value();
    jj["Propose"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::Propose &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::ProposeReply &pb) {
    jsonxx::json64 j, jj;
    j["err_code"] = pb.err_code();
    j["err_msg"] = pb.err_msg();
    j["chosen_value"] = pb.chosen_value();
    jj["ProposeReply"] = j;
    return jj;
}

std::string
ToString(const vpaxos_rpc::ProposeReply &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::ProposeReply &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::ProposeReply &pb) {
    jsonxx::json64 j, jj;
    j[0] = pb.err_msg();
    j[1] = pb.chosen_value();
    jj["ProposeReply"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::ProposeReply &pb) {
    return ToJsonTiny(pb).dump();
}



} // namespace vpaxos
