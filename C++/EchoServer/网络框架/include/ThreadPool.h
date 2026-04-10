#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool{//线程池，多线程Reactor核心
private:
    std::vector<std::thread> threads_;//存放所有工作线程
    std::queue<std::function<void()>> tasks_;//待执行任务队列
    std::mutex mtx_;//互斥锁
    std::condition_variable cv_;//条件变量：线程等待任务、唤醒线程执行任务
    bool stop_;//控制线程池释放关闭
public:
    explicit ThreadPool(int numThreads);//指定线程池的线程数量
    ~ThreadPool();//销毁线程池
    void addTask(std::function<void()> task);//公共接口：向线程池提交一个无参无返回值的任务
};
