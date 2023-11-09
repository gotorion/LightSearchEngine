#include "Thread.h"

#include <stdio.h>

#include <iostream>

using std::cout;
using std::endl;
__thread int threadID = -1;

Thread::Thread(ThreadCallback &&cb, int id)
    : _thid(0),
      _threadID(id),
      _isRunning(false),
      _cb(std::move(cb))  //注册回调函数
{}

Thread::~Thread() {}

//线程的开始与结束
void Thread::start() {
    // threadFunc如果是非静态成员函数，那么就会在第一个参数的位置
    //隐含一个this指针，就与线程入口函数的参数不对应
    /* int ret = pthread_create(&_thid,nullptr, threadFunc, nullptr); */
    int ret = pthread_create(&_thid, nullptr, threadFunc, this);
    if (ret) {
        perror("pthread_create");
        return;
    }

    _isRunning = true;
}

void Thread::stop() {
    if (_isRunning) {
        int ret = pthread_join(_thid, nullptr);
        if (ret) {
            perror("pthread_join");
            return;
        }
        _isRunning = false;
    }
}

//线程入口函数
void *Thread::threadFunc(void *args) {
    Thread *pth = static_cast<Thread *>(args);
    threadID = pth->_threadID;
    if (pth) {
        //线程指向的任务
        pth->_cb();  //执行回调函数,本质上就是ThreadPool中的doTask
    } else {
        cout << "pth == nullptr" << endl;
    }

    /* return nullptr; */
    pthread_exit(nullptr);
}
