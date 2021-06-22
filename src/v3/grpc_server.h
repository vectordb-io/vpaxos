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
#include "async_req_manager.h"

namespace vpaxos {

class GrpcServer {
  public:
    GrpcServer();
    GrpcServer(const GrpcServer&) = delete;
    GrpcServer& operator=(const GrpcServer&) = delete;
    ~GrpcServer();

    Status Init();
    Status Start();
    Status Stop();
    Status StartService();
    void ThreadAsyncCalled();
    void ThreadAsyncCall();

    Status AsyncProposeReply(const vpaxos_rpc::ProposeReply &reply, void *call);

    void IntendOnPing();
    void OnPing(AsyncTaskOnPing *p);
    Status AsyncPing(const vpaxos_rpc::Ping &request, const std::string &address, PingFinishFunc cb);

    void IntendOnPropose();
    void OnPropose(AsyncTaskOnPropose *p);

    void IntendOnPrepare();
    void OnPrepare(AsyncTaskOnPrepare *p);
    Status AsyncPrepare(const vpaxos_rpc::Prepare &request, const std::string &address, PrepareFinishFunc cb);

    void IntendOnAccept();
    void OnAccept(AsyncTaskOnAccept *p);
    Status AsyncAccept(const vpaxos_rpc::Accept &request, const std::string &address, AcceptFinishFunc cb);

    void IntendOnLearn();
    void OnLearn(AsyncTaskOnLearn *p);
    Status AsyncLearn(const vpaxos_rpc::Learn &request, const std::string &address, LearnFinishFunc cb);

    void set_on_ping_cb(OnPingFunc cb) {
        on_ping_cb_ = cb;
    }

    void set_on_propose_cb(OnProposeFunc cb) {
        on_propose_cb_ = cb;
    }

    void set_on_prepare_cb(OnPrepareFunc cb) {
        on_prepare_cb_ = cb;
    }

    void set_on_accept_cb(OnAcceptFunc cb) {
        on_accept_cb_ = cb;
    }

    void set_on_learn_cb(OnLearnFunc cb) {
        on_learn_cb_ = cb;
    }

  private:
    bool running_;
    OnPingFunc on_ping_cb_;
    OnProposeFunc on_propose_cb_;
    OnPrepareFunc on_prepare_cb_;
    OnAcceptFunc on_accept_cb_;
    OnLearnFunc on_learn_cb_;

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

    AsyncReqManager async_req_manager_;
};

} // namespace vpaxos

#endif
