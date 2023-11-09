#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__

#include <functional>
#include <queue>

#include "Condition.h"
#include "MutexLock.h"

using std::function;
using std::queue;

using ElemType = function<void()>;

class TaskQueue {
   public:
    TaskQueue(size_t queSize);
    ~TaskQueue();

    //向任务队列中添加任务
    void push(ElemType &&task);

    //从任务队列中取出任务
    ElemType pop();

    //任务队列的空与满
    bool empty() const;
    bool full() const;

    //唤醒所有等待在_notEmpty条件变量上的线程
    void wakeup();

   private:
    size_t _queSize;       //任务队列的大小
    queue<ElemType> _que;  //存放数据的容器
    MutexLock _mutex;      //互斥锁
    Condition _notEmpty;   //非空条件变量
    Condition _notFull;    //非满条件变量
    bool _flag;            //标识
};

#endif
