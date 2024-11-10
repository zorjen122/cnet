#include "fdManager.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

FdContext::FdContext()
    : fd(-1), isSock(0), isNonBlock(0), recvTimeOut(0), sendTimeOut(0), info()

{
}

void FdContext::init(int _fd, bool onSysNonBlock)
{
    {
        fd = _fd;
        assert(fstat(fd, &info) >= 0);

        if (S_ISSOCK(info.st_mode))
        {
            isSock = true;
            int flag = fcntl(fd, F_GETFL, 0);
            if (!(flag & O_NONBLOCK) && onSysNonBlock)
            {
                fcntl(fd, F_SETFL, flag | O_NONBLOCK);
                isNonBlock = true;
                std::cout << "[SET-NON-BLOCK!]\n";
            }
        }
    }
}

bool FdContext::isSocket()
{
    return isSock;
}

void FdContext::setTimeOut(FdContext::OutType type, uint64_t timeout)
{
    if (type == FdContext::OutType::Recv)
        recvTimeOut = timeout;
    else
        sendTimeOut = sendTimeOut;
}

uint64_t FdContext::getTimeOut(FdContext::OutType type)
{
    if (type == FdContext::OutType::Recv)
        return recvTimeOut;
    else
        return sendTimeOut;
}

bool FdContext::isSysNonBlock()
{
    return isNonBlock;
}

FdManager::FdManager(size_t size)
    : fdContext(size)
{
}

FdContext::SharedPtr FdManager::get(int fd, bool onSysNonBlock)
{
    if (fd < 0)
        return nullptr;

    if (fd >= fdContext.size())
        fdContext.resize(fd * 1.5);

    fdContext[fd].reset(new FdContext());
    fdContext[fd]->init(fd, onSysNonBlock);

    return fdContext[fd];
}

bool FdManager::del(int fd)
{
    if (fd < 0 || fd >= fdContext.size())
        return false;

    fdContext[fd].reset();
    return true;
}
