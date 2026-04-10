#include "EchoServer.h"
#include "Channel.h"
#include "Buffer.h"
#include "InetAddress.h"
#include <iostream>
#include <functional>
#include <unistd.h>
#include <thread>

EchoServer::EchoServer(EventLoop* loop, const InetAddress& listenAddr)//初始化服务器
    : loop_(loop),//主Reactor
      socket_(std::make_unique<Socket>()),
      acceptChannel_(std::make_unique<Channel>(loop, socket_->fd())){
    
    //设置端口复用
    socket_->setReuseAddr(true);
    socket_->setReusePort(true);

    socket_->bind(listenAddr);// 绑定IP+端口
    socket_->listen();//开始监听

    //新连接事件绑定到handleNewConnection
    acceptChannel_->setReadCallback(std::bind(&EchoServer::handleNewConnection,this));
    acceptChannel_->enableReading();//注册到epoll

    //初始化业务线程池
    threadPool_=std::make_unique<ThreadPool>(4);
    threadPool_->start();

    //创建3个从Reactor，处理客户端IO
    for(int i=0;i<3;++i){
        subLoops_.push_back(std::make_unique<EventLoop>());
    }
}

void EchoServer::start(){//启动服务器：运行所有subLoop
    for(auto& sub:subLoops_){
        std::thread([&]() {sub->loop();}).detach();
    }
}

void EchoServer::handleNewConnection(){//处理新客户端连接
    InetAddress clientAddr;
    int cfd=socket_->accept(clientAddr);//接受连接
    if(cfd<0) return;

    //轮询选择subLoop
    static int idx=0;
    auto* sub=subLoops_[idx++ % subLoops_.size()].get();

    //为客户端创建Channel和Buffer
    auto ch=std::make_unique<Channel>(sub,cfd);
    auto buf=std::make_unique<Buffer>();

    //消息到达时调用handleMessage
    ch->setReadCallback([this,cfd,buf=buf.get()](){
        handleMessage(cfd,buf);
    });

    ch->enableReading();//注册到epoll

    clientChannels_.push_back(std::move(ch));
    clientBuffers_.push_back(std::move(buf));
}

void EchoServer::handleMessage(int cfd, Buffer* buf){//Echo核心：读取数据并回显
    //丢到线程池，不阻塞IO线程
    threadPool_->addTask([=](){
        buf->readFd(cfd);//从客户端读数据

        if(buf->readableBytes()>0){
            buf->writeFd(cfd);//回写给客户端
        }
        else{
            close(cfd);//断开连接
        }
    });
}