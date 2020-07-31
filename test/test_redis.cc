// Copyright [2020] <Copyright pusidun@hotmail.com>
#include <gtest/gtest.h>

#include "swallow/base/redis.h"

TEST(RedisTest, HandleConnect) {
  {
    swallow::Redis cli;
    bool ret = cli.connect("127.0.0.1", 6379, 100);
    ASSERT_EQ(true, ret);
    if (!ret) {
      ret = cli.reconnect();
      EXPECT_EQ(true, ret);
    }
  }
}

TEST(RedisTest, HandleCmd) {
  swallow::Redis cli;
  bool ret = cli.connect("127.0.0.1", 6379, 100);
  ASSERT_EQ(true, ret);

  swallow::ReplyPtr reply = cli.cmd("flushdb");
  ASSERT_STREQ("OK", reply.get()->str);

  reply = cli.cmd("SETNX foo bar");
  ASSERT_EQ(reply.get()->type, REDIS_REPLY_INTEGER);
  ASSERT_EQ(1, reply.get()->integer);

  reply = cli.cmd("GET foo");
  ASSERT_EQ(reply.get()->type, REDIS_REPLY_STRING);
  ASSERT_STREQ("bar", reply.get()->str);
}

TEST(RedisTest, HandlePipline) {
  swallow::Redis cli;
  bool ret = cli.connect("127.0.0.1", 6379, 100);
  ASSERT_EQ(true, ret);

  swallow::ReplyPtr reply = cli.cmd("flushdb");
  ASSERT_STREQ("OK", reply.get()->str);

  cli.appendCmd("hmset hash key1 val1 key2 val2");
  cli.appendCmd("hmget hash key1 key2");
  reply = cli.getReply();
  ASSERT_EQ(reply.get()->type, REDIS_REPLY_STATUS);
  ASSERT_STREQ("OK", reply.get()->str);

  reply = cli.getReply();
  ASSERT_EQ(reply.get()->type, REDIS_REPLY_ARRAY);
  ASSERT_EQ(2, reply.get()->elements);
  ASSERT_STREQ("val1", reply.get()->element[0]->str);
  ASSERT_STREQ("val2", reply.get()->element[1]->str);
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
