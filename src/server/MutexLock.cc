#include "MutexLock.h"

#include <stdio.h>

MutexLock::MutexLock() {
    int ret = pthread_mutex_init(&_mutex, nullptr);
    if (ret) {
        perror("pthread_mutex_init");
        return;
    }
}

MutexLock::~MutexLock() {
    int ret = pthread_mutex_destroy(&_mutex);
    if (ret) {
        perror("pthread_mutex_destroy");
        return;
    }
}

//上锁与解锁
void MutexLock::lock() {
    int ret = pthread_mutex_lock(&_mutex);
    if (ret) {
        perror("pthread_mutex_lock");
        return;
    }
}
void MutexLock::trylock() {
    int ret = pthread_mutex_trylock(&_mutex);
    if (ret) {
        perror("pthread_mutex_trylock");
        return;
    }
}
void MutexLock::unlock() {
    int ret = pthread_mutex_unlock(&_mutex);
    if (ret) {
        perror("pthread_mutex_unlock");
        return;
    }
}

//获取数据成员的函数get
pthread_mutex_t *MutexLock::getMutexLockPtr() { return &_mutex; }
