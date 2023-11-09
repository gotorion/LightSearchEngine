#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <sys/epoll.h>

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "MutexLock.h"
#include "Timerfd.h"

using std::function;
using std::map;
using std::shared_ptr;
using std::vector;

class Acceptor;       //前向声明
class TcpConnection;  //前向声明

using TcpConnectionPtr = shared_ptr<TcpConnection>;
using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;
using Functor = function<void()>;

class EventLoop {
   public:
    EventLoop(Acceptor &acceptor);
    ~EventLoop();

    //事件是否在循环
    void loop();
    void unloop();

    //三个事件的注册
    void setNewConnectionCallback(TcpConnectionCallback &&cb);
    void setMessageCallback(TcpConnectionCallback &&cb);
    void setCloseCllback(TcpConnectionCallback &&cb);

   private:
    //封装了epoll_wait
    void waitEpollFd();

    //处理新的连接请求的函数
    void handleNewConnection();
    //处理老的连接请求上的数据的收发
    void handleMessage(int fd);

    //创建epoll的文件描述符
    int createEpollFd();
    //将文件描述符放在红黑树上进行监听
    void addEpollReadFd(int fd);
    //将文件描述符从红黑树上摘除
    void delEpollReadFd(int fd);

   public:
    //线程池中的工作线程唤醒EventLoop
    void wakeup();

   private:
    // EventLoop阻塞等待被线程池唤醒
    void handleRead();
    //创建_evtfd文件描述符的函数
    int createEventFd();
    //执行“任务”
    void doPendingFunctors();

   public:
    void runInLoop(Functor &&cb);

   private:
    int _epfd;         //就是epoll_create创建出来的文件描述符
    int _evtfd;        //用于通信的文件描述符(eventfd)
    TimerFd _timerfd;  // 用于定时更新Cache
    vector<struct epoll_event> _evtList;  //存放满足条件的文件描述符的结合
    bool _isLooping;                      //标识整个类是不是在循环
    Acceptor &
        _acceptor;  //作用就是为了调用accept函数，也就是为了三次握手建立连接的标志
    map<int, TcpConnectionPtr>
        _conns;  //存放的是文件描述符与TcpConnection的键值对

    TcpConnectionCallback _onNewConnectionCb;  //连接建立
    TcpConnectionCallback _onMessageCb;        //消息到达
    TcpConnectionCallback _onCloseCb;          //连接断开

    vector<Functor> _pengdings;  //存放待执行的“任务”（要发送的数据）
    MutexLock _mutex;            //互斥锁，互斥访问vector
};

#endif
