// Copyright [2019] <Copyright pusidun@hotmail.com>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <iostream>
#include <memory>

#include "swallow/base/log.h"

int64_t get_current_millis(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
  /* 通过LoggerMgr去创建、获取log对象 */
  auto logger = SWALLOW_LOG_GET("example_log");
  auto fmt = std::make_shared<swallow::LogFormatter>("%f%T%l%T%d%T%p%T%r%T%c %m%n");

  /* add stdout appender */
  auto stdoutappender = std::make_shared<swallow::StdoutLogAppender>();
  stdoutappender->setFormatter(fmt);
  stdoutappender->setLevel(swallow::LogLevel::DEBUG);
  logger->addAppender(stdoutappender);

  auto file_appender = std::make_shared<swallow::AsyncFileLogAppender>("./log.txt");
  file_appender->setFormatter(fmt);
  file_appender->setLevel(swallow::LogLevel::DEBUG);
  logger->addAppender(file_appender);

  SWALLOW_LOG_DEBUG(logger) << "TEST stdout and file output logs";
  SWALLOW_LOG_INFO(logger) << "TEST stdout and file output logs";
  SWALLOW_LOG_WARN(logger) << "TEST stdout and file output logs";
  SWALLOW_LOG_FATAL(logger) << "TEST stdout and file output logs";
  SWALLOW_LOG_ERROR(logger) << "TEST stdout and file output logs";

  /* 默认日志api.默认日志只输出到标准输出 */
  swallow::debug("default debug");
  swallow::info("default info");
  swallow::warn("default warn");
  swallow::fatal("default info");
  swallow::error("default info");
  return 0;
}
