#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__

#include <pthread.h>

#include "NonCopyable.h"

class MutexLock : NonCopyable {
   public:
    MutexLock();
    ~MutexLock();

    //上锁与解锁
    void lock();
    void trylock();
    void unlock();

    //获取数据成员的函数get
    pthread_mutex_t *getMutexLockPtr();

   private:
    pthread_mutex_t _mutex;  //
};

class MutexLockGuard {
   public:
    //在构造函数中初始化资源(上锁)
    MutexLockGuard(MutexLock &mutex) : _mutex(mutex) { _mutex.lock(); }

    //在析构函数中释放资源(解锁)
    ~MutexLockGuard() { _mutex.unlock(); }

   private:
    MutexLock &_mutex;
};

#endif
