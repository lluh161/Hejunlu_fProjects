#pragma once
#include <vector>
#include "Epoll.h"

class EventLoop{//事件循环，驱动整个框架
private:
    Epoll* epoll_;//负责系统级监听
    std::vector<Channel*> activeChannels_;//存放已经就绪的事件
public:
    EventLoop();//初始化epoll
    void loop();//不断调用epoll_wait监听事件，一旦有就绪事件，就取出并处理
    void updateChannel(Channel* ch);//把一个Channel（scoket+事件+回调）注册/更新/删除到Epoll中，让Epoll监听它的事件
};