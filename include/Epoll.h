#ifndef EPOLL_H
#define EPOLL_H

#include <vector>
//前置声明 Channel 类
class Channel;

//跨平台适配
#if defined(__linux__)
#include <sys/epoll.h>
//Linux直接使用原生epoll类型
using EpollEvent=epoll_event;

#elif defined(__APPLE__)
//macOS用kqueue模拟epoll接口，完全兼容上层代码
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

//兼容epoll宏定义
#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3
#define EPOLLIN  0x001
#define EPOLLOUT 0x004

//模拟epoll_event结构体，对齐Linux用法
struct EpollEvent {
    uint32_t events;
    void* data;//用于存储Channel指针
};

//封装kqueue实现epoll接口
int epoll_create(int size);
int epoll_ctl(int kq,int op,int fd,EpollEvent* event);
int epoll_wait(int kq,EpollEvent* events,int maxevents,int timeout);

//给上层代码用的别名
using epoll_event=EpollEvent;

#endif

//Epoll类定义
//封装epoll系统调用，事件监听：有数据可读可写，交给EventLoop
class Epoll{
private:
    int epfd_;
    std::vector<epoll_event> events_;
public:
    Epoll();
    ~Epoll();

    //注册要监听的事件
    void updateChannel(Channel* ch);
    //等事件发生并返回结果
    std::vector<Channel*> wait(int timeout=-1);
};

#endif