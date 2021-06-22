#include "env.h"
#include "node.h"
#include "coding.h"
#include "acceptor.h"

namespace vpaxos {

Acceptor::Acceptor() {
}

Status
Acceptor::Init() {
    Env::GetInstance().grpc_server()->set_on_prepare_cb(
        std::bind(&Acceptor::OnPrepare, this, std::placeholders::_1, std::placeholders::_2)
    );

    Env::GetInstance().grpc_server()->set_on_accept_cb(
        std::bind(&Acceptor::OnAccept, this, std::placeholders::_1, std::placeholders::_2)
    );

    Status s;
    s = PromisedBallot(promised_ballot_);
    if (s.IsNotFound()) {
        promised_ballot_.Clear();
        auto s1 = PersistPromisedBallot(promised_ballot_);
        assert(s1.ok());
    }

    s = AcceptedBallot(accepted_ballot_);
    if (s.IsNotFound()) {
        accepted_ballot_.Clear();
        auto s1 = PersistAcceptedBallot(accepted_ballot_);
        assert(s1.ok());
    }

    if (!accepted_ballot_.IsNull()) {
        s = AcceptedValue(accepted_value_);
        assert(s.ok());
    }

    return Status::OK();
}

void
Acceptor::OnPrepare(const vpaxos_rpc::Prepare &request, vpaxos_rpc::PrepareReply &reply) {
    TraceOnPrepare(request);

    Ballot receive_ballot;
    Pb2Ballot(request.ballot(), receive_ballot);

    if (promised_ballot_.IsNull() || receive_ballot > promised_ballot_) {
        promised_ballot_ = receive_ballot;
        auto s = PersistPromisedBallot(promised_ballot_);
        assert(s.ok());
    }

    // receive_ballot >  promised_ballot_ : reply agree
    // receive_ballot == promised_ballot_ : duplicated message, reply agree
    // receive_ballot <  promised_ballot_ : reply reject
    assert(!promised_ballot_.IsNull());

    Ballot2Pb(promised_ballot_, *reply.mutable_promised_ballot());
    Ballot2Pb(accepted_ballot_, *reply.mutable_accepted_ballot());
    if (!accepted_ballot_.IsNull()) {
        reply.set_accepted_value(accepted_value_);
    }

    reply.set_address(Config::GetInstance().MyAddress()->ToString());
    reply.set_async_flag(request.async_flag());
    TracePrepareReply(reply, request.address());
}

void
Acceptor::OnAccept(const vpaxos_rpc::Accept &request, vpaxos_rpc::AcceptReply &reply) {
    TraceOnAccept(request);

    Ballot receive_ballot;
    Pb2Ballot(request.ballot(), receive_ballot);

    // notice! here is ">="!
    if (promised_ballot_.IsNull() || receive_ballot >= promised_ballot_) {
        assert(receive_ballot >= accepted_ballot_);

        promised_ballot_ = receive_ballot;
        accepted_ballot_ = receive_ballot;
        accepted_value_ = request.value();

        auto s = PersistPromisedBallot(promised_ballot_);
        assert(s.ok());
        s = PersistAcceptedBallot(accepted_ballot_);
        assert(s.ok());
        s = PersistAcceptedValue(accepted_value_);
        assert(s.ok());

        reply.set_accepted_value(accepted_value_);
    }
    assert(!promised_ballot_.IsNull());
    Ballot2Pb(promised_ballot_, *reply.mutable_accepted_ballot());

    reply.set_address(Config::GetInstance().MyAddress()->ToString());
    reply.set_async_flag(request.async_flag());
    TraceAcceptReply(reply, request.address());
}

Status
Acceptor::PromisedBallot(Ballot &ballot) const {
    auto s = Env::GetInstance().PromisedBallot(ballot);
    return s;
}

Status
Acceptor::PersistPromisedBallot(const Ballot &ballot) {
    auto s = Env::GetInstance().PersistPromisedBallot(ballot);
    return s;
}

Status
Acceptor::AcceptedBallot(Ballot &ballot) const {
    auto s = Env::GetInstance().AcceptedBallot(ballot);
    return s;
}

Status
Acceptor::PersistAcceptedBallot(const Ballot &ballot) {
    auto s = Env::GetInstance().PersistAcceptedBallot(ballot);
    return s;
}

Status
Acceptor::AcceptedValue(std::string &accepted_value) const {
    auto s = Env::GetInstance().AcceptedValue(accepted_value);
    return s;
}

Status
Acceptor::PersistAcceptedValue(const std::string &accepted_value) {
    auto s = Env::GetInstance().PersistAcceptedValue(accepted_value);
    return s;
}

bool
Acceptor::HasAcceptedValue() const {
    auto s = Env::GetInstance().HasAcceptedValue();
    return s;
}

//-------------------------------------------------------------------------------------------
// for debug

void
Acceptor::TraceOnPrepare(const vpaxos_rpc::Prepare &request) const {
    TraceOnPrepareMini(request);
    TraceOnPrepareTiny(request);
    TraceOnPrepareVerbose(request);
}

void
Acceptor::TraceOnPrepareMini(const vpaxos_rpc::Prepare &request) const {
    std::string s;
    s.append("mini trace acceptor: ").append(Node::GetInstance().id().ToStringMini());
    s.append("    RecvFrom-").append(request.address()).append("-").append(::vpaxos::ToStringMini(request));
    LOG(INFO) << s;
}

void
Acceptor::TraceOnPrepareTiny(const vpaxos_rpc::Prepare &request) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace acceptor: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    RecvFrom-").append(request.address()).append("-").append(::vpaxos::ToStringTiny(request));
    LOG(INFO) << s;
}

