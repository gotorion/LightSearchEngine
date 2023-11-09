#include <iostream>
#include <queue>
#include <set>

#include "CandidateResult.h"
#include "Query.h"
#include "Recommender.h"
using std::priority_queue, std::pair, std::set;

string MixedRecommender::recommender(const string& queryString) {
    Query& MySQLHandle = Query::getInstance();
    set<pair<string, int>> queryRes;
    vector<string> singleChars = cutString(queryString);

    for (auto& str : singleChars) {
        if (str.size() == 1) {  // 查英文表
            string query =
                string(
                    "SELECT word, frequency FROM dictEn, indexEn WHERE "
                    "indexEn.alphabet = '")
                    .append(str)
                    .append("' AND dictEn.id = indexEn.indexId;");
            vector<vector<string>> tempRes = MySQLHandle.doQuery(query);
            for (auto& elem : tempRes) {
                queryRes.insert(std::make_pair(elem[0], atoi(elem[1].c_str())));
            }
        } else {
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
    }
    if (queryRes.size() == 0) {
        return "";
    }
    int retCount = queryRes.size();
    priority_queue<CandidateResult, vector<CandidateResult>, BetterCandidate>
        candis;
    for (auto& elem : queryRes) {
        int distance = editDistance(queryString, elem.first);
        candis.emplace(elem.first, elem.second, distance);
    }
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

vector<string> MixedRecommender::cutString(const string& chstr) {
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