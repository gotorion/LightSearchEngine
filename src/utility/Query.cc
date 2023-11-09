#include "Query.h"

#include <iostream>

#include "configure.h"

Query::Query() : _mysql(mysql_init(nullptr)) {
    _mysql = mysql_real_connect(
        _mysql, "localhost", "root",
        Configuration::getInstance()->getConfigMap()["MYSQL_PASSWARD"].c_str(),
        Configuration::getInstance()
            ->getConfigMap()["MYSQL_DATABASE_NAME"]
            .c_str(),
        0, nullptr, 0);

    if (_mysql == nullptr) {
        std::cout << "MySQL connect failed" << std::endl;
        return;
    }
}

Query& Query::getInstance() {
    static Query q;
    return q;
}

Query::~Query() { mysql_close(_mysql); }

void Query::doUpdateQuery(const string& sql) {
    int ret = mysql_query(_mysql, sql.c_str());
    if (ret) {
        mysql_error(_mysql);
    }
}

vector<vector<string>> Query::doQuery(const string& sql) {
    int ret = mysql_query(_mysql, sql.c_str());
    if (ret) {
        mysql_error(_mysql);
    }
    MYSQL_RES* sqlResult = mysql_store_result(_mysql);
    int totalRow = mysql_num_rows(sqlResult);
    int totalCol = mysql_num_fields(sqlResult);
    vector<vector<string>> resultArray;
    while (totalRow--) {
        MYSQL_ROW oneRow = mysql_fetch_row(sqlResult);
        vector<string> line;  // 一行的结果
        for (int i = 0; i < totalCol; ++i) {
            line.emplace_back(oneRow[i]);
        }
        resultArray.push_back(std::move(line));
    }
    mysql_free_result(sqlResult);  // 释放查询结果占据的空间
    return resultArray;
}