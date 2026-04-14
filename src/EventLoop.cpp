#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include <vector>

EventLoop::EventLoop()//创建Epoll实例，每个EventLoop拥有一个独立的Epoll
    :epoll_(new Epoll),looping_(false),timerManager_(new TimerManager){}

EventLoop::~EventLoop(){
    delete epoll_;
    delete timerManager_; 
}

void EventLoop::loop(){//启动事件循环
    looping_=true;
    while (true){
        // ================== 定时器开始 ==================
        timerManager_->handleExpiredTimers();
        int timeout = timerManager_->getNextExpireMs();
        if (timeout < 0) timeout = -1;
        // ================== 定时器结束 ==================

        auto activeChannels=epoll_->wait(timeout);//调用Epoll::poll，阻塞等待事件发生

        for (Channel* ch : activeChannels){//遍历所有发生事件的Channel，执行回调
            ch->handleRead();
        }
    }
}

void EventLoop::updateChannel(Channel* ch){//更新Channel到Epoll（增/改监听事件）
    epoll_->updateChannel(ch);
}

int EventLoop::runAfter(int delayMs, TimerCallback cb){
    auto expire = std::chrono::steady_clock::now() 
                + std::chrono::milliseconds(delayMs);
    return timerManager_->addTimer(expire, cb);
}

void EventLoop::delTimer(int timerId){
    timerManager_->delTimer(timerId);
}