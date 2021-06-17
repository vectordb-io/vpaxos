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
    Status PersistChosenValue(const std::string &chosen_value);

    void DebugLogRecv(std::string header, const google::protobuf::Message *m);
    void DebugLogSend(std::string header, std::string address, const google::protobuf::Message *m);

  private:
};

} // namespace vpaxos

#endif
