#ifndef __VPAXOS_ASYNC_TASK_CALL_H__
#define __VPAXOS_ASYNC_TASK_CALL_H__

#include <string>
#include <thread>
#include <memory>
#include <functional>
#include <grpcpp/grpcpp.h>
#include "vpaxos_rpc.grpc.pb.h"
#include "status.h"
#include "config.h"

namespace vpaxos {

class AsyncTaskCall {
  public:
    virtual void Process() = 0;
    virtual grpc::Status GetStatus() = 0;
    virtual ~AsyncTaskCall() {}
};

class AsyncTaskPing : public AsyncTaskCall {
  public:
    vpaxos_rpc::PingReply reply_;
    grpc::ClientContext ctx_;
    grpc::Status status_;
    std::unique_ptr<grpc::ClientAsyncResponseReader<vpaxos_rpc::PingReply>> response_reader_;
    PingFinishCallBack cb_;

    ~AsyncTaskPing() {}
    virtual void Process() override;
    virtual grpc::Status GetStatus() override;
};

class AsyncTaskPrepare : public AsyncTaskCall {
  public:
    vpaxos_rpc::PrepareReply reply_;
    grpc::ClientContext ctx_;
    grpc::Status status_;
    std::unique_ptr<grpc::ClientAsyncResponseReader<vpaxos_rpc::PrepareReply>> response_reader_;
    PrepareFinishCallBack cb_;

    ~AsyncTaskPrepare() {}
    virtual void Process() override;
    virtual grpc::Status GetStatus() override;
};

class AsyncTaskAccept : public AsyncTaskCall {
  public:
    vpaxos_rpc::AcceptReply reply_;
    grpc::ClientContext ctx_;
    grpc::Status status_;
    std::unique_ptr<grpc::ClientAsyncResponseReader<vpaxos_rpc::AcceptReply>> response_reader_;
    AcceptFinishCallBack cb_;

    ~AsyncTaskAccept() {}
    virtual void Process() override;
    virtual grpc::Status GetStatus() override;
};

class AsyncTaskLearn : public AsyncTaskCall {
  public:
    vpaxos_rpc::LearnReply reply_;
    grpc::ClientContext ctx_;
    grpc::Status status_;
    std::unique_ptr<grpc::ClientAsyncResponseReader<vpaxos_rpc::LearnReply>> response_reader_;
    LearnFinishCallBack cb_;

    ~AsyncTaskLearn() {}
    virtual void Process() override;
    virtual grpc::Status GetStatus() override;
};

} // namespace vpaxos

#endif
