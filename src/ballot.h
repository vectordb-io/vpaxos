#ifndef __VPAXOS_BALLOT_H__
#define __VPAXOS_BALLOT_H__

#include <stdint.h>
#include "jsonxx/json.hpp"
#include "nodeid.h"

namespace vpaxos {

class Ballot {
  public:
    Ballot()
        :proposal_id_(0),
         node_id_(0) {
    }

    Ballot(uint64_t node_id)
        :proposal_id_(0),
         node_id_(node_id) {
    }

    Ballot(uint64_t proposal_id, uint64_t node_id)
        :proposal_id_(proposal_id),
         node_id_(node_id) {
    }

    ~Ballot() = default;
    Ballot(const Ballot&) = default;
    Ballot& operator=(const Ballot&) = default;

    void Init(uint64_t proposal_id, uint64_t node_id) {
        proposal_id_ = proposal_id;
        node_id_ = node_id;
    }

    void Clear() {
        proposal_id_ = 0;
        node_id_ = 0;
    }

    bool IsNull() const {
        return (proposal_id_ == 0 && node_id_ == 0);
    }

    // ballot++, not ++ballot
    void operator++(int arg) {
        proposal_id_++;
    }

    bool operator!=(const Ballot &rhs) const {
        return (proposal_id_ != rhs.proposal_id_
                || node_id_ != rhs.node_id_);
    }

    bool operator==(const Ballot &rhs) const {
        return (proposal_id_ == rhs.proposal_id_
                && node_id_ == rhs.node_id_);
    }

    bool operator>(const Ballot &rhs) const {
        if (proposal_id_ == rhs.proposal_id_) {
            return node_id_ > rhs.node_id_;
        } else {
            return proposal_id_ > rhs.proposal_id_;
        }
    }

    bool operator>=(const Ballot &rhs) const {
        if (proposal_id_ == rhs.proposal_id_) {
            return node_id_ >= rhs.node_id_;
        } else {
            return proposal_id_ >= rhs.proposal_id_;
        }
    }

    void set_proposal_id(uint64_t proposal_id) {
        proposal_id_ = proposal_id;
    }

    uint64_t proposal_id() const {
        return proposal_id_;
    }

    void set_node_id(uint64_t node_id) {
        node_id_ = node_id;
    }

    uint64_t node_id() const {
        return node_id_;
    }

    jsonxx::json64 ToJson() const {
        jsonxx::json64 j, jj;
        j["proposal_id"] = proposal_id_;
        NodeId n(node_id_);
        j["node_id"] = n.ToJson();
        jj["Ballot"] = j;
        return jj;
    }

    std::string ToString() const {
        return ToJson().dump();
    }

    std::string ToStringPretty() const {
        return ToJson().dump(4, ' ');
    }

    jsonxx::json64 ToJsonTiny() const {
        jsonxx::json64 j;
        j["proposal"] = proposal_id_;
        NodeId n(node_id_);
        j["node"] = n.ToJsonTiny();
        return j;
    }

    std::string ToStringTiny() const {
        return ToJsonTiny().dump();
    }

  private:
    uint64_t proposal_id_;
    uint64_t node_id_;
};

} // namespace vpaxos

#endif
