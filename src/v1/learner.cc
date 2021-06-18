#include "env.h"
#include "node.h"
#include "coding.h"
#include "learner.h"

namespace vpaxos {

Learner::Learner() {
}

Status
Learner::Init() {
    Env::GetInstance().grpc_server()->set_on_learn_cb(
        std::bind(&Learner::OnLearn, this, std::placeholders::_1, std::placeholders::_2)
    );
    return Status::OK();
}

void
Learner::OnLearn(const vpaxos_rpc::Learn &request, vpaxos_rpc::LearnReply &reply) {
    TraceOnLearn(request);
    auto s = PersistChosenValue(request.chosen_value());
    if (s.ok()) {
        reply.set_msg("ok");
    } else {
        reply.set_msg("error");
    }
    TraceLearnReply(reply, request.address());
}

Status
Learner::Learn(const vpaxos_rpc::Learn &request, const std::string &address) {
    TraceLearn(request, address);
    auto s = Env::GetInstance().AsyncLearn(
                 request,
                 address,
                 std::bind(&Learner::OnLearnReply, this, std::placeholders::_1)
             );
    return s;
}

Status
Learner::OnLearnReply(const vpaxos_rpc::LearnReply &reply) {
    TraceOnLearnReply(reply);
    return Status::OK();
}

Status
Learner::LearnAll(std::string chosen_value) {
    for (auto &hp : Config::GetInstance().address_) {
        vpaxos_rpc::Learn request;
        request.set_address(Config::GetInstance().MyAddress()->ToString());
        request.set_chosen_value(chosen_value);
        auto s = Learn(request, hp->ToString());
        assert(s.ok());
    }
    return Status::OK();
}

bool
Learner::Chosen() const {
    std::string chosen_value;
    auto s = Env::GetInstance().ChosenValue(chosen_value);
    return !s.IsNotFound();
}

Status
Learner::ChosenValue(std::string &chosen_value) const {
    auto s = Env::GetInstance().ChosenValue(chosen_value);
    return s;
}

Status
Learner::PersistChosenValue(const std::string &value) {
    auto s = Env::GetInstance().PersistChosenValue(value);
    return s;
}

//------------------------------------------------------------------------------------
//for debug

jsonxx::json64
Learner::ToJson() const {
    std::string chosen_value;
    if (Chosen()) {
        auto s = ChosenValue(chosen_value);
        assert(s.ok());
    }
    jsonxx::json64 j;
    j["Learner"]["chosen_value"] = chosen_value;
    return j;
}

std::string
Learner::ToString() const {
    return ToJson().dump();
}

std::string
Learner::ToStringPretty() const {
    return ToJson().dump(4, ' ');
}

jsonxx::json64
Learner::ToJsonTiny() const {
    std::string chosen_value;
    if (Chosen()) {
        auto s = ChosenValue(chosen_value);
        assert(s.ok());
    }
    jsonxx::json64 j;
    j["Learner"]["chosen_value"] = chosen_value;
    return j;
}

std::string
Learner::ToStringTiny() const {
    return ToJsonTiny().dump();
}

void
Learner::TraceOnLearn(const vpaxos_rpc::Learn &request) const {
    TraceOnLearnMini(request);
    TraceOnLearnTiny(request);
    TraceOnLearnVerbose(request);
}

void
Learner::TraceOnLearnMini(const vpaxos_rpc::Learn &request) const {
    std::string s;
    s.append(" mini trace learner : ").append(Node::GetInstance().id().ToStringMini());
    s.append("    RecvFrom-").append(request.address()).append("-").append(::vpaxos::ToStringMini(request));
    LOG(INFO) << s;
}

void
Learner::TraceOnLearnTiny(const vpaxos_rpc::Learn &request) const {

}

void
Learner::TraceOnLearnVerbose(const vpaxos_rpc::Learn &request) const {

}

void
Learner::TraceLearnReply(const vpaxos_rpc::LearnReply &reply, const std::string &address) const {
    TraceLearnReplyMini(reply, address);
    TraceLearnReplyTiny(reply, address);
    TraceLearnReplyVerbose(reply, address);
}

void
Learner::TraceLearnReplyMini(const vpaxos_rpc::LearnReply &reply, const std::string &address) const {
    std::string s;
    s.append(" mini trace learner : ").append(Node::GetInstance().id().ToStringMini());
    s.append("    Send To -").append(address).append("-").append(::vpaxos::ToStringMini(reply));
    LOG(INFO) << s;
}

void
Learner::TraceLearnReplyTiny(const vpaxos_rpc::LearnReply &reply, const std::string &address) const {

}

void
Learner::TraceLearnReplyVerbose(const vpaxos_rpc::LearnReply &reply, const std::string &address) const {

}

void
Learner::TraceLearn(const vpaxos_rpc::Learn &request, const std::string &address) const {
    TraceLearnMini(request, address);
    TraceLearnTiny(request, address);
    TraceLearnVerbose(request, address);
}

void
Learner::TraceLearnMini(const vpaxos_rpc::Learn &request, const std::string &address) const {
    std::string s;
    s.append(" mini trace learner : ").append(Node::GetInstance().id().ToStringMini());
    s.append("    Send To -").append(address).append("-").append(::vpaxos::ToStringMini(request));
    LOG(INFO) << s;
}

void
Learner::TraceLearnTiny(const vpaxos_rpc::Learn &request, const std::string &address) const {

}

void
Learner::TraceLearnVerbose(const vpaxos_rpc::Learn &request, const std::string &address) const {

}

void
Learner::TraceOnLearnReply(const vpaxos_rpc::LearnReply &reply) const {
    TraceOnLearnReplyMini(reply);
    TraceOnLearnReplyTiny(reply);
    TraceOnLearnReplyVerbose(reply);
}

void
Learner::TraceOnLearnReplyMini(const vpaxos_rpc::LearnReply &reply) const {
    std::string s;
    s.append(" mini trace learner : ").append(Node::GetInstance().id().ToStringMini());
    s.append("    RecvFrom-").append(reply.address()).append("-").append(::vpaxos::ToStringMini(reply));
    LOG(INFO) << s;
}

void
Learner::TraceOnLearnReplyTiny(const vpaxos_rpc::LearnReply &reply) const {

}

void
Learner::TraceOnLearnReplyVerbose(const vpaxos_rpc::LearnReply &reply) const {

}



} // namespace vpaxos
