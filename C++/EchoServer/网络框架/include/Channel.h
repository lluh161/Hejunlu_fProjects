#pragma once
#include <functional>

class Channel{//封装fd+事件+回调，事件载体
public:
    using ReadCallback=std::function<void()>;//把std::function<void()>简写成ReadCallback

    Channel(int fd);//把一个文件描述符fd包装成一个 “事件监听通道"

    void handleRead();//处理读事件
    void setReadCallback(ReadCallback cb);//设置读回调参数

    int fd()const{return fd_;}//只读，不改
    int events() const{return events_;}//获取当前监听事件
    void setRevents(int revt){revents_=revt;}//设置世纪触发事件

    void enableReading();//开启读监听

private:
    int fd_;//要监控的IO对象
    int events_;//主动注册给内核的期待监听事件
    int revents_;//内核返回的实际触发事件

    ReadCallback readCallback_;//用来保存 “数据来了以后要干什么” 的函数
};
