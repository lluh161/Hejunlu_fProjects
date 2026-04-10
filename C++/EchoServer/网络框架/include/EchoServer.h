#pragma once
#include <vector>
#include <memory>
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"

class Channel;
class Buffer;
class InetAddress;

class EchoServer
{
public:
    EchoServer(EventLoop* loop,const InetAddress& listenAddr);//传入主事件循环+监听的IP端口，初始化服务器
    ~EchoServer()=default;//销毁

    void start();//开启子Reactor循环，开始处理连接

private:
    void handleNewConnection();//处理新客户端连接
    void handleMessage(int clientFd,Buffer* buf);//处理客户端消息

    EventLoop* loop_;//主Reactor：只负责监听新连接
    std::unique_ptr<Socket> socket_;//服务端监听套接字
    std::unique_ptr<Channel> acceptChannel_;//监听通道：绑定epoll，感知新连接
    std::unique_ptr<ThreadPool> threadPool_;//线程池：异步处理业务，不阻塞IO

    std::vector<std::unique_ptr<EventLoop>> subLoops_;//子Reactor：处理客户端IO
    std::vector<std::unique_ptr<Channel>> clientChannels_;//管理所有客户端连接通道
    std::vector<std::unique_ptr<Buffer>> clientBuffers_;//管理所有客户端数据缓冲区
};