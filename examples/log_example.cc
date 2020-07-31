// Copyright [2019] <Copyright pusidun@hotmail.com>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <iostream>

#include "swallow/base/log.h"

int64_t get_current_millis(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
  swallow::Logger::ptr logger(new swallow::Logger);
  swallow::LogFormatter::ptr fmt(
      new swallow::LogFormatter("%f%T%l%T%d%T%p%T%r%T%c%m%n"));

  swallow::StdoutLogAppender::ptr stdoutappender(
      new swallow::StdoutLogAppender);
  stdoutappender->setFormatter(fmt);
  stdoutappender->setLevel(swallow::LogLevel::DEBUG);
  logger->addAppender(stdoutappender);

  swallow::FileLogAppender::ptr file_appender(
      new swallow::FileLogAppender("./log.txt"));
  file_appender->setFormatter(fmt);
  file_appender->setLevel(swallow::LogLevel::DEBUG);
  logger->addAppender(file_appender);

  SWALLOW_LOG_INFO(logger) << "TEST stdout and file output logs";

  auto root_log = SWALLOW_LOG_ROOT();
  SWALLOW_LOG_WARN(root_log) << "root log test, micros test.";
  SWALLOW_LOG_FMT_WARN(root_log, "c style fmt:%s %d", "success", 0);

  return 0;
}
