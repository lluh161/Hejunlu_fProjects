#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <mutex>

// 时间戳
using TimeStamp = std::chrono::steady_clock::time_point;
// 定时器回调
using TimerCallback = std::function<void()>;

// 单个定时器
class Timer {
public:
    Timer(TimeStamp expire, TimerCallback cb);
    TimeStamp expireTime() const;
    void run();

private:
    TimeStamp expire_;
    TimerCallback cb_;
};

// 定时器管理器（小根堆）
class TimerManager {
public:
    TimerManager():id_(1) {};

    // 添加定时器
    int addTimer(TimeStamp expire, TimerCallback cb);

    // 处理超时
    void handleExpiredTimers();

    // 获取下一次超时时间（毫秒）
    int getNextExpireMs();
    
    void delTimer(int id);

private:
    void siftUp(int idx);
    void siftDown(int idx);
    std::vector<std::shared_ptr<Timer>> heap_;
    std::mutex mutex_;
    int id_;
};

#endif