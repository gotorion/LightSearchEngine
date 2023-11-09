#include "WebPageQuery.h"

#include <math.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "CppJieba.h"
#include "Query.h"

using std::getline;
using std::ifstream;
using std::istringstream;
using std::regex;
using std::regex_replace;
using json = nlohmann::json;
using std::cout;
using std::endl;
using std::to_string;

WebPageQuery::WebPageQuery() { loadStopWords(); }

// 加载 停用词文件, 存入 unorder_map
void WebPageQuery::loadStopWords() {
    _stopWords.clear();

    // ifstream ifs("../../data/stop_words_zh.txt");
    ifstream ifs(
        Configuration::getInstance()->getConfigMap()["WEB_STOP_WORD_PATH"]);
    if (!ifs.good()) {
        perror("load stop words_zh");
        exit(1);
    }

    string words;
    while (ifs >> words) {
        _stopWords.insert(words);
    }
    ifs.close();
}

// 分词, 并把结果存入 _queryWords
void WebPageQuery::cut(const string& sentence) {
    _queryWords.clear();

    // 句子 分词, 并去除停用词, 将结果存入 _queryWords
    // vector<string> queryWord = _wordCutTool.cut(sentence);
    vector<string> queryWord = JiebaTool::getInstance().cut(sentence);

    for (auto& elem : queryWord) {
        if (_stopWords.find(elem) == _stopWords.end()) {
            _tmpQueryWords.push_back(elem);
            _queryWords.insert(elem);
        }
    }
}

// 求 包含全部查询词的 文章Id
void WebPageQuery::getIntersectionId() {
    // 求并集
    // 文章 id : 单词出现个数
    unordered_map<int, int> result;

    for (auto& elem : _queryWords) {
        string query =
            "select webPageId from invertLib where word = '" + elem + "'";
        vector<vector<string>> tmp = Query::getInstance().doQuery(query);

        if (tmp.size() == 0) {
            continue;
        }

        for (auto& line : tmp) {
            int id = std::stoi(line[0]);
            if (result.find(id) == result.end()) {
                result[id] = 1;
            } else {
                result[id]++;
            }
        }
    }

    // for (auto& elem : result) {
    //     cout << elem.first << ":" << elem.second << " | ";
    // }
    // cout << endl << endl;

    // 求交集
    int wordsNum = _queryWords.size();
    for (auto& elem : result) {
        if (elem.second == wordsNum) {
            _intersectionId.push_back(elem.first);
        }
    }

    // FIXME:
    // 打印 intersectionId
    // cout << "intersectionId: ";
    // for (auto& elem : _intersectionId) {
    //     cout << elem << " ";
    // }
    // cout << endl;
}

// 计算查询词的权重, 存入 _queryWeight
void WebPageQuery::getQueryWordsWeight() {
    // 若 没有文章包含所有查询词, 则不必计算 查询词的特征向量
    if (_intersectionId.size() == 0) {
        return;
    }

    _queryWeight.clear();

    // 记录 w1^2 + w2^2 + ... + wn^2 的和
    double sumw2 = 0;

    string query = "select count(id) from webPageLib";
    vector<vector<string>> tmp = Query::getInstance().doQuery(query);
    int N = stod(tmp[0][0]);

    for (auto& elem : _queryWords) {
        query = "select count(word) from invertLib where word = '" + elem + "'";
        tmp = Query::getInstance().doQuery(query);
        double DF = stod(tmp[0][0]);

        double IDF = log2(N / (DF + 1));

        // w = tf * idf, 此处 tf 取 1
        // double W = 1.0 * IDF;
        double TF =
            std::count(_tmpQueryWords.begin(), _tmpQueryWords.end(), elem);
        double W = TF * IDF;

        _queryWeight.push_back(W);
        sumw2 += W * W;
    }

    for (auto& elem : _queryWeight) {
        elem /= sqrt(sumw2);
    }

    // FIXME:
    // cout << "查询词权重: ";
    // for (auto& elem : _queryWeight) {
    //     cout << elem << " ";
    // }
    // cout << endl;
}

// 获取 _intersectionId 中各篇文章的 特征向量
void WebPageQuery::getPageWeight() {
    _pageWeight.clear();

    for (auto& id : _intersectionId) {
        _pageWeight[id] = {};
        for (auto& word : _queryWords) {
            string query = "select weight from invertLib where word = '" +
                           word + "' and webPageId = " + to_string(id);
            vector<vector<string>> tmp = Query::getInstance().doQuery(query);

            double weight = stod(tmp[0][0]);
            _pageWeight[id].push_back(weight);
        }
    }

    // FIXME:打印测试
    // for (auto& elem : _pageWeight) {
    //     cout << "**********************************" << endl;
    //     cout << elem.first << ": ";
    //     for (auto& w : elem.second) {
    //         cout << w << " ";
    //     }
    //     cout << endl;
    // }
}

