#include <iostream>
#include "nodeid.h"

int main() {
    vpaxos::NodeId n1("127.0.0.1", 12345);
    std::cout << n1.ToString() << std::endl;
    std::cout << n1.ToJson() << std::endl;
    std::cout << n1.ToJsonPretty() << std::endl;
    std::cout << "-----------------------------" << std::endl;

    vpaxos::NodeId n2(72058139498786873);
    std::cout << n2.ToString() << std::endl;
    std::cout << n2.ToJson() << std::endl;
    std::cout << n2.ToJsonPretty() << std::endl;
    std::cout << "-----------------------------" << std::endl;

    vpaxos::NodeId n3("127.0.0.1:12345");
    std::cout << n3.ToString() << std::endl;
    std::cout << n3.ToJson() << std::endl;
    std::cout << n3.ToJsonPretty() << std::endl;
    std::cout << "-----------------------------" << std::endl;

    return 0;
}
