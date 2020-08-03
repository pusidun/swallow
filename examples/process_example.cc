// Copyright [2020] <Copyright pusidun@hotmail.com>
#include <unistd.h>

#include <iostream>

#include "swallow/base/log.h"
#include "swallow/base/process.h"

auto root_log = SWALLOW_LOG_ROOT();

int callback(int argc, char** argv) {
  SWALLOW_LOG_INFO(root_log) << "callback start, argc:" << argc;
  for (int i = 0; i < argc; ++i) SWALLOW_LOG_INFO(root_log) << argv[i];
  for (int i = 0; i < 5; ++i) {
    SWALLOW_LOG_INFO(root_log) << "sleep";
    sleep(5);
  }
  return 0;
}

int main(int argc, char** argv) {
  SWALLOW_LOG_INFO(root_log) << "start";
  swallow::start_process(argc, argv, callback, true);
  SWALLOW_LOG_INFO(root_log) << swallow::ProcessMgr.getInstance()->toString();
  return 0;
}
