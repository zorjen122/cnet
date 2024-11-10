#pragma once
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <list>

#include "coroutine.h"

class SchedulerTask
{
public:
  Coroutine::SharedPtr co;
  std::function<void()> func;
  size_t threadIndex;
  bool isPointThread;

  SchedulerTask() {}
  SchedulerTask(std::function<void()> f, size_t thread = 0)
  {
    co.reset(new Coroutine(f, true));
    isPointThread = thread == 0 ? 0 : 1;
    std::cout << "\t\tcount: " << co.use_count() << "\n";

    threadIndex = thread;
  }
  SchedulerTask(Coroutine::SharedPtr coroutine, size_t thread = 0)
  {
    assert(coroutine);

    isPointThread = thread == 0 ? 0 : 1;

    co = coroutine;
    std::cout << "\t\tcount: " << co.use_count() << "\n";

    threadIndex = thread;
  }

  // coroutine need SharedPtr created.
  SchedulerTask(Coroutine *coroutine, size_t thread = 0)
  {
    assert(coroutine);

    isPointThread = thread == 0 ? 0 : 1;

    co.reset(coroutine);

    threadIndex = thread;
  }
};

class Scheduler
{
public:
  using task_t = SchedulerTask;
  using threadId_t = std::thread::id;

  virtual void idle();
  virtual void tickle();
  virtual bool checkStoping();
  virtual ~Scheduler();

  Scheduler(size_t threadSize = 1, bool useCaller = true);

  void push(const task_t &task);
  void push(const std::function<void()> &func, size_t thread = 0);
  void push(Coroutine::SharedPtr coroutine, size_t thread = 0);
  void run();
  void start();
  void stop();

  static Coroutine *GetCoroutine();

  void setThis(Scheduler *obj);
  static Scheduler *getThis();

private:
  std::list<task_t> taskGroup;

  size_t poolSize;
  std::vector<std::unique_ptr<std::thread>> threadPool;
  std::vector<threadId_t> threadId;

  std::mutex scheMutex;

  Coroutine::SharedPtr callerCo;
  bool isUseCaller;
  std::thread::id callerId;

  bool isStop;
  size_t activeThreadSize;
  size_t activeIdleSize;
};