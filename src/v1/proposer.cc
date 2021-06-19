#include <cstdio>
#include <cstring>
#include <iostream>
#include "vpaxos_rpc.pb.h"
#include "env.h"
#include "node.h"
#include "coding.h"
#include "proposer.h"

namespace vpaxos {

jsonxx::json64
PrepareManager::ToJson() const {
    jsonxx::json64 j, jj;
    j["accept"] = accept_;
    j["quorum_"] = quorum_;
    j["promised_ballot"] = promised_ballot_.ToJson();
    j["max_accepted_ballot"] = max_accepted_ballot_.ToJson();
    j["accepted_value"] = accepted_value_;
    for (auto &kv : votes_) {
        j["votes"][kv.first] = ::vpaxos::ToJson(kv.second);
    }
    jj["PrepareManager"] = j;
    return jj;
}

std::string
PrepareManager::ToString() const {
    return ToJson().dump();
}

std::string
PrepareManager::ToStringPretty() const {
    return ToJson().dump(4, ' ');
}

jsonxx::json64
PrepareManager::ToJsonTiny() const {
    jsonxx::json64 j;
    j["promised_bal"] = promised_ballot_.ToJsonTiny();
    j["max_bal"] = max_accepted_ballot_.ToJsonTiny();
    j["accepted_value"] = accepted_value_;

    int votes = votes_.size();
    char buf[128];
    snprintf(buf, sizeof(buf), "%d-%d", quorum_, votes);
    j["votes"] = std::string(buf);
    return j;
}

std::string
PrepareManager::ToStringTiny() const {
    return ToJsonTiny().dump();
}

jsonxx::json64
AcceptManager::ToJson() const {
    jsonxx::json64 j, jj;
    j["learn"] = learn_;
    j["quorum_"] = quorum_;
    j["accepted_ballot"] = accepted_ballot_.ToJson();
    j["accepted_value"] = accepted_value_;
    for (auto &kv : votes_) {
        j["votes"][kv.first] = ::vpaxos::ToJson(kv.second);
    }
    jj["AcceptManager"] = j;
    return jj;
}

std::string
AcceptManager::ToString() const {
    return ToJson().dump();
}

std::string
AcceptManager::ToStringPretty() const {
    return ToJson().dump(4, ' ');
}

jsonxx::json64
AcceptManager::ToJsonTiny() const {
    jsonxx::json64 j;
    j["accepted_bal"] = accepted_ballot_.ToJsonTiny();
    j["accepted_value"] = accepted_value_;

    int votes = votes_.size();
    char buf[128];
    snprintf(buf, sizeof(buf), "%d-%d", quorum_, votes);
    j["votes"] = std::string(buf);
    return j;
}

std::string
AcceptManager::ToStringTiny() const {
    return ToJsonTiny().dump();
}

jsonxx::json64
Proposer::ToJson() const {
    jsonxx::json64 j, jj;
    j["propose_value"] = propose_value_;
    j["proposing"] = proposing_;
    j["prepare_manager"] = prepare_manager_.ToJson();;
    j["accept_manager"] = accept_manager_.ToJson();;

    Ballot mb;
    Status s = MaxBallot(mb);
    assert(s.ok());
    j["max_ballot"] = mb.ToJson();

    jj["Proposer"] = j;
    return jj;
}

std::string
Proposer::ToString() const {
    return ToJson().dump();
}

std::string
Proposer::ToStringPretty() const {
    return ToJson().dump(4, ' ');
}

jsonxx::json64
Proposer::ToJsonTiny() const {
    jsonxx::json64 j, jj;
    Ballot mb;
    Status s = MaxBallot(mb);
    assert(s.ok());
    j["1.max_bal"] = mb.ToJsonTiny();
    j["2.proposing"] = proposing_;
    j["3.propose_value"] = propose_value_;
    j["4.prepare_mgr"] = prepare_manager_.ToJsonTiny();;
    j["5.accept_mgr"] = accept_manager_.ToJsonTiny();;
    jj["Proposer"] = j;
    return jj;
}

std::string
Proposer::ToStringTiny() const {
    return ToJsonTiny().dump();
}

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

Proposer::Proposer()
    :proposing_(false),
     prepare_manager_(Config::GetInstance().Quorum()),
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
        s = PersistMaxBallot(current_ballot_);
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
    s = PersistMaxBallot(current_ballot_);
    assert(s.ok());
}

void
Proposer::NextBallot(Ballot b) {
    Status s;
    s = MaxBallot(current_ballot_);
    assert(s.ok());

    if (b > current_ballot_) {
        current_ballot_.set_proposal_id(b.proposal_id()+1);
    } else {
        current_ballot_++;
    }

    s = PersistMaxBallot(current_ballot_);
    assert(s.ok());
}

