// Copyright [2019] <Copyright pusidun@hotmail.com>
#include <memory>
#include "log.h"
#include "config.h"
#include "lock.h"

using namespace swallow;

int main() {
    swallow::Logger::ptr logger = SWALLOW_LOG_GET("config_example");
    logger->addAppender(std::make_shared<swallow::FileLogAppender>("config_test_log.txt"));
    logger->addAppender(std::make_shared<swallow::StdoutLogAppender>());
    SWALLOW_LOG_INFO(logger)<<"config test begin";

    swallow::Configure<int>::ptr port = 
        swallow::ConfigureManager::lookUp("system.port", "system port", 8080);
    swallow::Configure<float>::ptr ft = 
        swallow::ConfigureManager::lookUp("system.float", "system float", 89.7f);

    SWALLOW_LOG_INFO(logger)<<"get port value:"<<port->getValue();
    SWALLOW_LOG_INFO(logger)<<"get float value:"<<ft->getValue();
    return 0;
}