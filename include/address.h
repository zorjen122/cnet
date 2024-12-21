#pragma once

#include <memory>
#include <vector>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>

class IPAddress;

class Address
{
public:
    using SharedPtr = std::shared_ptr<Address>;

    static Address::SharedPtr create(const sockaddr *addr, socklen_t len);
    static bool lookup(std::vector<Address::SharedPtr> &result, const std::string &host,
                       int family, int sockType = 0, int protctol = 0);
    static Address::SharedPtr lookupAny(const std::string &host,
                                        int family, int sockType = 0, int protctol = 0);

    virtual int getFamily();
    virtual const sockaddr *getAddress() = 0;
    virtual socklen_t getAddressLength() = 0;

    virtual std::string getStringAddress() = 0;
};

class IPAddress : public Address
{
public:
};

class IPv4 : public IPAddress
{
public:
    using SharedPtr = std::shared_ptr<IPv4>;
    IPv4(const sockaddr_in &inAddress) : addr(inAddress) {}

    inline const sockaddr *getAddress() override;
    inline socklen_t getAddressLength() override;
    inline std::string getStringAddress() override;

private:
    struct sockaddr_in addr;
};

class IPv6 : public IPAddress
{
public:
    IPv6(const sockaddr_in6 &inAddress) : addr(inAddress) {}
    const sockaddr *getAddress() override;
    socklen_t getAddressLength() override;
    inline std::string getStringAddress() override;

private:
    struct sockaddr_in6 addr;
};

class UnixAddress : public Address
{
public:
    using SharedPtr = std::shared_ptr<UnixAddress>;
    const sockaddr *getAddress() override { return nullptr; }
    socklen_t getAddressLength() override { return 0; }
    inline std::string getStringAddress() override { return {}; }
    UnixAddress() {}
};
class UnknowAddress : public Address
{
public:
    using SharedPtr = std::shared_ptr<UnknowAddress>;
    const sockaddr *getAddress() override { return nullptr; }
    socklen_t getAddressLength() override { return 0; }
    inline std::string getStringAddress() override { return {}; }

    UnknowAddress() {}
};