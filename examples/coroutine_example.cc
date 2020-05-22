#include <unistd.h>
#include <memory>
#include <cstdlib>
#include "thread.h"
#include "coroutine.h"
#include "log.h"

void foo2() {
  std::string name = swallow::Thread::GetName();
  swallow::Coroutine::ptr co = swallow::Coroutine::GetCo();
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT())
      << name << "foo2 before yield,state:" << co->getStat();
  co->yield(); // step 2
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << name << "foo2 after yield"; // step 4
}

void foo() {
  std::string name = swallow::Thread::GetName();
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << name << " start";
  swallow::Coroutine::ptr co(new swallow::Coroutine(1024 * 128, foo2));
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT())
      << name << "foo before first resume state:" << co->getStat();
  co->resume(); // step 1
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT())
      << name << "foo before second resume state:" << co->getStat();
  co->resume(); // step 3
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << name << "foo after resume"; // step 5
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << name << "foo end";
}

int main() {
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "start test coroutine";
  const int thread_num = 3;
  swallow::Thread::ptr threads[thread_num];
  for (int i = 0; i < thread_num; ++i) {
    char name[32];
    sprintf(name, "Thread %d ", i);
    threads[i] = std::make_shared<swallow::Thread>(foo, name);
  }

  for (int i = 0; i < thread_num; ++i) 
    threads[i]->join();

  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "end test coroutine";
  return 0;
}
