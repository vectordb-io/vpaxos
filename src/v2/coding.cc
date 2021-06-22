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
    j["value"] = pb.value();
    jj["Propose"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::Propose &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJsonMini(const vpaxos_rpc::Propose &pb) {
    jsonxx::json64 j, jj;
    j[0] = pb.value();
    jj["Propose"] = j;
    return jj;
}

std::string
ToStringMini(const vpaxos_rpc::Propose &pb) {
    return ToJsonMini(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::ProposeReply &pb) {
    jsonxx::json64 j, jj;
    j["code"] = pb.code();
    j["msg"] = pb.msg();
    j["chosen_value"] = pb.chosen_value();
    j["ballot"] = ToJson(pb.ballot());
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
    j[0] = pb.msg();
    j[1] = pb.chosen_value();
    j[2] = ToJsonTiny(pb.ballot());
    jj["ProposeReply"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::ProposeReply &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJsonMini(const vpaxos_rpc::ProposeReply &pb) {
    jsonxx::json64 j, jj;
    j[0] = pb.msg();
    jj["ProposeReply"] = j;
    return jj;
}

std::string
ToStringMini(const vpaxos_rpc::ProposeReply &pb) {
    return ToJsonMini(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::Ballot &pb) {
    jsonxx::json64 j, jj;
    j["proposal_id"] = pb.proposal_id();
    j["node_id"] = pb.node_id();
    jj["Ballot"] = j;
    return jj;
}

std::string
ToString(const vpaxos_rpc::Ballot &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::Ballot &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::Ballot &pb) {
    jsonxx::json64 j, jj;
    j["proposal"] = pb.proposal_id();
    NodeId n(pb.node_id());
    j["node"] = n.ToJsonTiny();
    jj["Ballot"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::Ballot &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJsonMini(const vpaxos_rpc::Ballot &pb) {
    jsonxx::json64 j;
    j[0] = pb.proposal_id();
    j[1] = pb.node_id();
    return j;
}

std::string
ToStringMini(const vpaxos_rpc::Ballot &pb) {
    return ToJsonMini(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::Prepare &pb) {
    jsonxx::json64 j, jj;
    j["ballot"] = ToJson(pb.ballot());
    j["address"] = pb.address();
    j["async_flag"] = pb.async_flag();
    jj["Prepare"] = j;
    return jj;
}

std::string
ToString(const vpaxos_rpc::Prepare &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::Prepare &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::Prepare &pb) {
    jsonxx::json64 j, jj;
    j["bal"] = ToJsonTiny(pb.ballot());
    jj["Prepare"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::Prepare &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJsonMini(const vpaxos_rpc::Prepare &pb) {
    jsonxx::json64 j, jj;
    j["bal"] = ToJsonMini(pb.ballot());
    jj["Prepare"] = j;
    return jj;
}

std::string
ToStringMini(const vpaxos_rpc::Prepare &pb) {
    return ToJsonMini(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::PrepareReply &pb) {
    jsonxx::json64 j, jj;
    j["promised_ballot"] = ToJson(pb.promised_ballot());
    j["accepted_ballot"] = ToJson(pb.accepted_ballot());
    j["accepted_value"] = pb.accepted_value();
    j["address"] = pb.address();
    j["async_flag"] = pb.async_flag();
    jj["PrepareReply"] = j;
    return jj;
}

std::string
ToString(const vpaxos_rpc::PrepareReply &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::PrepareReply &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::PrepareReply &pb) {
    jsonxx::json64 j, jj;
    j["promised_bal"] = ToJsonTiny(pb.promised_ballot());
    j["accepted_bal"] = ToJsonTiny(pb.accepted_ballot());
    j["accepted_value"] = pb.accepted_value();
    jj["PrepareReply"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::PrepareReply &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJsonMini(const vpaxos_rpc::PrepareReply &pb) {
    jsonxx::json64 j, jj;
    j["promised_bal"] = ToJsonMini(pb.promised_ballot());
    j["accepted_bal"] = ToJsonMini(pb.accepted_ballot());
    j["accepted_value"] = pb.accepted_value();
    jj["PrepareReply"] = j;
    return jj;
}

std::string
ToStringMini(const vpaxos_rpc::PrepareReply &pb) {
    return ToJsonMini(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::Accept &pb) {
    jsonxx::json64 j, jj;
    j["ballot"] = ToJson(pb.ballot());
    j["value"] = pb.value();
    j["address"] = pb.address();
    j["async_flag"] = pb.async_flag();
    jj["Accept"] = j;
    return jj;
}

std::string
ToString(const vpaxos_rpc::Accept &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::Accept &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::Accept &pb) {
    jsonxx::json64 j, jj;
    j["bal"] = ToJsonTiny(pb.ballot());
    j["value"] = pb.value();
    jj["Accept"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::Accept &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::AcceptReply &pb) {
    jsonxx::json64 j, jj;
    j["accepted"] = pb.accepted();
    j["accepted_ballot"] = ToJson(pb.accepted_ballot());
    j["accepted_value"] = pb.accepted_value();
    j["trace_ballot"] = ToJson(pb.trace_ballot());
    j["address"] = pb.address();
    j["async_flag"] = pb.async_flag();
    jj["AcceptReply"] = j;
    return jj;
}

jsonxx::json64
ToJsonMini(const vpaxos_rpc::Accept &pb) {
    jsonxx::json64 j, jj;
    j["bal"] = ToJsonMini(pb.ballot());
    j["value"] = pb.value();
    jj["Accept"] = j;
    return jj;
}

std::string
ToStringMini(const vpaxos_rpc::Accept &pb) {
    return ToJsonMini(pb).dump();
}

std::string
ToString(const vpaxos_rpc::AcceptReply &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::AcceptReply &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::AcceptReply &pb) {
    jsonxx::json64 j, jj;
    j["accepted"] = pb.accepted();
    j["accepted_bal"] = ToJsonTiny(pb.accepted_ballot());
    j["accepted_value"] = pb.accepted_value();
    j["trace_bal"] = ToJsonTiny(pb.trace_ballot());
    jj["AcceptReply"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::AcceptReply &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJsonMini(const vpaxos_rpc::AcceptReply &pb) {
    jsonxx::json64 j, jj;
    j["accepted_bal"] = ToJsonMini(pb.accepted_ballot());
    j["accepted_value"] = pb.accepted_value();
    jj["AcceptReply"] = j;
    return jj;
}

std::string
ToStringMini(const vpaxos_rpc::AcceptReply &pb) {
    return ToJsonMini(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::Learn &pb) {
    jsonxx::json64 j, jj;
    j["chosen_value"] = pb.chosen_value();
    j["address"] = pb.address();
    jj["Learn"] = j;
    return jj;
}

std::string
ToString(const vpaxos_rpc::Learn &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::Learn &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::Learn &pb) {
    jsonxx::json64 j, jj;
    j["chosen_value"] = pb.chosen_value();
    j["address"] = pb.address();
    jj["Learn"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::Learn &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJsonMini(const vpaxos_rpc::Learn &pb) {
    jsonxx::json64 j, jj;
    j["chosen_value"] = pb.chosen_value();
    jj["Learn"] = j;
    return jj;
}

std::string
ToStringMini(const vpaxos_rpc::Learn &pb) {
    return ToJsonMini(pb).dump();
}

jsonxx::json64
ToJson(const vpaxos_rpc::LearnReply &pb) {
    jsonxx::json64 j, jj;
    j["msg"] = pb.msg();
    j["address"] = pb.address();
    jj["LearnReply"] = j;
    return jj;
}

std::string
ToString(const vpaxos_rpc::LearnReply &pb) {
    return ToJson(pb).dump();
}

std::string
ToStringPretty(const vpaxos_rpc::LearnReply &pb) {
    return ToJson(pb).dump(4, ' ');
}

jsonxx::json64
ToJsonTiny(const vpaxos_rpc::LearnReply &pb) {
    jsonxx::json64 j, jj;
    j["msg"] = pb.msg();
    j["address"] = pb.address();
    jj["LearnReply"] = j;
    return jj;
}

std::string
ToStringTiny(const vpaxos_rpc::LearnReply &pb) {
    return ToJsonTiny(pb).dump();
}

jsonxx::json64
ToJsonMini(const vpaxos_rpc::LearnReply &pb) {
    jsonxx::json64 j, jj;
    j["msg"] = pb.msg();
    jj["LearnReply"] = j;
    return jj;
}

std::string
ToStringMini(const vpaxos_rpc::LearnReply &pb) {
    return ToJsonMini(pb).dump();
}

} // namespace vpaxos
