#ifndef __VPAXOS_COMMON_H__
#define __VPAXOS_COMMON_H__

#include <sys/syscall.h>
#include "status.h"
#include "vpaxos_rpc.grpc.pb.h"

namespace vpaxos {

#define gettid() (syscall(SYS_gettid))

using OnPingCallBack = std::function<void (const vpaxos_rpc::Ping &request, vpaxos_rpc::PingReply &reply)>;
using PingFinishCallBack = std::function<Status (vpaxos_rpc::PingReply)>;

using OnProposeCallBack = std::function<void (const vpaxos_rpc::Propose &request, void *async_flag)>;

using OnPrepareCallBack = std::function<void (const vpaxos_rpc::Prepare &request, vpaxos_rpc::PrepareReply &reply)>;
using PrepareFinishCallBack = std::function<Status (vpaxos_rpc::PrepareReply)>;

using OnAcceptCallBack = std::function<void (const vpaxos_rpc::Accept &request, vpaxos_rpc::AcceptReply &reply)>;
using AcceptFinishCallBack = std::function<Status (vpaxos_rpc::AcceptReply)>;

using OnLearnCallBack = std::function<void (const vpaxos_rpc::Learn &request, vpaxos_rpc::LearnReply &reply)>;
using LearnFinishCallBack = std::function<Status (vpaxos_rpc::LearnReply)>;

} // namespace vpaxos

#endif
