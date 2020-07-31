// Copyright [2020] <Copyright pusidun@hotmail.com>
#include <iostream>

#include "swallow/base/redis.h"

void printReply(redisReply* ptr) {
  switch (ptr->type) {
    case REDIS_REPLY_STATUS:
      std::cout << "status reply. status:" << ptr->str << std::endl;
      break;
    case REDIS_REPLY_ERROR:
      std::cout << "error reply. error:" << ptr->str << std::endl;
      break;
    case REDIS_REPLY_INTEGER:
      std::cout << "reply int:" << ptr->integer << std::endl;
      break;
    case REDIS_REPLY_NIL:
      std::cout << "reply nil" << std::endl;
      break;
    case REDIS_REPLY_STRING:
      std::cout << "reply string:" << ptr->str << std::endl;
      break;
    case REDIS_REPLY_ARRAY:
      std::cout << "reply array :" << std::endl;
      for (unsigned int i = 0; i < ptr->elements; ++i)
        printReply(ptr->element[i]);
      break;
    default:
      break;
  }
}

int main() {
  swallow::Redis cli;
  cli.connect("127.0.0.1", 6379, 100);

  swallow::ReplyPtr reply = cli.cmd("SETNX foo bar");
  printReply(reply.get());
  reply = cli.cmd("GET foo");
  printReply(reply.get());

  // pipeline
  cli.appendCmd("hmset hash key1 val1 key2 val2");
  cli.appendCmd("hmget hash key1 key2");
  reply = cli.getReply();
  printReply(reply.get());
  reply = cli.getReply();
  printReply(reply.get());
  return 0;
}
