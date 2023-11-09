#ifndef __LRUCACHE_H__
#define __LRUCACHE_H__
#include <iostream>
#include <list>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
using std::list, std::unordered_map, std::string, std::pair;
using json = nlohmann::json;
using listIter = list<pair<string, string>>::iterator;

class LRUCache {
   public:
    LRUCache(int capacity);
    ~LRUCache();
    void put(const string& key, const string& val, bool update);
    string get(const string& key);
    void readFromFile(const string& filename);
    void writeToFile(const string& filename);
    void update(const LRUCache&);  // 用主Cache更新自己
    void mergePendingList(LRUCache& rhs);
    void clearPendingList();

   private:
    listIter keyExist(const string& key);

   private:
    int _capacity;
    list<pair<string, string>> _cachedData;
    unordered_map<string, listIter> _hashmap;
    list<pair<string, string>> _pendingUpdateList;
};

#endif