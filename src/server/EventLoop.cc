#include "EventLoop.h"

#include <stdio.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <iostream>

#include "Acceptor.h"
#include "CacheManager.h"
#include "TcpConnection.h"

using std::cout;
using std::endl;

EventLoop::EventLoop(Acceptor &acceptor)
    : _epfd(createEpollFd()),
      _evtfd(createEventFd()),
      _timerfd(
          2, 2,
          std::bind(&CacheManager::updateCache, CacheManager::getInstance())),
      // 设置每两秒更新Cache
      _evtList(1024),
      _isLooping(false),
      _acceptor(acceptor),
      _mutex() {
    //需要将listenfd放在红黑树上进行监听
    // 1、listenfd如何得到呢？
    int listenfd = _acceptor.fd();
    // 2、监听的函数设计好了没有呢？
    addEpollReadFd(listenfd);
    //监听用于通信的_evtfd
    addEpollReadFd(_evtfd);
    addEpollReadFd(_timerfd.fd());
}

EventLoop::~EventLoop() {
    close(_epfd);
    close(_evtfd);
}

//事件是否在循环
void EventLoop::loop() {
    _isLooping = true;
    while (_isLooping) {
        waitEpollFd();
    }
}

void EventLoop::unloop() { _isLooping = false; }

void EventLoop::setNewConnectionCallback(TcpConnectionCallback &&cb) {
    _onNewConnectionCb = std::move(cb);
}

void EventLoop::setMessageCallback(TcpConnectionCallback &&cb) {
    _onMessageCb = std::move(cb);
}

void EventLoop::setCloseCllback(TcpConnectionCallback &&cb) {
    _onCloseCb = std::move(cb);
}

//封装了epoll_wait
void EventLoop::waitEpollFd() {
    int nready = 0;
    do {
        nready = epoll_wait(_epfd, &*_evtList.begin(), _evtList.size(), 3000);
    } while (-1 == nready && errno == EINTR);  //中断

    if (-1 == nready) {
        perror("waitEpollFd");
        return;
    } else if (0 == nready) {
    } else {
        //如果满足条件的文件描述符超过了上线1024就应该要扩容
        if (nready == (int)_evtList.size()) {
            _evtList.resize(2 * nready);
        }

        for (int idx = 0; idx < nready; ++idx) {
            int fd = _evtList[idx].data.fd;
            if (fd == _acceptor.fd())  //是不是有新的连接请求
            {
                if (_evtList[idx].events & EPOLLIN) {
                    handleNewConnection();  //处理新的连接请求
                }
            } else if (fd == _evtfd)  //用于通信的文件描述符
            {
                if (_evtList[idx].events & EPOLLIN) {
                    handleRead();
                    doPendingFunctors();  //将所有的“任务”进行执行
                }
            } else if (fd == _timerfd.fd()) {  // Timerfd就绪，执行更新Cache
                if (_evtList[idx].events & EPOLLIN) {
                    _timerfd.handleRead();
                    _timerfd.handleTimerCallback();
                }
            } else {
                if (_evtList[idx].events & EPOLLIN) {
                    handleMessage(fd);  //处理老的连接请求
                }
            }
        }
    }
}

//处理新的连接请求的函数
void EventLoop::handleNewConnection() {
    //如果connfd有正确的返回结果，就表明三次握手建立成功了
    int connfd = _acceptor.accept();
    if (connfd < 0) {
        perror("handleNewConnection");
        return;
    }

    //将connfd放在红黑树上进行监听
    addEpollReadFd(connfd);

    //可以使用文件描述符创建TcpConnection对象
    //创建了TcpConnection的对象
    TcpConnectionPtr con(new TcpConnection(connfd, this));

    //此处是连接刚刚创建的位置
    con->setNewConnectionCallback(_onNewConnectionCb);  //连接建立的注册
    con->setMessageCallback(_onMessageCb);              //消息到达的注册
    con->setCloseCllback(_onCloseCb);                   //连接断开的注册

    //需要将文件描述符与TcpConnnection的对象存放在map中
    _conns.insert(std::make_pair(connfd, con));

    con->handleNewConnectionCallback();  // 1、连接建立的时机到了
}

//处理老的连接请求上的数据的收发
void EventLoop::handleMessage(int fd) {
    auto iter = _conns.find(fd);
    if (iter != _conns.end()) {
        bool flag = iter->second->isClosed();
        if (flag) {
            //连接马上要断开
            iter->second->handleCloseCallback();  // 2、连接断开的时机
            delEpollReadFd(fd);
            _conns.erase(iter);
        } else {
            //可以继续进行收发数据
            iter->second->handleMessageCallback();  // 3、消息到达的时机
            /* string msg = iter->second->receive(); */
            /* iter->second->send(msg); */
        }
    } else {
        cout << "该连接是不存在的 " << endl;
        return;
    }
}

//创建epoll的文件描述符
int EventLoop::createEpollFd() {
    int fd = epoll_create1(0);
    if (fd < 0) {
        perror("epoll_create1");
        return fd;
    }
    return fd;
}

//将文件描述符放在红黑树上进行监听
void EventLoop::addEpollReadFd(int fd) {
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &evt);
    if (ret < 0) {
        perror("epoll_ctl add");
        return;
    }
}
//将文件描述符从红黑树上摘除
void EventLoop::delEpollReadFd(int fd) {
    struct epoll_event evt;
    /* evt.events = EPOLLIN; */
    evt.data.fd = fd;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &evt);
    if (ret < 0) {
        perror("epoll_ctl del");
        return;
    }
}
//线程池中的工作线程唤醒EventLoop
void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t ret = ::write(_evtfd, &one, sizeof(one));
    if (ret != sizeof(one)) {
        perror("wakeup");
        return;
    }
}
// EventLoop阻塞等待被线程池唤醒
void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t ret = ::read(_evtfd, &one, sizeof(one));
    if (ret != sizeof(one)) {
        perror("read");
        return;
    }
}
//创建_evtfd文件描述符的函数
int EventLoop::createEventFd() {
    int fd = eventfd(10, 0);
    if (fd < 0) {
        perror("createEventFd");
        return fd;
    }
    return fd;
}

//执行“任务”
void EventLoop::doPendingFunctors() {
    //从vector拿出“任务”并且执行
    //"任务"就是要发送给客户端的数据msg与
    //发送数据能力的TcpConnection以及send
    //函数

    vector<Functor> tmp;
    {
        MutexLockGuard autoLock(_mutex);
        tmp.swap(_pengdings);  //将_pendings中的“任务”换出来
    }

    for (auto &cb : tmp) {
        cb();  //回调“任务”
    }
}
void EventLoop::runInLoop(Functor &&cb) {
    //块作用域
    {
        MutexLockGuard autoLock(_mutex);
        _pengdings.push_back(std::move(cb));
    }

    //"任务"已经存放到了vector，就表明已经
    //存在了EventLoop
    wakeup();
}
