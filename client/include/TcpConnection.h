#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include <string>

#include "InetAddr.h"
#include "Socket.h"
#include "SocketIO.h"

using std::string;

class TcpConnection {
   public:
    explicit TcpConnection(int fd);

    ~TcpConnection();

    bool isClosed();

    string toString();

    void send(const string &msg);

    string receive();

   private:
    InetAddr getLocalAddr();
    InetAddr getPeerAddr();

   private:
    Socket _sock;
    SocketIO _sockIO;
    InetAddr _peerAddr;
    InetAddr _localAddr;
};

#endif
