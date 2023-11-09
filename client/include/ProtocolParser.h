#ifndef __PROTOCOLPARSER_H__
#define __PROTOCOLPARSER_H__

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
using std::string, std::vector;
using json = nlohmann::json;

class ProtocolParser {
   public:
    //展示当前页面栏
    void displayPageNav(int curPage, int totalPage);

    //消息处理
    void handleMessage(const string &msg);

    //将要发送的消息打包成json格式
    string jsonMsgPack(const string &msgId, const string &msg);

   private:
    void handleWebPages(const string &msg);
    void splitPages(const vector<string> &);
    char readChar() const;

   private:
    const int _pageSize = 5;
};

#endif
