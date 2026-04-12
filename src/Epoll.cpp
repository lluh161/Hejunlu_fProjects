#include "Epoll.h"
#include "Channel.h"
#include <unistd.h>
#include <cstring>

//====================== Linux 原生实现 ======================
#if defined(__linux__)
Epoll::Epoll() {
    // 创建 epoll 实例，size 参数已废弃，传任意正数即可
    epfd_ = epoll_create1(0);
    if (epfd_ == -1) {
        perror("epoll_create1 failed");
        exit(EXIT_FAILURE);
    }
    // 预分配事件数组，避免频繁扩容
    events_.resize(1024);
}

Epoll::~Epoll() {
    close(epfd_);
}

void Epoll::updateChannel(Channel* ch) {
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.ptr = ch; // 绑定 Channel 指针
    ev.events = ch->getEvents(); // 获取 Channel 要监听的事件

    int fd = ch->getFd();
    if (ch->isInEpoll()) {
        // 已在 epoll 中，修改事件
        if (epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1) {
            perror("epoll_ctl mod failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // 新添加事件
        if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1) {
            perror("epoll_ctl add failed");
            exit(EXIT_FAILURE);
        }
        ch->setInEpoll(true);
    }
}

std::vector<Channel*> Epoll::wait(int timeout) {
    std::vector<Channel*> activeChannels;
    int n = epoll_wait(epfd_, events_.data(), events_.size(), timeout);
    for (int i = 0; i < n; ++i) {
        Channel* ch = static_cast<Channel*>(events_[i].data.ptr);
        ch->setRevents(events_[i].events); // 设置就绪事件
        activeChannels.push_back(ch);
    }
    // 动态扩容
    if (n == static_cast<int>(events_.size())) {
        events_.resize(events_.size() * 2);
    }
    return activeChannels;
}

// ====================== macOS kqueue 模拟实现 ======================
#elif defined(__APPLE__)
//实现epoll兼容接口
int epoll_create(int size){//创建epoll实例
    (void)size;
    return kqueue();
}

int epoll_ctl(int kq,int op,int fd,EpollEvent* event){//对epoll实例中的fd做操作
    struct kevent kev;
    EV_SET(&kev,fd,EVFILT_READ, 
            (op==EPOLL_CTL_DEL)?EV_DELETE:EV_ADD, 
            0,0,event->data);

    return kevent(kq,&kev,1,nullptr,0,nullptr);
}

int epoll_wait(int kq,EpollEvent* events,int maxevents,int timeout){//阻塞等待epoll实例中的就绪事件
    struct kevent kev[1024];
    struct timespec ts;
    if (timeout >= 0){
        ts.tv_sec=timeout / 1000;
        ts.tv_nsec=(timeout % 1000)*1000000;
    }

    int n=kevent(kq,nullptr,0,kev,maxevents,timeout>=0?&ts:nullptr);
    for(int i=0;i<n;++i){
        events[i].data=kev[i].udata;
        events[i].events=EPOLLIN;
    }
    return n;
}

//Epoll类实现
Epoll::Epoll(){
    epfd_=epoll_create(1024);
    if(epfd_==-1) {
        perror("epoll_create failed");
        exit(EXIT_FAILURE);
    }
    events_.resize(1024);
}

Epoll::~Epoll(){
    close(epfd_);
}

//注册监听事件
void Epoll::updateChannel(Channel* ch){
    //初始化结构体
    epoll_event ev;
    memset(&ev,0,sizeof(ev));
    //把Channel指针存入事件的data字段
    ev.data=ch;
    //从Channel中获取要监听的事件类型
    ev.events=ch->getEvents();

    //从Channel获取对应的socket fd
    int fd=ch->getFd();

    //判断Channel是否已在epoll中：是则修改，否则新增
    if(ch->isInEpoll()){
        if(epoll_ctl(epfd_,EPOLL_CTL_MOD,fd,&ev)==-1){
            perror("epoll_ctl mod failed");
            exit(EXIT_FAILURE);
        }
    }else{
        if(epoll_ctl(epfd_,EPOLL_CTL_ADD,fd,&ev)==-1){
            perror("epoll_ctl add failed");
            exit(EXIT_FAILURE);
        }
        //标记，避免重复添加
        ch->setInEpoll(true);
    }
}

//等待时间发生并返回结果
std::vector<Channel*> Epoll::wait(int timeout){
    //存储所以就绪的Channel指针
    std::vector<Channel*> activeChannels;

    //系统调用，阻塞等待内核返回就绪事件
    //epfd_: epoll 实例的文件描述符；events_.data(): 存储就绪事件的数组首地址
    //events_.size(): 数组最大容量；timeout: 超时时间（毫秒，-1 表示无限等待）
    int n=epoll_wait(epfd_,events_.data(),events_.size(),timeout);
    
    //遍历所以就绪事件，转化成Channe
    for(int i=0;i<n;++i){
        //从事件的data字段取出之前存入的Channel*，并强转类型
        Channel* ch=static_cast<Channel*>(events_[i].data);
        //把就绪的事件类型（读/写/异常）设置回Channel，供后续回调使用
        ch->setRevents(events_[i].events);
        //把Channel加入就绪列表，返回给上层EventLoop
        activeChannels.push_back(ch);
    }
    //动态扩容
    if (n==static_cast<int>(events_.size())){
        //翻倍数组容量
        events_.resize(events_.size()*2);
    }
    //返回所有的就绪Channel列表
    return activeChannels;
}

#endif