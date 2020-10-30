// Copyright [2019] <Copyright pusidun@hotmail.com>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <memory>

#include "swallow/base/log.h"

void bench(int howmany, std::shared_ptr<swallow::Logger> logger);

int main(int argc, char *argv[]) {
  int howmany = 0;
  try {
    if (argc > 1) howmany = atoi(argv[1]);
    // Async
    std::cout << "=================Async Benchmark================="
              << std::endl;
    auto async_log = SWALLOW_LOG_GET("async");
    auto async_file_appender =
        std::make_shared<swallow::AsyncFileLogAppender>("./async_bench.txt");
    async_file_appender->setLevel(swallow::LogLevel::DEBUG);
    async_log->addAppender(async_file_appender);
    bench(howmany, std::move(async_log));

    // Sync
    std::cout << "=================Sync Benchmark=================="
              << std::endl;
    auto sync_log = SWALLOW_LOG_GET("sync");
    auto sync_file_appender =
        std::make_shared<swallow::FileLogAppender>("./sync_bench.txt");
    sync_file_appender->setLevel(swallow::LogLevel::DEBUG);
    sync_log->addAppender(sync_file_appender);
    bench(howmany, std::move(sync_log));
  } catch (std::exception &e) {
    SWALLOW_LOG_ERROR(SWALLOW_LOG_ROOT()) << e.what();
    return -1;
  }
  std::this_thread::sleep_for(std::chrono::seconds(2));
  std::cout << "=================All FINISHED====================" << std::endl;
  return 0;
}

void bench(int howmany, std::shared_ptr<swallow::Logger> logger) {
  using std::chrono::high_resolution_clock;
  auto start = high_resolution_clock::now();

  for (auto i = 0; i < howmany; ++i) {
    SWALLOW_LOG_INFO(logger) << "This is logger appender!";
  }
  auto delta = high_resolution_clock::now() - start;
  auto delta_d =
      std::chrono::duration_cast<std::chrono::duration<double>>(delta).count();
  std::cout << "Elapsed:{" << delta_d << " secs} " << int(howmany / delta_d)
            << "/sec" << std::endl;
}
