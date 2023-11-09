#ifndef __PROTOCOLPARSER_H__
#define __PROTOCOLPARSER_H__
#include <nlohmann/json.hpp>
#include <string>

using std::string;
using json = nlohmann::json;
// TODO:协议解析类
class ProtocolParser {
   public:
    ProtocolParser(const string& msg);
    ~ProtocolParser();
    string doParse();  // 解析协议
    string packMsgToJson(const string& msgId,
                         const string& message);  // 打包协议
   private:
    bool isPureEnglish(const string& msg);
    bool isPureChinese(const string& msg);

   private:
    string _msg;
};
#endif