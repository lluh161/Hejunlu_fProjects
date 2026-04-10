#pragma once
#include <vector>
#include <sys/epoll.h>

class Channel;

class Epoll{//封装epoll系统调用，事件监听
private:
    int epfd_;
    stf::vector<epoll_event> event_;

public:
    Epoll();
    ~Epll();

    void updateChannel(Channel* ch);//注册要监听的事件
    std::vector<Channel*> wait(int timeout=-1);//等事件发生并返回结果
};