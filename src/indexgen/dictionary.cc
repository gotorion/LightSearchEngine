#include "dictionary.h"

#include <dirent.h>

#include <cctype>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include "CppJieba.h"
#include "DirScanner.h"
#include "Query.h"
#include "configure.h"
using std::endl, std::to_string;
using std::istringstream;
using std::ofstream;
//初始化中英文停止词，中英文文件路径
DicProducer::DicProducer()
    : _conf(Configuration::getInstance()->getConfigMap().begin(),
            Configuration::getInstance()->getConfigMap().end()) {
    DirScanner diren, dircn;
    string pathEn = _conf["EN_ART_PATH"];
    string pathCn = _conf["CN_ART_PATH"];
    string stopEn = _conf["STOP_WORDS_LIST_ENG_PATH"];
    string stopCn = _conf["STOP_WORDS_LIST_CN_PATH"];

    diren.traverse(pathEn);
    dircn.traverse(pathCn);
    _filesen = diren.getFiles();
    _filescn = dircn.getFiles();

    std::ifstream ifs1(stopEn);
    if (!ifs1.good()) {
        std::cerr << "filename1 error_code\n";
        return;
    }
    string tem;
    while (getline(ifs1, tem, '\n')) {
        std::istringstream iss(tem);
        string tmp;
        iss >> tmp;
        stopen.insert(tmp);
    }
    ifs1.close();

    std::ifstream ifs2(stopCn);
    if (!ifs2.good()) {
        std::cerr << "filename2 error_code\n";
        return;
    }
    while (getline(ifs2, tem, '\n')) {
        std::istringstream iss(tem);
        string tmp;
        iss >> tmp;
        stopcn.insert(tmp);
    }
    ifs2.close();
}
void DicProducer::buildEnDict() {
    //遍历所有文件
    for (auto &file : _filesen) {
        //创建文件输入流，判断创建是否成功
        std::ifstream ifs(file);
        if (!ifs.good()) {
            std::cerr << "open fail";
            return;
        }
        //从文件输入流中循环读取一个单词
        string line;
        while (getline(ifs, line)) {
            istringstream iss(line);
            string word;
            while (iss >> word) {
                //判断单词中有数字则跳过
                bool hasDigit = false;
                for (size_t i = 0; i < word.size(); i++) {
                    if (isdigit(word[i])) {
                        hasDigit = true;
                        break;
                    }
                    if (i < word.size() - 1 && i >= 1 && ispunct(word[i])) {
                        hasDigit = true;
                        break;
                    }
                }
                if (hasDigit) {
                    continue;
                }
                if (ispunct(word.back())) {
                    word.pop_back();
                }
                if (ispunct(word.front())) {
                    continue;
                }
                //将大写转换为小写
                if (word.size() == 0) continue;
                size_t idx = 0;
                while (idx < word.size()) {
                    if (isupper(word[idx])) {
                        word[idx] = tolower(word[idx]);
                    }
                    idx++;
                }

                //若单词为停用词则跳过
                if (stopen.find(word) != stopen.end()) {
                    continue;
                };

                //单词不在词典中则新增，在词典中则词频+1
                auto it = _dicten.find(word);
                if (it == _dicten.end()) {
                    _dicten.insert(std::make_pair(word, 1));
                } else {
                    _dicten[word]++;
                }
            }
        }
        ifs.close();
    }
    // 插单词表
    Query &MySQLHandle = Query::getInstance();
    for (auto it = _dicten.begin(); it != _dicten.end(); ++it) {
        string query = string("INSERT INTO dictEn (word, frequency) VALUES ('")
                           .append(it->first)
                           .append("', ")
                           .append(std::to_string(it->second))
                           .append(");");
        MySQLHandle.doUpdateQuery(query);
    }
}

void DicProducer::buildCnDict() {
    JiebaTool &jieba = JiebaTool::getInstance();
    vector<string> re;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    for (size_t i = 0; i < _filescn.size(); i++) {
        std::ifstream ifs(_filescn[i]);
        if (!ifs.good()) {
            std::cerr << "filename error_code\n";
            return;
        }
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        string str(buffer.str());
        re = jieba.cut(str);
        for (size_t j = 0; j < re.size(); j++) {
            if (re[j].size() == 0) continue;
            //去除中文标点
            std::wstring ws = conv.from_bytes(re[j]);
            std::wregex rule(L"[^a-zA-z\\d\u4E00-\u9FA5]");
            std::wstring wret = regex_replace(ws, rule, L"");
            bool flag = false;
            for (wchar_t ch : wret) {
                //检查是否是标点和空格
                if (iswpunct(ch) || iswspace(ch)) {
                    flag = true;
                    break;
                }
            }
            if (flag) continue;
            re[j] = conv.to_bytes(wret);
            //去除英文标点
            if (re[j].size() == 0) continue;
            if (re[j].size() == 1 && (ispunct(re[j][0]) || isalnum(re[j][0])))
                continue;
            //去除中英停用词
            auto itstopEn = stopcn.find(re[j]);
            auto itstopCn = stopen.find(re[j]);
            if (itstopCn != stopcn.end() || itstopEn != stopen.end()) continue;
            auto it = _dictcn.find(re[j]);
            if (it != _dictcn.end()) {
                _dictcn[re[j]]++;
            } else {
                _dictcn.insert(make_pair(re[j], 1));
            }
        }
        re.clear();
        ifs.close();
    }
    std::cout << _dictcn.size() << endl;
    int line = 1;
    for (auto it2 = _dictcn.begin(); it2 != _dictcn.end(); it2++) {
        string sql = "INSERT INTO dictCn (word,frequency) VALUES ('";
        string m = it2->first;
        sql += m;
        sql += "',";
        sql += std::to_string(it2->second);
        sql += ");";
        Query::getInstance().doUpdateQuery(sql);

        size_t i = 0;
        while (i < m.size()) {
            string str;
            if (m.size() - i < 3) break;
            str.push_back(m[i++]);
            str.push_back(m[i++]);
            str.push_back(m[i++]);
            auto itidx = _indexcn.find(str);
            if (itidx == _indexcn.end()) {
                set<int> m;
                m.insert(line);
                _indexcn.insert(make_pair(str, m));
            } else {
                _indexcn[str].insert(line);
            }
        }
        line++;
    }
}

void DicProducer::buildIndexcn() {
    for (auto it = _indexcn.begin(); it != _indexcn.end(); it++) {
        string sql = "INSERT INTO indexCn (word, indexId) VALUES ('";
        sql += it->first;
        sql += "',";
        for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            string substr = std::to_string(*it2);
            sql += substr;
            sql += ");";
            Query::getInstance().doUpdateQuery(sql);
            sql.pop_back();
            sql.pop_back();
            size_t pos = sql.find(substr);
            sql.replace(pos, substr.length(), "");
        }
    }
}

void DicProducer::buildIndexen() {
    Query &MySQLHandle = Query::getInstance();
    // 插字母索引表
    int idx = 1;
    for (auto &elem : _dicten) {
        string word = elem.first;
        for (char c : word) {
            string cstr(1, c);
            string query = string("INSERT INTO indexEn VALUES ('")
                               .append(cstr)
                               .append("', ")
                               .append(to_string(idx))
                               .append(");");
            MySQLHandle.doUpdateQuery(query);
        }
        ++idx;
    }
}
