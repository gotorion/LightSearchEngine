#include <fstream>
#include <iostream>
#include <set>
#include <sstream>

using std::cerr;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::set;

#include <ctype.h>
#include <string.h>

#include "configure.h"
Configuration *Configuration::_confPtr = nullptr;
void deleteSpace(string &words) {
    auto it = words.begin();
    while (it != words.end() && isspace(*it)) {
        ++it;
    }
    words.erase(words.begin(), it);
    for (it = words.end(); it != words.begin();) {
        it--;
        if (!isspace(*it)) {
            it++;
            break;
        }
    }
    words.erase(it, words.end());
}

void splitKeyValue(const string &line, string &key, string &value) {
    istringstream iss(line);
    if (!iss) {
        cerr << "splitKeyValue: bad iss" << endl;
        return;
    }
    iss >> key;
    value = iss.str();
    value.replace(value.begin(), value.begin() + key.size(), "");
    deleteSpace(value);
    if (value.size() == 0) {
        cerr << "splitKeyValue: value = 0" << endl;
    }
}

Configuration *Configuration::getInstance() {
    static Configuration _configuration(
        "../conf/server.conf");  //这里自定义配置文件路径
    return &_configuration;
}

Configuration::Configuration(const string &filepath) {
    set<string> confKeys = {"STOP_WORDS_LIST_CN_PATH",
                            "STOP_WORDS_LIST_ENG_PATH",
                            "CN_ART_PATH",
                            "EN_ART_PATH",
                            "CN_DICT_OUTPUT_PATH",
                            "EN_DICT_OUTPUT_PATH",
                            "CN_INDEX_OUTPUT_PATH",
                            "EN_INDEX_OUTPUT_PATH",
                            "DICT_PATH",
                            "HMM_PATH",
                            "USER_DICT_PATH",
                            "IDF_PATH",
                            "LOG_PATH",
                            "STOP_WORD_PATH",
                            "IP",
                            "PORT",
                            "TASK_QUEUE_SIZE",
                            "THREAD_POOL_SIZE",
                            "WEB_PAGE_PATH",
                            "WEB_STOP_WORD_PATH",
                            "WEB_INVERT_LIB",
                            "WEB_OFFSET_LIB",
                            "WEB_PAGE_LIB",
                            "LRU_BAK_PATH",
                            "MYSQL_PASSWARD",
                            "MYSQL_DATABASE_NAME"};  // 添加配置
    ifstream ifs(filepath);
    if (!ifs) {
        cerr << "Configuration: bad ifs" << endl;
        ifs.close();
        return;
    }
    string line;
    while (std::getline(ifs, line)) {
        deleteSpace(line);
        if (line.size() == 0) {
            continue;
        }
        string confKey;
        string confValue;
        splitKeyValue(line, confKey, confValue);

        // if( confKey == "STOP_WORDS_ZH" ||
        //     confKey == "STOP_WORDS_LIST_ENG_PATH"
        // )
        if (confKeys.find(confKey) != confKeys.end()) {
            if (_configMap.find(confKey) != _configMap.end()) {
                cerr << "Configuration: existed key = " << confKey << endl;
            } else {
                _configMap[confKey] = confValue;
            }
        }

        else {
            cerr << "Configuration: invalid key = " << confKey << endl;
            continue;
        }
    }

    // 判断有无
    int incomplete = 0;
    for (const string &ele : confKeys) {
        if (_configMap.find(ele) == _configMap.end()) {
            incomplete = 1;
            cerr << "Configuration: missing key = " << ele << endl;
        }
    }

    getStopWordList();
    ifs.close();
}

map<string, string> &Configuration::getConfigMap() { return _configMap; }

set<string> &Configuration::getStopWordList() {
    if (!_stopWordList.empty()) {
        return _stopWordList;
    }
    if (_configMap.find("STOP_WORDS_LIST_ENG_PATH") == _configMap.end()) {
        cerr << "getStopWordList: missing key = STOP_WORDS_LIST_ENG_PATH"
             << endl;
    } else {
        ifstream ifs(_configMap["STOP_WORDS_LIST_ENG_PATH"]);
        if (!ifs) {
            cerr << "getStopWordList: bad ifs" << endl;
        } else {
            string line;
            while (std::getline(ifs, line)) {
                deleteSpace(line);
                if (line.size() != 0) {
                    _stopWordList.insert(line);
                }
            }
        }
        ifs.close();
    }

    if (_configMap.find("STOP_WORDS_LIST_CN_PATH") == _configMap.end()) {
        cerr << "getStopWordList: missing key = STOP_WORDS_LIST_CN_PATH"
             << endl;
    } else {
        ifstream ifs(_configMap["STOP_WORDS_LIST_CN_PATH"]);
        if (!ifs) {
            cerr << "getStopWordList: bad ifs" << endl;

        } else {
            string line;
            while (std::getline(ifs, line)) {
                deleteSpace(line);
                if (line.size() != 0) {
                    _stopWordList.insert(line);
                }
            }
        }
        ifs.close();
    }
    return _stopWordList;
}

string Configuration::getItem(const string &key) { return _configMap[key]; }