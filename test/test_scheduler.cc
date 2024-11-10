#include "scheduler.h"
#include "coroutine.h"

#include <bits/stdc++.h>

void goo()
{
  static thread_local int i = 0;
  std::cout << "thread-" << (size_t)std::hash<std::thread::id>()(std::this_thread::get_id()) % 10 << " [goo] " << ++i << " !\n ";
  std::this_thread::sleep_for(std::chrono::seconds(1));

  Coroutine::GetThis()->yield();

  std::cout << "sub-co: goo by resume\n";
}

void foo()
{
  static thread_local int i = 0;
  std::cout << "thread-" << (size_t)std::hash<std::thread::id>()(std::this_thread::get_id()) % 10 << " foo " << ++i << " !\n ";
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

void eoo()
{
  static thread_local int i = 0;
  std::cout << "thread-" << (size_t)std::hash<std::thread::id>()(std::this_thread::get_id()) % 10 << " foo " << ++i << " !\n ";
  std::this_thread::sleep_for(std::chrono::seconds(1));

  Coroutine::SharedPtr co = std::make_shared<Coroutine>(&goo);
  Coroutine::GetThis();

  co->resume();

  std::cout << "sub-co: foo by yield\n";
  std::this_thread::sleep_for(std::chrono::seconds(1));

  co->resume();
  std::cout << "sub-co: foo by auto-yield\n";
}

int main()
{
  Scheduler sch(1, true);

  for (int i = 0; i < 3; ++i)
  {
    Scheduler::task_t task(&foo);
    sch.push(task);
  }

  sch.start();

  Coroutine::GetThis();
  Coroutine::SharedPtr co(new Coroutine(&foo, true));
  // Scheduler::task_t task(co.get());
  // sch.push(task);

  sch.push(co);

  sch.stop();

  getchar();
}