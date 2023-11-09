#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <functional>
#include <memory>
#include <vector>

#include "TaskQueue.h"

using std::function;
using std::unique_ptr;
using std::vector;

class Thread;  //前向声明

using Task = function<void()>;

class ThreadPool {
   public:
    ThreadPool(size_t threadNum, size_t queSize);
    ~ThreadPool();

    //线程池的开始与结束
    void start();
    void stop();

    //添加任务与获取任务
    void addTask(Task &&task);

   private:
    Task getTask();
    //线程池交给工作线程做的任务
    void doTask();

   private:
    size_t _threadNum;                    //子线程的数目
    size_t _queSize;                      //任务队列的大小
    vector<unique_ptr<Thread>> _threads;  //存放工作线程的容器
    TaskQueue _taskQue;                   //任务队列
    bool _isExit;                         //标识线程池是否退出的标志
};

#endif
