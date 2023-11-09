#ifndef _WEBPAGE_
#define _WEBPAGE_
#include <map>
#include <string>
#include <vector>
using std::map;
using std::string;
using std::vector;
#include "RssReader.h"
class WebPage {
   public:
    WebPage(RssItem &tem);

    int getDocId() { return _docId; }
    string getDoc();

    const static int TOPK_NUMBER = 20;
    int _docId;  //文档id
    string _docTitle;
    string _docUr;
    string _docContent;
    uint64_t _u;

    string _docSummary;
    vector<string> _topW;
    map<string, int> _wordsMap;
};

#endif
