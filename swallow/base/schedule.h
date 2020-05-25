/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file schedule.h
 * @brief schedule class
 * @date 2020-04-14
 */
#ifndef SWALLOW_BASE_SCHEDULE_H_
#define SWALLOW_BASE_SCHEDULE_H_
#include <atomic>
#include <list>
#include <vector>
#include <mutex>
#include "thread.h"
#include "coroutine.h"

#define STACKSIZE 1024 * 1024

namespace swallow {

thread_local std::atomic<long long> co_id;

// 线程中的协程信息
class CoInThreads {
 public:
  CoInThreads(Coroutine::ptr p, int id = 0) : tid(id) {
    co.swap(p);
  }
  int64_t tid; //Schedule m_threads的下标
  Coroutine::ptr co;
};

/*
* @brief 维护线程队列，协程挂到空闲的线程执行
*/
class Schedule {
 public:

  Schedule(int thread_num = 1);

  ~Schedule();

  
  template<typename Func>
  void schedule(Func f, int threadid) {
    CoInThreads cothreads(std::make_shared<Coroutine>(STACKSIZE, f), threadid);
    std::lock_guard<std::mutex> guard(mtx_co);
    m_co.push_back(cothreads);
  }

  void start();
  void stop();
  bool isStop();

  //协程yield出来，加入待处理队列m_co
  void yield();

  // 线程空转时执行该协程
  virtual void idle();

  static Schedule* GetThis();

 private:
  // 线程中的协程运行函数
  void co_run();

 private:
  std::vector<Thread::ptr> m_threads;
  std::mutex mtx_co;
  std::list<CoInThreads> m_co;  // 待处理协程队列
  int m_thread_num;
  Coroutine::ptr root_co;
  std::atomic<int> m_running_co = {0};
  bool m_stop;
};

}  // namespace swallow

#endif
