#include "fdManager.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

int main()
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("172.28.238.233");
    addr.sin_port = htons(2300);

    assert(bind(sock, (const sockaddr *)&addr, sizeof(addr)) >= 0);
    assert(listen(sock, 1024) >= 0);

    FdManager manager;
    manager.get(sock);

    int people = 0;
    int client = 0;
    for (;;)
    {
        client = accept(sock, (sockaddr *)&addr, 0);
        if (client < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                sleep(1);
                std::cout << "[for-tick]\n";

                continue;
            }
        }

        if (client >= 0)
            break;
    }

    char buf[1024] = {};
    int size = 0;
    size = read(client, buf, 1024);
    for (;;)
    {
        std::cout << "tick\n";
        if (size > 0)
        {
            // break;
            std::cout << buf << "\n";
        }
        sleep(1);
    }

    close(sock);

    return 0;
}