#include <cstdio>
#include <cstring>
#include <iostream>
#include "vpaxos_rpc.pb.h"
#include "env.h"
#include "node.h"
#include "coding.h"
#include "proposer.h"

namespace vpaxos {

PrepareManager::PrepareManager(int quorum)
    :accept_(false),
     quorum_(quorum) {
}

PrepareManager::~PrepareManager() {
}

bool
PrepareManager::accept() const {
    return accept_;
}

void
PrepareManager::set_accept() {
    accept_ = true;
}

void
PrepareManager::Vote(const vpaxos_rpc::PrepareReply &reply) {
    assert(reply.prepared());
    Ballot promised_ballot;
    Pb2Ballot(reply.promised_ballot(), promised_ballot);
    assert(promised_ballot == promised_ballot_);

    if (reply.ever_accepted()) {
        Ballot accepted_ballot;
        Pb2Ballot(reply.accepted_ballot(), accepted_ballot);

        if (max_accepted_ballot_.IsNull() || accepted_ballot > max_accepted_ballot_) {
            max_accepted_ballot_ = accepted_ballot;
            accepted_value_ = reply.accepted_value();
        }
    }

    votes_.insert(std::pair<std::string, vpaxos_rpc::PrepareReply>(reply.address(), reply));
}

bool
PrepareManager::Majority() const {
    return static_cast<int>(votes_.size()) >= quorum_;
}

void
PrepareManager::Reset(Ballot promised_ballot) {
    promised_ballot_ = promised_ballot;
    max_accepted_ballot_.Clear();
    accepted_value_.clear();
    votes_.clear();
    accept_ = false;
}

bool
PrepareManager::HasAcceptedValue() const {
    return !max_accepted_ballot_.IsNull();
}

std::string
PrepareManager::AcceptedValue() const {
    assert(!max_accepted_ballot_.IsNull());
    assert(!promised_ballot_.IsNull());
    return accepted_value_;
}

std::string
PrepareManager::ToString() const {
    std::string str;
    char buf[256];
    str.append("PrepareManager:\n");

    str.append("promised_ballot_: ");
    if (promised_ballot_.IsNull()) {
        str.append("null");
    } else {
        str.append(promised_ballot_.ToString());
    }
    str.append("\n");

    if (max_accepted_ballot_.IsNull()) {
        str.append("max_accepted_ballot_: null");
        str.append("\n");
        str.append("accepted_value_: null");
        str.append("\n");
    } else {
        str.append("max_accepted_ballot_: ");
        str.append(max_accepted_ballot_.ToString());
        str.append("\n");
        str.append("accepted_value_: ");
        str.append(accepted_value_);
        str.append("\n");
    }

    snprintf(buf, sizeof(buf), "quorum_: %d", quorum_);
    str.append(buf);
    str.append("\n");

    snprintf(buf, sizeof(buf), "agree: %lu", votes_.size());
    str.append(buf);
    str.append("\n");

    str.append("votes_:\n");
    for (auto &a : votes_) {
        str.append(a.first);
        str.append(" --> \n");
        str.append("<<\n");
        str.append(a.second.DebugString());
        str.append(">>\n");
    }

    return str;
}

AcceptManager::AcceptManager(int quorum)
    :learn_(false),
     quorum_(quorum) {
}

AcceptManager::~AcceptManager() {
}

void
AcceptManager::Vote(const vpaxos_rpc::AcceptReply &reply) {
    assert(reply.accepted());
    Ballot accepted_ballot;
    Pb2Ballot(reply.accepted_ballot(), accepted_ballot);
    assert(accepted_ballot == accepted_ballot_);

    if (votes_.empty()) {
        accepted_value_ = reply.accepted_value();
    } else {
        assert(accepted_value_ == reply.accepted_value());
    }

    votes_.insert(std::pair<std::string, vpaxos_rpc::AcceptReply>(reply.address(), reply));
}

std::string
AcceptManager::AcceptedValue() const {
    assert(!accepted_ballot_.IsNull());
    assert(votes_.size() > 0);
    return accepted_value_;
}

void
AcceptManager::Reset(Ballot accepted_ballot) {
    accepted_ballot_ = accepted_ballot;
    votes_.clear();
    accepted_value_.clear();
    learn_ = false;
}

bool
AcceptManager::learn() const {
    return learn_;
}

void
AcceptManager::set_learn() {
    learn_ = true;
}

bool
AcceptManager::Majority() const {
    return static_cast<int>(votes_.size()) >= quorum_;
}

std::string
AcceptManager::ToString() const {
    std::string str;
    char buf[256];
    str.append("AcceptManager:\n");

    str.append("accepted_ballot_: ");
    if (accepted_ballot_.IsNull()) {
        str.append("null");
    } else {
        str.append(accepted_ballot_.ToString());
    }
    str.append("\n");

    str.append("accepted_value_: ");
    str.append(accepted_value_);
    str.append("\n");

    snprintf(buf, sizeof(buf), "quorum_: %d", quorum_);
    str.append(buf);
    str.append("\n");

    snprintf(buf, sizeof(buf), "agree: %lu", votes_.size());
    str.append(buf);
    str.append("\n");

    str.append("votes_:\n");
    for (auto &a : votes_) {
        str.append(a.first);
        str.append(" --> \n");
        str.append("<<\n");
        str.append(a.second.DebugString());
        str.append(">>\n");
    }

    return str;
}

Proposer::Proposer()
    :prepare_manager_(Config::GetInstance().Quorum()),
     accept_manager_(Config::GetInstance().Quorum()) {
}

Status
Proposer::Init() {
    Env::GetInstance().grpc_server()->set_on_propose_cb(
        std::bind(&Proposer::OnPropose, this, std::placeholders::_1, std::placeholders::_2)
    );

    propose_value_.clear();

    Ballot max_ballot;
    Status s = MaxBallot(max_ballot);
    if (s.ok()) {
        current_ballot_ = max_ballot;
    } else if (s.IsNotFound()) {
        current_ballot_.Init(0, Node::GetInstance().Id());
        s = StoreMaxBallot(current_ballot_);
        assert(s.ok());
    } else {
        assert(0);
    }

    return Status::OK();
}

void
Proposer::NextBallot() {
    Status s;
    s = MaxBallot(current_ballot_);
    assert(s.ok());
    current_ballot_++;
    s = StoreMaxBallot(current_ballot_);
    assert(s.ok());
}

void
Proposer::NextBallot(Ballot b) {

    LOG(INFO) << "debug NextBallot begin " << "input:" << b.ToString() << " current_ballot_:" << current_ballot_.ToString();

    Status s;
    s = MaxBallot(current_ballot_);
    assert(s.ok());

    if (b > current_ballot_) {
        current_ballot_.set_proposal_id(b.proposal_id()+1);
    } else {
        current_ballot_++;
    }

    s = StoreMaxBallot(current_ballot_);
    assert(s.ok());



    LOG(INFO) << "debug NextBallot end " << "input:" << b.ToString() << " current_ballot_:" << current_ballot_.ToString();

}

Status
Proposer::Propose(std::string value) {
    NextBallot();
    prepare_manager_.Reset(current_ballot_);
    accept_manager_.Reset(current_ballot_);
    propose_value_ = value;

    auto s = PrepareAll();
    assert(s.ok());

    return Status::OK();
}

Status
Proposer::PrepareAll() {
    Status s;
    vpaxos_rpc::Prepare request;

    LOG(INFO) << "PrepareAll debug:" << current_ballot_.ToString();

    Ballot2Pb(current_ballot_, *request.mutable_ballot());
    Ballot2Pb(current_ballot_, *request.mutable_trace_ballot());
    request.set_address(Config::GetInstance().MyAddress()->ToString());

    for (auto &hp : Config::GetInstance().address_) {
        s = Prepare(request, hp->ToString());
        assert(s.ok());
    }

    return Status::OK();
}

Status
Proposer::AcceptAll() {
    Status s;
    vpaxos_rpc::Accept request;
    Ballot2Pb(current_ballot_, *request.mutable_ballot());
    Ballot2Pb(current_ballot_, *request.mutable_trace_ballot());
    request.set_address(Config::GetInstance().MyAddress()->ToString());

    if (prepare_manager_.HasAcceptedValue()) {
        request.set_value(prepare_manager_.AcceptedValue());
    } else {
        request.set_value(propose_value_);
    }

    for (auto &hp : Config::GetInstance().address_) {
        s = Accept(request, hp->ToString());
        assert(s.ok());
    }

    return Status::OK();
}

void
Proposer::OnPropose(const vpaxos_rpc::Propose &request, vpaxos_rpc::ProposeReply &reply) {
    DebugLog(false, __FUNCTION__, "x.x.x.x", &request);

    /*
    if (Node::GetInstance().learner()->Chosen()) {
        reply.set_msg("chosen");
        reply.set_chosen(true);
        std::string chosen_value;
        auto s = Node::GetInstance().learner()->ChosenValue(chosen_value);
        assert(s.ok());
        reply.set_chosen_value(chosen_value);
    } else {
    */


    auto s = Propose(request.value());
    assert(s.ok());
    reply.set_msg("ok");
    reply.set_chosen(false);
    //}
}


Status
Proposer::Prepare(const vpaxos_rpc::Prepare &request, const std::string &address) {
    DebugLog(true, __FUNCTION__, address, &request);
    Node::GetInstance().Sleep(100, 200);

    auto s = Env::GetInstance().AsyncPrepare(
                 request,
                 address,
                 std::bind(&Proposer::OnPrepareReply, this, std::placeholders::_1)
             );
    return s;
}

Status
Proposer::OnPrepareReply(const vpaxos_rpc::PrepareReply &reply) {
    DebugLog(false, __FUNCTION__, reply.address(), &reply);

    Status s;
    Ballot receive_ballot;
    Pb2Ballot(reply.promised_ballot(), receive_ballot);
    if (current_ballot_ > receive_ballot) {
        LOG(INFO) << "old ballot: " << "current_ballot_:" << current_ballot_.ToString()
                  << " receive_ballot:" << receive_ballot.ToString();
        return Status::OK();
    }

    if (reply.prepared()) {
        prepare_manager_.Vote(reply);
        if (prepare_manager_.Majority()) {
            if (!prepare_manager_.accept()) {
                s = AcceptAll();
                assert(s.ok());
                prepare_manager_.set_accept();
            } else {
                LOG(INFO) << "already broadcast accept for " << current_ballot_.ToString();
            }
        }
    } else {
        LOG(INFO) << "prepare is rejected, prepare again."
                  << " current_ballot_:[" << current_ballot_.ToString() << "]"
                  << " receive_ballot:[" << receive_ballot.ToString() << "]";


        LOG(INFO) << "debug NextBallot call begin: " << "input:" << receive_ballot.ToString() << " current_ballot_:" << current_ballot_.ToString();

        NextBallot(receive_ballot);

        LOG(INFO) << "debug NextBallot call end: " << "input:" << receive_ballot.ToString() << " current_ballot_:" << current_ballot_.ToString();


        LOG(INFO) << "after rejected, next_ballot: [" << current_ballot_.ToString() << "]";
        prepare_manager_.Reset(current_ballot_);
        accept_manager_.Reset(current_ballot_);
        auto s = PrepareAll();
        assert(s.ok());
    }

    return Status::OK();
}

Status
Proposer::Accept(const vpaxos_rpc::Accept &request, const std::string &address) {
    DebugLog(true, __FUNCTION__, address, &request);
    Node::GetInstance().Sleep(100, 200);

    auto s = Env::GetInstance().AsyncAccept(
                 request,
                 address,
                 std::bind(&Proposer::OnAcceptReply, this, std::placeholders::_1)
             );
    return s;
}

Status
Proposer::OnAcceptReply(const vpaxos_rpc::AcceptReply &reply) {
    DebugLog(false, __FUNCTION__, reply.address(), &reply);

    Status s;
    Ballot receive_ballot;
    Pb2Ballot(reply.accepted_ballot(), receive_ballot);
    if (receive_ballot != current_ballot_) {
        LOG(INFO) << "error ballot: " << "current_ballot_:" << current_ballot_.ToString()
                  << " receive_ballot:" << receive_ballot.ToString();
        return Status::OK();
    }

    if (reply.accepted()) {
        accept_manager_.Vote(reply);
        if (accept_manager_.Majority()) {
            std::string log_str;
            log_str.append("value chosen:[").append(accept_manager_.AcceptedValue())
            .append("] current_ballot_:[").append(current_ballot_.ToString()).append("]\n\n")
            .append("prepare_manager_:").append(prepare_manager_.ToString()).append("\n")
            .append("accept_manager_:").append(accept_manager_.ToString()).append("\n");
            LOG(INFO) << log_str;

            if (!accept_manager_.learn()) {
                Node::GetInstance().learner()->LearnAll(accept_manager_.AcceptedValue());
                accept_manager_.set_learn();
            } else {
                LOG(INFO) << "already broadcast learn for " << current_ballot_.ToString();
            }
        }
    } else {
        LOG(INFO) << "accept is rejected, prepare again. "
                  << " current_ballot_:[" << current_ballot_.ToString() << "]"
                  << " receive_ballot:[" << receive_ballot.ToString() << "]";
        NextBallot(receive_ballot);
        LOG(INFO) << "after rejected, next_ballot: [" << current_ballot_.ToString() << "]";
        prepare_manager_.Reset(current_ballot_);
        accept_manager_.Reset(current_ballot_);
        auto s = PrepareAll();
        assert(s.ok());
    }

    return Status::OK();
}

Status
Proposer::MaxBallot(Ballot &ballot) const {
    auto s = Env::GetInstance().MaxBallot(ballot);

    LOG(INFO) << "debug: MaxBallot" << ballot.ToString();

    return s;
}

Status
Proposer::StoreMaxBallot(const Ballot &ballot) {
    auto s = Env::GetInstance().StoreMaxBallot(ballot);
    return s;
}

void
Proposer::DebugLog(bool is_send, std::string header, std::string address, const google::protobuf::Message *m) {
    std::string str;
    char buf[256];

    str.append("\n");
    if (is_send) {
        str.append("|--------------send message---------------|");
    } else {
        str.append("|==============receive message============|");
    }
    str.append("\n");

    str.append("header:");
    str.append(header);
    str.append("\n");

    if (is_send) {
        str.append("to address:").append(address);
    } else {
        str.append("from address:").append(address);
    }
    str.append("\n");

    str.append("my address:").append(Config::GetInstance().MyAddress()->ToString());
    str.append("\n");

    snprintf(buf, sizeof(buf), "my node_id:%lu", Node::GetInstance().Id());
    str.append(buf);
    str.append("\n");

    str.append("current_ballot_:").append(current_ballot_.ToString());
    str.append("\n");

    snprintf(buf, sizeof(buf), "propose_value_: [%s]", propose_value_.c_str());
    str.append(buf);
    str.append("\n");

    str.append("\n");
    str.append(prepare_manager_.ToString());
    str.append("\n");

    str.append(accept_manager_.ToString());
    str.append("\n");

    Status s;
    str.append("store_max_ballot:");
    Ballot store_max_ballot;
    s = MaxBallot(store_max_ballot);
    assert(s.ok());
    if (store_max_ballot.IsNull()) {
        str.append("null");
    } else {
        str.append(store_max_ballot.ToString());
    }
    str.append("\n");

    if (is_send) {
        str.append("send message:\n<<\n").append(m->DebugString()).append(">>\n");
        str.append("|-----------------------------------------|");
    } else {
        str.append("receive message:\n<<\n").append(m->DebugString()).append(">>\n");
        str.append("|=========================================|");
    }
    str.append("\n");

    LOG(INFO) << str;
}

}  // namespace vpaxos
