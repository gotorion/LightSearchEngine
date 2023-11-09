#include "../include/TcpConnection.h"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::ostringstream;

TcpConnection::TcpConnection(int fd)
    : _sock(fd),
      _sockIO(fd),
      _peerAddr(getPeerAddr()),
      _localAddr(getLocalAddr()) {
    // cout << toString() << " is Connected" << endl;
}

TcpConnection::~TcpConnection() {
    // cout << toString() << " is Closed" << endl;
}

bool TcpConnection::isClosed() {
    char buf[10] = {0};
    int ret = recv(_sock.fd(), buf, sizeof(buf), MSG_PEEK);
    return ret == 0;  //没有收到消息则为关闭
}

string TcpConnection::toString() {
    ostringstream oss;
    oss << _localAddr.ip() << ":" << _localAddr.port() << "---->"
        << _peerAddr.ip() << ":" << _peerAddr.port();
    return oss.str();
}

InetAddr TcpConnection::getLocalAddr() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    //获取本端地址的函数getsockname
    int ret = getsockname(_sock.fd(), (struct sockaddr *)&addr, &len);
    if (-1 == ret) {
        perror("getsockname");
    }

    return InetAddr(addr);
}

InetAddr TcpConnection::getPeerAddr() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    //获取对端地址的函数getpeername
    int ret = getpeername(_sock.fd(), (struct sockaddr *)&addr, &len);
    if (-1 == ret) {
        perror("getpeername");
    }

    return InetAddr(addr);
}

void TcpConnection::send(const string &msg) { _sockIO.send(msg); }

string TcpConnection::receive() {
    string ret = _sockIO.receive();
    return ret;
}
