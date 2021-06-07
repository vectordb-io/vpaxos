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
        s = StorePromisedBallot(Ballot());
        assert(s.ok());
    }

    s = AcceptedBallot(ballot);
    if (s.IsNotFound()) {
        s = StoreAcceptedBallot(Ballot());
        assert(s.ok());
    }

    return Status::OK();
}

void
Acceptor::OnPrepare(const vpaxos_rpc::Prepare &request, vpaxos_rpc::PrepareReply &reply) {
    DebugLog(false, __FUNCTION__, request.address(), &request);
    Node::GetInstance().Sleep(100, 200);

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

    LOG(INFO) << "debug OnPrepare " << "promised_ballot:" << promised_ballot.ToString() <<" receive_ballot:" << receive_ballot.ToString();

    if (promised_ballot.IsNull() || receive_ballot > promised_ballot) {


        LOG(INFO) << "debug agree";


        s = StorePromisedBallot(receive_ballot);
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



        LOG(INFO) << "debug reject, return promised_ballot:" << promised_ballot.ToString();
    }

    Ballot2Pb(receive_ballot, *reply.mutable_trace_ballot());
    reply.set_address(Config::GetInstance().MyAddress()->ToString());
    reply.set_async_flag(request.async_flag());

    DebugLog(true, __FUNCTION__, request.address(), &reply);
}

void
Acceptor::OnAccept(const vpaxos_rpc::Accept &request, vpaxos_rpc::AcceptReply &reply) {
    DebugLog(false, __FUNCTION__, request.address(), &request);
    Node::GetInstance().Sleep(100, 200);

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

        s = StorePromisedBallot(receive_ballot);
        assert(s.ok());
        s = StoreAcceptedBallot(receive_ballot);
        assert(s.ok());
        s = StoreAcceptedValue(request.value());
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

    DebugLog(true, __FUNCTION__, request.address(), &reply);
}

Status
Acceptor::PromisedBallot(Ballot &ballot) const {
    auto s = Env::GetInstance().PromisedBallot(ballot);
    return s;
}

Status
Acceptor::StorePromisedBallot(const Ballot &ballot) {
    auto s = Env::GetInstance().StorePromisedBallot(ballot);
    return s;
}

Status
Acceptor::AcceptedBallot(Ballot &ballot) const {
    auto s = Env::GetInstance().AcceptedBallot(ballot);
    return s;
}

Status
Acceptor::StoreAcceptedBallot(const Ballot &ballot) {
    auto s = Env::GetInstance().StoreAcceptedBallot(ballot);
    return s;
}

Status
Acceptor::AcceptedValue(std::string &accepted_value) const {
    auto s = Env::GetInstance().AcceptedValue(accepted_value);
    return s;
}

Status
Acceptor::StoreAcceptedValue(const std::string &accepted_value) {
    auto s = Env::GetInstance().StoreAcceptedValue(accepted_value);
    return s;
}

bool
Acceptor::HasAcceptedValue() const {
    auto s = Env::GetInstance().HasAcceptedValue();
    return s;
}

void
Acceptor::DebugLog(bool is_send, std::string header, std::string address, const google::protobuf::Message *m) {
    std::string str;
    char buf[256];

    snprintf(buf, sizeof(buf), " <tid:%ld>", gettid());

    str.append("\n");
    if (is_send) {
        str.append("|--------------send message---------------|").append(buf);
    } else {
        str.append("|==============receive message============|").append(buf);
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

    Status s;

    str.append("store_promised_ballot:");
    Ballot store_promised_ballot;
    s = PromisedBallot(store_promised_ballot);
    assert(s.ok());
    if (store_promised_ballot.IsNull()) {
        str.append("null");
    } else {
        str.append(store_promised_ballot.ToString());
    }
    str.append("\n");

    str.append("store_accepted_ballot:");
    Ballot store_accepted_ballot;
    s = AcceptedBallot(store_accepted_ballot);
    assert(s.ok());
    if (store_accepted_ballot.IsNull()) {
        str.append("null");
    } else {
        str.append(store_accepted_ballot.ToString());
    }
    str.append("\n");

    str.append("store_accepted_value:");
    if (store_accepted_ballot.IsNull()) {
        str.append("null");
    } else {
        std::string store_accepted_value;
        s = AcceptedValue(store_accepted_value);
        assert(s.ok());
        str.append(store_accepted_value);
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
