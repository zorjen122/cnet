#pragma once

#include <time.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

using u64 = uint64_t;

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer>
{
    friend class TimerManager;

public:
    using SharedPtr = std::shared_ptr<Timer>;

    Timer(u64 ms, std::function<void()> function, bool onWhile, TimerManager *timerManager);

    bool refresh();
    u64 diff();

    struct Comparator
    {
        bool operator()(const Timer::SharedPtr &lhs, const Timer::SharedPtr &rhs) const;
    };

private:
    u64 start;
    u64 finish;
    u64 msec;

    std::function<void()> func;
    bool isWhile;

    TimerManager *manager;
};

class TimerManager
{
    friend class Timer;

public:
    TimerManager();
    virtual ~TimerManager();

    virtual void onTimerInsertAtFront() = 0;

    bool addTimer(u64 ms, std::function<void()> func, bool onWhile = false);
    bool addTimer(Timer::SharedPtr timer);
    bool hasTimer();

    bool expiredGroup(std::vector<std::function<void()>> &funcGroup);
    u64 getNextTimer();

private:
    std::mutex tmMutex;
    std::set<Timer::SharedPtr, Timer::Comparator> timerGroup;
    bool tickled;
};
