#ifndef __QUERY_H__
#define __QUERY_H__
#include <mysql/mysql.h>

#include <string>
#include <vector>
using std::vector, std::string;
// 单例类
class Query {
   public:
    static Query& getInstance();
    void doUpdateQuery(const string& sql);  // insert, update
    vector<vector<string>> doQuery(const string& sql);

   public:
    Query(const Query&) = delete;
    const Query& operator=(const Query&) = delete;

   private:
    Query();
    ~Query();

   private:
    MYSQL* _mysql = nullptr;
};

#endif