#include "../include/TcpClient.h"

#include <iostream>

#include "../include/Colours.h"
#include "../include/Configuration.h"
#include "../include/ProtocolParser.h"

using std::cin;
using std::cout;
using std::endl;

TcpClient::TcpClient(Configuration& conf) : _tcpCon(connection(conf)) {}

TcpClient::~TcpClient() {}

int TcpClient::connection(Configuration& conf) {
    string ip = conf.getConfMap()["ip"];
    unsigned short port = atoi(conf.getConfMap()["port"].c_str());
    Connection conn(ip, port);
    return conn.fd();
}

void TcpClient::start() {
    int messageId = 1;  //默认keyword search
    string str;
    while (1) {
        cout << "****************************************************" << endl;
        cout << "*                   '0': quit                      *" << endl;
        cout << "*                   '1': keyword search            *" << endl;
        cout << "*                   '2': web search                *" << endl;
        cout << "****************************************************" << endl;
        int messageId;
        readInteger(messageId);
    keyword:
        if (messageId == 1) {
            while (1) {
                cout << RED << "<Keyword>$ " << RESET;
                getline(cin, str, '\n');
                str.erase(std::remove_if(str.begin(), str.end(), ::isspace),
                          str.end());
                if (str.empty()) {
                    continue;
                } else if (str == "0") {
                    return;
                } else if (str == "1") {
                    messageId = 1;
                    goto keyword;
                } else if (str == "2") {
                    messageId = 2;
                    goto webpage;
                } else {
                    ProtocolParser pp;
                    string message = pp.jsonMsgPack("1", str);
                    _tcpCon.send(message);
                    message = _tcpCon.receive();
                    pp.handleMessage(message);
                }
            }
        }
    webpage:
        if (messageId == 2) {
            while (1) {
                cout << RED << "<Web>$ " << RESET;
                getline(cin, str);
                str.erase(std::remove_if(str.begin(), str.end(), ::isspace),
                          str.end());
                if (str.empty()) {
                    continue;
                } else if (str == "0") {
                    return;
                } else if (str == "1") {
                    messageId = 1;
                    goto keyword;
                } else if (str == "2") {
                    messageId = 2;
                    goto webpage;
                } else {
                    ProtocolParser pp;
                    string message = pp.jsonMsgPack("2", str);
                    _tcpCon.send(message);
                    message = _tcpCon.receive();
                    pp.handleMessage(message);
                }
            }
        }
    }
}

void TcpClient::readInteger(int& num) {
    cout << "What's your choice>>> ";
    while (cin >> num, !cin.eof()) {
        if (cin.bad()) {
            cout << "FATAL ERROR" << endl;
        } else if (cin.fail()) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid command, Enter again>>> ";
        } else if (num != 1 && num != 2 && num != 0) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid command, Enter again>>> ";
        } else {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
    }
}