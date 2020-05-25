/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file thread.h
 * @brief thread class,using posix thread library
 * @date 2019-11-19
 */
#ifndef SWALLOW_BASE_THREAD_H_
#define SWALLOW_BASE_THREAD_H_

#include <pthread.h>
#include <semaphore.h>
#include <functional>
#include <memory>
#include <string>
#include <atomic>

namespace swallow {

class Semaphore {
 public:
  Semaphore(uint32_t cnt = 0);
  ~Semaphore();
  void wait();
  void post();

 private:
  Semaphore(const Semaphore&) = delete;
  Semaphore(const Semaphore&&) = delete;
  Semaphore& operator=(const Semaphore&) = delete;

 private:
  sem_t m_sem;
};

class Thread {
 public:
  typedef std::shared_ptr<Thread> ptr;

  Thread(std::function<void()> cb, const std::string& name);
  ~Thread();

  int64_t getId() const { return m_id; }
  const std::string& getName() const { return m_name; }

  void join();
  static Thread* GetThis();
  static const std::string& GetName();
  static void SetName(const std::string& name);

 private:
  Thread(const Thread&) = delete;
  Thread(const Thread&&) = delete;
  Thread& operator=(const Thread&) = delete;

  static void* run(void* args);

 private:
  int64_t m_id = -1;
  pthread_t m_thread = 0;
  std::function<void()> m_cb;
  std::string m_name;
  Semaphore m_sem;
};

}  // namespace swallow

#endif  // SWALLOW_BASE_THREAD_H_
