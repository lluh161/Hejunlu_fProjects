#include "Channel.h"

//双参数构造
Channel::Channel(void* loop,int fd) :loop_(loop),fd_(fd),events_(0), revents_(0), inEpoll_(false) {(void)loop_;}

void Channel::setReadCallback(std::function<void()> cb){
    readCallback_=std::move(cb);////当Channel检测到可读事件时，就调用这个存好的回调函数
}

void Channel::handleRead(){//又回调函数，则执行对应的读业务
    if(readCallback_) readCallback_();
}

void Channel::enableReading(){//监听读事件+采用边缘触发式
    events_=EPOLLIN|EPOLLET;
}