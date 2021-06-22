#include  <functional>
#include <glog/logging.h>
#include "env.h"
#include "grpc_server.h"
#include "thread_pool.h"
#include "async_task_call.h"

namespace vpaxos {

grpc::Status
AsyncTaskPing::GetStatus() {
    return status_;
}

grpc::Status
AsyncTaskPrepare::GetStatus() {
    return status_;
}

grpc::Status
AsyncTaskAccept::GetStatus() {
    return status_;
}

void
AsyncTaskPing::Process() {
    auto s = cb_(reply_);
    assert(s.ok());
    // optimizing by memory pool
    delete this;
}

void
AsyncTaskPrepare::Process() {
    auto s = cb_(reply_);
    assert(s.ok());
    // optimizing by memory pool
    delete this;
}

void
AsyncTaskAccept::Process() {
    auto s = cb_(reply_);
    assert(s.ok());
    // optimizing by memory pool
    delete this;
}

grpc::Status
AsyncTaskLearn::GetStatus() {
    return status_;
}

void
AsyncTaskLearn::Process() {
    auto s = cb_(reply_);
    assert(s.ok());
    // optimizing by memory pool
    delete this;
}

} // namespace vpaxos
