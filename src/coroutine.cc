#include "coroutine.h"
#include "scheduler.h"

namespace Co
{
  static thread_local std::shared_ptr<Coroutine> threadCoroutine = nullptr;
  static thread_local Coroutine *coroutine = nullptr;

  static std::atomic<size_t> count{0};
  static std::atomic<size_t> id{0};
};

Coroutine::Coroutine()
{
  SetThis(this);
  state = RUNING;
  // stacksize = STACK_SIZE;
  // fstack = malloc(STACK_SIZE);

  // func = nullptr;
  // state = READY;
  // context.uc_link = nullptr;
  // context.uc_stack.ss_sp = fstack;
  // context.uc_stack.ss_size = STACK_SIZE;

  if (getcontext(&context))
    assert(0);

  ++Co::count;
}

Coroutine::Coroutine(std::function<void()> f, bool useScheduler, size_t stackSize)
{
  fstack = malloc(stackSize);
  stacksize = stackSize;

  onScheduler = useScheduler;
  state = READY;
  func = f;
  context.uc_link = nullptr;
  context.uc_stack.ss_sp = fstack;
  context.uc_stack.ss_size = stackSize;

  if (getcontext(&context))
    assert(0);

  makecontext(&context, &Coroutine::callFunction, 0);

  Co::id = ++Co::count;
}

void Coroutine::callFunction()
{
  Coroutine::SharedPtr mainCo = GetThis();
  assert(mainCo->func != nullptr);

  mainCo->func();
  mainCo->func = nullptr;
  mainCo->state = TERM;

  Coroutine *co = mainCo.get();
  mainCo.reset();

  assert(co != nullptr);
  co->yield();
}

void Coroutine::resume()
{
  assert(state != RUNING || state != TERM);
  SetThis(this);
  state = RUNING;

  if (onScheduler)
  {
    if (swapcontext(&(Scheduler::GetCoroutine()->context), &context))
      assert(0);
  }
  else
  {
    if (swapcontext(&(Co::threadCoroutine->context), &context))
      assert(0);
  }

  // std::cout << "Coroutine::SetThis\n";
}

void Coroutine::yield()
{
  assert(state == RUNING || state == TERM);
  SetThis(Co::threadCoroutine.get());

  if (state == RUNING)
    state = READY;

  if (onScheduler)
  {
    if (swapcontext(&context, &(Scheduler::GetCoroutine()->context)))
      assert(0);
  }
  else
  {
    if (swapcontext(&context, &(Co::threadCoroutine->context)))
      assert(0);
  }
  // std::cout << "Coroutine::yield\n";
}

Coroutine::SharedPtr Coroutine::GetThis()
{
  if (Co::coroutine)
  {
    return Co::coroutine->shared_from_this();
  }

  // else coroutine and threadCoroutine is empty.
  // here, in mainCo call Coroutine::Coroutine(), will SetThis(this) <=> t_fiber = this [addr: &mainCo].
  Coroutine::SharedPtr mainCo = std::make_shared<Coroutine>();
  Co::threadCoroutine = mainCo;

  assert(Co::threadCoroutine.get() == Co::coroutine);

  // then this a case is return Co::threadCoroutine address of shared_ptr.
  return Co::coroutine->shared_from_this();
}

void Coroutine::SetThis(Coroutine *coPtr)
{
  Co::coroutine = coPtr;
}

size_t Coroutine::getCount()
{
  return Co::count;
}

size_t Coroutine::getId()
{
  return Co::id;
}
short Coroutine::getState()
{
  return state;
}

void Coroutine::reset(std::function<void()> f)
{
  assert(state == TERM && fstack != nullptr);

  if (getcontext(&context))
    assert(1);

  state = READY;
  func = f;
  context.uc_link = nullptr;
  context.uc_stack.ss_sp = fstack;
  context.uc_stack.ss_size = stacksize;

  makecontext(&context, &Coroutine::callFunction, 0);
}