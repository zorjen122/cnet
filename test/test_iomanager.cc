#include "iomanager.h"
#include <iostream>
#include <assert.h>
#include <sys/socket.h>

// ----------------_---------------------
int arr[2];

void pfunc()
{
    char buf[1024] = {};
    read(arr[0], buf, 1024);
    std::cout << "[pfunc] buffer: " << buf << "\n";
}

void waitWrite()
{
    const char *s = "Hello, IOManager!\n";
    int fd = write(arr[1], s, strlen(s));
    assert(fd >= 0);
    std::cout
        << "waitWrite: write is over!\n";
}

void example1()
{
    IOManager iom(1, false);
    epoll_event ev = {};
    pipe(arr);
    ev.data.fd = arr[0];
    ev.events = EPOLLIN;

    // iom.add(ev, &pfunc);
    iom.add(arr[0], IOManager::READ, [&]()
            { pfunc(); });
    iom.addTimer(2000, waitWrite, true);
    // std::thread t(&waitWrite);

    iom.run();

    // t.join();
}

#include <sys/socket.h>
#include <arpa/inet.h>

static IOManager iomer(1, true);
void test_socket()
{
    int sock = socket(PF_INET, SOCK_STREAM, 0);

    sockaddr_in addr = {};
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8080);
    addr.sin_family = AF_INET;

    int rt = bind(sock, (sockaddr *)&addr, sizeof(addr));
    if (rt < 0)
        assert(0);

    listen(sock, 100);

    std::cout << "server on!\n";
    int client;
    sockaddr_in client_addr = {};
    int client_len = 0;

    static int sig = 0;

    for (int i = 0; i < 100; ++i)

    {
        client = accept(sock, (sockaddr *)&client_addr, (socklen_t *)&client_len);
        std::cout << "accept-" << i << "\n";

        iomer.add(client, IOManager::READ, [&]()
                  {
          std::cout << "read event!\n";
          char buf[1024] = {};
          int len = read(client, buf, 1024);
          if(len < 0)
          assert(0);
          std::cout << "buf: "<< buf << "\n"; 
          close(client); 
          sig = 1 ; });

        while (sig != 1)
        {
            sleep(1);
            std::cout << "wait!\n";
        }

        sig = 0;
    }
}

void example2()
{
    std::thread t(test_socket);

    iomer.run();
    t.join();
}

int main()
{
    example1();
    // example2();
    getchar();
}