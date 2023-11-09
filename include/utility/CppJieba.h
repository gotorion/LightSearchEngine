#ifndef __CPPJIEBA_H__
#define __CPPJIEBA_H__
#include "../cppjieba/Jieba.hpp"
#include "SplitTool.h"
using jieba = cppjieba::Jieba;
class JiebaTool : public SplitTool {
   public:
    static JiebaTool& getInstance();
    vector<string> cut(const string& target) override;

   private:
    JiebaTool();
    ~JiebaTool();

   private:
    jieba _jieba;
};

#endif