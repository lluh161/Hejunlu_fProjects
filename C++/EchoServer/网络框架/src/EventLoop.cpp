#include "EventLoop.h"
#include "Channel.h"
#include "Epoll.h"
#include <vector>

EventLoop::EventLoop()//创建Epoll实例，每个EventLoop拥有一个独立的Epoll
    :epoll_(new Epoll),looping_(false){}

EventLoop::~EventLoop()=default;

void EventLoop::loop(){//启动事件循环
    looping_=true;
    while (true){
        auto activeChannels=epoll_->poll(-1);//调用Epoll::poll，阻塞等待事件发生

        for (Channel* ch : activeChannels){//遍历所有发生事件的Channel，执行回调
            ch->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* ch){//更新Channel到Epoll（增/改监听事件）
    epoll_->updateChannel(ch);
}