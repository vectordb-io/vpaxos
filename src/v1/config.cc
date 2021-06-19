#include "config.h"

namespace vpaxos {

Config::Config()
    :ping_(false),
     learner_optimized_(false),
     max_timeout_ms_(-1) {
}

Config::~Config() {
}

Status
Config::Load(int argc, char **argv) {
    int option_index, option_value;
    option_index = 0;
    static struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"learner-optimized", no_argument, nullptr, 'l'},
        {"ping", no_argument, nullptr, 't'},  // t means test
        {"peers", required_argument, nullptr, 'p'},
        {"path", required_argument, nullptr, 's'},  // s means storage path
        {"me", required_argument, nullptr, 'm'},
        {"max_timeout", required_argument, nullptr, 'o'},
        {nullptr, 0, nullptr, 0}
    };

    vpaxos::HostAndPort me;
    std::vector<vpaxos::HostAndPort> peers;
    while ((option_value = getopt_long(argc, argv, "hp:m:", long_options, &option_index)) != -1) {
        switch (option_value) {
        case 'm':
            ParseMe(std::string(optarg), me);
            break;

        case 'o': {
            int tm;
            sscanf(optarg, "%d", &tm);
            max_timeout_ms_ = tm;
            break;
        }

        case 'l':
            learner_optimized_ = true;
            break;

        case 't':
            ping_ = true;
            break;

        case 'p':
            ParsePeers(std::string(optarg), peers);
            break;

        case 's':
            storage_path_ = std::string(optarg);
            break;

        case 'h':
            return Status::InvalidArgument("-h", "help");
            exit(0);

        default:
            return Status::InvalidArgument("-h", "help");
            exit(0);
        }
    }

    vpaxos::Config::GetInstance().address_.push_back(std::make_shared<vpaxos::HostAndPort>(me));
    for (auto hp : peers) {
        vpaxos::Config::GetInstance().address_.push_back(std::make_shared<vpaxos::HostAndPort>(hp));
    }

    return Status::OK();
}

int
Config::Quorum() const {
    int n = address_.size();
    return n /  2 + 1;
}

void
Config::ParseHostPort(std::string &hp, std::string &host, int &port) {
    char* psave = nullptr;
    const char *d = ":";
    char *p;
    p = strtok_r((char*)hp.c_str(), d, &psave);
    host = std::string(p);
    p = strtok_r(nullptr, d, &psave);
    sscanf(p, "%d", &port);
}

void
Config::ParseOne(std::string s, vpaxos::HostAndPort &hp) {
    ParseHostPort(s, hp.host_, hp.port_);
}

void
Config::ParsePeers(std::string s, std::vector<vpaxos::HostAndPort> &v) {
    char* psave = nullptr;
    const char *d = ",";
    char *p;
    p = strtok_r((char*)s.c_str(), d, &psave);
    while (p) {
        std::string tmp(p);
        vpaxos::HostAndPort hp;
        ParseOne(tmp, hp);
        v.push_back(hp);
        p = strtok_r(nullptr, d, &psave);
    }
}

void
Config::ParseMe(std::string s, vpaxos::HostAndPort &hp) {
    ParseOne(s, hp);
}

std::shared_ptr<HostAndPort>
Config::MyAddress() {
    auto it = address_.begin();
    if (it != address_.end()) {
        return *it;
    } else {
        return std::shared_ptr<HostAndPort>();
    }
}

std::string
Config::DebugString() const {
    std::string s;
    s.append("\n[\n");
    s.append("address: \n");
    for (auto hp : address_) {
        s.append(hp->ToString());
        s.append("\n");
    }
    s.append("]\n");
    return s;
}



} // namespace vpaxos
