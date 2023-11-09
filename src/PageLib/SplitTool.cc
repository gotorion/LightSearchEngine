#include "SplitTool.h"

#include <cstddef>
#include <map>
#include <vector>

#include "configure.h"
using std::map;
cppjieba::Jieba* SplitToolCppJieba::_jieBa = nullptr;

SplitToolCppJieba::SplitToolCppJieba() {
    if (!_jieBa) {
        map<string, string> map1 = Configuration::getInstance()->getConfigMap();
        _jieBa = new cppjieba::Jieba(map1["DICT_PATH"], map1["HMM_PATH"],
                                     map1["USER_DICT_PATH"], map1["IDF_PATH"],
                                     map1["WEB_STOP_WORD_PATH"]);
    }
}

vector<string> SplitToolCppJieba::cut(const string& sentence) {
    vector<string> words;
    _jieBa->CutAll(sentence, words);
    return words;
}
