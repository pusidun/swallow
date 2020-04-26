#include "thread.h"

#include <functional>
#include <memory>


namespace swallow {

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_name = "UNKNOW";
/**
 * @brief Thread impl
 **/
Thread::Thread(std::function<void()> cb, const std::string& name)
    : m_cb(cb), m_name(name) {
  if (m_name.empty()) m_name = "UNKNOW";
  pthread_create(&m_thread, nullptr, Thread::run, this);
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

  std::function<void()> cb;
  cb.swap(t_thread->m_cb);
  cb();
  return 0;
}
}  // namespace swallow
