#ifndef __WEBPAGEQUERY_H__
#define __WEBPAGEQUERY_H__
#include <functional>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "SplitTool.h"
#include "configure.h"

using std::multimap;
using std::pair;
using std::set;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

class WebPageQuery {
   public:
    WebPageQuery();
    string doQuery(const string& sentence);

   private:
    // 加载停用词
    void loadStopWords();

    // 分词, 并把结果存入 _queryWords
    void cut(const string& sentence);

    // 求 包含全部查询词的文章Id
    void getIntersectionId();

    // 计算查询词的权重, 存入 _queryWeight
    void getQueryWordsWeight();

    // 计算 unionId 中各篇文章的 特征向量
    void getPageWeight();

    // 计算 余弦相似度
    void getCos();

    // 获取查询结果
    string getQueryResult();

   private:
    // 储存 停用词
    unordered_set<string> _stopWords;

    // 分词结果
    vector<string> _tmpQueryWords;
    set<string> _queryWords;

    // 包含全部查询词的 文章id, 即 文章Id 的交集
    vector<int> _intersectionId;

    // 储存 查询词的权重
    vector<double> _queryWeight;

    // 储存 各文章对应的 特征向量
    unordered_map<int, vector<double>> _pageWeight;

    // 储存 各文章与查询词的余弦相似度, cos 为键, 文章 id 为值
    multimap<double, int, std::greater<double>> _pageCos;
};

#endif
