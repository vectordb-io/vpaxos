#ifndef __VPAXOS_CONFIG_H__
#define __VPAXOS_CONFIG_H__

#include <getopt.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include "status.h"

namespace vpaxos {

class HostAndPort {
  public:
    HostAndPort() = default;
    HostAndPort(const HostAndPort&) = default;

    HostAndPort(std::string host, int port)
        :host_(host), port_(port) {
    }

    std::string
    ToString() const {
        char buf[64];
        snprintf(buf, sizeof(buf), "%s:%d", host_.c_str(), port_);
        return std::string(buf);
    }

    std::string host_;
    int port_;
};

class Config {
  public:
    static Config&
    GetInstance() {
        static Config instance;
        return instance;
    }

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    Status Load(int argc, char **argv);
    std::string DebugString() const;
    int Quorum() const;

    std::shared_ptr<HostAndPort> MyAddress();


    bool ping() const {
        return ping_;
    }

    bool learner_optimized() const {
        return learner_optimized_;
    }

    const std::string& storage_path() const {
        return storage_path_;
    }

    int max_timeout_ms() const {
        return max_timeout_ms_;
    }

    std::vector<std::shared_ptr<HostAndPort>>& mutable_address() {
        return address_;
    }

    const std::vector<std::shared_ptr<HostAndPort>>& address() const {
        return address_;
    }

  private:
    Config();
    ~Config();

    void ParseHostPort(std::string &hp, std::string &host, int &port);
    void ParseOne(std::string s, vpaxos::HostAndPort &hp);
    void ParsePeers(std::string s, std::vector<vpaxos::HostAndPort> &v);
    void ParseMe(std::string s, vpaxos::HostAndPort &hp);

    bool ping_;
    bool learner_optimized_;
    int max_timeout_ms_;
    std::string storage_path_;

    // address_[0] is mine
    std::vector<std::shared_ptr<HostAndPort>> address_;
};

} // namespace vpaxos

#endif
