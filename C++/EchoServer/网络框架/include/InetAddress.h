#pragma once
#include <netinet/in.h>

class InetAddress{//封装IP+端口，同一管理地址
private:
    sockaddr_in addr_;//存IP+端口+协议族
public:
    InetAddress(int port);//网络地址封装类的构造函数，仅传入端口号,专门给服务器监听使用
    const sockaddr_in& getAddr() const{return addr_;}//给bind/connect等底层socket系统调用提供地址结构，同时禁止外部修改内部地址，保证封装安全
};

