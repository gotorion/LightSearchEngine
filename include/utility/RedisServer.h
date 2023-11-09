#ifndef __REDISSERVER_H__
#define __REDISSERVER_H__

#include <pthread.h>
#include <sw/redis++/redis++.h>

#include <string>

using namespace sw::redis;
using std::string;

class RedisServer {
   public:
    static RedisServer& getInstance();

    void put(const string& key, const string& value);
    string get(const string& key);

   private:
    RedisServer();
    ~RedisServer();
    RedisServer(const RedisServer&) = delete;
    RedisServer& operator=(const RedisServer&) = delete;

   private:
    Redis _redis;
};

#endif
