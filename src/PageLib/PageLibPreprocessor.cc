#include "PageLibPreprocessor.h"

#include <math.h>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "DirScanner.h"
#include "Query.h"
#include "Simhasher.hpp"
#include "configure.h"

PageLibPreprocessor::PageLibPreprocessor()
    : _wordCutter(new SplitToolCppJieba()) {}

void PageLibPreprocessor::doProcess() {
    map<string, string> map1 = Configuration::getInstance()->getConfigMap();
    string path = map1["WEB_PAGE_PATH"];

    DirScanner dir;
    dir.traverse(path);
    vector<string> file = dir.getFiles();
    for (size_t i = 0; i < file.size(); i++) {
        RssReader m;
        m.parseRss(file[i]);
        m.dump(*this);
    }
}

void PageLibPreprocessor::insert(RssItem& tem) {
    WebPage m(tem);
    _pageLib.push_back(m);
}

void PageLibPreprocessor::cutRedundantPages() {
    auto slowIter = _pageLib.begin();
    for (; slowIter != _pageLib.end(); slowIter++) {
        auto fastIter = slowIter;
        fastIter++;
        while (fastIter != _pageLib.end()) {
            if (simhash::Simhasher::isEqual(fastIter->_u, slowIter->_u)) {
                // 删除 元素后 返回 指向 待删除元素下一个元素 的迭代器
                fastIter = _pageLib.erase(fastIter);
            } else {
                fastIter++;
            }
        }
    }
}
void PageLibPreprocessor::createXml() {
    cout << "----" << endl;
    vector<pair<int, int>> file2;
    auto it = _pageLib.begin();
    int i = 0;
    for (; it != _pageLib.end(); it++) {
        it->_docId = ++i;
        string sql =
            "INSERT INTO webPageLib (id, url,title,description) VALUES (";
        sql += to_string(it->_docId);
        sql += ",'";
        sql += it->_docUr;
        sql += "','";
        sql += it->_docTitle;
        sql += "','";
        sql += it->_docContent;
        sql += "');";
        Query::getInstance().doUpdateQuery(sql);
    }
    cout << "----" << endl;
}

void PageLibPreprocessor::buildInvertIndex() {
    // 加载 停用词文件, 存入 unorder_map
    unordered_set<string> stopWords;

    auto map1 = Configuration::getInstance()->getConfigMap();
    ifstream ifs(map1["STOP_WORD_PATH"]);
    if (!ifs.good()) {
        perror("buildInvertIndex");
        return;
    }

    string words;
    while (ifs >> words) {
        stopWords.insert(words);
    }

    ifs.close();

    /************************************************/

    // 储存 单词 及 包含该单词的 文档数量
    unordered_map<string, double> pagesCount;

    // 储存 每篇文章 的 词频统计, 其中 vector 索引表示 文章 id, 值为该篇文章的
    // 词频map
    vector<unordered_map<string, double>> pagesWordCount;

    for (auto& page : _pageLib) {
        vector<string> words = _wordCutter->cut(page.getDoc());

        // 遍历分词得到的 vector, 统计每篇文章的词频
        unordered_map<string, double> wordCount;
        for (auto& str : words) {
            // 如果为停用词, 则跳过
            if (stopWords.find(str) != stopWords.end()) {
                continue;
            }

            if (str == " " || str == "\n" || str == "　" || str == " ") {
                continue;
            }

            int nums = wordCount.count(str);
            if (nums == 0) {
                wordCount[str] = 1;
            } else {
                wordCount[str]++;
            }
        }

        // 更新 出现单词的 文档数量
        for (auto& elem : wordCount) {
            int nums = pagesCount.count(elem.first);
            if (nums == 0) {
                pagesCount[elem.first] = 1;
            } else {
                pagesCount[elem.first]++;
            }
        }

        pagesWordCount.push_back(wordCount);
    }

    // 计算 权重 w'
    // 1. TF: 某词 在 文章中 出现的次数
    // 2. DF: 出现 某词 的文档数量
    // 3. IDF = log2(N/(DF+1)) 逆文档频率, N 为 文档总数
    // 4. w = TF * IDF
    // 5. w' = w / sqrt(w1^2 + w2^2 + ... + wn^2) 归一化处理
    for (int i = 0; i < pagesWordCount.size(); ++i) {
        auto& elem = pagesWordCount[i];

        // 记录 w1^2 + w2^2 + ... + wn^2 的和
        double sum = 0;

        // 更新 每篇文章的 词频 为 w权重
        for (auto& pairFreq : elem) {
            double IDF =
                log2(_pageLib.size() / (pagesCount[pairFreq.first] + 1));
            double w = pairFreq.second * IDF;
            sum += w * w;
            pairFreq.second = w;
        }

        // 更新 每篇文章 w权重 为 w'权重 并写入 倒排索引map
        double sqrtW = sqrt(sum);

        for (auto& pairFreq : elem) {
            pairFreq.second /= sqrtW;
            _invertIndexLib[pairFreq.first].push_back(
                make_pair(i + 1, pairFreq.second));
        }
    }

    // 倒排索引库写入 mysql
    // string creatTble = "create table invertLib(id int AUTO_INCREMENT, word
    // VARCHAR(100), webPageId int, weight DOUBLE, primary key(id), INDEX(word,
    // webPageId))"; Query::getInstance().doUpdateQuery(creatTble);

    for (auto& pairFreq : _invertIndexLib) {
        string word = pairFreq.first;
        for (auto elem : pairFreq.second) {
            int id = elem.first;
            double weight = elem.second;

            string query =
                "insert into invertLib (word, webPageId, weight) values('" +
                word + "', " + to_string(id) + ", " + to_string(weight) + ")";

            Query::getInstance().doUpdateQuery(query.c_str());
        }
    }
}
