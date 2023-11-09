#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include <functional>
#include <memory>

#include "InetAddress.h"
#include "Socket.h"
#include "SocketIO.h"

using std::function;
using std::shared_ptr;

class EventLoop;  //前向声明

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
    using TcpConnectionPtr = shared_ptr<TcpConnection>;
    using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;

   public:
    explicit TcpConnection(int fd, EventLoop *loop);
    ~TcpConnection();
    void send(const string &msg);
    void sendInLoop(const string &msg);
    string receive();
    //为了方便调试的函数
    string toString();

    bool isClosed() const;

    //三个事件的注册
    void setNewConnectionCallback(const TcpConnectionCallback &cb);
    void setMessageCallback(const TcpConnectionCallback &cb);
    void setCloseCllback(const TcpConnectionCallback &cb);

    //三个事件进行执行
    void handleNewConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();

   private:
    //获取本端地址与对端地址
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();

   private:
    EventLoop *_loop;  //为了让TcpConnection知道EventLoop的存在
    SocketIO _sockIO;

    //为了调试而加入的函数
    Socket _sock;
    InetAddress _localAddr;  //本端的地址
    InetAddress _peerAddr;   //对端的地址

    TcpConnectionCallback _onNewConnectionCb;  //连接建立
    TcpConnectionCallback _onMessageCb;        //消息到达
    TcpConnectionCallback _onCloseCb;          //连接断开
};

#endif
