#include "CppJieba.h"

const char* const DICT_PATH = "../include/cppjieba/dict/jieba.dict.utf8";
const char* const HMM_PATH = "../include/cppjieba/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../include/cppjieba/dict/user.dict.utf8";
const char* const IDF_PATH = "../include/cppjieba/dict/idf.utf8";
const char* const STOP_WORD_PATH = "../include/cppjieba/dict/stop_words.utf8";

JiebaTool::JiebaTool()
    : _jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH) {
    // std::cout << "jieba init success" << std::endl;
}

JiebaTool::~JiebaTool() {}

JiebaTool& JiebaTool::getInstance() {
    static JiebaTool jieba;
    return jieba;
}

vector<string> JiebaTool::cut(const string& target) {
    vector<string> words;
    _jieba.CutAll(target, words);
    return words;
}
// test
// int main() {
//     JiebaTool myjieba;
//     string target = "我是一个中国人";
//     const vector<string>& res = myjieba.cut(target);
//     for (auto& str : res) {
//         std::cout << str << std::endl;
//     }
//     return 0;
// }