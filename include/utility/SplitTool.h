#ifndef __SPLITTOOL_H__
#define __SPLITTOOL_H__

#include <string>
#include <vector>
using std::string;
using std::vector;

class SplitTool {
   public:
    virtual vector<string> cut(const string& target) = 0;
};
#endif
