/**
 * @copyright Copyright [2019]
 * @author pusidun@hotmail.com
 * @file lock.h
 * @brief log class
 * @date 2019-11-12
 */
#ifndef SWALLOW_BASE_LOCK_H_
#define SWALLOW_BASE_LOCK_H_
#include <pthread.h>
#include <iostream>
#include "nocopyable.h"

namespace swallow {

class MutexLock: public nocopyable {
 public:
    MutexLock() {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~MutexLock() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

 private:
    pthread_mutex_t m_mutex;
};

class MutexLockGuard: nocopyable {
 public:
    explicit MutexLockGuard(MutexLock& _mutex): m_mutex(_mutex) {
        m_mutex.lock();
    }

    ~MutexLockGuard() {
        m_mutex.unlock();
    }

 private:
    MutexLock& m_mutex;
};

}  // namespace swallow

#endif  // SWALLOW_BASE_LOCK_H_
