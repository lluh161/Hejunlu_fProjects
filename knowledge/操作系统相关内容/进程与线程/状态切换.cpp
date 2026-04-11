//上下文切换
/*
    定义：CPU暂停当前执行任务，保存其状态(寄存器、程序计数器、栈指针等)，加载另一个任务的状态并继续执行的过程
    作用：实现多任务并发，让单核CPU看起来同时运行多个程序
    开销来源：保存/恢复状态、刷新CPU缓存、调度器调度，因此线程切换比进程切换快得多。
*/

//用户态和内核态
/*
    用户态：进程运行在用户空间，只能访问受限资源，不能直接操作硬件，系统调用会触发切换。
    内核态：进程运行在内核空间，拥有最高权限，可直接操作硬件、访问所有内存。
    切换场景：系统调用（如fork()、pthread_create()）、中断、异常时，会从用户态切换到内核态，执行完成后再切回用户态。
    开销影响：频繁的态切换会大幅降低性能，因此高性能IO（如epoll）会减少态切换次数。

*/

//进程状态转换
/*
    创建态 → 就绪态 → 运行态 → 终止态
               ↓↑    ↓↑
                阻塞态
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
*/

//pthread
#include <pthread.h>
#include <stdio.h>

void* thread_func(void* arg) {
    int id = *(int*)arg;
    printf("线程ID: %d，系统线程ID: %lu\n", id, pthread_self());
    return NULL;
}
int main() {
    pthread_t tid;
    int arg = 1;
    // 创建线程
    pthread_create(&tid, NULL, thread_func, &arg);
    // 等待线程结束
    pthread_join(tid, NULL);
    return 0;
}