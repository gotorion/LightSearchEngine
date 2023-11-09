#ifndef __TIMERFD_H__
#define __TIMERFD_H__

#include <functional>

using std::function;

class TimerFd {
    using TimerFdCallback = function<void()>;

   public:
    TimerFd(int initSec, int peridocSec, TimerFdCallback &&cb);
    ~TimerFd();
    void handleRead();
    int createTimerFd();
    void setTimer(int initSec, int peridocSec);
    void handleTimerCallback();
    int fd() const;

   private:
    int _timerfd;         //定时器文件描述符
    int _initSec;         //起始时间
    int _peridocSec;      //周期时间
    TimerFdCallback _cb;  //待执行的任务
};

#endif
