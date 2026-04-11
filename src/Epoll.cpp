#include "Epoll.h"
#include "Channel.h"
#include <unistd.h>
#include <cstring>

// ====================== Linux 原生实现 ======================
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
// 实现epoll兼容接口
int epoll_create(int size) {
    (void)size;
    return kqueue();
}

int epoll_ctl(int kq, int op, int fd, EpollEvent* event) {
    struct kevent kev;
    EV_SET(&kev, fd, EVFILT_READ, 
            (op == EPOLL_CTL_DEL) ? EV_DELETE : EV_ADD, 
            0, 0, event->data);

    return kevent(kq, &kev, 1, nullptr, 0, nullptr);
}

int epoll_wait(int kq, EpollEvent* events, int maxevents, int timeout) {
    struct kevent kev[1024];
    struct timespec ts;
    if (timeout >= 0) {
        ts.tv_sec = timeout / 1000;
        ts.tv_nsec = (timeout % 1000) * 1000000;
    }

    int n = kevent(kq, nullptr, 0, kev, maxevents, timeout >= 0 ? &ts : nullptr);
    for (int i = 0; i < n; ++i) {
        events[i].data = kev[i].udata;
        events[i].events = EPOLLIN;
    }
    return n;
}

// Epoll 类实现（和 Linux 完全一致，上层代码零修改）
Epoll::Epoll() {
    epfd_ = epoll_create(1024);
    if (epfd_ == -1) {
        perror("epoll_create failed");
        exit(EXIT_FAILURE);
    }
    events_.resize(1024);
}

Epoll::~Epoll() {
    close(epfd_);
}

void Epoll::updateChannel(Channel* ch) {
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data = ch;
    ev.events = ch->getEvents();

    int fd = ch->getFd();
    if (ch->isInEpoll()) {
        if (epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1) {
            perror("epoll_ctl mod failed");
            exit(EXIT_FAILURE);
        }
    } else {
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
        Channel* ch = static_cast<Channel*>(events_[i].data);
        ch->setRevents(events_[i].events);
        activeChannels.push_back(ch);
    }
    if (n == static_cast<int>(events_.size())) {
        events_.resize(events_.size() * 2);
    }
    return activeChannels;
}

#endif