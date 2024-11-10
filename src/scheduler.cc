#include "scheduler.h"
#include "hook.h"

namespace SchedulerUtil
{
  static thread_local Scheduler *scheduler = nullptr;

  static thread_local Coroutine *schedulerCo = nullptr;
};

Scheduler::Scheduler(size_t threadSize, bool useCaller)
    : threadPool(threadSize), threadId(threadSize),
      scheMutex(), isStop(false), isUseCaller(useCaller), activeThreadSize(0), activeIdleSize(0)
{
  poolSize = threadSize;

  if (useCaller)
  {
    SchedulerUtil::scheduler = this;
    --poolSize;

    Coroutine::GetThis();
    callerCo.reset(new Coroutine(std::bind(&Scheduler::run, this), false));
    SchedulerUtil::schedulerCo = callerCo.get();
    callerId = std::this_thread::get_id();
  }
}

Scheduler::~Scheduler()
{
  std::cout << "Scheduler::~Scheduler\n";
  std::cout << "Scheduler thread pool size: " << threadPool.size() << "\n";
}

Coroutine *Scheduler::GetCoroutine()
{
  return SchedulerUtil::schedulerCo;
}

void Scheduler::start()
{
  {
    std::lock_guard<std::mutex> lock(scheMutex);
    for (int i = 0; i < poolSize; ++i)
    {
      threadPool[i] = std::move(std::make_unique<std::thread>(std::bind(&Scheduler::run, this)));
      threadId[i] = threadPool[i]->get_id();
    }
  }

  if (isUseCaller)
    callerCo->resume();
}

void Scheduler::run()
{
  std::cout << "\t\t\t[Run]!\n";
  setThis(this);
  hook::setEnableHook(true);

  bool onTickle;

  if (std::this_thread::get_id() != callerId)
    SchedulerUtil::schedulerCo = Coroutine::GetThis().get();
  assert(SchedulerUtil::schedulerCo != nullptr);

  Coroutine::SharedPtr idleCo = std::make_shared<Coroutine>(std::bind(&Scheduler::idle, this), true);

  for (;;)
  {
    task_t execTask;
    {
      std::lock_guard<std::mutex> lock(scheMutex);

      auto task = taskGroup.begin();
      while (task != taskGroup.end())
      {
        size_t index = task->threadIndex;
        if (task->isPointThread && threadPool[index]->get_id() != std::this_thread::get_id())
        {
          // std::cout << "\t\t[no-point-this-thread]\n";
          onTickle = true;
          ++task;
          continue;
        }
        else if (task->isPointThread && threadPool[index]->get_id() == std::this_thread::get_id())
        {
          std::cout << "\t\t[point-this-thread] : idx-" << index << "\n";
        }

        execTask = *task;
        taskGroup.erase(task++);
        ++activeThreadSize;
        break;
      }
      onTickle |= !taskGroup.empty();
    }

    if (onTickle)
    {
      tickle();
      onTickle = false;
    }

    if (execTask.co)
    {
      execTask.co->resume();
      --activeThreadSize;

      std::cout << "\t\t[execTask] ok!\n";
    }
    else
    {
      if (idleCo->getState() == Coroutine::TERM)
      {
        // std::cout << "\t\t\t[idle-TERM]!\n";
        break;
      }

      // std::cout << "\t\t\t[idle]!\n";
      ++activeIdleSize;
      idleCo->resume();
      --activeIdleSize;
    }
  }
  std::cout << "\t\t[Scheduler::run exit!]\n";
}

void Scheduler::push(const Scheduler::task_t &task)
{
  taskGroup.push_back(task);
}

void Scheduler::push(const std::function<void()> &func, size_t thread)
{
  taskGroup.push_back(task_t(func, thread));
}

void Scheduler::push(Coroutine::SharedPtr coroutine, size_t thread)
{
  taskGroup.push_back(task_t(coroutine, thread));
}

void Scheduler::setThis(Scheduler *obj)
{
  SchedulerUtil::scheduler = obj;
}

void Scheduler::idle()
{
  while (!checkStoping())
  {
    // std::cout << "\t\t\t[idle-yield]!\n";
    Coroutine::GetThis()->yield();
  }
}
bool Scheduler::checkStoping()
{
  std::lock_guard<std::mutex> lock(scheMutex);

  // std::cout << "taskGroup.empty() = " << taskGroup.empty() << "\tactiveThreadSize = " << activeThreadSize << "\tisStop:" << isStop << "\n";
  return isStop && taskGroup.empty() &&
         activeThreadSize == 0;
}

void Scheduler::stop()
{
  if (isUseCaller)
    assert(getThis() == this);
  else
    assert(getThis() != this);

  std::cout << "isStop = true!\n";
  isStop = true;
  if (checkStoping())
    return;

  for (int i = 0; i < poolSize; ++i)
    tickle();

  if (isUseCaller)
    tickle();

  if (isUseCaller)
    callerCo->resume();

  for (auto &th : threadPool)
    th->join();

  threadPool.clear();
}

Scheduler *Scheduler::getThis()
{
  return SchedulerUtil::scheduler;
}

void Scheduler::tickle()
{
  // std::cout << "\t\t\t[tickle]!\n";
}