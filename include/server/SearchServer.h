#ifndef __SearchSERVER_H__
#define __SearchSERVER_H__

#include "TcpConnection.h"
#include "TcpServer.h"
#include "ThreadPool.h"
#include "configure.h"
#include "mylogger.h"
class SearchServer {
   public:
    // SearchServer(size_t threadNum, size_t queueNum,const string& ip, unsigned
    // short port);
    SearchServer();
    ~SearchServer();
    //服务器启停
    void start();
    void stop();

    //三个回调
    void onNewConnection(const TcpConnectionPtr &con);
    void onMessage(const TcpConnectionPtr &con);
    void onClose(const TcpConnectionPtr &con);
    void threadTask(const TcpConnectionPtr &con, const string &msg);

   private:
    Configuration *_conf;
    ThreadPool _pool;
    TcpServer _server;
};

#endif