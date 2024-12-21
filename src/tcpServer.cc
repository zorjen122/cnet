#include "tcpServer.h"

#include <assert.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>

#include "socket.h"

TcpServer::TcpServer(IOManager *_acceptorWorker, IOManager *_ioWorker)
    : acceptorWorker(_acceptorWorker),
      ioWorker(_ioWorker),
      sockGroup(),
      serv(),
      recvTimeout(60 * 1000 * 2),
      onStop(false) {}

TcpServer::~TcpServer() {
  for (auto &sock : sockGroup) sock->close();
  std::cout << "TcpServer::~TcpServer\n";
}

bool TcpServer::connect() { return true; }

bool TcpServer::bind(const std::vector<Address::SharedPtr> &address,
                     std::vector<Address::SharedPtr> &fails, bool ssl) {
  for (auto &addr : address) {
    serv = net::Socket::CreateTCP((net::Socket::Family)addr->getFamily());

    if (!serv) assert(0);

    if (!serv->bind(addr)) {
      std::cout << "\t\t[TcpServer bind-addr fails!]\n";
      fails.push_back(addr);
      continue;
    }
    if (!serv->listen()) {
      std::cout << "\t\t[TcpServer listen-addr fails!]\n";
      fails.push_back(addr);
      continue;
    }

    sockGroup.push_back(serv);
  }

  return !sockGroup.empty();
}

bool TcpServer::accept(
    net::Socket::SharedPtr sock,
    const std::function<void(net::Socket::SharedPtr)> &acceptHandle) {
  static int i = 0;
  while (!onStop) {
    std::cout << "[accept...]\n";
    auto client = sock->accept();
    if (client) {
      std::cout << "[client-" << ++i << "]\n";

      client->setRecvTimeout(recvTimeout);
      ioWorker->push([acceptHandle, client]() {
        std::cout << "[Push cli-handle!]\n";
        acceptHandle(client);
      });
      sleep(1);
    } else {
      std::cout << "[ERROR: accept is wrong!, errno = " << strerror(errno)
                << "]\n";
    }
  }
}

bool TcpServer::accept(net::Socket::SharedPtr sock) {
  TcpServer::accept(sock, std::bind(&TcpServer::clientHandle,
                                    shared_from_this(), std::placeholders::_1));
}

void TcpServer::clientHandle(net::Socket::SharedPtr sock) {
  std::cout << "[virtualTcpServer::~clientHandle]!\n";
}

bool TcpServer::setRecvTimeout(int64_t ms) { recvTimeout = ms; }

bool TcpServer::start(
    const std::function<void(net::Socket::SharedPtr)> &acceptHandle) {
  // if (!onStop)
  //     return true;
  // onStop = true;
  for (auto &sock : sockGroup) {
    using overloadType = bool (TcpServer::*)(
        net::Socket::SharedPtr,
        const std::function<void(net::Socket::SharedPtr)> &);

    acceptorWorker->push(
        std::bind(static_cast<overloadType>(&TcpServer::accept),
                  shared_from_this(), sock, acceptHandle));
  }

  return true;
}

bool TcpServer::start() {
  for (auto &sock : sockGroup) {
    using overloadType = bool (TcpServer::*)(net::Socket::SharedPtr);

    acceptorWorker->push(
        std::bind(static_cast<overloadType>(&TcpServer::accept),
                  shared_from_this(), sock));
  }

  return true;
}
