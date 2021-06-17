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


} // namespace vpaxos

#endif
