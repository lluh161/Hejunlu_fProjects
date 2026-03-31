//进程与线程
//区别：进程是“房子”，线程是“房子里的人”，房子是独立的，人共享房子里的资源。
/*
    特性                   进程                           线程        
    本质             资源分配的最小单位                CPU调度的最小单位
    地址空间        独立虚拟地址空间，不共享           共享所属进程的地址空间
    资源开销       大(需分配空间、文件句柄等)        极小(仅需栈、寄存器、线程ID)
    上下文切换      慢(需切换页表、刷新缓存)             快(仅切换寄存器、栈)
    通信方式    管道、消息队列、共享内存、socket     直接共享全局变量、堆内存(需同步)
    崩溃影响        进程崩溃不影响其他进程            线程崩溃会导致整个进程崩溃
    系统调用          fork()、exec()          pthread_create()、C++ std::thread
*/
//上下文切换
/*
    定义：CPU暂停当前执行任务，保存其状态(寄存器、程序计数器、栈指针等)，加载另一个任务的状态并继续执行的过程
    作用：实现多任务并发，让单核CPU看起来同时运行多个程序
    开销来源：保存/恢复状态、刷新CPU缓存、调度器调度，因此线程切换比进程切换快得多。
*/
//核心系统调用
/*
    fork()：创建进程
        作用：在当前进程中创建一个完全相同的子进程（复制父进程的地址空间、资源）。
        特点：调用一次，返回两次：父进程返回子进程PID，子进程返回0，出错返回-1。
             子进程从fork()之后的代码开始执行，和父进程并发运行。
             现代系统采用写时复制，仅在修改数据时才复制内存，大幅提升效率。
    pthread：POSIX线程库（C语言原生线程）
            作用：C语言标准的线程库，用于创建、管理线程，是Linux/macOS多线程的底层基础。
            核心函数：pthread_create()：创建线程
                    pthread_join()：等待线程结束
                    pthread_mutex_t：互斥锁，实现线程同步
            注意：C++11之后推荐用std::thread，更安全、更易用，底层仍封装pthread。
    C++11多线程：std::thread（现代C++首选）
        核心优势：跨平台、面向对象、RAII 自动管理，避免原生pthread的手动管理风险。
        核心组件：std::thread：线程类，创建线程
                std::mutex/std::lock_guard：互斥锁，保证线程安全
                std::this_thread::get_id()：获取当前线程ID
                std::this_thread::sleep_for()：线程休眠
*/

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