#include "EventLoop.h"
#include "InetAddress.h"
#include "EchoServer.h"

int main()
{
    EventLoop mainLoop;//主事件循环
    InetAddress addr(8888);//监听8888
    EchoServer server(&mainLoop, addr);//创建Echo服务器

    server.start();//启动服务器
    mainLoop.loop();//启动主循环

    return 0;
}