Status
Proposer::Propose(std::string value, void *flag) {
    NextBallot();
    prepare_manager_.Reset(current_ballot_);
    accept_manager_.Reset(current_ballot_);
    propose_value_ = value;

    auto s = PrepareAll(flag);
    assert(s.ok());
    return Status::OK();
}

Status
Proposer::PrepareAll(void *flag) {
    Status s;
    vpaxos_rpc::Prepare request;

    Ballot2Pb(current_ballot_, *request.mutable_ballot());
    Ballot2Pb(current_ballot_, *request.mutable_trace_ballot());
    request.set_address(Config::GetInstance().MyAddress()->ToString());
    request.set_async_flag(reinterpret_cast<uint64_t>(flag));

    for (auto &hp : Config::GetInstance().address()) {
        s = Prepare(request, hp->ToString());
        assert(s.ok());
    }

    return Status::OK();
}

Status
Proposer::AcceptAll(void *flag) {
    Status s;
    vpaxos_rpc::Accept request;
    Ballot2Pb(current_ballot_, *request.mutable_ballot());
    Ballot2Pb(current_ballot_, *request.mutable_trace_ballot());
    request.set_address(Config::GetInstance().MyAddress()->ToString());
    request.set_async_flag(reinterpret_cast<uint64_t>(flag));

    if (prepare_manager_.HasAcceptedValue()) {
        request.set_value(prepare_manager_.AcceptedValue());
    } else {
        request.set_value(propose_value_);
    }

    for (auto &hp : Config::GetInstance().address()) {
        s = Accept(request, hp->ToString());
        assert(s.ok());
    }
    return Status::OK();
}

void
Proposer::OnPropose(const vpaxos_rpc::Propose &request, void *async_flag) {
    TraceOnPropose(request);

    if (Config::GetInstance().learner_optimized()) {
        if (Node::GetInstance().learner()->Chosen()) {
            std::string chosen_value;
            Status s = Node::GetInstance().learner()->ChosenValue(chosen_value);
            assert(s.ok());

            vpaxos_rpc::ProposeReply reply;
            reply.set_code(0);
            reply.set_msg("learner-optimized value chosen");
            reply.set_chosen_value(chosen_value);
            Env::GetInstance().AsyncProposeReply(reply, async_flag);
        }
    }

    if (proposing_) {
        vpaxos_rpc::ProposeReply reply;
        reply.set_code(2);
        std::string err_msg = "proposing value: ";
        err_msg.append(propose_value_);
        reply.set_msg(err_msg);
        Env::GetInstance().AsyncProposeReply(reply, async_flag);
    }
    proposing_ = true;
    auto s = Propose(request.value(), async_flag);
    assert(s.ok());
}

Status
Proposer::Prepare(const vpaxos_rpc::Prepare &request, const std::string &address) {
    Node::GetInstance().Sleep();
    TracePrepare(request, address);
    auto s = Env::GetInstance().AsyncPrepare(
                 request,
                 address,
                 std::bind(&Proposer::OnPrepareReply, this, std::placeholders::_1)
             );
    return s;
}

Status
Proposer::OnPrepareReply(const vpaxos_rpc::PrepareReply &reply) {
    TraceOnPrepareReply(reply);
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
                s = AcceptAll(reinterpret_cast<void*>(reply.async_flag()));
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


//        LOG(INFO) << "debug NextBallot call begin: " << "input:" << receive_ballot.ToString() << " current_ballot_:" << current_ballot_.ToString();

        NextBallot(receive_ballot);

//        LOG(INFO) << "debug NextBallot call end: " << "input:" << receive_ballot.ToString() << " current_ballot_:" << current_ballot_.ToString();


        LOG(INFO) << "after rejected, next_ballot: [" << current_ballot_.ToString() << "]";
        prepare_manager_.Reset(current_ballot_);
        accept_manager_.Reset(current_ballot_);
        auto s = PrepareAll(reinterpret_cast<void*>(reply.async_flag()));
        assert(s.ok());
    }

    return Status::OK();
}

Status
Proposer::Accept(const vpaxos_rpc::Accept &request, const std::string &address) {
    Node::GetInstance().Sleep();
    TraceAccept(request, address);
    auto s = Env::GetInstance().AsyncAccept(
                 request,
                 address,
                 std::bind(&Proposer::OnAcceptReply, this, std::placeholders::_1)
             );
    return s;
}