// 计算 余弦相似度
void WebPageQuery::getCos() {
    _pageCos.clear();

    for (auto& elem : _pageWeight) {
        int id = elem.first;
        auto pageWeight = elem.second;

        double cos, xy, x, y;
        cos = xy = x = y = 0;

        for (int i = 0; i < pageWeight.size(); ++i) {
            xy += _queryWeight[i] * pageWeight[i];
            x += _queryWeight[i] * _queryWeight[i];
            y += pageWeight[i] * pageWeight[i];
        }
        x = sqrt(x);
        y = sqrt(y);
        cos = xy / (x * y);

        _pageCos.insert({cos, id});
    }

    // 打印测试
    // cout << "余弦相似度: " << endl;
    // for (auto& elem : _pageCos) {
    //     cout << "+++++++++++++++++++++++++++++" << endl;
    //     cout << elem.second << ": " << elem.first << endl;
    // }
}

// 执行查询
string WebPageQuery::getQueryResult() {
    vector<string> queryResult;

    for (auto& elem : _pageCos) {
        string result;
        int id = elem.second;

        string query =
            "select url, title, description from webPageLib where id = " +
            to_string(id);
        vector<vector<string>> tmp = Query::getInstance().doQuery(query);

        if (tmp.size() == 0) {
            continue;
        }

        result = "Id: " + std::to_string(id) + "\n";
        result += "Title: " + tmp[0][1] + "\n";
        result += "Url: " + tmp[0][0] + "\n";

        // 取文章内容, 并去除空白字符
        string desc = tmp[0][2];
        regex e("\\s|　|(&nbsp;)|(nbsp;)|(showPlayer\\([^)]+\\);)");
        desc = regex_replace(desc, e, "");

        // 求所有查询词 第一次出现的位置
        set<int> index;
        for (auto& elem : _queryWords) {
            int tmp = desc.find(elem);
            index.insert(tmp);
        }

        // 判断词距, 确定需要 截取的句子数量
        auto slowIt = index.begin();
        auto fastIt = index.begin();
        fastIt++;
        for (; fastIt != index.end();) {
            if (*fastIt < *slowIt + 60) {
                fastIt = index.erase(fastIt);
            } else {
                slowIt++;
                fastIt++;
            }
        }

        // 从文章中截取需要的句子
        string summary;
        for (auto& elem : index) {
            string tmp;
            if (elem >= 60) {
                tmp = desc.substr(elem - 60, 120);
            } else {
                tmp = desc.substr(0, 120);
            }

            // 1110xxxx 10xxxxxx 10xxxxxx
            // 切除开头不完整 中文字符
            if ((tmp[0] & 0xe0) == 0xe0) {
            } else if ((tmp[1] & 0xe0) == 0xe0) {
                tmp = tmp.substr(1);
            } else {
                tmp = tmp.substr(2);
            }

            // 切除末尾不完整 中文字符
            int size = tmp.size();
            if ((tmp[size - 1] & 0xe0) == 0xe0) {
                tmp = tmp.substr(0, size - 1);
            } else if ((tmp[size - 2] & 0xe0) == 0xe0) {
                tmp = tmp.substr(0, size - 2);
            }
            summary += tmp + "...";
        }
        desc = summary;

        // 将 内容的 查询词高亮
        for (auto& elem : _queryWords) {
            regex e(elem);
            desc = regex_replace(desc, e, "\e[93m" + elem + "\e[39m");
        }
        result += "Summary: " + desc;

        queryResult.push_back(result);
    }

    json retJson;
    if (queryResult.size() != 0) {
        retJson["WebPage"] = queryResult;
        return retJson.dump(-1, ' ', false, json::error_handler_t::ignore);
    } else {
        return "";
    }
}

string WebPageQuery::doQuery(const string& sentence) {
    // 分词, 并把结果存入 _queryWords
    cut(sentence);

    // 求 包含全部查询词的文章Id
    getIntersectionId();

    // 计算 待查词的 权重
    getQueryWordsWeight();

    // 计算 unionId 中各篇文章的 特征向量
    getPageWeight();

    // 计算 余弦相似度
    getCos();

    return getQueryResult();
}
