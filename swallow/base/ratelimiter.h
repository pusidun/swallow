/**
 * @copyright Copyright [2021]
 * @author pusidun@hotmail.com
 * @file ratelimiter.h
 * @brief ratelimiter header
 * @date 2021-06-08
 */
#ifndef SWALLOW_BASE_RATELIMITER_H_
#define SWALLOW_BASE_RATELIMITER_H_

#include <cstdint>
#include <sys/time.h>
#include <mutex>
#include <unistd.h>

namespace swallow
{
//  token bucket. Refer to Guava
class RateLimiter {
 public:
  // smooth bursty
  RateLimiter(double permits_per_sec=1):stored_permits(0),next_free_ticket_micros(now()),
                                        stable_interval_micros(1.0/permits_per_sec),max_permits(1){};

  void setMaxPermits(double mp) {
    std::lock_guard<std::mutex> guard(m_mutex);
    max_permits = mp;
  }

  void doSetRate(double permits_per_sec, long now_micros) {
    std::lock_guard<std::mutex> guard(m_mutex);
    rsync(now_micros);
    stable_interval_micros = 1.0/permits_per_sec;
  }

  //@brief Acquire the given number of @param[permits] if it can be obtained 
  //       without exceeding the specified @param[timeout]
  bool tryAcquire(int permits, double timeout) {
    std::lock_guard<std::mutex> guard(m_mutex);
    double now_time = now();
    rsync(now_time);
    if(permits <= stored_permits) {
      stored_permits -= permits;
      return true;
    }
    double next_time = std::max(next_free_ticket_micros,now_time) + (permits - stored_permits)*stable_interval_micros;
    double wait_time = next_time-now_time;
    if(wait_time > timeout)
      return false;
    stored_permits = 0;
    next_free_ticket_micros = next_time;
    usleep(wait_time*1000000);
    return wait_time;
  }

  //  Acquires the given number of permits from @param[permits], blocking until the request
  //  can be granted. Tells the amount of time slept, if any.
  double acquire(int permits) {
    std::lock_guard<std::mutex> guard(m_mutex);
    double wait_time = 0;
    double now_time = now();
    rsync(now_time);
    if(permits <= stored_permits) {
      stored_permits -= permits;
      return wait_time;
    }
    double next_time = std::max(next_free_ticket_micros,now_time) + (permits - stored_permits)*stable_interval_micros;
    wait_time = next_time-now_time;
    stored_permits = 0;
    next_free_ticket_micros = next_time;
    usleep(wait_time*1000000);
    return wait_time;
  }

  double acquire(){return acquire(1);}

 private:
  long now() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec;
  }

  void rsync(long now_time) {
    if(now_time > next_free_ticket_micros){
      double new_permits = (now_time-next_free_ticket_micros)/stable_interval_micros;
      stored_permits = std::min(max_permits, stored_permits+new_permits);
      next_free_ticket_micros = now_time;
    }
  }

 private:
  std::mutex m_mutex;
  double stored_permits;  // 桶中令牌
  double next_free_ticket_micros;  //下次获得令牌时间点
  double stable_interval_micros;  //放入令牌时间间隔
  double max_permits;   // 桶中最多可以保存的令牌数
};
} // namespace swallow

#endif  // SWALLOW_BASE_RATELIMITER_H_