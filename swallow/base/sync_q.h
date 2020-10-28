/**
 * @copyright Copyright [2020]
 * @author pusidun@hotmail.com
 * @file sync_q.h
 * @brief sync queue using c++ standard library
 * @date 2020-10-26
 */
#ifndef SWALLOW_BASE_SYNC_Q_H_
#define SWALLOW_BASE_SYNC_Q_H_

#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>

namespace swallow {

/* thread safe */
template <typename T>
class SyncQueue {
 public:
  SyncQueue(int size) : m_size(size), m_stop(false) {}

  void Put(const T& x) { Add(x); }

  void Put(T&& x) { Add(std::forward<T>(x)); }

  void Take(std::list<T>& list) {
    std::unique_lock<std::mutex> guard(m_mutex);
    m_notEmpty.wait(guard, [this]() { return m_stop || NotEmpty(); });
    if (m_stop) return;
    list = std::move(m_queue);
    m_notFull.notify_one();
  }

  void Take(T& t) {
    std::unique_lock<std::mutex> guard(m_mutex);
    m_notEmpty.wait(guard, [this] { return m_stop || NotEmpty(); });
    if (m_stop) return;
    t = m_queue.front();
    m_queue.pop_front();
    m_notFull.notify_one();
  }

  void Stop() {
    {
      std::lock_guard<std::mutex> guard(m_mutex);
      m_stop = true;
    }
    m_notFull.notify_all();
    m_notEmpty.notify_all();
  }

 private:
  bool Empty() {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_queue.empty();
  }

  bool Full() {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_queue.size() == m_size;
  }
  size_t Size() {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_queue.size();
  }

  bool NotFull() const {
    bool full = m_queue.size() >= m_size;
    return !full;
  }

  bool NotEmpty() const {
    bool empty = m_queue.empty();
    return !empty;
  }

  template<typename F>
  void Add(F&& x) {
    std::unique_lock<std::mutex> guard(m_mutex);
    m_notFull.wait(guard, [this] { return m_stop || NotFull(); });
    if (m_stop) return;
    m_queue.push_back(x);
    m_notEmpty.notify_one();
  }

 private:
  std::list<T> m_queue;
  std::mutex m_mutex;
  std::condition_variable m_notEmpty;
  std::condition_variable m_notFull;
  size_t m_size;
  bool m_stop;
};
}  // namespace swallow

#endif  // SWALLOW_BASE_SYNC_Q_H_
