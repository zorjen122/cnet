#include "iomanager.h"

#include <iostream>

#include "coroutine.h"
#include "scheduler.h"

IOManager::IOManager(size_t threadSize, bool onCaller)
    : Scheduler(threadSize, onCaller),
      ioContext(10),
      tick(),
      pendingEventCount(0),
      epollTimeOut(2000) {
  int rt = 0;

  ep = epoll_create1(0);
  assert(rt >= 0);

  rt = pipe(tick);
  assert(rt >= 0);

  rt = fcntl(tick[0], F_SETFL, O_NONBLOCK);
  assert(rt >= 0);

  epoll_event event = {};
  event.data.fd = tick[0];
  event.events = EPOLLIN | EPOLLET;

  rt = epoll_ctl(ep, EPOLL_CTL_ADD, tick[0], &event);
  assert(rt >= 0);
}

IOManager::~IOManager() {
  stop();
  close(ep);
  close(tick[0]);
  close(tick[1]);
  std::cout << "IOManager::~IOManager\n";
}

void IOManager::idle() {
  int rt = 0;
  constexpr short EPOLL_EVENT_MAX = 256;
  epoll_event eventGroup[EPOLL_EVENT_MAX] = {};

  for (;;) {
    uint64_t nextTime = getNextTimer();
    uint64_t timeout = std::min(nextTime, (uint64_t)epollTimeOut);
    if (checkStoping() && nextTime == ~0ull) {
      std::cout << "\t\t[Stop]!\n";
      break;
    }

    rt = epoll_wait(ep, eventGroup, EPOLL_EVENT_MAX, timeout);

    if (rt < 0)
      break;
    else if (rt == 0)
      std::cout << "\t\t\t[epoll_wait timeout!]\n";

    std::vector<std::function<void()>> funcGroup = {};
    TimerManager::expiredGroup(funcGroup);
    for (auto &f : funcGroup) {
      std::cout << "[push-timer!]\n";
      push(task_t(f));
    }

    for (int i = 0; i < rt; ++i) {
      epoll_event &event = eventGroup[i];
      int triggerEventType = 0;

      if (event.data.fd == tick[0]) {
        char readbuf = 0;
        read(tick[0], &readbuf, 1);

        continue;
      }

      if (event.events & (EPOLLERR | EPOLLHUP))
        event.events |= (EPOLLIN | EPOLLOUT) & ioContext[i].eventType;

      if (event.events & EPOLLIN) triggerEventType |= EPOLLIN;
      if (event.events & EPOLLOUT) triggerEventType |= EPOLLOUT;

      if (triggerEventType == 0x0) continue;

      int clearEventType = ioContext[i].eventType & (~triggerEventType);
      int op = (clearEventType == 0x0) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
      event.events = EPOLLET | clearEventType;

      epoll_ctl(ep, op, ioContext[i].fd, &event);

      int index = event.data.fd;
      if (triggerEventType & EPOLLIN) {
        std::cout << "\t\t\t[push!]\n";
        // assert(ioContext[index].func != nullptr);
        trigger(ioContext[index], EPOLLIN);
      } else if (triggerEventType & EPOLLOUT) {
        std::cout << "\t\t\t[push!]\n";
        // assert(ioContext[index].func != nullptr);
        trigger(ioContext[index], EPOLLOUT);
      }
    }

    Coroutine::SharedPtr co = Coroutine::GetThis();
    auto cur = co.get();
    co.reset();
    cur->yield();
  }
}
void IOManager::tickle() {
  write(tick[1], "T", 1);
  std::cout << "\t\t\t[tickle!]\n";
}

void IOManager::add(epoll_event event, const std::function<void()> &callback) {
  int rt = 0;

  std::lock_guard<std::mutex> lock(ioMutex);

  int fd = event.data.fd;
  if (fd >= ioContext.size()) ioContext.resize(fd * 1.5);

  if (ioContext[fd].eventType & event.events) {
    std::cout << "\t\t\t[event-exist]!\n";
    assert(0);
  }

  epoll_event epEvent;
  epEvent.events = EPOLLET | event.events | ioContext[fd].eventType;
  epEvent.data.fd = event.data.fd;

  int op = (ioContext[fd].eventType == 0x0) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
  rt = epoll_ctl(ep, op, event.data.fd, &epEvent);
  assert(rt >= 0);

  if (ioContext[fd].func != nullptr) {
    std::cout << "\t\t\t[event-func exist]!\n";
    assert(0);
  }

  ioContext[fd] = IOContext(callback, fd, event.events);
  ioContext[fd].eventType |= event.events;

  ++pendingEventCount;
}

void IOManager::add(int fd, int eventType,
                    const std::function<void()> &callback) {
  epoll_event event;
  event.data.fd = fd;
  event.events = eventType;

  add(event, callback);
}

void IOManager::trigger(IOContext &ioc, int eventType) {
  if (ioc.func == nullptr) {
    auto cur = Coroutine::GetThis();
    assert(cur->getState() == Coroutine::RUNING);

    push(task_t(cur));
    return;
  }

  ioc.eventType = ioc.eventType & (~eventType);
  push(task_t(ioc.func));
  ioc.func = nullptr;

  --pendingEventCount;
}

bool IOManager::checkStoping() {
  return Scheduler::checkStoping() && pendingEventCount == 0 &&
         !TimerManager::hasTimer();
}

void IOManager::onTimerInsertAtFront() { tickle(); }

IOManager *IOManager::getThis() {
  return dynamic_cast<IOManager *>(Scheduler::getThis());
}
