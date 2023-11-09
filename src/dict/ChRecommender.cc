#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>

#include "Query.h"
#include "Recommender.h"
using std::priority_queue, std::pair, std::set;
using json = nlohmann::json;

string ChRecommender::recommender(const string& queryWord) {
    // 修改为查询数据库
    Query& MySQLHandle = Query::getInstance();
    vector<string> singleChars = cutString(queryWord);
    set<pair<string, int>> queryRes;

    for (const string& str : singleChars) {
        string query =
            string(
                "SELECT dictCn.word, frequency FROM dictCn, indexCn WHERE "
                "indexCn.word = '")
                .append(str)
                .append("' AND dictCn.id = indexCn.indexId;");
        vector<vector<string>> tempRes = MySQLHandle.doQuery(query);
        for (auto& elem : tempRes) {
            queryRes.insert(std::make_pair(elem[0], atoi(elem[1].c_str())));
        }
    }
    if (queryRes.size() == 0) {
        return "";
    }
    int retCount = queryRes.size();
    priority_queue<CandidateResult, vector<CandidateResult>, BetterCandidate>
        candis;
    for (auto& elem : queryRes) {
        int distance = editDistance(queryWord, elem.first);
        candis.emplace(elem.first, elem.second, distance);
    }
    vector<string> ret;
    // 循环条件防止返回的关键字个数小于5个，否则出现段错误
    for (int i = 0; i < std::min(5, retCount); ++i) {
        CandidateResult top = candis.top();
        ret.push_back(top._word);
        candis.pop();
    }
    json retJson;
    retJson["Keywords"] = ret;
    return retJson.dump(-1, ' ', false, json::error_handler_t::ignore);
}
vector<string> ChRecommender::cutString(const string& chstr) {
    set<string> output;
    string ch;
    int len;
    for (size_t i = 0; i != chstr.size(); i += len) {
        unsigned char byte = (unsigned)chstr[i];
        if (byte >= 0xFC) {
            len = 6;
        } else if (byte >= 0xF8) {
            len = 5;
        } else if (byte >= 0xF0) {
            len = 4;
        } else if (byte >= 0xE0) {
            len = 3;
        } else if (byte >= 0xc0) {
            len = 2;
        } else {
            len = 1;
        }
        ch = chstr.substr(i, len);
        output.insert(ch);
    }
    return vector<string>(output.begin(), output.end());
}