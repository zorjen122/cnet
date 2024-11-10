#include "iomanager.h"
#include "socket.h"
#include <iostream>
#include <assert.h>

int main()
{
    using namespace net;

    Socket::SharedPtr sock = Socket::CreateTCP(Socket::Family::IPv4);

    Address::SharedPtr addr = Address::lookupAny("127.0.0.1:1010", AF_INET);
    assert(sock->connect(addr));

    char readbuf[1024] = {};
    std::cout << "recv... " << "\n";
    assert(sock->recv(readbuf, 1024, 0) != -1);
    std::cout << ": " << readbuf << "\n";

    std::cout << "send..." << "\n";

    const char *buf = "HELLO, SOCKET!";
    assert(sock->send(buf, strlen(buf), 0) != -1);
    std::cout << "Send[HELLO, SOCKET!] OK!\n";

    // assert(sock->close());

    return 0;
}