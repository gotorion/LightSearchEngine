#include "WebPage.h"
#define LOGGER_LEVEL LL_WARN
#include <map>

#include "Simhasher.hpp"
#include "configure.h"
using std::map;
using namespace simhash;
map<string, string> mapWb = Configuration::getInstance()->getConfigMap();
Simhasher simhasher(mapWb["DICT_PATH"], mapWb["HMM_PATH"], mapWb["IDF_PATH"],
                    mapWb["WEB_STOP_WORD_PATH"]);

WebPage::WebPage(RssItem &tem) {
    _docTitle = tem.title;
    _docUr = tem.link;
    if (tem.content.size() == 0) {
        _docContent = tem.description;
    } else {
        _docContent = tem.content;
    }
    uint64_t ul = 0;
    size_t topN = 5;
    simhasher.make(_docContent, topN, ul);
    _u = ul;
}

string WebPage::getDoc() { return _docContent; }
