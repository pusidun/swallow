/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file coroutinue.cc
 * @brief coroutinue class
 * @date 2020-04-14
 */
#include <iostream>
#include <cassert>
#include "log.h"
#include "coroutine.h"

namespace swallow {

static thread_local std::shared_ptr<Coroutine> s_main_co;  // 主协程,所以协程和主协程切换
static thread_local Coroutine* s_curr_co = nullptr;        // 当前运行协程

Coroutine::Coroutine(uint64_t stack_size, CB cb, void* args): \
    stacksize(stack_size), m_cb(cb), m_args(args) {
    m_stack = (char *)malloc(stacksize);
    assert(m_stack != nullptr);
    getcontext(&m_ctx);
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = stacksize;
    m_ctx.uc_link = nullptr;

    m_state = READY;
    //makecontext的变长参数默认大小是sizeof(int), 64bit机器指针默认8Byte,直接传有问题
    //拆分成2个32b的指针
    uintptr_t ptr = (uintptr_t)args;
    makecontext(&m_ctx, (void (*)(void))(MainFunc), 2, ptr, ptr >> 32);

    // 第一次运行时主协程为空,初始化线程局部变量
    if(!s_main_co){
        Coroutine::ptr mainco(new Coroutine);
        s_main_co = mainco;
    }
}

Coroutine::~Coroutine() {
    if(m_stack)
        free(m_stack);
}

Coroutine::ptr Coroutine::GetCo() {
    if(s_curr_co)
        return s_curr_co->shared_from_this();
    return nullptr;
}

void Coroutine::SetCo(Coroutine* co) {
    s_curr_co = co;
}

/*
* @brief 挂起当前协程,切回主协程
*/
void Coroutine::yield() {
    SetCo(s_main_co.get());
    if(m_state == RUNNING) {
        m_state = SUSPEND;
    }
    if(m_state == DEAD || m_state == SUSPEND) {
        s_main_co->m_state = RUNNING;
        swapcontext(&m_ctx, &(s_main_co->m_ctx));
    }
}

/*
* @brief 恢复协程
*/
void Coroutine::resume() {
    SetCo(this);
    if(m_state == READY || m_state == SUSPEND) {
        m_state = RUNNING;
        s_main_co->m_state = SUSPEND;
        swapcontext(&(s_main_co->m_ctx), &m_ctx);
    }
}

void Coroutine::MainFunc(uint32_t low32, uint32_t hi32) {
    Coroutine::ptr co = GetCo();
    uintptr_t arg =  ((uintptr_t)hi32 << 32) | (uintptr_t)low32 ;
    try {
        assert(co->m_state == RUNNING);
        co->m_cb((void*)arg);
        co->m_cb = nullptr;
        co->m_state = DEAD;
    } catch(...) {

    }
    auto r = co.get();
    co.reset();
    r->yield();
}

}  // namespace swallow
