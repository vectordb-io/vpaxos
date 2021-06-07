#include  <functional>
#include <glog/logging.h>
#include "env.h"
#include "grpc_server.h"
#include "thread_pool.h"

namespace vpaxos {

GrpcServer::GrpcServer()
    :running_(false),
     cq_out_(std::make_unique<grpc::CompletionQueue>()) {
}

GrpcServer::~GrpcServer() {
    server_->Shutdown();
    cq_in_->Shutdown();
    cq_out_->Shutdown();
}

void
GrpcServer::IntendOnLearn() {
    // optimizing by memory pool
    auto p = new AsyncTaskOnLearn(&service_, cq_in_.get());
    assert(on_learn_cb_);
    p->cb_ = on_learn_cb_;
    service_.RequestRpcLearn(&(p->ctx_), &(p->request_), &(p->responder_), p->cq_in_, p->cq_in_, p);
}

void
GrpcServer::OnLearn(AsyncTaskOnLearn *p) {
    p->cb_(p->request_, p->reply_);
    p->done_ = true;
    p->responder_.Finish(p->reply_, grpc::Status::OK, p);
}

Status
GrpcServer::AsyncLearn(const vpaxos_rpc::Learn &request, const std::string &address, LearnFinishCallBack cb) {
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    std::unique_ptr<vpaxos_rpc::VPaxos::Stub> stub = vpaxos_rpc::VPaxos::NewStub(channel);
    // optimizing by memory pool
    auto p = new AsyncTaskLearn();
    p->cb_ = cb;

    p->response_reader_ = stub->PrepareAsyncRpcLearn(&(p->ctx_), request, cq_out_.get());
    p->response_reader_->StartCall();
    p->response_reader_->Finish(&p->reply_, &p->status_, (void*)p);

    return Status::OK();
}

void
GrpcServer::IntendOnPing() {
    // optimizing by memory pool
    auto p = new AsyncTaskOnPing(&service_, cq_in_.get());
    assert(on_ping_cb_);
    p->cb_ = on_ping_cb_;
    service_.RequestRpcPing(&(p->ctx_), &(p->request_), &(p->responder_), p->cq_in_, p->cq_in_, p);
}

void
GrpcServer::OnPing(AsyncTaskOnPing *p) {
    p->cb_(p->request_, p->reply_);
    p->done_ = true;
    p->responder_.Finish(p->reply_, grpc::Status::OK, p);
}

Status
GrpcServer::AsyncPing(const vpaxos_rpc::Ping &request, const std::string &address, PingFinishCallBack cb) {
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    std::unique_ptr<vpaxos_rpc::VPaxos::Stub> stub = vpaxos_rpc::VPaxos::NewStub(channel);
    // optimizing by memory pool
    auto p = new AsyncTaskPing();
    p->cb_ = cb;

    p->response_reader_ = stub->PrepareAsyncRpcPing(&(p->ctx_), request, cq_out_.get());
    p->response_reader_->StartCall();
    p->response_reader_->Finish(&p->reply_, &p->status_, (void*)p);

    return Status::OK();
}

void
GrpcServer::IntendOnPropose() {
    // optimizing by memory pool
    auto p = new AsyncTaskOnPropose(&service_, cq_in_.get());
    assert(on_propose_cb_);
    p->cb_ = on_propose_cb_;
    service_.RequestRpcPropose(&(p->ctx_), &(p->request_), &(p->responder_), p->cq_in_, p->cq_in_, p);
}

void
GrpcServer::OnPropose(AsyncTaskOnPropose *p) {
    p->cb_(p->request_, p->reply_);
    p->done_ = true;
    p->responder_.Finish(p->reply_, grpc::Status::OK, p);
}

void
GrpcServer::IntendOnPrepare() {
    // optimizing by memory pool
    auto p = new AsyncTaskOnPrepare(&service_, cq_in_.get());
    assert(on_prepare_cb_);
    p->cb_ = on_prepare_cb_;
    service_.RequestRpcPrepare(&(p->ctx_), &(p->request_), &(p->responder_), p->cq_in_, p->cq_in_, p);
}

void
GrpcServer::OnPrepare(AsyncTaskOnPrepare *p) {
    p->cb_(p->request_, p->reply_);
    p->done_ = true;
    p->responder_.Finish(p->reply_, grpc::Status::OK, p);
}

Status
GrpcServer::AsyncPrepare(const vpaxos_rpc::Prepare &request, const std::string &address, PrepareFinishCallBack cb) {
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    std::unique_ptr<vpaxos_rpc::VPaxos::Stub> stub = vpaxos_rpc::VPaxos::NewStub(channel);
    // optimizing by memory pool
    auto p = new AsyncTaskPrepare();
    p->cb_ = cb;

    p->response_reader_ = stub->PrepareAsyncRpcPrepare(&(p->ctx_), request, cq_out_.get());
    p->response_reader_->StartCall();
    p->response_reader_->Finish(&p->reply_, &p->status_, (void*)p);

    return Status::OK();
}

void
GrpcServer::IntendOnAccept() {
    // optimizing by memory pool
    auto p = new AsyncTaskOnAccept(&service_, cq_in_.get());
    assert(on_accept_cb_);
    p->cb_ = on_accept_cb_;
    service_.RequestRpcAccept(&(p->ctx_), &(p->request_), &(p->responder_), p->cq_in_, p->cq_in_, p);
}

void
GrpcServer::OnAccept(AsyncTaskOnAccept *p) {
    p->cb_(p->request_, p->reply_);
    p->done_ = true;
    p->responder_.Finish(p->reply_, grpc::Status::OK, p);
}

Status
GrpcServer::AsyncAccept(const vpaxos_rpc::Accept &request, const std::string &address, AcceptFinishCallBack cb) {
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    std::unique_ptr<vpaxos_rpc::VPaxos::Stub> stub = vpaxos_rpc::VPaxos::NewStub(channel);
    // optimizing by memory pool
    auto p = new AsyncTaskAccept();
    p->cb_ = cb;

    p->response_reader_ = stub->PrepareAsyncRpcAccept(&(p->ctx_), request, cq_out_.get());
    p->response_reader_->StartCall();
    p->response_reader_->Finish(&p->reply_, &p->status_, (void*)p);

    return Status::OK();
}


Status
GrpcServer::StartService() {
    std::string server_address(Config::GetInstance().MyAddress()->ToString());
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);
    cq_in_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();
    running_ = true;

