#include  <functional>
#include <glog/logging.h>
#include "env.h"
#include "async_task_called.h"

namespace vpaxos {

void
AsyncTaskOnPrepare::Process() {
    if (done_) {
        // optimizing by memory pool
        delete this;
    } else {
        Env::GetInstance().thread_pool()->ProduceOne(std::bind(&GrpcServer::OnPrepare, Env::GetInstance().grpc_server(), this));
        Env::GetInstance().grpc_server()->IntendOnPrepare();
    }
}

void
AsyncTaskOnPing::Process() {
    if (done_) {
        // optimizing by memory pool
        delete this;
    } else {
        Env::GetInstance().thread_pool()->ProduceOne(std::bind(&GrpcServer::OnPing, Env::GetInstance().grpc_server(), this));
        Env::GetInstance().grpc_server()->IntendOnPing();
    }
}

void
AsyncTaskOnPropose::Process() {
    if (done_) {
        // optimizing by memory pool
        delete this;
    } else {
        Env::GetInstance().thread_pool()->ProduceOne(std::bind(&GrpcServer::OnPropose, Env::GetInstance().grpc_server(), this));
        Env::GetInstance().grpc_server()->IntendOnPropose();
    }
}

void
AsyncTaskOnAccept::Process() {
    if (done_) {
        // optimizing by memory pool
        delete this;
    } else {
        Env::GetInstance().thread_pool()->ProduceOne(std::bind(&GrpcServer::OnAccept, Env::GetInstance().grpc_server(), this));
        Env::GetInstance().grpc_server()->IntendOnAccept();
    }
}

void
AsyncTaskOnLearn::Process() {
    if (done_) {
        // optimizing by memory pool
        delete this;
    } else {
        Env::GetInstance().thread_pool()->ProduceOne(std::bind(&GrpcServer::OnLearn, Env::GetInstance().grpc_server(), this));
        Env::GetInstance().grpc_server()->IntendOnLearn();
    }
}

} // namespace vpaxos
