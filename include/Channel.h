#pragma once
#include <functional>
#include <cstdint>

//跨平台事件宏
#if defined(__linux__)
#include <sys/epoll.h>
#elif defined(__APPLE__)
#define EPOLLIN  0x001
#define EPOLLET  0x80000000
#endif

using ReadCallback=std::function<void()>;//把std::function<void()>简写成ReadCallback

class Channel{//封装fd为事件对象
public:
    //双参数构造
    Channel(void* loop,int fd);

    //禁止拷贝，允许移动（unique_ptr需要）
    Channel(const Channel&)=delete;
    Channel& operator=(const Channel&)=delete;
    Channel(Channel&&)=default;
    Channel& operator=(Channel&&)=default;
    
    //回调
    void handleRead();//处理读事件
    void setReadCallback(ReadCallback cb);//设置读回调参数
    void enableReading();//开启读监听

    //成员访问
    int getFd()const{return fd_;}//只读，不改
    uint32_t getEvents() const{return events_;}//获取当前监听事件
    void setRevents(int revt){revents_=revt;}//设置世纪触发事件
    bool isInEpoll() const {return inEpoll_;}
    void setInEpoll(bool in) {inEpoll_=in;}

private:
    void* loop_=nullptr;
    int fd_;//要监控的IO对象
    uint32_t events_=0;//主动注册给内核的期待监听事件
    uint32_t revents_=0;//内核返回的实际触发事件
    bool inEpoll_=false;

    ReadCallback readCallback_;//用来保存 “数据来了以后要干什么” 的函数
};