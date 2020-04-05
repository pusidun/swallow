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

class MutexLock {
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

class RWLock {
 public:
    RWLock() {
        pthread_rwlock_init(&m_rwlock, nullptr);
    }

    ~RWLock() {
        pthread_rwlock_destroy(&m_rwlock);
    }

    void rdlock() {
        pthread_rwlock_rdlock(&m_rwlock);
    }

    void wrlock() {
        pthread_rwlock_wrlock(&m_rwlock);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_rwlock);
    }

 private:
    pthread_rwlock_t m_rwlock;
};

template<typename T>
class RdLockGuard {
 public:
    explicit RdLockGuard(T& _rdlock): m_rdlock(_rdlock){ 
        m_rdlock.rdlock();
    }

    ~RdLockGuard() {
        m_rdlock.unlock();
    }

 private:
    T m_rdlock;
};

template<typename T>
class WrLockGuard {
 public:
    explicit WrLockGuard(T& _wrlock): m_wrlock(_wrlock){ 
        m_wrlock.wrlock();
    }

    ~WrLockGuard() {
        m_wrlock.unlock();
    }
    
 private:
    T m_wrlock;
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
