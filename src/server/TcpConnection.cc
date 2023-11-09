#include "TcpConnection.h"

#include <iostream>
#include <sstream>

#include "EventLoop.h"

using std::ostringstream;

TcpConnection::TcpConnection(int fd, EventLoop *loop)
    : _loop(loop),
      _sockIO(fd),
      _sock(fd),
      _localAddr(getLocalAddr()),
      _peerAddr(getPeerAddr()) {}

TcpConnection::~TcpConnection() {}

void TcpConnection::send(const string &msg) {
    int len = msg.length();
    _sockIO.writen((char *)&len, sizeof(int));
    _sockIO.writen(msg.c_str(), msg.size());
}

//--------------------------------------------------------------
//该函数怎么实现呢？或者说实现什么东西呢？
//实质就是需要将处理好之后的msg，也就是函数sendInLoop
//中的参数msg传递给EventLoop/Reactor
//即使将msg发送给了EventLoop对象，但是EventLoop本身
//没有发送数据的能力，所以在此刻需要将处理好之后的
//数据msg以及发送数据的能力TcpConnection都一次发给
// EventLoop，这样EventLoop才能将数据发送给客户端
//所以“任务”就是指：要发送的数据以及发送数据能力的
// TcpConnection对象以及send函数
void TcpConnection::sendInLoop(const string &msg) {
    if (_loop) {
        // void runInLoop(function<void()> &&cb);
        _loop->runInLoop(std::bind(&TcpConnection::send, this, msg));
    }
}
//--------------------------------------------------------------

string TcpConnection::receive() {
    int len;
    _sockIO.readn((char *)(&len), sizeof(int));
    char buff[65534] = {0};
    _sockIO.readLine(buff, len + 1);
    return string(buff);
}

string TcpConnection::toString() {
    ostringstream oss;
    oss << _localAddr.ip() << ":" << _localAddr.port() << "---->"
        << _peerAddr.ip() << ":" << _peerAddr.port();

    return oss.str();
}

bool TcpConnection::isClosed() const {
    char buf[10] = {0};
    int ret = ::recv(_sock.fd(), buf, sizeof(buf), MSG_PEEK);
    return (0 == ret);
}

void TcpConnection::setNewConnectionCallback(const TcpConnectionCallback &cb) {
    _onNewConnectionCb = cb;
}

void TcpConnection::setMessageCallback(const TcpConnectionCallback &cb) {
    _onMessageCb = cb;
}

void TcpConnection::setCloseCllback(const TcpConnectionCallback &cb) {
    _onCloseCb = cb;
}

void TcpConnection::handleNewConnectionCallback() {
    if (_onNewConnectionCb) {
        /* _onNewConnectionCb(shared_ptr<TcpConnection>(this));//执行回调函数 */
        _onNewConnectionCb(shared_from_this());  //执行回调函数
    }
}
void TcpConnection::handleMessageCallback() {
    if (_onMessageCb) {
        _onMessageCb(shared_from_this());  //执行回调函数
    }
}

void TcpConnection::handleCloseCallback() {
    if (_onCloseCb) {
        _onCloseCb(shared_from_this());  //执行回调函数
    }
}
//获取本端的网络地址信息
InetAddress TcpConnection::getLocalAddr() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    //获取本端地址的函数getsockname
    int ret = getsockname(_sock.fd(), (struct sockaddr *)&addr, &len);
    if (-1 == ret) {
        perror("getsockname");
    }

    return InetAddress(addr);
}

//获取对端的网络地址信息
InetAddress TcpConnection::getPeerAddr() {
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);
    //获取对端地址的函数getpeername
    int ret = getpeername(_sock.fd(), (struct sockaddr *)&addr, &len);
    if (-1 == ret) {
        perror("getpeername");
    }

    return InetAddress(addr);
}
