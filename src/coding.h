#ifndef __VPAXOS_CODING_H__
#define __VPAXOS_CODING_H__

#include <string>
#include "jsonxx/json.hpp"
#include "vpaxos_rpc.pb.h"
#include "ballot.h"

namespace vpaxos {

void Ballot2Pb(const Ballot &ballot, vpaxos_rpc::Ballot &pb);
void Pb2Ballot(const vpaxos_rpc::Ballot &pb, Ballot &ballot);

void Ballot2Str(const Ballot &ballot, std::string &s);
bool Str2Ballot(const std::string &s, Ballot &ballot);

jsonxx::json64 ToJson(const vpaxos_rpc::Ping &pb);
std::string ToString(const vpaxos_rpc::Ping &pb);
std::string ToStringPretty(const vpaxos_rpc::Ping &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::Ping &pb);
std::string ToStringTiny(const vpaxos_rpc::Ping &pb);

jsonxx::json64 ToJson(const vpaxos_rpc::PingReply &pb);
std::string ToString(const vpaxos_rpc::PingReply &pb);
std::string ToStringPretty(const vpaxos_rpc::PingReply &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::PingReply &pb);
std::string ToStringTiny(const vpaxos_rpc::PingReply &pb);

jsonxx::json64 ToJson(const vpaxos_rpc::Propose &pb);
std::string ToString(const vpaxos_rpc::Propose &pb);
std::string ToStringPretty(const vpaxos_rpc::Propose &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::Propose &pb);
std::string ToStringTiny(const vpaxos_rpc::Propose &pb);

jsonxx::json64 ToJson(const vpaxos_rpc::ProposeReply &pb);
std::string ToString(const vpaxos_rpc::ProposeReply &pb);
std::string ToStringPretty(const vpaxos_rpc::ProposeReply &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::ProposeReply &pb);
std::string ToStringTiny(const vpaxos_rpc::ProposeReply &pb);

jsonxx::json64 ToJson(const vpaxos_rpc::Ballot &pb);
std::string ToString(const vpaxos_rpc::Ballot &pb);
std::string ToStringPretty(const vpaxos_rpc::Ballot &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::Ballot &pb);
std::string ToStringTiny(const vpaxos_rpc::Ballot &pb);

jsonxx::json64 ToJson(const vpaxos_rpc::Prepare &pb);
std::string ToString(const vpaxos_rpc::Prepare &pb);
std::string ToStringPretty(const vpaxos_rpc::Prepare &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::Prepare &pb);
std::string ToStringTiny(const vpaxos_rpc::Prepare &pb);

jsonxx::json64 ToJson(const vpaxos_rpc::PrepareReply &pb);
std::string ToString(const vpaxos_rpc::PrepareReply &pb);
std::string ToStringPretty(const vpaxos_rpc::PrepareReply &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::PrepareReply &pb);
std::string ToStringTiny(const vpaxos_rpc::PrepareReply &pb);

jsonxx::json64 ToJson(const vpaxos_rpc::Accept &pb);
std::string ToString(const vpaxos_rpc::Accept &pb);
std::string ToStringPretty(const vpaxos_rpc::Accept &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::Accept &pb);
std::string ToStringTiny(const vpaxos_rpc::Accept &pb);

jsonxx::json64 ToJson(const vpaxos_rpc::AcceptReply &pb);
std::string ToString(const vpaxos_rpc::AcceptReply &pb);
std::string ToStringPretty(const vpaxos_rpc::AcceptReply &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::AcceptReply &pb);
std::string ToStringTiny(const vpaxos_rpc::AcceptReply &pb);

jsonxx::json64 ToJson(const vpaxos_rpc::Learn &pb);
std::string ToString(const vpaxos_rpc::Learn &pb);
std::string ToStringPretty(const vpaxos_rpc::Learn &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::Learn &pb);
std::string ToStringTiny(const vpaxos_rpc::Learn &pb);

jsonxx::json64 ToJson(const vpaxos_rpc::LearnReply &pb);
std::string ToString(const vpaxos_rpc::LearnReply &pb);
std::string ToStringPretty(const vpaxos_rpc::LearnReply &pb);
jsonxx::json64 ToJsonTiny(const vpaxos_rpc::LearnReply &pb);
std::string ToStringTiny(const vpaxos_rpc::LearnReply &pb);

} // namespace vpaxos

#endif