Status
Proposer::OnAcceptReply(const vpaxos_rpc::AcceptReply &reply) {
    TraceOnAcceptReply(reply);
    Status s;
    Ballot receive_ballot;
    Pb2Ballot(reply.accepted_ballot(), receive_ballot);


    if (current_ballot_ > receive_ballot) {
        LOG(INFO) << "old ballot: " << "current_ballot_:" << current_ballot_.ToString()
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

            TraceChosen(accept_manager_.AcceptedValue());


            // reply to client
            vpaxos_rpc::ProposeReply propose_reply;
            std::string err_msg;
            if (propose_value_ == accept_manager_.AcceptedValue()) {
                propose_reply.set_code(0);
                err_msg = "value chosen: ";
            } else {
                propose_reply.set_code(1);
                err_msg = "another value chosen: ";
            }
            err_msg.append(accept_manager_.AcceptedValue());
            propose_reply.set_msg(err_msg);
            propose_reply.set_chosen_value(accept_manager_.AcceptedValue());
            uint64_t flag = reply.async_flag();
            Env::GetInstance().AsyncProposeReply(propose_reply, reinterpret_cast<void*>(flag));

            // learn
            if (!accept_manager_.learn()) {
                Node::GetInstance().learner()->LearnAll(accept_manager_.AcceptedValue());
                accept_manager_.set_learn();
            } else {
                LOG(INFO) << "already broadcast learn for " << current_ballot_.ToString();
            }

            proposing_ = false;
        }
    } else {
        LOG(INFO) << "accept is rejected, prepare again. "
                  << " current_ballot_:[" << current_ballot_.ToString() << "]"
                  << " receive_ballot:[" << receive_ballot.ToString() << "]";
        NextBallot(receive_ballot);
        LOG(INFO) << "after rejected, next_ballot: [" << current_ballot_.ToString() << "]";
        prepare_manager_.Reset(current_ballot_);
        accept_manager_.Reset(current_ballot_);
        auto s = PrepareAll(reinterpret_cast<void*>(reply.async_flag()));
        assert(s.ok());
    }

    return Status::OK();
}

Status
Proposer::MaxBallot(Ballot &ballot) const {
    auto s = Env::GetInstance().MaxBallot(ballot);
    return s;
}

Status
Proposer::PersistMaxBallot(const Ballot &ballot) {
    auto s = Env::GetInstance().PersistMaxBallot(ballot);
    return s;
}

//---------------------------------------------------------------------------------------------------
// for debug

void
Proposer::TraceOnPropose(const vpaxos_rpc::Propose &request) const {
    TraceOnProposeMini(request);
    TraceOnProposeTiny(request);
    TraceOnProposeVerbose(request);
}

void
Proposer::TraceOnProposeMini(const vpaxos_rpc::Propose &request) const {
    std::string s;
    s.append("mini trace proposer: ").append(Node::GetInstance().id().ToStringMini());
    s.append("    RecvFrom-x.x.x.x-").append(::vpaxos::ToStringMini(request));
    LOG(INFO) << s;
}

void
Proposer::TraceOnProposeTiny(const vpaxos_rpc::Propose &request) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace proposer: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    RecvFrom-x.x.x.x-").append(::vpaxos::ToStringTiny(request));
    LOG(INFO) << s;
}

void
Proposer::TraceOnProposeVerbose(const vpaxos_rpc::Propose &request) const {
    std::string s;
    std::string state = ToStringPretty();
    s.append("node:").append(Node::GetInstance().id().ToString()).append("\n");
    s.append("verbose trace proposer:\n\n");
    s.append(state).append("\n\n");
    s.append("receive from : x.x.x.x").append(":\n");
    s.append(::vpaxos::ToStringPretty(request)).append("\n\n");
    LOG(INFO) << s;
}

void
Proposer::TracePrepare(const vpaxos_rpc::Prepare &request, const std::string &address) const {
    TracePrepareMini(request, address);
    TracePrepareTiny(request, address);
    TracePrepareVerbose(request, address);
}

void
Proposer::TracePrepareMini(const vpaxos_rpc::Prepare &request, const std::string &address) const {
    std::string s;
    s.append("mini trace proposer: ").append(Node::GetInstance().id().ToStringMini());
    s.append("    Send To -").append(address).append("-").append(::vpaxos::ToStringMini(request));
    LOG(INFO) << s;
}

void
Proposer::TracePrepareTiny(const vpaxos_rpc::Prepare &request, const std::string &address) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace proposer: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    SendTo-").append(address).append("-").append(::vpaxos::ToStringTiny(request));
    LOG(INFO) << s;
}

void
Proposer::TracePrepareVerbose(const vpaxos_rpc::Prepare &request, const std::string &address) const {
    std::string s;
    std::string state = ToStringPretty();
    s.append("node:").append(Node::GetInstance().id().ToString()).append("\n");
    s.append("verbose trace proposer:\n\n");
    s.append(state).append("\n\n");
    s.append("send to : ").append(address).append(":\n");
    s.append(::vpaxos::ToStringPretty(request)).append("\n\n");
    LOG(INFO) << s;
}

void
Proposer::TraceOnPrepareReply(const vpaxos_rpc::PrepareReply &reply) const {
    TraceOnPrepareReplyMini(reply);
    TraceOnPrepareReplyTiny(reply);
    TraceOnPrepareReplyVerbose(reply);
}

