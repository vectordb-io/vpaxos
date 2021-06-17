#include <iostream>
#include "nodeid.h"

int main() {
    {
        vpaxos::NodeId n("127.0.0.1", 12345);
        std::cout << n.ToString() << std::endl;
        std::cout << n.ToStringTiny() << std::endl;
        std::cout << n.ToStringPretty() << std::endl;
        std::cout << "-----------------------------" << std::endl << std::endl;
    }

    {
        vpaxos::NodeId n(72058139498786873);
        std::cout << n.ToString() << std::endl;
        std::cout << n.ToStringTiny() << std::endl;
        std::cout << n.ToStringPretty() << std::endl;
        std::cout << "-----------------------------" << std::endl << std::endl;
    }

    {
        vpaxos::NodeId n("127.0.0.1:12345");
        std::cout << n.ToString() << std::endl;
        std::cout << n.ToStringTiny() << std::endl;
        std::cout << n.ToStringPretty() << std::endl;
        std::cout << "-----------------------------" << std::endl << std::endl;
    }

    return 0;
}
