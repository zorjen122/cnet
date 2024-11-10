#pragma once

#include <vector>
#include <memory>
#include <sys/stat.h>
#include <sys/socket.h>

/*  System function. */
class FdContext
{

public:
    using SharedPtr = std::shared_ptr<FdContext>;
    enum OutType
    {
        Recv = SO_SNDTIMEO,
        Send = SO_SNDTIMEO
    };

    FdContext();
    void init(int _fd, bool onSysNonBlock = true);

    void setTimeOut(OutType type, uint64_t timeout);
    uint64_t getTimeOut(OutType type);

    bool isSocket();
    bool isSysNonBlock();

private:
    int fd;
    bool isSock;
    bool isNonBlock;
    struct stat info;
    uint64_t recvTimeOut;
    uint64_t sendTimeOut;
};

class FdManager
{
public:
    FdManager(size_t _size = 32);

    FdContext::SharedPtr get(int _fd, bool onSysNonBlock = true);
    bool del(int _fd);

    std::vector<FdContext::SharedPtr> fdContext;
};
