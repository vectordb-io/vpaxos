#ifndef __VPAXOS_STORAGE_H__
#define __VPAXOS_STORAGE_H__

#include <cassert>
#include <string>
#include <leveldb/db.h>
#include "ballot.h"
#include "status.h"

namespace vpaxos {

class Storage {
  public:
// for proposer
#define Key_Proposer_MaxBallot std::string("Key_Proposer_MaxBallot")

// for acceptor
#define Key_Acceptor_PromisedBallot std::string("Key_Acceptor_PromisedBallot")
#define Key_Acceptor_AcceptedBallot std::string("Key_Acceptor_AcceptedBallot")
#define Key_Acceptor_AcceptedValue std::string("Key_Acceptor_AcceptedValue")

// for all
#define Key_Chosen_Value std::string("Key_Chosen_Value")

    Storage(const std::string &s);
    ~Storage();
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    Status Init();

    // only proposer call these methods
    Status MaxBallot(Ballot &ballot) const;
    Status PersistMaxBallot(const Ballot &ballot);

    // only acceptor call these methods
    Status PromisedBallot(Ballot &ballot) const;
    Status PersistPromisedBallot(const Ballot &ballot);
    bool HasPromisedBallot();

    Status AcceptedBallot(Ballot &ballot) const;
    Status PersistAcceptedBallot(const Ballot &ballot);
    bool HasAcceptedBallot();

    Status AcceptedValue(std::string &accepted_value) const;
    Status PersistAcceptedValue(const std::string &accepted_value);
    bool HasAcceptedValue() const;

    // proposer, acceptor, learner can call these methods
    Status ChosenValue(std::string &chosen_value) const;
    Status PersistChosenValue(const std::string &chosen_value);
    bool HasChosenValue() const;

  private:
    std::string path_;
    leveldb::DB* db_;
};

} // namespace vpaxos

#endif
