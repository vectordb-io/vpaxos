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
    Ballot ballot;

    s = PromisedBallot(ballot);
    if (s.IsNotFound()) {
        s = PersistPromisedBallot(Ballot());
        assert(s.ok());
    }

    s = AcceptedBallot(ballot);
    if (s.IsNotFound()) {
        s = PersistAcceptedBallot(Ballot());
        assert(s.ok());
    }

    return Status::OK();
}

void
Acceptor::OnPrepare(const vpaxos_rpc::Prepare &request, vpaxos_rpc::PrepareReply &reply) {
    //Node::GetInstance().Sleep(100, 200);
    TraceOnPrepareTiny(request);
    TraceOnPrepare(request);

    Status s;
    Ballot receive_ballot;
    Ballot promised_ballot, accepted_ballot;
    std::string accepted_value;

    Pb2Ballot(request.ballot(), receive_ballot);
    s = PromisedBallot(promised_ballot);
    assert(s.ok());

    if (receive_ballot == promised_ballot) {
        LOG(INFO) << " repeat ballot: " << "receive_ballot:" << receive_ballot.ToString()
                  << " promised_ballot:" << promised_ballot.ToString();
        assert(0);
    }

//    LOG(INFO) << "debug OnPrepare " << "promised_ballot:" << promised_ballot.ToString() <<" receive_ballot:" << receive_ballot.ToString();

    if (promised_ballot.IsNull() || receive_ballot > promised_ballot) {


//        LOG(INFO) << "debug agree";


        s = PersistPromisedBallot(receive_ballot);
        assert(s.ok());
        reply.set_prepared(true);
        Ballot2Pb(receive_ballot, *reply.mutable_promised_ballot());

        s = AcceptedBallot(accepted_ballot);
        assert(s.ok());
        if (!accepted_ballot.IsNull()) {
            reply.set_ever_accepted(true);
            Ballot2Pb(accepted_ballot, *reply.mutable_accepted_ballot());

            s = AcceptedValue(accepted_value);
            assert(s.ok());
            reply.set_accepted_value(accepted_value);

        } else {
            reply.set_ever_accepted(false);
        }

    } else {
        reply.set_prepared(false);
        Ballot2Pb(promised_ballot, *reply.mutable_promised_ballot());



//        LOG(INFO) << "debug reject, return promised_ballot:" << promised_ballot.ToString();
    }

    Ballot2Pb(receive_ballot, *reply.mutable_trace_ballot());
    reply.set_address(Config::GetInstance().MyAddress()->ToString());
    reply.set_async_flag(request.async_flag());


    TracePrepareReplyTiny(reply, request.address());
    TracePrepareReply(reply, request.address());
}

void
Acceptor::OnAccept(const vpaxos_rpc::Accept &request, vpaxos_rpc::AcceptReply &reply) {
    //Node::GetInstance().Sleep(100, 200);
    TraceOnAcceptTiny(request);
    TraceOnAccept(request);

    Status s;
    Ballot receive_ballot;
    Ballot promised_ballot, accepted_ballot;
    std::string accepted_value;

    Pb2Ballot(request.ballot(), receive_ballot);
    s = PromisedBallot(promised_ballot);
    assert(s.ok());

    if (promised_ballot.IsNull() || receive_ballot >= promised_ballot) {
        //----------------------------------------------------------
        {
            auto s = AcceptedBallot(accepted_ballot);
            assert(s.ok());
            if (!accepted_ballot.IsNull()) {
                assert(receive_ballot >= accepted_ballot);
            }
        }
        //----------------------------------------------------------

        s = PersistPromisedBallot(receive_ballot);
        assert(s.ok());
        s = PersistAcceptedBallot(receive_ballot);
        assert(s.ok());
        s = PersistAcceptedValue(request.value());
        assert(s.ok());

        reply.set_accepted(true);
        Ballot2Pb(receive_ballot, *reply.mutable_accepted_ballot());
        reply.set_accepted_value(request.value());

    } else {
        reply.set_accepted(false);

        // add this line ???
        Ballot2Pb(promised_ballot, *reply.mutable_accepted_ballot());
    }

    Ballot2Pb(receive_ballot, *reply.mutable_trace_ballot());
    reply.set_address(Config::GetInstance().MyAddress()->ToString());
    reply.set_async_flag(request.async_flag());

    TraceAcceptReplyTiny(reply, request.address());
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

void
Acceptor::TraceOnPrepare(const vpaxos_rpc::Prepare &request) const {
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
Acceptor::TraceOnPrepareTiny(const vpaxos_rpc::Prepare &request) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace acceptor: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    RecvFrom-").append(request.address()).append("-").append(::vpaxos::ToStringTiny(request));
    LOG(INFO) << s;
}

void
Acceptor::TracePrepareReply(const vpaxos_rpc::PrepareReply &reply, const std::string &address) const {
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
Acceptor::TracePrepareReplyTiny(const vpaxos_rpc::PrepareReply &reply, const std::string &address) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace acceptor: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    SendTo-").append(address).append("-").append(::vpaxos::ToStringTiny(reply));
    LOG(INFO) << s;
}

void
Acceptor::TraceOnAccept(const vpaxos_rpc::Accept &request) const {
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
Acceptor::TraceOnAcceptTiny(const vpaxos_rpc::Accept &request) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace acceptor: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    RecvFrom-").append(request.address()).append("-").append(::vpaxos::ToStringTiny(request));
    LOG(INFO) << s;
}

void
Acceptor::TraceAcceptReply(const vpaxos_rpc::AcceptReply &reply, const std::string &address) const {
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
Acceptor::TraceAcceptReplyTiny(const vpaxos_rpc::AcceptReply &reply, const std::string &address) const {
    std::string s;
    std::string state = "State-" + ToStringTiny();
    s.append("tiny trace acceptor: ").append(Node::GetInstance().id().ToStringTiny());
    s.append("    ").append(state);
    s.append("    SendTo-").append(address).append("-").append(::vpaxos::ToStringTiny(reply));
    LOG(INFO) << s;
}

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
