#include "hook.h"

#include "coroutine.h"
#include "iomanager.h"

namespace hook {
unsigned int Sleep(unsigned int second) {
  if (!enableHook) {
    std::cout << "\t\t[sys-sleep!]\n";
    return ::sleep(second);
  }

  Coroutine::SharedPtr co = Coroutine::GetThis();
  IOManager *iom = IOManager::getThis();

  iom->addTimer(
      second * 1000,
      std::bind((void (Scheduler::*)(Coroutine::SharedPtr, size_t thread)) &
                    IOManager::push,
                iom, co, 0));
  std::cout << "\t\t[Sleep-yield]!\n";

  Coroutine::GetThis()->yield();

  std::cout << "\t[Sleep!]\n";
  return 0;
}
void setEnableHook(bool flag) { enableHook = flag; }
};  // namespace hook
