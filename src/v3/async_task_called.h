#ifndef __VPAXOS_ASYNC_TASK_CALLED_H__
#define __VPAXOS_ASYNC_TASK_CALLED_H__

#include <string>
#include <thread>
#include <memory>
#include <functional>
#include <grpcpp/grpcpp.h>
#include "vpaxos_rpc.grpc.pb.h"
#include "status.h"
#include "config.h"

namespace vpaxos {

class AsyncTaskCalled {
  public:
    virtual void Process() = 0;
    virtual ~AsyncTaskCalled() {}
};

class AsyncTaskOnPrepare : public AsyncTaskCalled {
  public:
    AsyncTaskOnPrepare(vpaxos_rpc::VPaxos::AsyncService* service,
                       grpc::ServerCompletionQueue* cq)
        :service_(service), cq_in_(cq),
         responder_(&ctx_),
         done_(false) {
    }

    ~AsyncTaskOnPrepare() {}

    virtual void Process() override;

    vpaxos_rpc::VPaxos::AsyncService* service_;
    grpc::ServerCompletionQueue* cq_in_;
    grpc::ServerContext ctx_;

    grpc::ServerAsyncResponseWriter<vpaxos_rpc::PrepareReply> responder_;
    vpaxos_rpc::Prepare request_;
    vpaxos_rpc::PrepareReply reply_;

    bool done_;
    OnPrepareFunc cb_;
};

class AsyncTaskOnAccept : public AsyncTaskCalled {
  public:
    AsyncTaskOnAccept(vpaxos_rpc::VPaxos::AsyncService* service,
                      grpc::ServerCompletionQueue* cq)
        :service_(service), cq_in_(cq),
         responder_(&ctx_),
         done_(false) {
    }

    ~AsyncTaskOnAccept() {}

    virtual void Process() override;

    vpaxos_rpc::VPaxos::AsyncService* service_;
    grpc::ServerCompletionQueue* cq_in_;
    grpc::ServerContext ctx_;

    grpc::ServerAsyncResponseWriter<vpaxos_rpc::AcceptReply> responder_;
    vpaxos_rpc::Accept request_;
    vpaxos_rpc::AcceptReply reply_;

    bool done_;
    OnAcceptFunc cb_;
};

class AsyncTaskOnPing : public AsyncTaskCalled {
  public:
    AsyncTaskOnPing(vpaxos_rpc::VPaxos::AsyncService* service,
                    grpc::ServerCompletionQueue* cq)
        :service_(service), cq_in_(cq),
         responder_(&ctx_),
         done_(false) {
    }

    ~AsyncTaskOnPing() {}

    virtual void Process() override;

    vpaxos_rpc::VPaxos::AsyncService* service_;
    grpc::ServerCompletionQueue* cq_in_;
    grpc::ServerContext ctx_;

    grpc::ServerAsyncResponseWriter<vpaxos_rpc::PingReply> responder_;
    vpaxos_rpc::Ping request_;
    vpaxos_rpc::PingReply reply_;

    bool done_;
    OnPingFunc cb_;
};

class AsyncTaskOnPropose : public AsyncTaskCalled {
  public:
    AsyncTaskOnPropose(vpaxos_rpc::VPaxos::AsyncService* service,
                       grpc::ServerCompletionQueue* cq)
        :service_(service), cq_in_(cq),
         responder_(&ctx_),
         done_(false) {
    }

    ~AsyncTaskOnPropose() {}

    virtual void Process() override;

    vpaxos_rpc::VPaxos::AsyncService* service_;
    grpc::ServerCompletionQueue* cq_in_;
    grpc::ServerContext ctx_;

    grpc::ServerAsyncResponseWriter<vpaxos_rpc::ProposeReply> responder_;
    vpaxos_rpc::Propose request_;
    vpaxos_rpc::ProposeReply reply_;

    bool done_;
    OnProposeFunc cb_;
};

class AsyncTaskOnLearn : public AsyncTaskCalled {
  public:
    AsyncTaskOnLearn(vpaxos_rpc::VPaxos::AsyncService* service,
                     grpc::ServerCompletionQueue* cq)
        :service_(service), cq_in_(cq),
         responder_(&ctx_),
         done_(false) {
    }

    ~AsyncTaskOnLearn() {}

    virtual void Process() override;

    vpaxos_rpc::VPaxos::AsyncService* service_;
    grpc::ServerCompletionQueue* cq_in_;
    grpc::ServerContext ctx_;

    grpc::ServerAsyncResponseWriter<vpaxos_rpc::LearnReply> responder_;
    vpaxos_rpc::Learn request_;
    vpaxos_rpc::LearnReply reply_;

    bool done_;
    OnLearnFunc cb_;
};

} // namespace vpaxos

#endif
