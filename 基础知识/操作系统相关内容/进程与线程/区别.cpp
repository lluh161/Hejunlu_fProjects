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

//fork
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(){
    pid_t pid=fork();
    if (pid<0){
        perror("fork failed");
        return 1;
    } else if(pid==0){
        //子进程：fork返回0
        printf("我是子进程,PID: %d\n", getpid());
    } else{
        //父进程：fork返回子进程PID
        printf("我是父进程,PID: %d,子进程PID: %d\n",getpid(),pid);
        wait(NULL); // 等待子进程结束，避免僵尸进程
    }
    return 0;
}