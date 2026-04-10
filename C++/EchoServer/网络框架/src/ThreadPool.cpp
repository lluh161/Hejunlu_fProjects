#include "ThreadPool.h"

//构造函数：启动numThreads个工作线程
ThreadPool::ThreadPool(int numThreads):stop_(false){
    //创建指定数量的线程
    for(int i=0;i<numThreads;++i){
        threads_.emplace_back([this](){
            while(true){
                std::function<void()> task;

                //锁作用域，只在操作队列时加锁
                {
                    std::unique_lock<std::mutex> lock(mtx_);
                    //等待：有任务或线程池停止
                    cv_.wait(lock,[this](){
                        return stop_||!tasks_.empty();
                    });

                    //如果线程池停止且没有任务，退出线程
                    if(stop_&&tasks_.empty()){
                        return;
                    }

                    //取出队首任务
                    task=std::move(tasks_.front());
                    tasks_.pop();
                }

                //执行任务（无锁状态下执行，不阻塞其他线程）
                if(task){
                    task();
                }
            }
        });
    }
}

// 析构函数：停止线程池，回收所有线程
ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        stop_ = true;
    }

    // 唤醒所有等待的线程
    cv_.notify_all();

    // 等待所有线程执行完毕
    for (std::thread& t : threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
}

// 添加任务到线程池
void ThreadPool::addTask(std::function<void()> task) {
    std::lock_guard<std::mutex> lock(mtx_);
    
    // 如果线程池已经停止，拒绝添加任务
    if (stop_) {
        return;
    }

    tasks_.push(std::move(task));
    cv_.notify_one(); // 唤醒一个线程来执行
}