void
Proposer::TraceOnPrepareReplyMini(const vpaxos_rpc::PrepareReply &reply) const {
    std::string s;
    s.append("mini trace proposer: ").append(Node::GetInstance().id().ToStringMini());
    s.append("    RecvFrom-").append(reply.address()).append("-").append(::vpaxos::ToStringMini(reply));
    LOG(INFO) << s;
}

void
Proposer::TraceOnPrepareReplyTiny(const vpaxos_rpc::PrepareReply &reply) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace proposer: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    RecvFrom-").append(reply.address()).append("-").append(::vpaxos::ToStringTiny(reply));
    LOG(INFO) << s;
}

void
Proposer::TraceOnPrepareReplyVerbose(const vpaxos_rpc::PrepareReply &reply) const {
    std::string s;
    std::string state = ToStringPretty();
    s.append("node:").append(Node::GetInstance().id().ToString()).append("\n");
    s.append("verbose trace proposer:\n\n");
    s.append(state).append("\n\n");
    s.append("recv from : ").append(reply.address()).append(":\n");
    s.append(::vpaxos::ToStringPretty(reply)).append("\n\n");
    LOG(INFO) << s;
}

void
Proposer::TraceAccept(const vpaxos_rpc::Accept &request, const std::string &address) const {
    TraceAcceptMini(request, address);
    TraceAcceptTiny(request, address);
    TraceAcceptVerbose(request, address);
}

void
Proposer::TraceAcceptMini(const vpaxos_rpc::Accept &request, const std::string &address) const {
    std::string s;
    s.append("mini trace proposer: ").append(Node::GetInstance().id().ToStringMini());
    s.append("    Send To -").append(address).append("-").append(::vpaxos::ToStringMini(request));
    LOG(INFO) << s;
}

void
Proposer::TraceAcceptTiny(const vpaxos_rpc::Accept &request, const std::string &address) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace proposer: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    SendTo-").append(address).append("-").append(::vpaxos::ToStringTiny(request));
    LOG(INFO) << s;
}

void
Proposer::TraceAcceptVerbose(const vpaxos_rpc::Accept &request, const std::string &address) const {
    std::string s;
    std::string state = ToStringPretty();
    s.append("node:").append(Node::GetInstance().id().ToString()).append("\n");
    s.append("verbose trace proposer:\n\n");
    s.append(state).append("\n\n");
    s.append("send to : ").append(address).append(":\n");
    s.append(::vpaxos::ToStringPretty(request)).append("\n\n");
    LOG(INFO) << s;
}

void
Proposer::TraceOnAcceptReply(const vpaxos_rpc::AcceptReply &reply) const {
    TraceOnAcceptReplyMini(reply);
    TraceOnAcceptReplyTiny(reply);
    TraceOnAcceptReplyVerbose(reply);
}

void
Proposer::TraceOnAcceptReplyMini(const vpaxos_rpc::AcceptReply &reply) const {
    std::string s;
    s.append("mini trace proposer: ").append(Node::GetInstance().id().ToStringMini());
    s.append("    RecvFrom-").append(reply.address()).append("-").append(::vpaxos::ToStringMini(reply));
    LOG(INFO) << s;
}

void
Proposer::TraceOnAcceptReplyTiny(const vpaxos_rpc::AcceptReply &reply) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace proposer: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    RecvFrom-").append(reply.address()).append("-").append(::vpaxos::ToStringTiny(reply));
    LOG(INFO) << s;
}

void
Proposer::TraceOnAcceptReplyVerbose(const vpaxos_rpc::AcceptReply &reply) const {
    std::string s;
    std::string state = ToStringPretty();
    s.append("node:").append(Node::GetInstance().id().ToString()).append("\n");
    s.append("verbose trace proposer:\n\n");
    s.append(state).append("\n\n");
    s.append("recv from : ").append(reply.address()).append(":\n");
    s.append(::vpaxos::ToStringPretty(reply)).append("\n\n");
    LOG(INFO) << s;
}

void
Proposer::TraceChosen(const std::string &value) const {
    TraceChosenMini(value);
    TraceChosenTiny(value);
    TraceChosenVerbose(value);
}

void
Proposer::TraceChosenMini(const std::string &value) const {
    std::string s;
    s.append("mini trace proposer: ").append(Node::GetInstance().id().ToStringMini());
    s.append("    ValueChosen:").append(value);
    LOG(INFO) << s;
}

void
Proposer::TraceChosenTiny(const std::string &value) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace proposer: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    ValueChosen:").append(value);
    LOG(INFO) << s;
}

void
Proposer::TraceChosenVerbose(const std::string &value) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("verbose trace proposer: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    ValueChosen:").append(value);
    LOG(INFO) << s;
}

} // namespace vpaxos
