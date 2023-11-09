#include "ThreadPool.h"

#include <unistd.h>

#include <iostream>

#include "Thread.h"

using std::cout;
using std::endl;

ThreadPool::ThreadPool(size_t threadNum, size_t queSize)
    : _threadNum(threadNum),
      _queSize(queSize),
      _taskQue(_queSize),
      _isExit(false) {
    //为了防止vector的频繁扩容,可以预留空间
    _threads.reserve(_threadNum);
}

ThreadPool::~ThreadPool() {}

//线程池的开始与结束
void ThreadPool::start() {
    // 1、先将工作线程创建出来
    for (int idx = 0; idx < _threadNum; ++idx) {
        //线程池交给工作线程做的任务对应的函数是什么？
        // doTask
        unique_ptr<Thread> up(
            new Thread(std::bind(&ThreadPool::doTask, this), idx));
        _threads.push_back(std::move(up));
    }
    // 2、将所有创建出来的工作线程运行起来
    for (auto &th : _threads) {
        th->start();
    }
}

void ThreadPool::stop() {
    //保证任务执行完毕之后，线程池才能退出，工作线程才能退出
    //言外之意，只要任务队列中有任务，线程池就不能退出
    while (!_taskQue.empty()) {
        sleep(1);
    }

    //将标志位设置为true，表明线程池要退出了
    _isExit = true;
    //将所有在_notEmpty条件变量上的睡眠的线程都唤醒
    _taskQue.wakeup();
    /* _notEmpty.notifyAll(); */

    //将线程池中的工作线程都退出
    for (auto &th : _threads) {
        th->stop();
    }
}

//添加任务与获取任务
void ThreadPool::addTask(Task &&task) {
    if (task) {
        _taskQue.push(std::move(task));
    }
}

Task ThreadPool::getTask() { return _taskQue.pop(); }

//线程池交给工作线程做的任务
void ThreadPool::doTask() {
    //线程池之所以不能退出的原因？
    //如果执行getTask，拿到了任务之后会执行process函数，如果process
    //函数的执行速率非常慢的话，那么stop函数就有足够的时间将_isExit
    //设置为true，那么再走doTask函数中的while循环的时候，不满足
    //条件就进不到while，就不会阻塞；但是如果process函数执行的速率
    //非常的快，那么stop函数中的_isExit还没有来得及设置为true的
    //时候，doTask中的while已经进去了，那么会继续执行getTask函数，
    //而getTask函数在任务队列为空的时候，就会阻塞住,所以线程池就
    //无法退出了
    //
    //只要线程池不退出，工作线程就应该一直执行任务
    while (!_isExit) {
        // Task = function<void()>
        Task taskcb = getTask();
        if (taskcb) {
            //回调函数的执行,其实就是执行的MyTask中的process函数
            taskcb();
        } else {
            cout << "nullptr == taskcb" << endl;
        }
    }
}
