#pragma once

#include "socket.h"
#include "iomanager.h"
#include "stream.h"
#include "timer.h"
#include "address.h"
#include <vector>

class TcpServer : public std::enable_shared_from_this<TcpServer>
{
public:
    using SharedPtr = std::shared_ptr<TcpServer>;

    TcpServer(IOManager *_acceptorWorker = IOManager::getThis(), IOManager *_ioWorker = IOManager::getThis());

    virtual ~TcpServer();

    bool bind(const std::vector<Address::SharedPtr> &address, std::vector<Address::SharedPtr> &fails, bool ssl = false);

    bool start(const std::function<void(net::Socket::SharedPtr client)> &acceptHandle);

    bool connect();

    // bool accept(net::Socket::SharedPtr sock);
    bool accept(net::Socket::SharedPtr sock, const std::function<void(net::Socket::SharedPtr client)> &acceptHandle);

    bool setRecvTimeout(int64_t ms);

    virtual void clientHandle();

public:
    IOManager *acceptorWorker;
    IOManager *ioWorker;
    std::vector<net::Socket::SharedPtr> sockGroup;
    net::Socket::SharedPtr serv;
    bool onStop;
    int64_t recvTimeout;
};