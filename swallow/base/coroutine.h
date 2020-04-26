/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file coroutinue.h
 * @brief coroutinue class
 * @date 2020-04-14
 */
#ifndef SWALLOW_BASE_COROUTINE_H_
#define SWALLOW_BASE_COROUTINE_H_
#include <ucontext.h>

#include <cassert>
#include <functional>
#include <memory>

#include "log.h"

namespace swallow {

class Coroutine : public std::enable_shared_from_this<Coroutine> {
 public:
  typedef std::shared_ptr<Coroutine> ptr;
  typedef void (*CB)(void*);

  enum coState { READY = 0, RUNNING = 1, SUSPEND = 2, DEAD = 3 };

  Coroutine(uint64_t stack_size, CB cb, void* args);

  ~Coroutine();

  coState getStat() const { return m_state; }

  /*
   * @brief 挂起当前协程,切回主协程
   */
  void yield();

  /*
   * @brief 启动/恢复协程
   */
  void resume();

  /*
   * @brief 获取当前正在运行的协程
   */
  static Coroutine::ptr GetCo();

  /*
   * @brief 设置当前正在运行的协程
   */
  static void SetCo(Coroutine* co);

  static void MainFunc(uint32_t low32, uint32_t hi32);

 private:
  /*
   * @brief 初始化主协程,仅当s_main_co为空时会调用
   */
  Coroutine() {
    m_state = READY;
    SetCo(this);
    getcontext(&m_ctx);
  }

 private:
  ucontext_t m_ctx;
  coState m_state;
  char* m_stack;
  uint64_t stacksize = 1024 * 128;
  CB m_cb;
  void* m_args;

  Logger::ptr m_log = SWALLOW_LOG_GET("coroutine");
};

}  // namespace swallow

#endif  // SWALLOW_BASE_COROUTINE_H_
