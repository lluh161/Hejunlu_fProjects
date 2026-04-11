#pragma once
#include "InetAddress.h"

class Socket{//封装底层系统调用，建立连接
public:
    Socket();
    ~Socket();

    //禁止拷贝：拷贝构造+拷贝赋值：保证系统资源的唯一
    Socket(const Socket&)=delete;
    Socket& operator=(const Socket&)=delete;

    //设置：按要求运行
    void setReusePort(bool on);
    void setReuseAddr(bool on);//端口立即复用
    void setNonBlock(bool on);//设置为非阻塞态

    //网络操作：收发数据
    void bind(const InetAddress& addr);
    bool listen(int backlog=5);//监听
    int accept(InetAddress& clientAddr);//等待客户端，获取客户端地址//等待客户端
    
    int fd()const{return fd_;}//返回fd_，不修改任何元素

private:
    int fd_;
};