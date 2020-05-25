/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file coroutinue.cc
 * @brief coroutinue class
 * @date 2020-04-14
 */
#include <cassert>
#include <iostream>
#include "coroutine.h"
#include "log.h"

namespace swallow {
// 主协程,所有协程和主协程切换
static thread_local std::shared_ptr<Coroutine> s_main_co = nullptr;
// 当前运行协程
static thread_local Coroutine* s_curr_co = nullptr;

Coroutine::Coroutine(uint64_t stack_size, CB cb)
    : stacksize(stack_size), m_cb(cb) {
  m_stack = (char*)malloc(stacksize);
  assert(m_stack != nullptr);
  getcontext(&m_ctx);
  m_ctx.uc_stack.ss_sp = m_stack;
  m_ctx.uc_stack.ss_size = stacksize;
  m_ctx.uc_link = nullptr;

  m_state = READY;
  makecontext(&m_ctx, MainFunc, 0);

  // 第一次运行时主协程为空,初始化线程局部变量
  if (!s_main_co) {
    Coroutine::ptr mainco(new Coroutine);
    s_main_co = mainco;
  }
}

Coroutine::~Coroutine() {
  if (m_stack) free(m_stack);
}

Coroutine::ptr Coroutine::GetCo() {
  if (s_curr_co) return s_curr_co->shared_from_this();
  return nullptr;
}

void Coroutine::SetCo(Coroutine* co) { s_curr_co = co; }

/*
 * @brief 挂起当前协程,切回主协程: 1.当前协程设置为主协程 2.RUNNING状态修改为SUSPEND 3.主协程设置为运行,切换
 */
void Coroutine::yield() {
  SetCo(s_main_co.get());
  if (m_state == RUNNING) {
    m_state = SUSPEND;
  }
  if (m_state == DEAD || m_state == SUSPEND) {
    s_main_co->m_state = RUNNING;
    swapcontext(&m_ctx, &(s_main_co->m_ctx));
  }
}

/*
 * @brief 恢复协程 1.当前协程设置为本协程 2.READY SUSPEND状态下,切换到本协程
 */
void Coroutine::resume() {
  SetCo(this);
  if (m_state == READY || m_state == SUSPEND) {
    m_state = RUNNING;
    assert(s_main_co != nullptr);
    s_main_co->m_state = SUSPEND;
    swapcontext(&(s_main_co->m_ctx), &m_ctx);
  }
}

void Coroutine::MainFunc() {
  Coroutine::ptr co = GetCo();
  try {
    assert(co->m_state == RUNNING);
    co->m_cb();
    co->m_cb = nullptr;
    co->m_state = DEAD;
  } catch (...) {
    throw std::logic_error("coroutine failed");
  }
  // co会让引用计数+1 这里需要释放一次.执行完毕yield到主协程
  auto r = co.get();
  co.reset();
  r->yield();
}

}  // namespace swallow
