#pragma once

class Socket{
public:
    Socket();
    ~Socket();

    //禁止拷贝：拷贝构造+拷贝赋值：保证系统资源的唯一
    Socket(const Socket&)=delete;
    Socket& operator=(const Socket&)=delete;

    //设置：按要求运行
    void setNonBlock();//设置为非阻塞态
    void setReuseAddr();//端口立即复用

    //网络操作：收发数据
    bool bind(int port);//绑定端口
    bool listen(int backlog=5);//监听
    int accept();//等待客户端
    
    int fd() const{return fd_;}//返回fd_，不修改任何元素

private:
    int fd_;
};