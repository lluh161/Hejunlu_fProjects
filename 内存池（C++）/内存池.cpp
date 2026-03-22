//内存池简易实现：底层内存管理能力
/*程序要求：预先向系统申请一大块内存 自己管理这块内存，不再频繁调用 malloc/new
          提供两个接口：allocate(size) → 从池子里 “拿一块内存”，free(ptr) → 把内存 “还回池子”
          内部自动做：分割大块（满足小需求） 合并空闲块（减少内存碎片）*/

//需求
/*.1、存什么：一堆内存块
   2、做什么操作：申请内存、释放内存
   3、限制：减少频繁malloc/free，提升效率
   4、规则：先复用空闲块，不够再申请*/

//难点
/*1、如何管理一堆内存块
  2、如何快速找到空闲块
  3、如何避免内存泄漏、野指针*/

//工具
/*1、链表：串起所有内存块
  2、结构体/类：封装内存块信息
  3、指针操作：底层内存管理*/

//功能
/*1、初始化内存池
  2、分配内存块
  3、释放内存块
  4、销毁内存池
  5、遍历空闲块*/

//边界
/*1、无空闲块：扩容
  2、重复释放 → 报错/标记
  3、释放后必须清空指针
  4、销毁时必须释放所有内存*/

#include <iostream>
#include <cstdlib>//内存操作
using namespace std;

//内存块结构体，每一块内存的状态
struct MemoryBlock{
    //内存块的起始地址，指针
    void* strart;
    //内存块的大小
    size_t size;
    //是否空闲，布尔值
    bool isFree;
    //指向下一内存块，指针，链表管理
    MemoryBlock* next;
    //快速初始化成员,构造函数
    MemoryBlock(void* s,size_t sz,bool free){
        strart=s;
        size=sz;
        isFree=free;
        next=nullptr;
    }
};

//写内存池类
class MemoryPool{
private:
    //内存池起始地址，指针
    void* m_poolStart;
    //内存池总大小
    size_t m_totalSize;
    //管理内存块的链表头指针
    MemoryBlock* m_freeList;
public:
    //构造函数:创建内存池
    MemoryPool(size_t totalSize);
    //析构函数；销毁内存池
    ~MemoryPool();

    //从内存池中分配一块大小指定的内存
    void* allocate(size_t size);
    //释放内存归还给内存池
    void free(void* ptr);
};

//内存池内构造函数的实现
MemoryPool::MemoryPool(size_t totalSize){
    //给定的内存池总大小
    m_totalSize=totalSize;
    //申请一块连续的内存，并保存起始地址指针
    m_poolStart=malloc(totalSize);
    //创建第一个空闲内存块节点，放在头部
    //含有内存起始地址，块大小，空闲状态
    m_freeList=new MemoryBlock(m_poolStart,totalSize,true);
}

//实现MemoryPool类中的allocate分配内存函数
void* MemoryPool::allocate(size_t size){
    //内存池的头节点作为遍历起点
    MemoryBlock* cur=m_freeList;

    //遍历空闲链表
    while(cur!=nullptr){
        //当前空闲，并且大小>=需要分配的大小
        if(cur->isFree&&cur->size>=size){
            //标记已占用
            cur->isFree=false;
            //返回该内存块的起始地址，分配成功
            return cur->strart;
        }
        cur=cur->next;
    }
    //没有可用内存块，返回空指针，分配失败
    return nullptr;
}

//释放内存
void MemoryPool::free(void* ptr){
    //指向空闲链表头节点
    MemoryBlock* cur=m_freeList;
    //遍历空闲链表
    while (cur!=nullptr){
        //找到要释放的内存块
        if(cur->strart==ptr){
            //标记为空闲
            cur->isFree=true;
            return;
        }
        //未找到，继续下一个节点
        cur=cur->next;
    }
     
}
//析构函数：释放整个内存池
MemoryPool::~MemoryPool(){
    free(m_poolStart);
}

//主程序入口
int main(){
    MemoryPool pool(1024);

    void* p1=pool.allocate(128);
    void* p2=pool.allocate(256);

    pool.free(p1);
    pool.free(p2);
    
    return 0;
}
