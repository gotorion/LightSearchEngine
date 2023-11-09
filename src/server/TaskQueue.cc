#include "TaskQueue.h"

TaskQueue::TaskQueue(size_t queSize)
    : _queSize(queSize),
      _que(),
      _mutex(),
      _notEmpty(_mutex),
      _notFull(_mutex),
      _flag(true) {}

TaskQueue::~TaskQueue() {}

//向任务队列中添加任务
void TaskQueue::push(ElemType &&task) {
    MutexLockGuard autoLock(_mutex);  //利用栈对象的生命周期管理资源

    while (full())  //虚假唤醒
    {
        _notFull.wait();  //生产者进行睡眠
    }

    //如果任务队列不满，就可以生产任务
    _que.push(std::move(task));

    //只要任务队列中有一个任务，就需要将消费者唤醒
    _notEmpty.notify();
}

//从任务队列中取出任务
ElemType TaskQueue::pop() {
    // 1、先上锁
    MutexLockGuard autoLock(_mutex);  //利用栈对象的生命周期管理资源

    // 2、任务队列是不是空的
    while (empty() && _flag) {
        // 2.1、为空，消费者需要睡眠
        _notEmpty.wait();
    }

    if (_flag) {
        // 2.2、不为空，消费者就可以消费数据
        ElemType tmp = _que.front();
        _que.pop();

        // 3、只要任务队列中有一个以上的空位置，就可以唤醒生产者
        _notFull.notify();

        // 4、最后进行解锁
        return tmp;
    } else {
        return nullptr;
    }
}

//任务队列的空与满
bool TaskQueue::empty() const { return 0 == _que.size(); }

bool TaskQueue::full() const { return _queSize == _que.size(); }

void TaskQueue::wakeup() {
    _flag = false;
    _notEmpty.notifyAll();
}
