#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__
#include <string>
#include <vector>

using std::pair;
using std::string;
using std::vector;

class Dictionary {
   public:
    virtual vector<pair<string, int>> doQuery(const string& word) = 0;
};

#endif