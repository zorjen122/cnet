#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <assert.h>
#include <string.h>

int main(int argc, char *argv[])
{

    if (argc != 2)
        assert(0);
    int sock = socket(PF_INET, SOCK_STREAM, 0);

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(atoi(argv[1]));
    if (connect(sock, (const sockaddr *)&addr, sizeof(addr)) < 0)
        assert(0);
    std::cout << "Connect!\n";

    int x = 0;
    for (;;)
    {

        char buf[1024] = {};
        std::cout << "$ ";
        scanf("%s", buf);
        if (!strcmp(buf, "quit\n"))
            break;

        x = write(sock, buf, strlen(buf));

        x = read(sock, buf, 1024);
        assert(x != -1);
        std::cout << buf << "\n";

        assert(x != -1);
    }

    x = close(sock);
    assert(x != -1);

    return 0;
}