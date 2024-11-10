#include "address.h"
#include <string>

int main()
{
    Address::SharedPtr addr = Address::lookupAny("www.baidu.com:80", AF_INET);

    const sockaddr_in *paddr = (const sockaddr_in *)(addr->getAddress());
    char buf[1024] = {};
    inet_ntop(addr->getAddress()->sa_family, &paddr->sin_addr, buf, 1024);

    printf("address: %s\n", buf);

    return 0;
}