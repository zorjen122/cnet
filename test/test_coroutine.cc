#include <iostream>
#include <thread>
#include <chrono>
#include "coroutine.h"

void foo()
{
  static thread_local int i = 0;
  ++i;
  std::cout << "foo " << i << "\n";
  Coroutine::GetThis()->yield();
  ++i;
  std::cout << "foo " << i << "\n";
  Coroutine::GetThis()->yield();
}

#include <memory>
void test()
{
  // std::shared_ptr<Coroutine> co = std::make_shared<Coroutine>(&foo);
  Coroutine co(&foo);
  Coroutine::GetThis();
  std::cout << "count: " << Coroutine::GetThis()->getCount() << "\n";

  co.resume();
  std::cout << "test\n";
  std::this_thread::sleep_for(std::chrono::seconds(1));
  co.resume();
}

#include <thread>
#include <vector>
void test2()
{
  std::vector<std::thread> th;
  for (int i = 0; i < 3; ++i)
  {
    th.push_back(std::thread(&test));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "\n";
  }

  for (auto &it : th)
    it.join();

  std::cout << "Ok\n";
}

int main()
{
  test();
}