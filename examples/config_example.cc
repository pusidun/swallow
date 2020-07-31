// Copyright [2019] <Copyright pusidun@hotmail.com>
#include <memory>

#include "config.h"
#include "lock.h"
#include "log.h"

using namespace swallow;

void print_cb(const float &old, const float &now) {
  SWALLOW_LOG_INFO(SWALLOW_LOG_ROOT())
      << "config change , old value :" << old << " new value:" << now;
}

int main() {
  swallow::Logger::ptr logger = SWALLOW_LOG_GET("config_example");
  logger->addAppender(
      std::make_shared<swallow::FileLogAppender>("config_test_log.txt"));
  logger->addAppender(std::make_shared<swallow::StdoutLogAppender>());
  SWALLOW_LOG_INFO(logger) << "config test begin";

  swallow::Configure<int>::ptr port =
      swallow::ConfigureManager::lookUp("system.port", "system port", 8080);
  swallow::Configure<float>::ptr rate =
      swallow::ConfigureManager::lookUp("system.rate", "cpu rate", 65.7f);

  SWALLOW_LOG_INFO(logger) << "get port value:" << port->getValue();
  SWALLOW_LOG_INFO(logger) << "get float value:" << rate->getValue();

  rate->addWatcher(print_cb);
  rate->setValue(40.5f);

  return 0;
}
