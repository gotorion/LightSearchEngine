#include "Timerfd.h"

#include <poll.h>
#include <stdio.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <cstdint>
#include <iostream>

TimerFd::TimerFd(int initSec, int peridocSec, TimerFdCallback &&cb)
    : _timerfd(createTimerFd()),
      _initSec(initSec),
      _peridocSec(peridocSec),
      _cb(std::move(cb))  //回调函数的注册
{
    setTimer(initSec, peridocSec);
}

TimerFd::~TimerFd() {
    setTimer(0, 0);
    close(_timerfd);
}

//封装了read函数
void TimerFd::handleRead() {
    uint64_t one = 1;
    ssize_t ret = ::read(_timerfd, &one, sizeof(one));
    if (ret != sizeof(one)) {
        perror("handleRead");
        return;
    }
}
//使用TimerFd创建文件描述符
int TimerFd::createTimerFd() {
    int fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd < 0) {
        perror("createTimerFd");
        return fd;
    }

    return fd;
}
//设置定时器
void TimerFd::setTimer(int initSec, int peridocSec) {
    struct itimerspec value;
    value.it_value.tv_sec = initSec;  //起始时间
    value.it_value.tv_nsec = 0;

    value.it_interval.tv_sec = peridocSec;  //周期时间
    value.it_interval.tv_nsec = 0;

    int ret = timerfd_settime(_timerfd, 0, &value, nullptr);
    if (ret < 0) {
        perror("timerfd_settime");
        return;
    }
}

int TimerFd::fd() const { return _timerfd; }

void TimerFd::handleTimerCallback() {
    if (_cb) {
        _cb();
    }
}