void
Acceptor::TraceOnPrepareVerbose(const vpaxos_rpc::Prepare &request) const {
    std::string s;
    std::string state = ToStringPretty();
    s.append("node:").append(Node::GetInstance().id().ToString()).append("\n");
    s.append("verbose trace proposer:\n\n");
    s.append(state).append("\n\n");
    s.append("recv from : ").append(request.address()).append(":\n");
    s.append(::vpaxos::ToStringPretty(request)).append("\n\n");
    LOG(INFO) << s;
}

void
Acceptor::TracePrepareReply(const vpaxos_rpc::PrepareReply &reply, const std::string &address) const {
    TracePrepareReplyMini(reply, address);
    TracePrepareReplyTiny(reply, address);
    TracePrepareReplyVerbose(reply, address);
}

void
Acceptor::TracePrepareReplyMini(const vpaxos_rpc::PrepareReply &reply, const std::string &address) const {
    std::string s;
    s.append("mini trace acceptor: ").append(Node::GetInstance().id().ToStringMini());
    s.append("    Send To -").append(address).append("-").append(::vpaxos::ToStringMini(reply));
    LOG(INFO) << s;
}

void
Acceptor::TracePrepareReplyTiny(const vpaxos_rpc::PrepareReply &reply, const std::string &address) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace acceptor: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    SendTo-").append(address).append("-").append(::vpaxos::ToStringTiny(reply));
    LOG(INFO) << s;
}

void
Acceptor::TracePrepareReplyVerbose(const vpaxos_rpc::PrepareReply &reply, const std::string &address) const {
    std::string s;
    std::string state = ToStringPretty();

    s.append("node:").append(Node::GetInstance().id().ToString()).append("\n");
    s.append("verbose trace proposer:\n\n");

    s.append(state).append("\n\n");
    s.append("send to : ").append(address).append(":\n");
    s.append(::vpaxos::ToStringPretty(reply)).append("\n\n");
    LOG(INFO) << s;
}

void
Acceptor::TraceOnAccept(const vpaxos_rpc::Accept &request) const {
    TraceOnAcceptMini(request);
    TraceOnAcceptTiny(request);
    TraceOnAcceptVerbose(request);
}

void
Acceptor::TraceOnAcceptMini(const vpaxos_rpc::Accept &request) const {
    std::string s;
    s.append("mini trace acceptor: ").append(Node::GetInstance().id().ToStringMini());
    s.append("    RecvFrom-").append(request.address()).append("-").append(::vpaxos::ToStringMini(request));
    LOG(INFO) << s;
}

