#include "address.h"
#include <assert.h>
#include <iostream>

Address::SharedPtr Address::create(const sockaddr *addr, socklen_t len)
{
    if (addr == nullptr)
        return nullptr;

    Address::SharedPtr obj;

    switch (addr->sa_family)
    {
    case AF_INET:
        obj.reset(new IPv4(*(const sockaddr_in *)addr));
        break;
    case AF_INET6:
        obj.reset(new IPv6(*(const sockaddr_in6 *)addr));
        break;
    default:
        obj.reset(new UnknowAddress());
        break;
    }

    return obj;
}

bool Address::lookup(std::vector<Address::SharedPtr> &result, const std::string &host,
                     int family, int sockType, int protctol)
{
    if (host.empty())
        return false;

    addrinfo infoReq, *infoRsp, *next;

    memset(&infoReq, 0, sizeof(infoReq));
    infoReq.ai_family = family;
    infoReq.ai_socktype = sockType;
    infoReq.ai_protocol = protctol;

    std::string node;
    const char *service = (const char *)memchr(host.c_str(), ':', host.size());

    // if host != ipv6
    const char *end = host.c_str() + host.size();
    if (!(memchr(service + 1, ':', end - service - 1)))
    {
        node = host.substr(0, service - host.c_str());
        service++;
    }

    // ipv6 todo.
    if (node.empty())
        return false;

    int status = getaddrinfo(node.c_str(), service, &infoReq, &infoRsp);
    if (status)
        assert(0);

    next = infoRsp;
    while (next)
    {
        result.push_back(Address::create(next->ai_addr, next->ai_addrlen));

        next = next->ai_next;
    }

    return !result.empty();
}

Address::SharedPtr Address::lookupAny(const std::string &host,
                                      int family, int sockType, int protctol)
{
    std::vector<Address::SharedPtr> build;
    if (!lookup(build, host, family, sockType, protctol))
        assert(0);

    return build.front();
}

inline int Address::getFamily()
{
    return getAddress()->sa_family;
}

inline const sockaddr *IPv4::getAddress()
{
    return (const sockaddr *)&addr;
}
inline socklen_t IPv4::getAddressLength()
{
    return sizeof(addr);
}

inline std::string IPv4::getStringAddress()
{
    char buf[20] = {};
    const char *p = inet_ntop(AF_INET, (sockaddr *)&addr, buf, 20);
    if (!p)
        assert(0);

    return std::string(std::move(buf));
}

inline std::string IPv6::getStringAddress()
{
    char buf[20] = {};
    const char *p = inet_ntop(AF_INET, (sockaddr *)&addr, buf, 20);
    if (!p)
        assert(0);

    return std::string(std::move(buf));
}

inline const sockaddr *IPv6::getAddress()
{
    return (const sockaddr *)&addr;
}

inline socklen_t IPv6::getAddressLength()
{
    return sizeof(addr);
}
