#include "Timer.h"
#include <algorithm>

// 构造：初始化超时时间和回调
Timer::Timer(TimeStamp expire, TimerCallback cb)
    : expire_(expire), cb_(cb)
{
}

// 获取超时时间
TimeStamp Timer::expireTime() const {
    return expire_;
}

// 执行回调
void Timer::run() {
    if (cb_) {  // 如果回调有效，就执行
        cb_();
    }
}

// ==============================
// TimerManager 定时器管理器实现
// ==============================

// 添加定时器到小根堆
int TimerManager::addTimer(TimeStamp expire, TimerCallback cb) {
    std::lock_guard<std::mutex> lock(mutex_);  // 加锁，保证线程安全

    // 创建定时器对象
    auto timer = std::make_shared<Timer>(expire, cb);

    // 加入堆尾部
    heap_.push_back(timer);
    
    // 向上调整，维持小根堆结构
    siftUp(heap_.size() - 1);

    return id_++;  // 返回唯一ID，自增
}

// 处理所有超时的定时器
void TimerManager::handleExpiredTimers() {
    std::lock_guard<std::mutex> lock(mutex_);

    // 获取当前时间
    auto now = std::chrono::steady_clock::now();

    // 循环检查堆顶（最早超时的定时器）
    while (!heap_.empty()) {
        auto top = heap_[0];  // 堆顶 = 最早要超时的

        // 如果堆顶还没超时，直接退出
        if (top->expireTime() > now) {
            break;
        }

        // 已超时 → 执行回调
        top->run();

        // 把堆顶移到最后并删除
        std::pop_heap(heap_.begin(), heap_.end(), [](auto a, auto b) {
            return a->expireTime() > b->expireTime();
        });
        heap_.pop_back();
    }
}

// 获取下一个定时器的超时毫秒数
int TimerManager::getNextExpireMs() {
    std::lock_guard<std::mutex> lock(mutex_);

    // 没有定时器 → 返回-1，表示epoll_wait永久等待
    if (heap_.empty()) {
        return -1;
    }

    auto now = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
        heap_[0]->expireTime() - now);

    // 如果已经超时，返回0，立即处理
    return diff.count() > 0 ? diff.count() : 0;
}


// 小根堆向上调整
void TimerManager::siftUp(int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;  // 找到父节点

        // 当前节点 >= 父节点 → 不用调整
        if (heap_[idx]->expireTime() >= heap_[parent]->expireTime())
            break;

        // 交换父子节点
        std::swap(heap_[idx], heap_[parent]);
        idx = parent;
    }
}

void TimerManager::delTimer(int id) {}

// 小根堆向下调整
void TimerManager::siftDown(int idx) {
    int n = heap_.size();
    while (true) {
        int left = idx * 2 + 1;   // 左孩子
        int right = idx * 2 + 2;  // 右孩子
        int smallest = idx;      // 最小节点下标

        // 找三个节点中最小的
        if (left < n && heap_[left]->expireTime() < heap_[smallest]->expireTime())
            smallest = left;
        if (right < n && heap_[right]->expireTime() < heap_[smallest]->expireTime())
            smallest = right;

        // 不用调整了
        if (smallest == idx)
            break;

        // 交换
        std::swap(heap_[idx], heap_[smallest]);
        idx = smallest;
    }
}