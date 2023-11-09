#include "../include/SocketIO.h"

#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

SocketIO::SocketIO(int fd) : _fd(fd) {}

SocketIO::~SocketIO() { close(_fd); }

int SocketIO::fd() { return _fd; }

// len = 10000    1500/6     1000/1
//代码的目的就是：如果想读len个字节，就循环
//调用read函数，如果没有read失败，那么就
//可以保证读到len个字节后退出
int SocketIO::readN(char *buf, int len) {
    int left = len;
    char *pstr = buf;
    int ret = 0;

    while (left > 0) {
        ret = read(_fd, pstr, left);
        if (-1 == ret && errno == EINTR)  //中断
        {
            continue;
        } else if (-1 == ret) {
            perror("read error -1");
            return -1;
        } else if (0 == ret) {
            break;
        } else {
            pstr += ret;
            left -= ret;
        }
    }

    return len - left;
}

int SocketIO::writeN(const char *buf, int len) {
    int left = len;
    const char *pstr = buf;
    int ret = 0;

    while (left > 0) {
        ret = write(_fd, pstr, left);
        if (-1 == ret && errno == EINTR) {
            continue;
        } else if (-1 == ret) {
            perror("writen error -1");
            return -1;
        } else if (0 == ret) {
            break;
        } else {
            pstr += ret;
            left -= ret;
        }
    }

    return len - left;
}

string SocketIO::receive() {
    int len = 0;
    int ret = recv(_fd, &len, sizeof(int), MSG_WAITALL);
    if (ret == -1) {
        perror("read");
        return "";
    }
    char *buf = new char[len + 1]();
    ret = readN(buf, len);

    if (ret == -1) {
        return "";
    }
    string res = string(buf);
    delete[] buf;
    return res;
}

int SocketIO::send(const string &str) {
    int len = str.size();
    int ret = write(_fd, &len, sizeof(int));

    ret = writeN(str.c_str(), len);
    if (ret == -1) {
        return -1;
    }
    return ret;
}
