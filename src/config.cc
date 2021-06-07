#include "config.h"

namespace vpaxos {

Status
Config::Load(int argc, char **argv) {
    int option_index, option_value;
    option_index = 0;
    static struct option long_options[] = {
        {"help", no_argument, nullptr, 'h'},
        {"ping", no_argument, nullptr, 't'},  // t means test
        {"peers", required_argument, nullptr, 'p'},
        {"path", required_argument, nullptr, 's'},  // s means storage path
        {"me", required_argument, nullptr, 'm'},
        {nullptr, 0, nullptr, 0}
    };

    vpaxos::HostAndPort me;
    std::vector<vpaxos::HostAndPort> peers;
    while ((option_value = getopt_long(argc, argv, "hp:m:", long_options, &option_index)) != -1) {
        switch (option_value) {
        case 'm':
            ParseMe(std::string(optarg), me);
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

} // namespace vpaxos
