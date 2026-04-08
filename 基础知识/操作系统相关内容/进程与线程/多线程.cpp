/*
C++11多线程：std::thread（现代C++首选）
        核心优势：跨平台、面向对象、RAII 自动管理，避免原生pthread的手动管理风险。
        核心组件：std::thread：线程类，创建线程
                std::mutex/std::lock_guard：互斥锁，保证线程安全
                std::this_thread::get_id()：获取当前线程ID
                std::this_thread::sleep_for()：线程休眠
*/
//多线程访问共享资源时，需用互斥锁(std::mutex)保护数据一致性

//用C++创建多线程+打印线程ID
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

//互斥锁：防止cout打印乱序
std::mutex g_mtx;

//线程执行函数
void print_thread_id(int index)
{
    //上锁，保证打印完整不乱
    std::lock_guard<std::mutex> lock(g_mtx);

    //打印：自定义编号+系统真实线程ID
    std::cout<<"线程序号:"<<index
            <<"|系统线程ID:"<<std::this_thread::get_id()
            <<std::endl;
}

int main(){
    std::cout<<"===开始创建多线程==="<<std::endl;

    //存放线程对象
    std::vector<std::thread> threads;
    //创建5个线程
    for(int i=1;i<=5;++i){
        threads.emplace_back(print_thread_id,i);
    }

    //等待所有线程执行完毕
    for(auto& t:threads){
        t.join();
    }

    std::cout<<"===所有线程执行完成==="<<std::endl;
    return 0;
}