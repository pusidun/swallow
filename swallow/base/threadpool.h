/**
 * @copyright Copyright [2020]
 * @author pusidun@hotmail.com
 * @file threadpool.h
 * @brief thread pool using c++ standard library
 * @date 2020-10-26
 */
#ifndef SWALLOW_BASE_THREADPOOL_H_
#define SWALLOW_BASE_THREADPOOL_H_

#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

#include "sync_q.h"

namespace swallow {

const int MaxTask = 20;
class ThreadPool {
 public:
  typedef std::function<void()> Task;

  ThreadPool(int numThreads = std::thread::hardware_concurrency())
      : m_threadsize(numThreads), m_q(MaxTask) {
    Start();
  }

  ~ThreadPool() { Stop(); }

  void Stop() {
    std::call_once(m_flag, [this] { StopThreadGroup(); });
  }

  void AddTask(Task&& task) { m_q.Put(std::forward<Task>(task)); }

  void AddTask(const Task& task) { m_q.Put(task); }

 private:
  void Start() {
    m_running = true;
    for (int i = 0; i < m_threadsize; ++i) {
      m_threadgroup.push_back(std::make_shared<std::thread>([this]() {
        while (m_running) {
          std::list<Task> lst;
          m_q.Take(lst);
          for (auto& task : lst) {
            if (!m_running) return;
            task();
          }
        }
      }));
    }
  }

  void StopThreadGroup() {
    m_running = false;
    m_q.Stop();
    for (auto itr : m_threadgroup)
      if (itr) itr->join();
    m_threadgroup.clear();
  }

 private:
  std::list<std::shared_ptr<std::thread>> m_threadgroup;
  int m_threadsize;
  SyncQueue<Task> m_q;
  std::atomic_bool m_running;
  std::once_flag m_flag;
};
}  // namespace swallow

#endif  // SWALLOW_BASE_THREADPOOL_H_
