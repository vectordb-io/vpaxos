#ifndef __VPAXOS_LEARNER_H__
#define __VPAXOS_LEARNER_H__

#include "status.h"

namespace vpaxos {

class Learner {
  public:
    Learner();
    ~Learner() = default;
    Learner(const Learner&) = delete;
    Learner& operator=(const Learner&) = delete;
    Status Init();

    void OnLearn(const vpaxos_rpc::Learn &request, vpaxos_rpc::LearnReply &reply);
    Status Learn(const vpaxos_rpc::Learn &request, const std::string &address);
    Status OnLearnReply(const vpaxos_rpc::LearnReply &reply);
    Status LearnAll(std::string chosen_value);

    bool Chosen() const;
    Status ChosenValue(std::string &chosen_value) const;

  private:
    Status PersistChosenValue(const std::string &chosen_value);

  private: // for debug
    jsonxx::json64 ToJson() const;
    std::string ToString() const;
    std::string ToStringPretty() const;
    jsonxx::json64 ToJsonTiny() const;
    std::string ToStringTiny() const;

    void TraceOnLearn(const vpaxos_rpc::Learn &request) const;
    void TraceOnLearnMini(const vpaxos_rpc::Learn &request) const;
    void TraceOnLearnTiny(const vpaxos_rpc::Learn &request) const;
    void TraceOnLearnVerbose(const vpaxos_rpc::Learn &request) const;

    void TraceLearnReply(const vpaxos_rpc::LearnReply &reply, const std::string &address) const;
    void TraceLearnReplyMini(const vpaxos_rpc::LearnReply &reply, const std::string &address) const;
    void TraceLearnReplyTiny(const vpaxos_rpc::LearnReply &reply, const std::string &address) const;
    void TraceLearnReplyVerbose(const vpaxos_rpc::LearnReply &reply, const std::string &address) const;

    void TraceLearn(const vpaxos_rpc::Learn &request, const std::string &address) const;
    void TraceLearnMini(const vpaxos_rpc::Learn &request, const std::string &address) const;
    void TraceLearnTiny(const vpaxos_rpc::Learn &request, const std::string &address) const;
    void TraceLearnVerbose(const vpaxos_rpc::Learn &request, const std::string &address) const;

    void TraceOnLearnReply(const vpaxos_rpc::LearnReply &reply) const;
    void TraceOnLearnReplyMini(const vpaxos_rpc::LearnReply &reply) const;
    void TraceOnLearnReplyTiny(const vpaxos_rpc::LearnReply &reply) const;
    void TraceOnLearnReplyVerbose(const vpaxos_rpc::LearnReply &reply) const;
};

} // namespace vpaxos

#endif
