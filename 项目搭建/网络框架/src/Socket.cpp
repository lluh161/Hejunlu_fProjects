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

void Socket::setNonBlock(){
    int flag=fcntl(fd_,F_GETFL);//获取fd_原有属性
    fcntl(fd_,F_SETFL,flag|O_NONBLOCK);//新增非阻塞属性到flag
}

void Socket::setReuseAddr(){
    int opt=1;
    //给套字节本身设置各种运行参数
    setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
}

bool Socket::bind(int port){
    sockaddr_in addr{};//定义并清零一个 IPv4 地址结构体变量。
    addr.sin_family=AF_INET;//用IPv4地址协议
    addr.sin_port=htons(port);//设置端口号
    addr.sin_addr.s_addr=INADDR_ANY;//监听本机所有可用网卡IP
    return ::bind(fd_,(sockaddr*)&addr,sizeof(addr))!=-1;//调用系统bind函数绑定IP端口，返回绑定是否成功
}

bool Socket::listen(int backlog){
    return ::listen(fd_,backlog)!=-1;//调用系统listen，返回是否成功
}

int Socket::accept(){
    return ::accept(fd_,nullptr,nullptr);//等客户端连接，一连接就返回一个新的socket用于通信
}