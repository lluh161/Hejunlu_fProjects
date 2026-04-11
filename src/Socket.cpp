#include "Socket.h"//自己定义的头文件
#include <unistd.h>//Linux/Unix标准系统调用：read/write/close/sleep
#include <fcntl.h>//文件描述符控制：fcntl()，用于把socket设为非阻塞
#include <sys/socket.h>//套接字核心API：socket()、bind()、listen()、accept()、connect()
#include <netinet/in.h>//网络地址结构：sockaddr_in、htons/ntohs端口/IP转换

Socket::Socket(){
    //创建一个TCP网络套接字，返回文件描述符fd_
    fd_=socket(AF_INET,SOCK_STREAM,0);//（使用IPv4，流式套字节，自动对应TCP协议）
}

Socket::~Socket(){
    if(fd_>=0)//如果文件打开了，有效
        close(fd_);
}

void Socket::bind(const InetAddress& addr){
    ::bind(fd_, (sockaddr*)&addr.getAddr(), sizeof(sockaddr_in));
}

bool Socket::listen(int backlog){
    return ::listen(fd_,backlog)!=-1;//调用系统listen，返回是否成功
}

int Socket::accept(){
    return ::accept(fd_,nullptr,nullptr);//等客户端连接，一连接就返回一个新的socket用于通信
}

void Socket::setReuseAddr(bool on) {
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

void Socket::setReusePort(bool on) {
    int opt = on ? 1 : 0;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
}

void Socket::setNonBlock(bool on) {
    int flags = fcntl(fd_, F_GETFL, 0);
    if (on)
        fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
    else
        fcntl(fd_, F_SETFL, flags & ~O_NONBLOCK);
}