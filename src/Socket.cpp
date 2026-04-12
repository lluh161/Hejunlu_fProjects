#include "Socket.h"//自己定义的头文件
#include <unistd.h>//Linux/Unix标准系统调用：read/write/close/sleep
#include <fcntl.h>//文件描述符控制：fcntl()，用于把socket设为非阻塞
#include <sys/socket.h>//套接字核心API：socket()、bind()、listen()、accept()、connect()
#include <netinet/in.h>//网络地址结构：sockaddr_in、htons/ntohs端口/IP转换

Socket::Socket(){//创建一个TCP网络套接字，返回文件描述符fd_
    fd_=socket(AF_INET,SOCK_STREAM,0);//（使用IPv4，流式套字节，自动对应TCP协议）
}

Socket::~Socket(){
    if(fd_>=0)//如果文件打开了，有效
        close(fd_);
}

void Socket::bind(const InetAddress& addr){//绑定端口
    ::bind(fd_, (sockaddr*)&addr.getAddr(), sizeof(sockaddr_in));
}

bool Socket::listen(int backlog){//开启监听
    return ::listen(fd_,backlog)!=-1;//调用系统listen，返回是否成功
}

int Socket::accept(InetAddress& clientAddr){//等待客户端，获取客户端地址
    struct sockaddr_in client_addr;
    socklen_t client_len=sizeof(client_addr);
    int client_fd=::accept(fd_,(struct sockaddr*)&client_addr,&client_len);
    return client_fd;
}

void Socket::setReuseAddr(){//端口立即复用
    int opt=1;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
}

void Socket::setReusePort(){//多线程/进程绑定一个端口，实现负载均衡
    int opt=1;
    setsockopt(fd_,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
}

void Socket::setNonBlock(bool on){//设置为非阻塞态
    int flags=fcntl(fd_,F_GETFL,0);
    if(on)
        fcntl(fd_,F_SETFL,flags|O_NONBLOCK);
    else
        fcntl(fd_,F_SETFL,flags&~O_NONBLOCK);
}