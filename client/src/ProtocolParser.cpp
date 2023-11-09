#include "../include/ProtocolParser.h"

#include <sys/socket.h>

#include <iostream>
#include <vector>

#include "../include/Colours.h"
using std::cout;
using std::endl, std::cin;
using std::vector;

void ProtocolParser::handleMessage(const string &msg) {
    auto msgJson = json::parse(msg);
    string messageId = msgJson["messageId"];
    string message = msgJson["message"];

    if (messageId == "100") {  // 关键字推荐
        if (message == "") {
            cout << "没有找到关键字" << endl;
        } else {
            auto wordsJson = json::parse(message);
            vector<string> keywords = wordsJson["Keywords"];
            cout << YELLOW << ">>> ";
            for (auto &keyword : keywords) {
                cout << keyword << " ";
            }
            cout << endl;
        }
    } else if (messageId == "200") {  // 网页搜索
        if (message == "") {
            cout << "没有找到想要的网页" << endl;
            return;
        } else {
            handleWebPages(message);
        }
    }
}

void ProtocolParser::handleWebPages(const string &msg) {
    auto websJson = json::parse(msg);
    vector<string> webpages = websJson["WebPage"];
    splitPages(webpages);
}

void ProtocolParser::splitPages(const vector<string> &pages) {
    // BUG:翻页功能
    int curIdx = 0;
    cout << BOLDYELLOW << "<----共查询到 " << pages.size() << " 个网页,当前在第"
         << (curIdx / _pageSize) + 1 << "页---->" << RESET << endl;
    while (true) {
        if (pages.size() <= _pageSize) {  // 如果少于5个网页就不用分了
            for (auto &page : pages) {
                cout << "--------------------------------------" << endl;
                cout << page << endl;
            }
            cout << "--------------------------------------" << endl;
            break;
        } else {
            for (int i = 0; i < _pageSize; ++i) {
                cout << "--------------------------------------" << endl;
                cout << pages[curIdx + i] << endl;
            }
            cout << "--------------------------------------" << endl;
            cout << GREEN << endl
                 << "--------< 上一页 -<q to quit>- 下一页 >--------" << RESET
                 << endl
                 << endl;
            while (true) {
                char select = readChar();
                if (select == '<' && curIdx >= _pageSize &&
                    curIdx < pages.size()) {  // 往前翻页
                    system("clear");
                    curIdx -= _pageSize;
                    cout << BOLDYELLOW << "<----共查询到 " << pages.size()
                         << " 个网页,当前在第" << (curIdx / _pageSize) + 1
                         << "页---->" << RESET << endl;
                    for (int i = 0; i < _pageSize; ++i) {
                        cout << "--------------------------------------"
                             << endl;
                        cout << pages[curIdx + i] << endl;
                    }
                    cout << "--------------------------------------" << endl;
                } else if (select == '>' && curIdx < pages.size() - _pageSize &&
                           curIdx >= 0) {
                    system("clear");
                    curIdx += _pageSize;
                    cout << BOLDYELLOW << "<----共查询到 " << pages.size()
                         << " 个网页,当前在第" << (curIdx / _pageSize) + 1
                         << "页---->" << RESET << endl;
                    for (int i = 0; i < _pageSize && curIdx + i < pages.size();
                         ++i) {
                        cout << "--------------------------------------"
                             << endl;
                        cout << pages[curIdx + i] << endl;
                    }
                    cout << "--------------------------------------" << endl;
                } else if (select == 'q') {
                    return;
                } else if (select == '>') {
                    cout << "没有更多内容了..." << std::endl;
                } else if (select == '<') {
                    cout << "你已到达第一页..." << std::endl;
                }
                cout << GREEN << endl
                     << "--------< 上一页 -<q to quit>- 下一页 >--------"
                     << RESET << endl
                     << endl;
            }
        }
    }
}

char ProtocolParser::readChar() const {
    char ch;
    while (cin >> ch, !cin.eof()) {
        if (cin.bad()) {
            cout << "FATAL ERROR" << endl;
        } else if (cin.fail()) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else if (ch != '<' && ch != '>' && ch != 'q') {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
    }
    return ch;
}

string ProtocolParser::jsonMsgPack(const string &msgId, const string &msg) {
    json msgJson;
    msgJson["messageId"] = msgId;
    msgJson["message"] = msg;
    return msgJson.dump(-1, ' ', false, json::error_handler_t::ignore);
}
