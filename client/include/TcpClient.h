#ifndef __TCPCLIENT_H__
#define __TCPCLIENT_H__

#include "Connection.h"
#include "TcpConnection.h"

class Configuration;

class TcpClient {
    friend class ProtocolParser;

   public:
    TcpClient(Configuration& conf);

    ~TcpClient();

    void start();

   private:
    //连接服务器
    int connection(Configuration& conf);
    void readInteger(int& num);

   private:
    TcpConnection _tcpCon;
};

#endif
