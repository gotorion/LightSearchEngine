#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "NonCopyable.h"
/* #include "MutexLock.h" */
#include <pthread.h>

class MutexLock;  //前向声明

class Condition : NonCopyable {
   public:
    Condition(MutexLock &mutex);
    ~Condition();

    //睡眠
    void wait();

    //唤醒函数
    void notify();
    void notifyAll();

   private:
    MutexLock &_mutex;  //互斥锁的引用
    pthread_cond_t _cond;
};
#endif
