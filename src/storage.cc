#include "node.h"
#include "coding.h"
#include "config.h"
#include "storage.h"

namespace vpaxos {

Storage::~Storage() {
    delete db_;
}

Storage::Storage(const std::string &s)
    :path_(s) {
}

Status
Storage::Init() {
    Status s;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path_, &db_);
    assert(status.ok());

    return Status::OK();
}

Status
Storage::ChosenValue(std::string &chosen_value) const {
    auto s = db_->Get(leveldb::ReadOptions(), Key_Chosen_Value, &chosen_value);
    if (s.IsNotFound()) {
        return Status::NotFound(Key_Chosen_Value);
    }

    return Status::OK();
}

Status
Storage::StoreChosenValue(const std::string &chosen_value) {
    leveldb::WriteOptions wo;
    wo.sync = true;
    auto s = db_->Put(wo, Key_Chosen_Value, chosen_value);
    assert(s.ok());

    return Status::OK();
}

bool
Storage::HasChosenValue() const {
    std::string buf;
    leveldb::Status s;

    s = db_->Get(leveldb::ReadOptions(), Key_Chosen_Value, &buf);
    return !s.IsNotFound();
}

Status
Storage::MaxBallot(Ballot &ballot) const {
    std::string buf;
    leveldb::Status s;

    s = db_->Get(leveldb::ReadOptions(), Key_Proposer_MaxBallot, &buf);
    if (s.IsNotFound()) {
        return Status::NotFound(Key_Proposer_MaxBallot);
    }

    bool ret = DecodeBallot(buf, ballot);
    assert(ret);
    return Status::OK();
}

Status
Storage::StoreMaxBallot(const Ballot &ballot) {
    std::string buf;
    EncodeBallot(buf, ballot);

    leveldb::WriteOptions wo;
    wo.sync = true;
    auto s = db_->Put(wo, Key_Proposer_MaxBallot, buf);
    assert(s.ok());


    LOG(INFO) << "debug StoreMaxBallot" << ballot.ToString();



    return Status::OK();
}

Status
Storage::PromisedBallot(Ballot &ballot) const {
    std::string buf;
    leveldb::Status s;

    s = db_->Get(leveldb::ReadOptions(), Key_Acceptor_PromisedBallot, &buf);
    if (s.IsNotFound()) {
        return Status::NotFound(Key_Acceptor_PromisedBallot);
    }

    bool ret = DecodeBallot(buf, ballot);
    assert(ret);
    return Status::OK();
}

Status
Storage::StorePromisedBallot(const Ballot &ballot) {
    std::string buf;
    EncodeBallot(buf, ballot);

    leveldb::WriteOptions wo;
    wo.sync = true;
    auto s = db_->Put(wo, Key_Acceptor_PromisedBallot, buf);
    assert(s.ok());

    LOG(INFO) << "debug StorePromisedBallot " << ballot.ToString();

    return Status::OK();
}

bool
Storage::HasPromisedBallot() {
    std::string buf;
    leveldb::Status s;

    s = db_->Get(leveldb::ReadOptions(), Key_Acceptor_PromisedBallot, &buf);
    return !s.IsNotFound();
}

Status
Storage::AcceptedBallot(Ballot &ballot) const {
    std::string buf;
    leveldb::Status s;

    s = db_->Get(leveldb::ReadOptions(), Key_Acceptor_AcceptedBallot, &buf);
    if (s.IsNotFound()) {
        return Status::NotFound(Key_Acceptor_AcceptedBallot);
    }

    bool ret = DecodeBallot(buf, ballot);
    assert(ret);
    return Status::OK();
}

Status
Storage::StoreAcceptedBallot(const Ballot &ballot) {
    std::string buf;
    EncodeBallot(buf, ballot);

    leveldb::WriteOptions wo;
    wo.sync = true;
    auto s = db_->Put(wo, Key_Acceptor_AcceptedBallot, buf);
    assert(s.ok());

    return Status::OK();
}

bool
Storage::HasAcceptedBallot() {
    std::string buf;
    leveldb::Status s;

    s = db_->Get(leveldb::ReadOptions(), Key_Acceptor_AcceptedBallot, &buf);
    return !s.IsNotFound();
}

Status
Storage::AcceptedValue(std::string &accepted_value) const {
    auto s = db_->Get(leveldb::ReadOptions(), Key_Acceptor_AcceptedValue, &accepted_value);
    if (s.IsNotFound()) {
        return Status::NotFound(Key_Acceptor_AcceptedValue);
    }

    return Status::OK();
}

Status
Storage::StoreAcceptedValue(const std::string &accepted_value) {
    leveldb::WriteOptions wo;
    wo.sync = true;
    auto s = db_->Put(wo, Key_Acceptor_AcceptedValue, accepted_value);
    assert(s.ok());

    return Status::OK();
}

bool
Storage::HasAcceptedValue() const {
    std::string buf;
    leveldb::Status s;

    s = db_->Get(leveldb::ReadOptions(), Key_Acceptor_AcceptedValue, &buf);
    return !s.IsNotFound();
}

}  // namespace vpaxos
