#include "tcpServer.h"

#include <assert.h>
#include <iostream>

TcpServer::TcpServer(IOManager *_acceptorWorker, IOManager *_ioWorker)
    : acceptorWorker(_acceptorWorker),
      ioWorker(_ioWorker),
      sockGroup(),
      serv(),
      recvTimeout(60 * 1000 * 2),
      onStop(false)
{
}

TcpServer::~TcpServer()
{
    for (auto &sock : sockGroup)
        sock->close();
    std::cout << "TcpServer::~TcpServer\n";
}

bool TcpServer::connect()
{
    return true;
}

bool TcpServer::bind(const std::vector<Address::SharedPtr> &address, std::vector<Address::SharedPtr> &fails, bool ssl)
{

    for (auto &addr : address)
    {
        serv = net::Socket::CreateTCP((net::Socket::Family)addr->getFamily());

        if (!serv)
            assert(0);

        if (!serv->bind(addr))
        {
            std::cout << "\t\t[TcpServer bind-addr fails!]\n";
            fails.push_back(addr);
            continue;
        }
        if (!serv->listen())
        {
            std::cout << "\t\t[TcpServer listen-addr fails!]\n";
            fails.push_back(addr);
            continue;
        }

        sockGroup.push_back(serv);
    }

    return !sockGroup.empty();
}

// bool TcpServer::accept(net::Socket::SharedPtr sock)
// {
//     while (!onStop)
//     {
//         auto client = sock->accept();
//         if (!client)
//             assert(0);
//         client->setRecvTimeout(recvTimeout);
//         ioWorker->push(std::bind(&TcpServer::clientHandle, shared_from_this()));
//     }
// }

bool TcpServer::accept(net::Socket::SharedPtr sock, const std::function<void(net::Socket::SharedPtr client)> &acceptHandle)
{

    while (!onStop)
    {
        std::cout << "accept...\n";
        auto client = sock->accept();
        static int i = 0;
        std::cout << "client-" << ++i << "\n";
        if (!client)
            assert(0);
        client->setRecvTimeout(recvTimeout);
        // auto self = shared_from_this
        ioWorker->push([acceptHandle, client]()
                       { acceptHandle(client); });
    }
}

void TcpServer::clientHandle()
{
    std::cout << "[virtualTcpServer::~clientHandle]!\n";
}

bool TcpServer::setRecvTimeout(int64_t ms)
{
    recvTimeout = ms;
}

bool TcpServer::start(const std::function<void(net::Socket::SharedPtr client)> &acceptHandle)
{
    // exist this case only start() function by two-call.
    // if (!onStop)
    //     return true;

    // onStop = true;
    for (auto &sock : sockGroup)
        acceptorWorker->push(std::bind(&TcpServer::accept, shared_from_this(), sock, acceptHandle));

    return true;
}
