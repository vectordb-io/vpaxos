#include "env.h"
#include "node.h"

namespace vpaxos {

Env::Env()
    :storage_(Config::GetInstance().storage_path()) {

}

Env::~Env() {
}

Status
Env::AsyncLearn(const vpaxos_rpc::Learn &request, const std::string &address, LearnFinishCallBack cb) {
    auto s = grpc_server_.AsyncLearn(request, address, cb);
    return s;
}

Status
Env::AsyncPing(const vpaxos_rpc::Ping &request, const std::string &address, PingFinishCallBack cb) {
    auto s = grpc_server_.AsyncPing(request, address, cb);
    return s;
}

Status
Env::AsyncPrepare(const vpaxos_rpc::Prepare &request, const std::string &address, PrepareFinishCallBack cb) {
    auto s = grpc_server_.AsyncPrepare(request, address, cb);
    return s;
}

Status
Env::AsyncAccept(const vpaxos_rpc::Accept &request, const std::string &address, AcceptFinishCallBack cb) {
    auto s = grpc_server_.AsyncAccept(request, address, cb);
    return s;
}

Status
Env::Init() {
    Status s;
    s = storage_.Init();
    assert(s.ok());

    return Status::OK();
}

Status
Env::Start() {
    Status s;
    thread_pool_.Start(1);
    s = grpc_server_.Start();
    assert(s.ok());
    s = timer_.Start();
    assert(s.ok());

    while(!timer_.running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    if (Config::GetInstance().ping()) {
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        vpaxos::Env::GetInstance().timer()->RunEvery(std::bind(&vpaxos::Node::PingAll, &(vpaxos::Node::GetInstance())), 3000);
    }

    return Status::OK();
}

Status
Env::Stop() {
    Status s;
    s = timer_.Stop();
    assert(s.ok());
    s = grpc_server_.Stop();
    assert(s.ok());
    thread_pool_.Stop();

    return Status::OK();
}

Status
Env::MaxBallot(Ballot &ballot) const {
    auto s = storage_.MaxBallot(ballot);
    return s;
}

Status
Env::StoreMaxBallot(const Ballot &ballot) {
    auto s = storage_.StoreMaxBallot(ballot);
    return s;
}

Status
Env::ChosenValue(std::string &chosen_value) const {
    auto s = storage_.ChosenValue(chosen_value);
    return s;
}

Status
Env::StoreChosenValue(const std::string &chosen_value) {
    auto s = storage_.StoreChosenValue(chosen_value);
    return s;
}

bool
Env::HasChosenValue() const {
    auto s = storage_.HasChosenValue();
    return s;
}

Status
Env::PromisedBallot(Ballot &ballot) const {
    auto s = storage_.PromisedBallot(ballot);
    return s;
}

Status
Env::StorePromisedBallot(const Ballot &ballot) {
    auto s = storage_.StorePromisedBallot(ballot);
    return s;
}

bool
Env::HasPromisedBallot() {
    auto s = storage_.HasPromisedBallot();
    return s;
}

Status
Env::AcceptedBallot(Ballot &ballot) const {
    auto s = storage_.AcceptedBallot(ballot);
    return s;
}

Status
Env::StoreAcceptedBallot(const Ballot &ballot) {
    auto s = storage_.StoreAcceptedBallot(ballot);
    return s;
}

bool
Env::HasAcceptedBallot() {
    auto s = storage_.HasAcceptedBallot();
    return s;
}

Status
Env::AcceptedValue(std::string &accepted_value) const {
    auto s = storage_.AcceptedValue(accepted_value);
    return s;
}

Status
Env::StoreAcceptedValue(const std::string &accepted_value) {
    auto s = storage_.StoreAcceptedValue(accepted_value);
    return s;
}

bool
Env::HasAcceptedValue() const {
    auto s = storage_.HasAcceptedValue();
    return s;
}

}  // namespace vpaxos
