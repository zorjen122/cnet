#pragma once

#include "stream.h"
#include "address.h"
#include "hook.h"
#include "util.h"
#include "fdManager.h"
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <memory>

namespace net
{
    class Socket : public std::enable_shared_from_this<Socket>
    {
    public:
        using SharedPtr = std::shared_ptr<Socket>;

        enum Family
        {
            UNIX = AF_UNIX,
            IPv4 = AF_INET,
            IPv6 = AF_INET6
        };

        enum Type
        {
            TCP = SOCK_STREAM,
            UDP = SOCK_DGRAM
        };

        Socket(int _family, int _type, int _protocol, bool _onSysNonBlock = true);

        static Socket::SharedPtr CreateTCP(Socket::Family type);
        static Socket::SharedPtr CreateUDP(Socket::Family type);

        Socket::SharedPtr bind(Address::SharedPtr address);
        Socket::SharedPtr accept();

        bool listen(size_t backlog = 1024);
        bool close();
        bool connect(Address::SharedPtr address);

        int send(const void *buf, size_t length, int flag);
        int recv(void *buf, size_t length, int flag);

        template <class T>
        bool setOption(int level, int option, const T &value);
        template <class T>
        bool getOption(int level, int option, T *value);

        bool setRecvTimeout(int64_t ms);
        bool setSendTimeout(int64_t ms);

        int64_t getRecvTimeout();
        int64_t getSendTimeout();

        Address::SharedPtr getAddress();
        int getFamily();
        int getSocketType();
        int getProtocol();
        int getDiscriptor();

    protected:
        bool init(int sock);
        bool newSocket();
        bool initSocket();

    private:
        int fd;
        int family;
        int sockType;
        int protocol;
        bool isConnected;
        bool onSysNonBlock;
        Address::SharedPtr localAddress;
    };

};
