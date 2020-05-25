#include "schedule.h"
#include "log.h"
#include "coroutine.h"
#include <unistd.h>

void foo() {
    SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "foo before yield";
    swallow::Schedule* sch = swallow::Schedule::GetThis();
    sch->yield();
    SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "foo after yield";
}

void foo2() {
    SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "foo2 before yield";
    swallow::Schedule* sch = swallow::Schedule::GetThis();
    sch->yield();
    SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "foo2 after yield";
}

int main() {
    SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT()) << "create schedule";
    swallow::Schedule sch(1);
    sch.schedule(&foo,0);
    sch.schedule(&foo2,0);
    sch.start();
    sch.stop();
    return 0;
}
