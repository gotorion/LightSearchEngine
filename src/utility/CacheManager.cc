#include "CacheManager.h"

#include <pthread.h>

#include <iostream>

CacheManager* CacheManager::_pInstance = nullptr;
pthread_once_t CacheManager::_ponce = PTHREAD_ONCE_INIT;

CacheManager* CacheManager::getInstance() {
    if (_pInstance == nullptr) {
        pthread_once(&_ponce, init);
    }
    return _pInstance;
}

void CacheManager::initCache(int size, const string& filename) {
    _path = filename;
    // 或者在这里使用_caches.reserve(size)提前分配空间
    for (int i = 0; i < size; ++i) {
        _caches.push_back(LRUCache(size));  // 构造LRUCache
        // BUG: 一边构造Cache一边读文件会出问题，需要下面单独写个循环读
    }
    for (int i = 0; i < _caches.size(); ++i) {
        _caches[i].readFromFile(filename);
    }
}

LRUCache& CacheManager::getCache(int threadID) { return _caches.at(threadID); }

void CacheManager::updateCache() {
    // 以0号Cache为基准更新所有Cache
    LRUCache& base = _caches[0];
    for (int i = 1; i < _caches.size(); ++i) {  // 合并pending
        base.mergePendingList(_caches[i]);
    }
    for (int i = 0; i < _caches.size(); ++i) {
        _caches[i].update(base);
    }
    for (int i = 0; i < _caches.size(); ++i) {
        _caches[i].clearPendingList();
    }
    base.writeToFile(_path);
}

CacheManager::CacheManager() {}

CacheManager::~CacheManager() {}

void CacheManager::init() {
    if (_pInstance == nullptr) {
        _pInstance = new CacheManager();
    }
    atexit(destroy);
}

void CacheManager::destroy() {
    if (_pInstance) {
        delete _pInstance;
        _pInstance = nullptr;
    }
}