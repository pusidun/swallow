#include "schedule.h"
#include "log.h"
#include "coroutine.h"

void foo() {
    swallow::Coroutine::ptr co = swallow::Coroutine::GetCo();
    SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "foo before yield";
    co->yield();
    SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "foo after yield";
}

void foo2() {
    swallow::Coroutine::ptr co = swallow::Coroutine::GetCo();
    SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "foo2 before yield";
    co->yield();
    SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "foo2 after yield";
}

int main() {
    SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "create schedule";
    swallow::Schedule sch(2);
    sch.schedule(&foo,1);
    sch.schedule(&foo2,2);
    while(1);
    return 0;
}
