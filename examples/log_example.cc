#include <iostream>
#include <time.h>
#include "log.h"
#include "unistd.h"

#define SWALLOW_LOG_LEVEL(logger, level)                                            \
    if (logger->getLevel() <= level)                                                   \
    swallow::LogEventWrapper(                                                       \
        swallow::LogEvent::ptr(new swallow::LogEvent(                               \
            logger, level, __FILE__, __LINE__, 0, 456,                              \
            123, time(0), "thread 0")))                                             \
        .getSS()


int main() {
    swallow::Logger::ptr logger(new swallow::Logger);
    swallow::LogFormatter::ptr fmt(new swallow::LogFormatter("%d  %f：%l  %t:%F  %p  %m%n"));

    swallow::StdoutLogAppender::ptr stdoutappender(new swallow::StdoutLogAppender);
    stdoutappender->setFormatter(fmt);
    stdoutappender->setLevel(swallow::LogLevel::DEBUG);
    logger->addAppender(stdoutappender);

    swallow::FileLogAppender::ptr file_appender(new swallow::FileLogAppender("./log.txt"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(swallow::LogLevel::DEBUG);
    logger->addAppender(file_appender);

    SWALLOW_LOG_LEVEL(logger, swallow::LogLevel::INFO)<<"log.h test succcess!";

    return 0;
}