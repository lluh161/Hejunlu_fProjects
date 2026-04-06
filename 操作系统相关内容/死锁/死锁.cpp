//死锁
/*
    概念:指两个或多个线程/进程在执行过程中，因争夺资源而造成的一种互相等待的现象，若无外力干涉，它们都将无法推进下去，程序永久卡死。
    必要条件                           含义	                                            破坏方法（预防死锁）
            互斥条件	    资源只能被一个线程持有，无法共享	                      尽量使用共享资源（如读写锁），大部分场景无法破坏
            请求与保持条件	 线程已持有一个资源，同时请求另一个被其他线程持有的资源	       一次性申请所有需要的资源，不边用边申请
            不可剥夺条件	资源只能被持有者主动释放，不能被强行抢占	                申请不到资源时，主动释放已持有的资源
            循环等待条件	多个线程形成循环链，每个线程都在等待下一个线程持有的资源	   对资源全局排序，线程必须按顺序申请（最常用）
        只有4个条件同时满足，才会发生死锁；破坏任意一个，就能避免死锁。
    处理策略:预防死锁：破坏4个必要条件，从根源杜绝死锁（安全性高，但性能开销大）
            避免死锁：不破坏条件，动态预判系统是否处于安全状态（如银行家算法），避免进入不安全状态
            检测死锁：允许死锁发生，定期检测资源分配图，发现死锁后解除
            解除死锁：检测到死锁后，通过终止线程、剥夺资源等方式打破死锁
*/

//死锁复现代码
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

//定义两个互斥锁
mutex mtx1;
mutex mtx2;

//线程A：先拿mtx1，再拿mtx2
void threadA(){
    lock_guard<mutex> lock1(mtx1);
    cout<<"线程A拿到了mtx1，等待mtx2..."<<endl;
    //模拟业务耗时，放大死锁概率
    this_thread::sleep_for(chrono::milliseconds(100));
    lock_guard<mutex> lock2(mtx2);
    cout<<"线程A拿到了mtx2，执行完成"<<endl;
}

// 线程B：先拿mtx2，再拿mtx1
void threadB() {
    lock_guard<mutex> lock1(mtx2);
    cout<<"线程B拿到了mtx2，等待mtx1..."<<endl;
    this_thread::sleep_for(chrono::milliseconds(100));
    lock_guard<mutex> lock2(mtx1);
    cout << "线程B拿到了mtx1，执行完成" << endl;
}

int main(){
    cout<<"=====死锁演示====="<<endl;
    thread t1(threadA);
    thread t2(threadB);
    
    t1.join();
    t2.join();
    
    cout<<"程序执行完成"<<endl;//无法显示
    return 0;
}