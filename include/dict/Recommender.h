#ifndef __RECOMMENDER_H__
#define __RECOMMENDER_H__
#include <memory>
#include <nlohmann/json.hpp>
#include <queue>
#include <string>
#include <vector>

#include "CandidateResult.h"
#include "EditDistance.h"
#include "TcpConnection.h"
using std::shared_ptr;
using std::string, std::vector;
using json = nlohmann::json;
// 接口类
class Recommender {
   public:
    virtual string recommender(const string&) = 0;
};

using TcpConnectionPtr = shared_ptr<TcpConnection>;

class EnRecommender : public Recommender {
   public:
    EnRecommender() = default;
    ~EnRecommender() = default;
    string recommender(const string&) override;
};

class ChRecommender : public Recommender {
   public:
    ChRecommender() = default;
    ~ChRecommender() = default;
    string recommender(const string&) override;
    vector<string> cutString(const string& chstr);
};

class MixedRecommender : public Recommender {
   public:
    string recommender(const string&) override;
    vector<string> cutString(const string& chstr);
};

string toLower(const string& str);
#endif
