#include "timer.h"
#include <iostream>
#include <assert.h>

namespace util
{
    u64 getThisTimeMs()
    {
        timespec t;
        clock_gettime(CLOCK_MONOTONIC_RAW, &t);
        u64 result = (t.tv_sec * 1000) + (t.tv_nsec / 1000000);

        return result;
    }
}

Timer::Timer(u64 ms, std::function<void()> function, bool onWhile, TimerManager *timerManager)
    : msec(ms), func(function), isWhile(onWhile), manager(timerManager)
{
    u64 t = util::getThisTimeMs();
    start = t;
    finish = t + ms;
}

bool Timer::refresh()
{
    std::lock_guard<std::mutex> lock(manager->tmMutex);
    auto self = shared_from_this();
    auto it = manager->timerGroup.find(self);

    if (!func)
    {
        return false;
    }

    if (it == manager->timerGroup.end())
    {
        return false;
    }

    finish = util::getThisTimeMs() + msec;

    manager->timerGroup.erase(it);
    manager->timerGroup.insert(self);

    return true;
}

u64 Timer::diff()
{
    std::lock_guard<std::mutex> lock(manager->tmMutex);
    u64 nowMs = util::getThisTimeMs();

    if (nowMs >= finish)
        return 0;
    else
        return finish - nowMs;
}

bool Timer::Comparator::operator()(const Timer::SharedPtr &lhs, const Timer::SharedPtr &rhs) const
{
    if (!lhs && !rhs)
        return false;
    if (!lhs)
        return true;
    if (!rhs)
        return false;
    if (lhs->finish < rhs->finish)
        return true;
    if (lhs->finish > rhs->finish)
        return false;

    return lhs.get() < rhs.get();
}

TimerManager::TimerManager() : tickled(false)
{
}
TimerManager::~TimerManager()
{
}

bool TimerManager::addTimer(u64 ms, std::function<void()> func, bool onWhile)
{
    std::lock_guard<std::mutex> lock(tmMutex);
    Timer::SharedPtr timer(new Timer(ms, func, onWhile, this));
    auto it = timerGroup.insert(timer).first;
    if (it == timerGroup.begin())
    {
        tickled = true;
        onTimerInsertAtFront();
    }

    return true;
}

bool TimerManager::addTimer(Timer::SharedPtr timer)
{
    std::lock_guard<std::mutex> lock(tmMutex);
    auto it = timerGroup.insert(timer).first;
    if (it == timerGroup.begin())
    {
        tickled = true;
        onTimerInsertAtFront();
    }

    return true;
}

bool TimerManager::hasTimer()
{
    std::lock_guard<std::mutex> lock(tmMutex);
    return !timerGroup.empty();
}

bool TimerManager::expiredGroup(std::vector<std::function<void()>> &funcGroup)
{
    std::lock_guard<std::mutex> lock(tmMutex);
    u64 nowMs = util::getThisTimeMs();

    std::vector<Timer::SharedPtr> expiredCont = {};
    if (timerGroup.empty())
        return false;

    auto minTimerIter = timerGroup.begin();

    auto minTimer = (*minTimerIter)->finish;
    if (nowMs < minTimer)
        return false;

    Timer::SharedPtr nowTimer(new Timer(nowMs, nullptr, false, nullptr));
    auto endTimerIter = timerGroup.lower_bound(nowTimer);

    while (endTimerIter != timerGroup.end() && (*endTimerIter)->finish == nowMs)
        ++endTimerIter;

    expiredCont.insert(expiredCont.begin(), minTimerIter, endTimerIter);
    timerGroup.erase(minTimerIter, endTimerIter);

    funcGroup.reserve(expiredCont.size());
    for (auto &timer : expiredCont)
    {
        funcGroup.push_back(timer->func);
        std::cout << "[expiredGroup: push-timer]\n";
        if (timer->isWhile)
        {
            timer->finish = nowMs + timer->msec;
            timerGroup.insert(timer);
        }
        else
        {
            timer->func = nullptr;
        }
    }

    std::cout << "over expired!\n";
    return true;
}

u64 TimerManager::getNextTimer()
{
    Timer::SharedPtr timer = {};
    {
        std::lock_guard<std::mutex> lock(tmMutex);
        tickled = false;
        if (timerGroup.empty())
            return ~0ull;

        timer = *timerGroup.begin();
    }
    return timer->diff();
}
