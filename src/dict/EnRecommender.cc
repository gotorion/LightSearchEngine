#include <queue>
#include <set>

#include "CandidateResult.h"
#include "EditDistance.h"
#include "Query.h"
#include "Recommender.h"
using std::priority_queue, std::pair, std::set;
string toLower(const string& str) {
    string ret = str;
    for (auto& ch : ret) {
        if (isupper(ch)) {
            ch = tolower(ch);
        }
    }
    return ret;
}
string EnRecommender::recommender(const string& queryString) {
    // 修改为查询数据库
    Query& MySQLHandle = Query::getInstance();
    set<pair<string, int>> queryRes;
    for (const char& ch : queryString) {
        string query = string(
                           "SELECT word, frequency FROM dictEn, indexEn WHERE "
                           "indexEn.alphabet = '")
                           .append(string(1, ch))
                           .append("' AND dictEn.id = indexEn.indexId;");
        vector<vector<string>> tempRes = MySQLHandle.doQuery(query);
        for (auto& elem : tempRes) {
            queryRes.insert(std::make_pair(elem[0], atoi(elem[1].c_str())));
        }
    }
    if (queryRes.size() == 0) {
        return "";
    }
    int retCount = queryRes.size();
    // 构建优先级队列
    priority_queue<CandidateResult, vector<CandidateResult>, BetterCandidate>
        candis;
    for (auto& elem : queryRes) {
        int distance = editDistance(queryString, elem.first);
        candis.emplace(elem.first, elem.second, distance);
    }
    // 遍历完成后优先级队列里保存的是有序的关键字
    vector<string> ret;
    for (int i = 0; i < std::min(5, retCount); ++i) {
        CandidateResult top = candis.top();
        ret.push_back(top._word);
        candis.pop();
    }
    json retJson;
    retJson["Keywords"] = ret;
    return retJson.dump(-1, ' ', false, json::error_handler_t::ignore);
}