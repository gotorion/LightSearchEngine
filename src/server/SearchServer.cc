#include "SearchServer.h"

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <type_traits>

#include "CacheManager.h"
#include "ProtocolParser.h"
#include "Recommender.h"
#include "RedisServer.h"
#include "Thread.h"
#include "Time.h"
#include "WebPageQuery.h"
using std::atoi;
// SearchServer::SearchServer(size_t threadNum, size_t queSize, const string
// &ip,
//                            unsigned short port)
//     : _pool(threadNum, queSize), _server(ip, port) {}
SearchServer::SearchServer()
    : _conf(Configuration::getInstance()),
      _pool(atoi(_conf->getItem("THREAD_POOL_SIZE").c_str()),
            atoi(_conf->getItem("TASK_QUEUE_SIZE").c_str())),
      _server(_conf->getItem("IP"), atoi(_conf->getItem("PORT").c_str())) {}
SearchServer::~SearchServer() {}

void SearchServer::start() {
    using namespace std::placeholders;
    using std::bind;
    _pool.start();
    _server.setAllCallback(bind(&SearchServer::onNewConnection, this, _1),
                           bind(&SearchServer::onMessage, this, _1),
                           bind(&SearchServer::onClose, this, _1));
    _server.start();
}

void SearchServer::stop() {
    _server.stop();

    _pool.stop();
}

void SearchServer::onNewConnection(const TcpConnectionPtr &con) {
    std::cout << con->toString() << " has connected!" << std::endl;
}

void SearchServer::onMessage(const TcpConnectionPtr &con) {
    string queryWord = con->receive();
    _pool.addTask(std::bind(&SearchServer::threadTask, this, con, queryWord));
}

void SearchServer::onClose(const TcpConnectionPtr &con) {
    std::cout << con->toString() << " disconnected!" << std::endl;
}

void SearchServer::threadTask(const TcpConnectionPtr &con, const string &msg) {
    // 业务代码在这里
    // 解析协议，处理消息
    json msgJson = json::parse(msg);
    string messageId = msgJson["messageId"];
    string message = msgJson["message"];
    // 先查询缓存
    std::cout << "Current thread: " << threadID;
    if (messageId == "1") {
        Time t;
        LRUCache &LRU = CacheManager::getInstance()->getCache(threadID);
        string getFromLRU = LRU.get(message);
        if (getFromLRU.size() == 0) {
            std::cout << " Cache missed, total time: ";
            ProtocolParser parser(msg);
            string tocMessage = parser.doParse();
            if (tocMessage == "") {
                tocMessage = parser.packMsgToJson("100", tocMessage);
            } else {
                LRU.put(message, tocMessage, false);
            }
            t.end();
            con->sendInLoop(tocMessage);
        } else {
            std::cout << " Cache hit, total time: ";
            t.end();
            LogInfo("Cache命中,关键字查询: %s", message.c_str());
            con->sendInLoop(getFromLRU);
        }
    } else {
        // 网页缓存
        Time t;
        string result = RedisServer::getInstance().get(message);
        if (result.size() == 0) {
            std::cout << " Cache missed, total time: ";
            ProtocolParser parser(msg);
            string tocMessage = parser.doParse();
            if (tocMessage != "") {  // 查到网页了才能往缓存里放
                RedisServer::getInstance().put(message, tocMessage);
            } else {
                tocMessage = parser.packMsgToJson("200", tocMessage);
            }
            t.end();
            con->sendInLoop(tocMessage);

        } else {
            std::cout << " Cache hit, total time: ";
            t.end();
            LogInfo("Cache命中,网页查询: %s", message.c_str());
            con->sendInLoop(result);
        }
    }
}