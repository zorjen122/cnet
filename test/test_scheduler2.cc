#include <bits/stdc++.h>
#include "scheduler.h"
using namespace std;

Scheduler sch(4);

void foo()
{
  static thread_local int i = 0;
  std::cout << "thread-" << (size_t)std::hash<std::thread::id>()(std::this_thread::get_id()) % 10 << " foo " << ++i << " !\n ";
  std::this_thread::sleep_for(std::chrono::seconds(1));

  Coroutine::GetThis()->yield();
}

void test1()
{
  // this_thread::sleep_for(chrono::seconds(1));
  for (int i = 0; i < 2; ++i)
  {
    sch.push(Scheduler::task_t(&foo, 1));
  }
}

int main()
{
  thread t(test1);

  sch.start();

  getchar();
}