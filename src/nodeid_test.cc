#include <iostream>
#include "nodeid.h"

int main() {
    vpaxos::NodeId n1("127.0.0.1", 12345);
    std::cout << n1.ToString() << std::endl;

    vpaxos::NodeId n2(72058139498786873);
    std::cout << n2.ToString() << std::endl;

    vpaxos::NodeId n3("127.0.0.1:12345");
    std::cout << n3.ToString() << std::endl;

    return 0;
}
