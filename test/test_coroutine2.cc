#include "coroutine.h"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
using namespace std;

void foo()
{
  cout << "foo() call\n";
  for (int i = 0; i < 10; ++i)
  {
    for (int j = 0; j < i; ++j)
    {
      cout << "*";
    }
    cout << "\n";
  }
  cout << "foo() is ok!\n";
}

void foo2()
{

  cout << "foo2() call\n";
  for (int i = 9; i >= 0; --i)
  {
    for (int j = 0; j < i; ++j)
    {
      cout << "*";
    }
    Coroutine::GetThis()->yield();
    cout << "\n";
  }
  cout << "foo2() is ok!\n";
}

void test()
{
  shared_ptr<Coroutine> co = make_shared<Coroutine>(&foo);
  assert(co->getState() == Coroutine::READY);
  Coroutine::GetThis();
  co->resume();

  assert(co->getState() == Coroutine::TERM);

  co->reset(foo2);
  co->resume();

  for (int i = 0; i < 9; ++i)
  {
    cout << "-" << i;
    co->resume();
    this_thread::sleep_for(chrono::seconds(1));
  }
  co->resume();
  cout << "resume-foo2 is over\n";
}

int main()
{
  test();
}