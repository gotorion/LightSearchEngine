#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <string.h>

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using std::map;
using std::set;
using std::string;
using std::unordered_set, std::unordered_map;
using std::vector;

class DicProducer {
   public:
    DicProducer();
    void buildEnDict();
    void buildCnDict();
    void buildIndexcn();
    void buildIndexen();

   private:
    vector<string> _filesen;
    vector<string> _filescn;
    unordered_map<string, int> _dicten;
    map<string, int> _dictcn;
    unordered_map<string, set<int>> _indexen;
    map<string, set<int>> _indexcn;
    unordered_set<string> stopen;
    unordered_set<string> stopcn;
    map<string, string> _conf;
};

#endif
