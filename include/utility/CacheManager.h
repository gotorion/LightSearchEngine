#ifndef __CACHEMANAGER_H__
#define __CACHEMANAGER_H__

#include <vector>

#include "LRUCache.h"
using std::vector;
class CacheManager {
   public:
    static CacheManager* getInstance();
    void initCache(int size, const string& filename);
    // 初始化LRU个数应和线程数一样，同时使用持久化文件对所有的Cache进行初始化
    LRUCache& getCache(int threadID);  // 通过线程名获取对应的Cache
    void updateCache();

   private:
    CacheManager();
    ~CacheManager();
    CacheManager(const CacheManager&) = delete;
    const CacheManager& operator=(const CacheManager&) = delete;
    static void init();
    static void destroy();

   private:
    string _path;
    vector<LRUCache> _caches;
    static CacheManager* _pInstance;
    static pthread_once_t _ponce;
};

#endif