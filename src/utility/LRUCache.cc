#include "LRUCache.h"

#include <fstream>
#include <iostream>
#include <sstream>

using std::ofstream, std::ifstream, std::istringstream, std::pair, std::fstream;

LRUCache::LRUCache(int capacity)
    : _capacity(capacity), _cachedData(), _hashmap(), _pendingUpdateList() {}

LRUCache::~LRUCache() {}

listIter LRUCache::keyExist(const string& key) {
    auto it = _hashmap.find(key);
    return (it == _hashmap.end()) ? _cachedData.end() : it->second;
}

string LRUCache::get(const string& key) {
    auto it = keyExist(key);

    if (it != _cachedData.end()) {        // 找到元素,放到队头
        pair<string, string> elem = *it;  // 暂存元素
        _cachedData.erase(it);            // 从队列中移除
        _cachedData.push_front(elem);     // 再重新放到队头
        _hashmap[elem.first] = _cachedData.begin();
        _pendingUpdateList.push_front(std::move(elem));
        return _cachedData.front().second;
    } else {
        return string();
    }
}

void LRUCache::put(const string& key, const string& val, bool update) {
    auto it = keyExist(key);
    if (it != _cachedData.end()) {
        pair<string, string> elem = *it;
        _cachedData.erase(it);                // 先从淘汰队列里删除
        _cachedData.emplace_front(key, val);  // 更新值,放到队头
    } else {  // 插入新值并依据容量淘汰元素
        _cachedData.emplace_front(key, val);
        if (_cachedData.size() > _capacity) {  // 淘汰元素
            auto lastElem = _cachedData.back();
            _hashmap.erase(lastElem.first);
            _cachedData.pop_back();
        }
    }
    _hashmap[key] = _cachedData.begin();  // 更新迭代器位置
    if (!update) {
        // 如果是正常记录要放入待同步队列，如果是Cache更新操作防止重复放入
        _pendingUpdateList.emplace_front(key, val);
    }
}

void LRUCache::readFromFile(const string& filename) {
    // 如果文件不存在则新建文件，读到LRU里面的内容是空的
    ifstream ifs(filename, std::ios::in);
    if (!ifs.is_open()) {
        std::cout << "LRU readFromFile failed" << std::endl;
    }
    string line, key, value;
    // 读取备份文件到Cache
    while (getline(ifs, line)) {
        if (line.size() != 0) {
            istringstream iss(line);
            while (iss >> key >> value) {
                // 读的时候从队头往队尾构建
                _cachedData.push_back(std::make_pair(key, value));
            }
        }
    }

    for (auto it = _cachedData.begin(); it != _cachedData.end(); it++) {
        _hashmap[it->first] = it;
    }
    ifs.close();
}

void LRUCache::writeToFile(const string& filename) {
    // 如果文件不存在则新建，如果存在则清空内容
    ofstream ofs(filename, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        std::cout << "LRU writeToFile failed" << std::endl;
    }
    for (auto& elem : _cachedData) {
        // 写的时候从淘汰队列队头开始写
        ofs << elem.first << " " << elem.second << std::endl;
    }
    ofs.close();
}

void LRUCache::update(const LRUCache& rhs) {
    // _pendingUpdateList里最新的更新在队头，所以用另一个Cache更新要反着来
    for (auto it = rhs._pendingUpdateList.rbegin();
         it != rhs._pendingUpdateList.rend(); ++it) {
        put(it->first, it->second, true);
    }
}

void LRUCache::mergePendingList(LRUCache& rhs) {
    if (this == &rhs) {
        return;
    }
    while (rhs._pendingUpdateList.size() > 0) {
        _pendingUpdateList.push_back(rhs._pendingUpdateList.front());
        rhs._pendingUpdateList.pop_front();
    }
}

void LRUCache::clearPendingList() { _pendingUpdateList.clear(); }