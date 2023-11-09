#ifndef _RSSREADER_H_
#define _RSSREADER_H_
#include <fstream>
#include <iostream>
#include <regex>

#include "tinyxml2.h"
class PageLibPreprocessor;
using namespace std;
;
struct RssItem {
    string title;
    string link;
    string description;
    string content;
};
class RssReader {
   public:
    void replaceRegex(string &str);
    void traversingXML(tinyxml2::XMLNode *node);
    void parseRss(string &name);
    void dump(PageLibPreprocessor &m);

   private:
    vector<RssItem> _rss;
};

#endif
