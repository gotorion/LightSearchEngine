#ifndef _PAGELIBPREPROCESSIR_H_
#define _PAGELIBPREPROCESSIR_H_
#include <list>
#include <set>
#include <unordered_map>
#include <vector>

#include "RssReader.h"
#include "SplitTool.h"
#include "WebPage.h"

using std::list;

class PageLibPreprocessor {
   public:
    PageLibPreprocessor();
    void doProcess();          //预处理：导入全部数据
    void cutRedundantPages();  //冗余网页去重
    void insert(RssItem& tem);
    void createXml();
    void buildInvertIndex();

   private:
    list<WebPage> _pageLib;

    // 分词类
    SplitTool* _wordCutter;
    // 倒排索引, 词语 : <文章id:权重, ...>
    unordered_map<string, vector<pair<int, double>>> _invertIndexLib;
};
#endif
