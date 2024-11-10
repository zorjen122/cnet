#pragma once
#include <ucontext.h>
#include <functional>
#include <atomic>
#include <iostream>
#include <assert.h>
#include <memory>

#define STACK_SIZE 1024 * 10

class Coroutine : public std::enable_shared_from_this<Coroutine>
{
public:
  using SharedPtr = std::shared_ptr<Coroutine>;
  enum State
  {
    READY,
    RUNING,
    TERM
  };

protected:
  ucontext_t context;
  std::function<void()> func;
  State state;
  void *fstack;
  size_t stacksize;

  bool onScheduler;

public:
  Coroutine();
  Coroutine(std::function<void()> f, bool useScheduler = false, size_t stackSize = STACK_SIZE);

  static SharedPtr GetThis();
  static void SetThis(Coroutine *co);

  static void callFunction();

  void resume();
  void yield();
  void reset(std::function<void()> f);

  size_t getCount();
  size_t getId();
  short getState();
};
