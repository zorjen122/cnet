#include "../src/tcpServer.h"
#include "../src/iomanager.h"

#include <iostream>
#include <assert.h>

IOManager iom(2, true);

void echo(net::Socket::SharedPtr client)
{
    std::cout << "echo...\n";
    char buf[1024] = {};

    hook::Sleep(10);
    std::cout << "back this!\n";
    int rt = client->recv(buf, 1024, 0);
    assert(rt != -1);
    std::cout << "read-buf: " << buf << "\n";

    rt = client->send(buf, strlen(buf), 0);
    assert(rt != -1);
    std::cout << "send-to: " << buf << " [state: over!]\n";

    std::cout << "recv...\n";
    // iom.add(client->getDiscriptor(), IOManager::READ, std::bind(echo, client));
}

void run()
{
    TcpServer::SharedPtr serv(new TcpServer());
    assert(serv->acceptorWorker == &iom && serv->ioWorker == &iom);
    std::vector<Address::SharedPtr> addr = {};
    // Address::lookup(addr, "127.0.0.1:2020", AF_INET);
    addr.push_back(Address::lookupAny("127.0.0.1:8080", AF_INET));

    for (auto &it : addr)
    {
        std::cout << "addr: " << it->getStringAddress() << "\n";
    }

    std::vector<Address::SharedPtr> fails = {};
    assert(serv->bind(addr, fails));

    assert(serv->start([serv](net::Socket::SharedPtr client)
                       { std::cout << "recv...\n";
                        iom.add(client->getDiscriptor(), IOManager::READ, std::bind(echo, client)); }));
}

int main()
{
    iom.push(&run);

    iom.start();

    // iom.stop();

    return 0;
}