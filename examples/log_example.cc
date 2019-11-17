#include <iostream>
#include <time.h>
#include "log.h"
#include "unistd.h"

int main()
{
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

    SWALLOW_LOG_INFO(logger)<<"log.h test succcess!";
    SWALLOW_LOG_DEBUG(logger)<<"debug level test";
    auto l = swallow::LoggerMgr::getInstance()->getLogger("root");
    SWALLOW_LOG_WARN(l)<<"LoggerMgr test";

    SWALLOW_LOG_FMT_WARN(l,"c style fmt:%s %d", "success", 12345);

    return 0;
}