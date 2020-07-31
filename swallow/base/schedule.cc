/**
 * @copyright Copyright [2020]
 * @author pusidun@hotmail.com
 * @file schedule.cc
 * @brief schedule class
 * @date 2020-03-18
 */
#include "swallow/base/schedule.h"

#include <unistd.h>

#include <functional>
#include <memory>

#include "swallow/base/log.h"

namespace swallow {
static std::shared_ptr<Logger> log = SWALLOW_LOG_GET("system");

static thread_local Coroutine::ptr s_t_idle = nullptr;
static thread_local Schedule* s_t_sch;

/*
 * @params thread_num schedule模块初始化线程数
 */
Schedule::Schedule(int thread_num) : m_thread_num(thread_num) {
  if (thread_num < 1) m_thread_num = 1;
  m_threads.resize(m_thread_num);
  m_stop = false;
}

Schedule::~Schedule() {}

Schedule* Schedule::GetThis() { return s_t_sch; }

// 每个线程执行的函数，调度本线程内的协程
void Schedule::co_run() {
  // 在线程执行函数处初始化本线程的idle协程,否则没有main co供切换
  if (!s_t_idle)
    s_t_idle.reset(new Coroutine(STACKSIZE, std::bind(&Schedule::idle, this)));
  s_t_sch = this;
  while (true) {
    Coroutine::ptr run_co = nullptr;
    {
      std::lock_guard<std::mutex> guard(mtx_co);
      SWALLOW_LOG_INFO(log) << "m_co size:" << m_co.size();
      for (auto itr = m_co.begin(); itr != m_co.end(); itr++) {
        if (itr->tid != -1 && itr->tid != Thread::GetThis()->getId()) continue;
        if (itr->co->getStat() == Coroutine::DEAD) {
          m_co.erase(itr);
          continue;
        }
        if (itr->co->getStat() == Coroutine::READY ||
            itr->co->getStat() == Coroutine::SUSPEND) {
          run_co = itr->co;
          m_running_co++;
          m_co.erase(itr);
          break;
        }
      }
    }
    if (run_co != nullptr) {
      run_co->resume();
      if (run_co->getStat() == Coroutine::DEAD) m_running_co--;
      SWALLOW_LOG_INFO(log) << run_co->getStat();
      run_co.reset();
    } else if (s_t_idle->getStat() != Coroutine::DEAD) {
      s_t_idle->resume();
    } else
      break;
  }
}

// 初始化线程
void Schedule::start() {
  if (m_stop) return;
  for (int i = 0; i < m_thread_num; ++i) {
    m_threads[i].reset(
        new Thread(std::bind(&Schedule::co_run, this), std::to_string(i)));
  }
}

// 等所有线程结束
void Schedule::stop() {
  m_stop = true;
  for (auto& i : m_threads) {
    i->join();
  }
}

bool Schedule::isStop() {
  std::lock_guard<std::mutex> guard(mtx_co);
  return m_stop && m_running_co == 0 && m_co.empty();
}

// yield
void Schedule::yield() {
  m_running_co--;
  auto curr_co = Coroutine::GetCo();
  {
    std::lock_guard<std::mutex> guard(mtx_co);
    m_co.push_back(CoInThreads(curr_co, Thread::GetThis()->getId()));
  }
  curr_co->yield();
}

// 空转时执行
void Schedule::idle() {
  while (!isStop()) {
    SWALLOW_LOG_INFO(log) << "schedule idle...";
    sleep(3);
    swallow::Coroutine::GetCo()->yield();
  }
}

}  // namespace swallow
