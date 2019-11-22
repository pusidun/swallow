// Copyright [2019] <Copyright pusidun@hotmail.com>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <iostream>
#include "log.h"

int64_t get_current_millis(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main() {
    swallow::Logger::ptr logger(new swallow::Logger);
    swallow::LogFormatter::ptr fmt(new swallow::LogFormatter("%f%T%l%T%d%T%p%T%r%T%c%m%n"));

    swallow::StdoutLogAppender::ptr stdoutappender(new swallow::StdoutLogAppender);
    stdoutappender->setFormatter(fmt);
    stdoutappender->setLevel(swallow::LogLevel::DEBUG);
    logger->addAppender(stdoutappender);

    swallow::FileLogAppender::ptr file_appender(new swallow::FileLogAppender("./log.txt"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(swallow::LogLevel::DEBUG);
    logger->addAppender(file_appender);

    // performance test
    /*
    uint64_t start_ts = get_current_millis();
    for (int i = 0; i < 1e6; ++i)
        SWALLOW_LOG_ERROR(logger) << "write something to log.my id is " << i;
    uint64_t end_ts = get_current_millis();
    SWALLOW_LOG_INFO(logger) << "time use:" << end_ts-start_ts;
    */

    auto l = SWALLOW_LOG_ROOT();
    SWALLOW_LOG_WARN(l)<<"LoggerMgr test";

    SWALLOW_LOG_FMT_WARN(l,"c style fmt:%s %d", "success", 12345);

    return 0;
}
