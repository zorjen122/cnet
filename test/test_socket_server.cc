#include "iomanager.h"
#include "socket.h"

int main()
{
    using namespace net;
    auto sock = Socket::CreateTCP(Socket::Family::IPv4);
    auto addr = Address::lookupAny("127.0.0.1:1010", AF_INET);

    sock->bind(addr)->listen(1024);
    std::cout << "accept-wait...\n";
    auto client = sock->accept();

    std::cout << "sned-wait...\n";
    const char *buf = "Hello, World!\n";
    int size = client->send(buf, strlen(buf), 0);
    assert(size != -1);
    std::cout << "size: " << size << "\n";

    {
        IOManager iom(1, false);
        iom.add(client->getDiscriptor(), IOManager::READ, [client]() -> void
                {
        char readbuf[1024] = {};
        client->recv(readbuf, 1024, 0);
        std::cout << "read-buf: " << readbuf << "\n"; });

        iom.run();
    }

    getchar();
    client->close();

    sock->close();

    return 0;
}