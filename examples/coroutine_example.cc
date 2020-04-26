#include <unistd.h>

#include "coroutine.h"
#include "log.h"

void foo2(void* arg) {
  int* id = (int*)arg;
  swallow::Coroutine::ptr co = swallow::Coroutine::GetCo();
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT())
      << "thread " << *id << " before yield,state:" << co->getStat();
  co->yield();
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "thread " << *id << " after yield";
}

void* foo(void* arg) {
  int* tid = (int*)arg;
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "thread " << *tid << " start";
  swallow::Coroutine::ptr co(new swallow::Coroutine(1024 * 128, foo2, arg));
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT())
      << "thread " << *tid << " before first resume state:" << co->getStat();
  co->resume();
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT())
      << "thread " << *tid << " before second resume state:" << co->getStat();
  co->resume();
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "thread " << *tid << " after resume";
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "thread " << *tid << " end";
  return nullptr;
}

int main() {
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "start test coroutine";
  const int thread_num = 3;
  pthread_t tid[thread_num];
  for (int i = 0; i < thread_num; ++i) {
    pthread_create(&tid[i], nullptr, foo, &tid[i]);
  }

  for (int i = 0; i < thread_num; ++i) pthread_join(tid[i], nullptr);

  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "end test coroutine";
  return 0;
}
