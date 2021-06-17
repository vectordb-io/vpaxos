#include "env.h"
#include "node.h"
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
    LOG(INFO) << "value learned: [" << request.chosen_value() << "]";
    auto s = PersistChosenValue(request.chosen_value());
    if (s.ok()) {
        reply.set_msg("ok");
    } else {
        reply.set_msg("error");
    }
}

Status
Learner::Learn(const vpaxos_rpc::Learn &request, const std::string &address) {
    auto s = Env::GetInstance().AsyncLearn(
                 request,
                 address,
                 std::bind(&Learner::OnLearnReply, this, std::placeholders::_1)
             );
    return s;
}

Status
Learner::OnLearnReply(const vpaxos_rpc::LearnReply &reply) {
    LOG(INFO) << "receive from " << reply.address() << ": " << reply.msg();
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

void
Learner::DebugLogRecv(std::string header, const google::protobuf::Message *m) {
    LOG(INFO) << "";
    LOG(INFO) << "|-----------receive message---------------|";
    LOG(INFO) << header;
    LOG(INFO) << "my address: " << Config::GetInstance().MyAddress()->ToString();
    LOG(INFO) << "my node_id: " << Node::GetInstance().Id();

    Status s;
    std::string store_chosen_value;
    s = ChosenValue(store_chosen_value);
    if (s.ok()) {
        LOG(INFO) << "store_chosen_value: " << "[" << store_chosen_value << "]";
    } else if (s.IsNotFound()) {
        LOG(INFO) << "store_chosen_value: null";
    } else {
        assert(0);
    }

    std::string msg = "msg:\n<<\n" + m->DebugString() + ">>\n";
    LOG(INFO) << msg;
    LOG(INFO) << "|-----------------------------------------|";
    LOG(INFO) << "";
}

void
Learner::DebugLogSend(std::string header, std::string address, const google::protobuf::Message *m) {
    LOG(INFO) << "";
    LOG(INFO) << "|--------------send message---------------|";
    LOG(INFO) << header;
    LOG(INFO) << "to address: " << address;
    LOG(INFO) << "my address: " << Config::GetInstance().MyAddress()->ToString();
    LOG(INFO) << "my node_id: " << Node::GetInstance().Id();

    Status s;
    std::string store_chosen_value;
    s = ChosenValue(store_chosen_value);
    if (s.ok()) {
        LOG(INFO) << "store_chosen_value: " << "[" << store_chosen_value << "]";
    } else if (s.IsNotFound()) {
        LOG(INFO) << "store_chosen_value: null";
    } else {
        assert(0);
    }

    std::string msg = "msg:\n<<\n" + m->DebugString() + ">>\n";
    LOG(INFO) << msg;
    LOG(INFO) << "|-----------------------------------------|";
    LOG(INFO) << "";
}

} // namespace vpaxos
