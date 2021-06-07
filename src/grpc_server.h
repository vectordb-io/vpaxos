#ifndef __VPAXOS_GRPC_SERVER_H__
#define __VPAXOS_GRPC_SERVER_H__

#include <string>
#include <thread>
#include <memory>
#include <functional>
#include <grpcpp/grpcpp.h>
#include "vpaxos_rpc.grpc.pb.h"
#include "status.h"
#include "config.h"
#include "async_task_called.h"
#include "async_task_call.h"

namespace vpaxos {

class GrpcServer {
  public:
    GrpcServer();
    GrpcServer(const GrpcServer&) = delete;
    GrpcServer& operator=(const GrpcServer&) = delete;
    ~GrpcServer();

    Status Start();
    Status Stop();
    Status StartService();
    void ThreadAsyncCalled();
    void ThreadAsyncCall();

    void IntendOnPing();
    void OnPing(AsyncTaskOnPing *p);
    Status AsyncPing(const vpaxos_rpc::Ping &request, const std::string &address, PingFinishCallBack cb);

    void IntendOnPropose();
    void OnPropose(AsyncTaskOnPropose *p);

    void IntendOnPrepare();
    void OnPrepare(AsyncTaskOnPrepare *p);
    Status AsyncPrepare(const vpaxos_rpc::Prepare &request, const std::string &address, PrepareFinishCallBack cb);

    void IntendOnAccept();
    void OnAccept(AsyncTaskOnAccept *p);
    Status AsyncAccept(const vpaxos_rpc::Accept &request, const std::string &address, AcceptFinishCallBack cb);

    void IntendOnLearn();
    void OnLearn(AsyncTaskOnLearn *p);
    Status AsyncLearn(const vpaxos_rpc::Learn &request, const std::string &address, LearnFinishCallBack cb);

    void set_on_ping_cb(OnPingCallBack cb) {
        on_ping_cb_ = cb;
    }

    void set_on_propose_cb(OnProposeCallBack cb) {
        on_propose_cb_ = cb;
    }

    void set_on_prepare_cb(OnPrepareCallBack cb) {
        on_prepare_cb_ = cb;
    }

    void set_on_accept_cb(OnAcceptCallBack cb) {
        on_accept_cb_ = cb;
    }

    void set_on_learn_cb(OnLearnCallBack cb) {
        on_learn_cb_ = cb;
    }

  private:
    bool running_;
    OnPingCallBack on_ping_cb_;
    OnProposeCallBack on_propose_cb_;
    OnPrepareCallBack on_prepare_cb_;
    OnAcceptCallBack on_accept_cb_;
    OnLearnCallBack on_learn_cb_;

    std::unique_ptr<grpc::CompletionQueue> cq_out_;
    std::unique_ptr<grpc::ServerCompletionQueue> cq_in_;
    vpaxos_rpc::VPaxos::AsyncService service_;
    std::unique_ptr<grpc::Server> server_;

    // rpc call in, put it into task queue, wait for response
    std::unique_ptr<std::thread> thread_called_;

    // rpc call out, wait for response
    std::unique_ptr<std::thread> thread_call_;

    // boot thread
    std::unique_ptr<std::thread> boot_thread_;
};

} // namespace vpaxos

#endif
