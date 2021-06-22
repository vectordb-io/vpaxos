#ifndef __VPAXOS_COMMON_H__
#define __VPAXOS_COMMON_H__

#include <sys/syscall.h>
#include "status.h"
#include "vpaxos_rpc.grpc.pb.h"

namespace vpaxos {

#define gettid() (syscall(SYS_gettid))

using OnPingFunc = std::function<void (const vpaxos_rpc::Ping &request, vpaxos_rpc::PingReply &reply)>;
using PingFinishFunc = std::function<Status (vpaxos_rpc::PingReply)>;

using OnProposeFunc = std::function<void (const vpaxos_rpc::Propose &request, void *async_flag)>;

using OnPrepareFunc = std::function<void (const vpaxos_rpc::Prepare &request, vpaxos_rpc::PrepareReply &reply)>;
using PrepareFinishFunc = std::function<Status (vpaxos_rpc::PrepareReply)>;

using OnAcceptFunc = std::function<void (const vpaxos_rpc::Accept &request, vpaxos_rpc::AcceptReply &reply)>;
using AcceptFinishFunc = std::function<Status (vpaxos_rpc::AcceptReply)>;

using OnLearnFunc = std::function<void (const vpaxos_rpc::Learn &request, vpaxos_rpc::LearnReply &reply)>;
using LearnFinishFunc = std::function<Status (vpaxos_rpc::LearnReply)>;

} // namespace vpaxos

#endif