void
Acceptor::TraceOnAcceptTiny(const vpaxos_rpc::Accept &request) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace acceptor: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    RecvFrom-").append(request.address()).append("-").append(::vpaxos::ToStringTiny(request));
    LOG(INFO) << s;
}

void
Acceptor::TraceOnAcceptVerbose(const vpaxos_rpc::Accept &request) const {
    std::string s;
    std::string state = ToStringPretty();

    s.append("node:").append(Node::GetInstance().id().ToString()).append("\n");
    s.append("verbose trace proposer:\n\n");

    s.append(state).append("\n\n");
    s.append("recv from : ").append(request.address()).append(":\n");
    s.append(::vpaxos::ToStringPretty(request)).append("\n\n");
    LOG(INFO) << s;
}

void
Acceptor::TraceAcceptReply(const vpaxos_rpc::AcceptReply &reply, const std::string &address) const {
    TraceAcceptReplyMini(reply, address);
    TraceAcceptReplyTiny(reply, address);
    TraceAcceptReplyVerbose(reply, address);
}

void
Acceptor::TraceAcceptReplyMini(const vpaxos_rpc::AcceptReply &reply, const std::string &address) const {
    std::string s;
    s.append("mini trace acceptor: ").append(Node::GetInstance().id().ToStringMini());
    s.append("    Send To -").append(address).append("-").append(::vpaxos::ToStringMini(reply));
    LOG(INFO) << s;
}

void
Acceptor::TraceAcceptReplyTiny(const vpaxos_rpc::AcceptReply &reply, const std::string &address) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace acceptor: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    SendTo-").append(address).append("-").append(::vpaxos::ToStringTiny(reply));
    LOG(INFO) << s;
}

void
Acceptor::TraceAcceptReplyVerbose(const vpaxos_rpc::AcceptReply &reply, const std::string &address) const {
    std::string s;
    std::string state = ToStringPretty();

    s.append("node:").append(Node::GetInstance().id().ToString()).append("\n");
    s.append("verbose trace proposer:\n\n");

    s.append(state).append("\n\n");
    s.append("send to : ").append(address).append(":\n");
    s.append(::vpaxos::ToStringPretty(reply)).append("\n\n");
    LOG(INFO) << s;
}

//------------------------------------------------------------------------------------------------------------------

jsonxx::json64
Acceptor::ToJson() const {
    Status s;
    Ballot promised_ballot;
    s = PromisedBallot(promised_ballot);
    assert(s.ok());

    Ballot accepted_ballot;
    s = AcceptedBallot(accepted_ballot);
    assert(s.ok());

    std::string accepted_value;
    if (HasAcceptedValue()) {
        s = AcceptedValue(accepted_value);
        assert(s.ok());
    }

    jsonxx::json64 j, jj;
    j["promised_ballot"] = promised_ballot.ToJson();
    j["accepted_ballot"] = accepted_ballot.ToJson();
    j["accepted_value"] = accepted_value;
    jj["Acceptor"] = j;
    return jj;
}

std::string
Acceptor::ToString() const {
    return ToJson().dump();
}

std::string
Acceptor::ToStringPretty() const {
    return ToJson().dump(4, ' ');
}

jsonxx::json64
Acceptor::ToJsonTiny() const {
    Status s;
    Ballot promised_ballot;
    s = PromisedBallot(promised_ballot);
    assert(s.ok());

    Ballot accepted_ballot;
    s = AcceptedBallot(accepted_ballot);
    assert(s.ok());

    std::string accepted_value;
    if (HasAcceptedValue()) {
        s = AcceptedValue(accepted_value);
        assert(s.ok());
    }

    jsonxx::json64 j, jj;
    j["promised_bal"] = promised_ballot.ToJsonTiny();
    j["accepted_bal"] = accepted_ballot.ToJsonTiny();
    j["accepted_value"] = accepted_value;
    jj["Acceptor"] = j;
    return jj;
}

std::string
Acceptor::ToStringTiny() const {
    return ToJsonTiny().dump();
}

} // namespace vpaxos
