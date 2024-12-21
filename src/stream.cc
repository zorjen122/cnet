#include "stream.h"
#include "socket.h"
#include <iostream>
#include <unistd.h>

ssize_t Stream::readFixed(const char *buf, size_t size)
{
    size_t count = size;
    int64_t rt = 0;
    while (count > 0)
    {
        rt = read(buf, size);
        if (rt <= 0)
            return rt;
        count -= rt;
    }

    return size;
}
ssize_t Stream::writeFixed(const char *buf, size_t size)
{
    size_t count = size;
    int64_t rt = 0;
    while (count > 0)
    {
        rt = write(buf, size);
        if (rt <= 0)
            return rt;
        count -= rt;
    }

    return size;
}


SocketStream::SocketStream(net::Socket::SharedPtr sock) : Socket(sock){}

ssize_t SocketStream::read(const char *buf, size_t size)
{
    return Socket->recv((void*)buf, size, 0);
}

ssize_t SocketStream::write(const char* buf, size_t size){
    return Socket->send((void*)buf,size, 0);
}

int SocketStream::close()
{
    return Socket->close();
}