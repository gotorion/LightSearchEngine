#include "RedisServer.h"

#include <sw/redis++/redis.h>

RedisServer& RedisServer::getInstance() {
    static RedisServer _pInstance;
    return _pInstance;
}

RedisServer::RedisServer() : _redis("tcp://127.0.0.1:6379") {}

RedisServer::~RedisServer() {}

void RedisServer::put(const string& key, const string& value) {
    // 切换到 1 号数据库
    _redis.command("select", "1");

    _redis.set(key, value);
}

string RedisServer::get(const string& key) {
    // 切换到 1 号数据库
    _redis.command("select", "1");

    auto it = _redis.get(key);

    if (it) {
        return it->c_str();
    } else {
        return "";
    }
}