#include "EchoServer.h"
#include "Channel.h"
#include "Buffer.h"
#include "Http.h"
#include "InetAddress.h"
#include <iostream>
#include <functional>
#include <unistd.h>
#include <thread>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>

EchoServer::EchoServer(EventLoop* loop, const InetAddress& listenAddr)//初始化服务器
    : loop_(loop),//主Reactor
      socket_(std::make_unique<Socket>()),
      acceptChannel_(std::make_unique<Channel>(loop,socket_->fd())),
      threadPool_(std::make_unique<ThreadPool>(4))
{
    (void)loop_;
    //设置端口复用
    socket_->setReuseAddr();
    socket_->setReusePort();

    socket_->bind(listenAddr);//绑定IP+端口
    socket_->listen();//开始监听

    //新连接事件绑定到handleNewConnection
    acceptChannel_->setReadCallback(std::bind(&EchoServer::handleNewConnection,this));
    acceptChannel_->enableReading();//注册到epoll
    loop_->updateChannel(acceptChannel_.get());

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
/*
void EchoServer::handleNewConnection(){//处理新客户端连接
    InetAddress clientAddr;
    int cfd=socket_->accept(clientAddr);//接受连接
    if(cfd<0) return;

    //轮询选择subLoop
    static int idx=0;
    auto* sub=subLoops_[idx++ % subLoops_.size()].get();

    //为客户端创建Channel和Buffer
     auto ch = std::make_unique<Channel>(sub, cfd);
    auto buf=std::make_unique<Buffer>();

    //消息到达时调用handleMessage
    ch->setReadCallback([this,cfd,buf=buf.get()](){
        handleMessage(cfd,buf);
    });

    ch->enableReading();//注册到epoll

    clientChannels_.push_back(std::move(ch));
    clientBuffers_.push_back(std::move(buf));
}
*/

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <string.h>

// 处理新客户端连接（唯一版本）
void EchoServer::handleNewConnection()
{
    printf("✅ 新客户端连接\n");

    // 1. 接受客户端连接
    int cfd = accept(socket_->fd(), nullptr, nullptr);
    if (cfd < 0) {
        perror("accept 失败");
        return;
    }

    // 2. 禁用Nagle算法，确保数据立即发送，解决macOS延迟问题
    int opt = 1;
    setsockopt(cfd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));

    // 3. 标准HTTP响应（严格匹配Content-Length）
    const char* body = "Hello HTTP Server!";
    size_t body_len = strlen(body); // 自动计算长度，彻底避免手动计数错误

    std::string resp =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Length: " + std::to_string(body_len) + "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        std::string(body);

    // 4. 阻塞式send，确保数据100%完整发送
    ssize_t sent = send(cfd, resp.c_str(), resp.size(), MSG_NOSIGNAL);
    if (sent != resp.size()) {
        perror("send 失败");
        close(cfd);
        return;
    }

    printf("✅ 响应发送完成：%zd 字节\n", sent);

    // 5. 优雅关闭，等待客户端确认，避免强制断连
    shutdown(cfd, SHUT_WR);
    
    // 阻塞等待客户端关闭，确保数据完整接收
    char buf[1024];
    while (read(cfd, buf, sizeof(buf)) > 0);

    // 6. 最后关闭socket
    close(cfd);
    printf("✅ 连接优雅关闭\n");
}

// 处理客户端发来的请求，并返回HTTP响应
void EchoServer::handleMessage(int cfd, Buffer* buf)
{
    // --------------------- 第一步：读取浏览器发来的请求数据 ---------------------
    // 必须读取！不读会导致浏览器一直等待，页面不显示
    buf->readFd(cfd);

    // --------------------- 第二步：发送标准 HTTP 响应给浏览器 ---------------------
    // 这是所有浏览器（Chrome/Safari/Edge）都能识别的标准格式
    const char* response =
        "HTTP/1.1 200 OK\r\n"                  // HTTP 状态行：请求成功
        "Content-Type: text/html; charset=utf-8\r\n" // 告诉浏览器：这是HTML页面
        "Content-Length: 20\r\n"               // 页面内容长度（必须准确）
        "Connection: close\r\n"                // 响应完成后关闭连接
        "\r\n"                                 // 空行：分隔请求头和内容（必须有）
        "Hello HTTP Server!";                  // 网页显示的文字

    // 发送HTTP响应给浏览器
    send(cfd, response, strlen(response), 0);

    // --------------------- 第三步：优雅关闭连接（关键！浏览器必须要这一步） ---------------------
    // 只关闭“写端”，告诉浏览器：数据发完了
    // 不直接 close，避免数据还没发完就断开连接
    shutdown(cfd, SHUT_WR);

    // 释放内存
    delete buf;

    // 打印日志：证明响应发送成功
    printf("✅ 响应发送成功！浏览器已收到页面～\n");
}

