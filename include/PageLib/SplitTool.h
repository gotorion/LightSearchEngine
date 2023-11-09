#ifndef __SPLITTOOL_H__
#define __SPLITTOOL_H__

#include <string>
#include <vector>

#include "../simhash/cppjieba/Jieba.hpp"

using std::string;
using std::vector;

// 抽象类
class SplitTool {
   public:
    virtual ~SplitTool() {}
    // 纯虚函数, 提供接口
    virtual vector<string> cut(const string& sentence) = 0;
};

class SplitToolCppJieba : public SplitTool {
   public:
    SplitToolCppJieba();
    ~SplitToolCppJieba() {}
    vector<string> cut(const string& sentence) override;

   private:
    static cppjieba::Jieba* _jieBa;
};

#endif