    LOG(INFO) << "grpc server message_in_thread start ...";
    thread_called_ = std::make_unique<std::thread>(&GrpcServer::ThreadAsyncCalled, this);

    LOG(INFO) << "grpc server message_out_thread start ...";
    thread_call_ = std::make_unique<std::thread>(&GrpcServer::ThreadAsyncCall, this);

    IntendOnPing();
    IntendOnPropose();
    IntendOnPrepare();
    IntendOnAccept();
    IntendOnLearn();

    return Status::OK();
}

Status
GrpcServer::Start() {
    LOG(INFO) << "grpc server start ...";

    LOG(INFO) << "grpc server boot_thread start ...";
    boot_thread_ = std::make_unique<std::thread>(&GrpcServer::StartService, this);

    return Status::OK();
}

Status
GrpcServer::Stop() {
    LOG(INFO) << "grpc server stop ...";
    running_ = false;

    boot_thread_->join();
    LOG(INFO) << "grpc server boot_thread stop ...";

    thread_called_->join();
    LOG(INFO) << "grpc server message_in_thread stop ...";

    thread_call_->join();
    LOG(INFO) << "grpc server message_out_thread stop ...";

    return Status::OK();
}

void
GrpcServer::ThreadAsyncCalled() {
    void* tag;
    bool ok;
    while (running_) {
        cq_in_->Next(&tag, &ok);
        assert(ok);

        AsyncTaskCalled *p = static_cast<AsyncTaskCalled*>(tag);
        p->Process();
    }
}

void
GrpcServer::ThreadAsyncCall() {
    void* tag;
    bool ok = false;
    while (cq_out_->Next(&tag, &ok) && running_) {
        AsyncTaskCall *p = static_cast<AsyncTaskCall*>(tag);
        assert(ok);

        if (p->GetStatus().ok()) {
            p->Process();
        } else {
            LOG(ERROR) << "err:" << p->GetStatus().error_message();
        }
    }
}

} // namespace vpaxos
