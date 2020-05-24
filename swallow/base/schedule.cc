#include <functional>
#include <memory>
#include <unistd.h>
#include "schedule.h"
#include "log.h"

namespace swallow {
static std::shared_ptr<Logger> log = SWALLOW_LOG_GET("system");

static thread_local Coroutine::ptr s_t_idle = nullptr;
static thread_local Schedule* s_t_sch;

/*
* @params thread_num schedule模块初始化线程数
*/
Schedule::Schedule(int thread_num) : m_thread_num(thread_num) {
    if(thread_num < 1) m_thread_num = 1;
    m_threads.resize(m_thread_num);
    for(int i = 0; i < m_thread_num; ++i) {
        m_threads[i].reset(new Thread(std::bind(&Schedule::co_run, this), std::to_string(i)));
    }
    s_t_idle.reset(new Coroutine(32, std::bind(&Schedule::idle, this)));
}

Schedule::~Schedule(){}

Schedule* Schedule::GetThis() {
    return s_t_sch;
}

// 每个线程执行的函数，调度本线程内的协程
void Schedule::co_run() {
    std::cout<<"co_run"<<std::endl;
    s_t_sch = this;
    while(true) {
        Coroutine::ptr  run_co = nullptr;
        {
            std::lock_guard<std::mutex> guard(mtx_co);
            for(auto itr = m_co.begin(); itr != m_co.end();itr++) {
                if( itr->tid < 0 || itr->tid >= m_thread_num)
                    continue;
                if( itr->co->getStat() == Coroutine::DEAD) {
                    m_co.erase(itr);
                    continue;
                }
                if( itr->co->getStat() == Coroutine::READY || itr->co->getStat() == Coroutine::SUSPEND) {
                    run_co = itr->co;
                }
            }
            if(run_co != nullptr) {
                run_co->resume();
                run_co.reset();
            }
            else {
                idle();
            }
        }
    }
}

// 空转时执行，等待唤醒
void Schedule::idle() {
    SWALLOW_LOG_INFO(log) << "schedule idle...";
    sleep(1);
}

}
