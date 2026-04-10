#include "Epoll.h"
#include "Channel.h"

Epoll::Epoll(){
    epfd_=epoll_create1(0);//创建epoll管理器
    events_.resize(1024);//准备1024个就绪事件
}

Epoll::~Epoll(){
    close(epfd_);
}

void Epoll::updateChannel(Channel* ch){
    epoll_even ev{};//创建epoll对象
    ev.data.prt=ch;//把自定义的“通道对象”绑到事件上
    ev.events=ch->events();//设置要监听的事件
    epoll_ctl(epfd_,EPOLl_CTL_ADD,ch->fd(),&ev);//把fd加入epoll监听
}

std::vector<Channel*> Epoll::wait(int timeout){
    int n=epoll_wait(epfd_,events_.data(),events_.size(),timeout);//等待IO事件发生，返回就绪事件个数n
    std::vector<Channel*> res;//新建vector存放Channel
    for(int i=0;i<n;i++){//遍历就绪事件
        Channel* ch=(Channel*)events_[i].data.ptr;//从事件中取出之前绑定的Channel指针
        ch->setRevents(events_[i].events);//把实际发生的事件设置到Channel中
        res.push_back(ch);//将这个就绪Channel加入结果列表
    }
    return res;
}
