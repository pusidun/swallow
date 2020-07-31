// Copyright [2020] <Copyright pusidun@hotmail.com>
#include <gtest/gtest.h>

#include "log.h"

TEST(LogTest, HandleLogFormat) {
  swallow::Logger::ptr logger(new swallow::Logger);
  EXPECT_EQ("root", logger->getName());
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
