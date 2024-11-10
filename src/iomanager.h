#pragma once

#include "scheduler.h"
#include "coroutine.h"
#include "timer.h"
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <vector>

struct IOContext
{
    IOContext() : func(), fd() {}
    IOContext(std::function<void()> _func, int _fd, int _eventType)
    {
        func = _func;
        fd = _fd;
        eventType = _eventType;
    }

    std::function<void()> func;
    int fd;
    int eventType;
};

class IOManager : public Scheduler, public TimerManager
{
public:
    enum
    {
        READ = 0x01,
        WRITE = 0x04
    };

    IOManager(size_t threadSize, bool onCaller);
    ~IOManager();

    void idle() override;
    void tickle() override;
    bool checkStoping() override;
    void onTimerInsertAtFront() override;

    void add(epoll_event event, const std::function<void()> &callback);
    void add(int fd, int eventType, const std::function<void()> &callback);
    void del(int fd, int event);
    void cancel(int fd, int event);
    void trigger(IOContext &ioc, int eventType);

    static IOManager *getThis();

private:
    int ep;
    int tick[2];
    std::vector<IOContext> ioContext;
    std::mutex ioMutex;

    int pendingEventCount;
    size_t epollTimeOut;
};