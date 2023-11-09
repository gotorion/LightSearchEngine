#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>

#include <functional>
#include <string>
using std::function;
using std::string;
extern __thread int threadID;
class Thread {
    using ThreadCallback = function<void()>;

   public:
    Thread(ThreadCallback &&cb, int id);
    ~Thread();

    //线程的开始与结束
    void start();
    void stop();

   private:
    //线程入口函数
    static void *threadFunc(void *args);

   private:
    pthread_t _thid;  //线程id
    int _threadID;
    bool _isRunning;     //线程是否运行的标识
    ThreadCallback _cb;  //需要执行的任务
};

#endif
