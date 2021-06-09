// Copyright [2021] <Copyright pusidun@hotmail.com>
#include <iostream>
#include <thread>
#include "swallow/base/ratelimiter.h"
#include "swallow/base/log.h"

int main() {
    auto logger = SWALLOW_LOG_GET("example_log");
    swallow::RateLimiter ratelimiter(5);

    auto func = [&](int tid){
        for(int i=1; i<=10; ++i) {
            double wait_time = ratelimiter.acquire(i);
            SWALLOW_LOG_INFO(logger) << tid << " acquire " << i << " wait " << wait_time << " seconds";
        }
    };
    std::thread t1(func, 1);
    std::thread t2(func, 2);
    t1.join();
    t2.join();
    return 0;
}