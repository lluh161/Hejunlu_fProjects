//虚拟内存
/*
    概念:虚拟内存是操作系统给每个进程提供的抽象、独立的地址空间，让每个进程都以为自己独占了整个内存，实际由物理内存+磁盘交换区共同支撑
    作用:内存隔离：每个进程的虚拟地址独立，互不干扰，避免越界访问
        地址空间扩展：让程序可以使用比物理内存更大的地址空间，解决物理内存不足
        简化内存管理：对程序员透明，无需关心物理内存布局
        I/O优化：结合mmap实现零拷贝IO，提升大文件读写性能

*/

//分页与分段
/*
    对比维度	        分页（Paging）	                     分段（Segmentation）
    划分方式	    固定大小的页（通常 4KB）	          按逻辑段（代码段、数据段、堆、栈）划分
    碎片问题	无外部碎片，仅内部碎片（页内未用空间）	         有外部碎片，需内存整理
    对程序员	        完全透明，无需感知	                  程序员可见，按逻辑划分
    共享与保护	        页级共享，粒度细	                段级共享，粒度粗，符合程序逻辑
    地址转换	    页表（多级页表、TLB 加速）	                      段表
*/

//内存映射mmap
/*
    mmap是Linux/macOS的系统调用，将文件/设备直接映射到进程的虚拟地址空间，
    实现:零拷贝IO:直接操作内存=操作文件，减少用户态->内核态的拷贝次数，大幅提升大文件读写性能
        进程间共享内存:多个进程映射同一块文件/内存，实现高效IPC
        匿名映射:不关联文件，用于分配堆内存、进程间共享内存
    虚拟地址如何转化为物理地址:虚拟地址->页号+页内偏移->查页表得到物理页号->物理页号+页内偏移=物理地址现代CPU用TLB(快表)缓存页表项，加速地址转换
*/

#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
using namespace std;

int main(){
    cout<<"=====虚拟内存与mmap演示====="<<endl;

    const char* filename="test_mmap.txt";
    const char* content="Hello, 虚拟内存! 这是mmap内存映射测试";
    size_t len=strlen(content);

    //1.创建并打开文件
    int fd=open(filename,O_RDWR|O_CREAT,0666);
    if (fd==-1){
        perror("open failed");
        return 1;
    }

    //2.扩展文件大小（必须，否则mmap映射无效）
    if(ftruncate(fd,len)==-1){
        perror("ftruncate failed");
        close(fd);
        return 1;
    }

    //3.mmap映射：将文件映射到进程虚拟地址空间
    void* map_addr = mmap(
        NULL,//内核自动选择映射地址
        len,//映射长度
        PROT_READ|PROT_WRITE,//读写权限
        MAP_SHARED,//共享映射，修改会同步到文件
        fd,//关联的文件描述符
        0//从文件起始位置映射
    );

    if(map_addr == MAP_FAILED){
        perror("mmap failed");
        close(fd);
        return 1;
    }

    //4.直接操作内存 = 操作文件（零拷贝）
    memcpy(map_addr, content, len);
    cout<<"映射内存内容:"<<(char*)map_addr<<endl;
    cout<<"映射的虚拟地址:"<<map_addr<<endl;

    //5.解除映射、关闭文件
    munmap(map_addr,len);
    close(fd);

    //6.读取文件验证
    fd=open(filename,O_RDONLY);
    char buf[1024]={0};
    read(fd,buf,len);
    cout<<"\n文件实际内容:"<<buf<<endl;
    close(fd);

    // 删除测试文件
    unlink(filename);

    return 0;
}