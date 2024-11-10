#include "socket.h"
#include <iostream>
#include <assert.h>

namespace net
{
    Socket::Socket(int _family, int _socketType, int _protocol, bool _onSysNonBlock)
        : fd(-1), family(_family), sockType(_socketType), protocol(_protocol), localAddress(), onSysNonBlock(true)
    {
    }

    bool Socket::initSocket()
    {
        if (fd == -1)
            assert(0);

        if (setOption(SOL_SOCKET, SO_REUSEADDR, 1))
        {
            if (sockType == Type::TCP)
                setOption(IPPROTO_TCP, TCP_NODELAY, 0);
        }

        return true;
    }

    template <class T>
    bool Socket::setOption(int level, int option, const T &value)
    {
        if (setsockopt(fd, level, option, &value, sizeof(value)) == -1)
        {
            net::status.setValue(errno, strerror(errno));
            assert(0);
            return false;
        }

        return true;
    }

    template <class T>
    bool Socket::getOption(int level, int option, T *value)
    {
        if (getsockopt(fd, level, option, value, nullptr) == -1)
        {
            net::status.setValue(errno, strerror(errno));
            assert(0);

            return false;
        }
        return true;
    }

    Socket::SharedPtr Socket::CreateTCP(Socket::Family type)
    {
        Socket::SharedPtr sock = std::make_shared<Socket>(type, Socket::Type::TCP, 0);
        return sock;
    }

    Socket::SharedPtr Socket::CreateUDP(Socket::Family type)
    {
        Socket::SharedPtr sock(new Socket(type, Socket::Type::UDP, 0));
        return sock;
    }
    bool Socket::newSocket()
    {
        fd = socket(family, sockType, protocol);
        assert(fd != -1);
        initSocket();

        return true;
    }

    Socket::SharedPtr Socket::bind(Address::SharedPtr address)
    {
        if (fd == -1)
            newSocket();

        localAddress = address;

        const sockaddr_in *addr = (const sockaddr_in *)localAddress->getAddress();

        std::cout << "[bind]: addr-" << localAddress->getStringAddress() << "\tport-" << htons(addr->sin_port) << "\n";

        if (::bind(fd, localAddress->getAddress(), localAddress->getAddressLength()) == -1)
        {
            net::status.setValue(errno, strerror(errno));
            assert(0);
        }

        return shared_from_this();
    }

    bool Socket::listen(size_t backlog)
    {
        if (::listen(fd, backlog) == -1)
        {
            net::status.setValue(errno, strerror(errno));
            assert(0);
            return false;
        }

        return true;
    }

    Socket::SharedPtr Socket::accept()
    {
        Socket::SharedPtr clientSock(new Socket(family, sockType, protocol));

        int client = ::accept(fd, nullptr, nullptr);
        if (client == -1)
        {
            net::status.setValue(errno, strerror(errno)), assert(0);
            assert(0);
        }

        if (clientSock->init(client))
            return clientSock;

        return nullptr;
    }

    int Socket::send(const void *buf, size_t length, int flag)
    {
        if (!isConnected && fd == -1)
            assert(0);

        int rt = ::send(fd, buf, length, flag);
        assert(rt != -1);
        return rt;
    }
    int Socket::recv(void *buf, size_t length, int flag)
    {
        if (!isConnected && fd == -1)
            assert(0);

        return ::recv(fd, buf, length, flag);
    }

    bool Socket::init(int sock)
    {
        auto context = Singleton<FdManager>::GetInstance()->get(sock, onSysNonBlock);
        if (!context || !context->isSocket())
        {
            assert(0);
            return false;
        }

        fd = sock;
        isConnected = true;
        initSocket();
        getAddress();

        return true;
    }

    int Socket::getFamily()
    {
        return family;
    }

    int Socket::getProtocol()
    {
        return protocol;
    }

    int Socket::getSocketType()
    {
        return sockType;
    }

    Address::SharedPtr Socket::getAddress()
    {
        if (localAddress)
            return localAddress;

        if (fd == -1)
            assert(0);

        sockaddr addr = {};
        socklen_t len = sizeof(sockaddr);
        if (getsockname(fd, &addr, &len) < 0)
        {
            net::status.setValue(errno, strerror(errno));
            assert(0);
        }

        localAddress = Address::create(&addr, len);
        std::cout << localAddress->getStringAddress() << "\n";

        return localAddress;
    }

    bool Socket::connect(Address::SharedPtr address)
    {
        if (fd == -1)
            newSocket();
        localAddress = address;
        if (::connect(fd, localAddress->getAddress(), localAddress->getAddressLength()) == -1)
        {
            net::status.setValue(errno, strerror(errno));
            assert(0);
            return false;
        }

        return true;
    }

    bool Socket::close()
    {
        if (!isConnected)
            return false;

        if (::close(fd) == -1)
        {
            net::status.setValue(errno, strerror(errno));
            assert(0);
            return false;
        }

        return true;
    }

    int Socket::getDiscriptor()
    {
        return fd;
    }

    bool Socket::setRecvTimeout(int64_t ms)
    {
        struct timeval v
        {
            int(ms / 1000),
                int(ms % 1000 * 1000)
        };
        return setOption(SOL_SOCKET, SO_RCVTIMEO, v);
    }

    bool Socket::setSendTimeout(int64_t ms)
    {
        struct timeval v
        {
            int(ms / 1000),
                int(ms % 1000 * 1000)
        };
        return setOption(SOL_SOCKET, SO_SNDTIMEO, v);
    }

    int64_t Socket::getRecvTimeout()
    {
        int64_t v = 0;
        getOption(SOL_SOCKET, SO_RCVTIMEO, &v);

        return v;
    }
    int64_t Socket::getSendTimeout()
    {
        int64_t v = 0;
        getOption(SOL_SOCKET, SO_SNDTIMEO, &v);

        return v;
    }
};
