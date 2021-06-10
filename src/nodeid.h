#ifndef __VPAXOS_NODEID_H__
#define __VPAXOS_NODEID_H__

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>

namespace vpaxos {

class NodeId {
  public:
    NodeId(uint64_t code)
        : code_(code) {
        Code2Node(code, host_, port_);
        Node2Address(host_, port_, address_);
    }

    NodeId(const std::string &address)
        :address_(address) {
        Address2Node(address, host_, port_);
        Node2Code(host_, port_, code_);
    }

    NodeId(const std::string &host, uint32_t port)
        :host_(host), port_(port) {
        Node2Address(host, port, address_);
        Node2Code(host, port, code_);
    }

    NodeId(const NodeId&) = default;
    NodeId& operator=(const NodeId&) = default;
    ~NodeId() = default;

    std::string host() const {
        return host_;
    }

    uint32_t port() const {
        return port_;
    }

    std::string address() const {
        return address_;
    }

    uint64_t code() const {
        return code_;
    }

    std::string ToString() const {
        char buf[128];
        snprintf(buf, sizeof(buf), "host:%s|port:%u|address:%s|code:%lu", host_.c_str(), port_, address_.c_str(), code_);
        return std::string(buf);
    }

  private:
    void Address2Node(const std::string& address, std::string &host, uint32_t &port) {
        char* psave = nullptr;
        const char *d = ":";
        char *p;
        p = strtok_r((char*)address.c_str(), d, &psave);
        host = std::string(p);
        p = strtok_r(nullptr, d, &psave);
        sscanf(p, "%d", &port);
    }

    void Code2Node(uint64_t code, std::string &host, uint32_t &port) {
        port_ = code & 0x00000000FFFFFFFF;
        uint32_t host_uint32 = code >> 32 & 0x00000000FFFFFFFF;
        struct in_addr addr;
        addr.s_addr = host_uint32;
        char buf[32];
        auto p = inet_ntop(AF_INET, &addr, buf, sizeof(buf));
        assert(p);
        host_ = std::string(buf);
    }

    void Node2Address(const std::string &host, uint32_t port, std::string& address) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s:%u", host.c_str(), port);
        address = buf;
    }

    void Node2Code(const std::string &host, uint32_t port, uint64_t &code) {
        uint32_t host_uint32 = (uint32_t)inet_addr(host.c_str());
        assert(host_uint32 != (uint32_t)-1);
        code_ = (static_cast<uint64_t>(host_uint32) << 32) | port;
    }

    std::string host_;
    uint32_t port_;

    std::string address_;
    uint64_t code_;
};

}  // namespace vpaxos

#endif
