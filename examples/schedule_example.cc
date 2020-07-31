// Copyright [2020] <Copyright pusidun@hotmail.com>
#include <unistd.h>

#include "swallow/base/coroutine.h"
#include "swallow/base/log.h"
#include "swallow/base/schedule.h"

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
  sch.schedule(&foo, 0);
  sch.schedule(&foo2, 0);
  sch.start();
  sch.stop();
  return 0;
}
