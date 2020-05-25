#include <functional>
#include <memory>
#include <iostream>
#include "thread.h"
#include "log.h"

namespace swallow {

//当前线程和当前线程名
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_name = "UNKNOW";

static swallow::Logger::ptr g_logger = SWALLOW_LOG_GET("system");

std::atomic<uint64_t> tid = {0};

/**
 * @brief Semaphore impl
 **/
Semaphore::Semaphore(uint32_t cnt) {
  if (sem_init(&m_sem, 0, cnt)) throw std::logic_error("sem init failed");
}

Semaphore::~Semaphore() { sem_destroy(&m_sem); }

void Semaphore::wait() {
  if (sem_wait(&m_sem)) throw std::logic_error("sem_wait error");
}

void Semaphore::post() {
  if (sem_post(&m_sem)) throw std::logic_error("sem_post error");
}

/**
 * @brief Thread impl
 **/
Thread::Thread(std::function<void()> cb, const std::string& name)
    : m_cb(cb), m_name(name), m_sem(1) {
  if (m_name.empty()) m_name = "UNKNOW";
  int ret = pthread_create(&m_thread, nullptr, Thread::run, this);
  if( ret ) {
    SWALLOW_LOG_ERROR(g_logger) << "pthread create failed";
  }
  m_id = tid;
  ++tid;
  m_sem.wait();
}

Thread::~Thread() { pthread_detach(m_thread); }

void Thread::join() {
  pthread_join(m_thread, nullptr);
  m_thread = 0;
}

Thread* Thread::GetThis() { return t_thread; }

const std::string& Thread::GetName() { return t_name; }

void Thread::SetName(const std::string& name) {
  if (name.empty()) return;
  if (t_thread) t_thread->m_name = name;
  t_name = name;
}

void* Thread::run(void* args) {
  Thread* thread = (Thread*)args;
  t_thread = thread;
  t_name = t_thread->m_name;
  pthread_setname_np(pthread_self(), t_name.substr(0, 15).c_str());

  //设置线程的取消状态和取消类型
  //允许取消
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
  //取消后立即退出，可能存在资源未释放问题，用户线程需要做好应对
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, nullptr);

  std::function<void()> cb;
  cb.swap(t_thread->m_cb);
  thread->m_sem.post();
  cb();
  return 0;
}

}  // namespace swallow
