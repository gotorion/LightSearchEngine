#include "ProtocolParser.h"

#include <memory>

#include "Recommender.h"
#include "WebPageQuery.h"
#include "mylogger.h"
using std::unique_ptr;

ProtocolParser::ProtocolParser(const string& msg) : _msg(msg) {}

ProtocolParser::~ProtocolParser() {}

string ProtocolParser::doParse() {
    auto msgJson = json::parse(_msg);
    string messageId = msgJson["messageId"];
    string message = msgJson["message"];

    if (!isPureEnglish(message) && !isPureChinese(message)) {
        // 中英文混合
        if (messageId == "1") {
            unique_ptr<Recommender> reco(new MixedRecommender());
            string recoResult = reco->recommender(message);
            if (recoResult == "") {
                return "";
            }
            return packMsgToJson("100", recoResult);
        } else {
            return "";
        }
    } else if (isPureEnglish(message)) {
        // 只有英文，走英文关键字查询或网页查询
        if (messageId == "1") {  // 关键字查询
            unique_ptr<Recommender> reco(new EnRecommender());
            string lowMessage = toLower(message);  // 先转小写查询
            string recoResult = reco->recommender(lowMessage);
            if ("" == recoResult) {
                return recoResult;
            }
            LogInfo("Cache未命中,关键字查询: %s", message.c_str());
            return packMsgToJson("100", recoResult);
        } else {  // 网页查询
            return packMsgToJson("200", "");
        }
    } else {
        // 只有中文，走中文关键字查询或网页查询
        if (messageId == "1") {  // 中文关键字查询
            unique_ptr<Recommender> reco(new ChRecommender());
            string recoResult = reco->recommender(message);
            // std::cout << "reco Result = " << std::endl;
            if (recoResult == "") {
                return recoResult;
            }
            LogInfo("Cache未命中,关键字查询: %s", message.c_str());
            return packMsgToJson("100", recoResult);
        } else {
            LogInfo("Cache未命中,网页查询: %s", message.c_str());
            unique_ptr<WebPageQuery> pageQuery(new WebPageQuery());
            string webResult = pageQuery->doQuery(message);
            if (webResult == "") {
                return "";
            } else {
                return packMsgToJson("200", webResult);
            }
        }
    }
}

string ProtocolParser::packMsgToJson(const string& msgId, const string& msg) {
    json pack;
    pack["messageId"] = msgId;
    pack["message"] = msg;
    return pack.dump(-1, ' ', false, json::error_handler_t::ignore);
}

bool ProtocolParser::isPureEnglish(const string& msg) {
    for (auto& ch : msg) {
        if (!isalpha(ch) && ch != ' ') {
            return false;
        }
    }
    return true;
}

bool ProtocolParser::isPureChinese(const string& msg) {
    for (auto& ch : msg) {
        if (isalpha(ch)) {
            return false;
        }
    }
    return true;
}