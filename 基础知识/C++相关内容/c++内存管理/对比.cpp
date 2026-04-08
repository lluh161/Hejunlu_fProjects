//对比new/delete&malloc/free
#include <iostream>
#include <cstdlib>
using namespace std;
//概念差异：
    //都是用于在堆上的申请释放内存的工具new不用算大小、不用强转、还能自动初始化，比malloc省事安全
    /*流程：
    new申请内存->调用对象的构造函数，完成初始化；属于c++，
    malloc申请内存->无初始化，是垃圾值；属于c，头文件<cstdlib>
    */
    /*处理：
    new/delete返回具体类型指针；有new[]/delete[]专门处理数组
    malloc/free返回void*，需要手动强转化；手动计算数值大小
    */
   /*释放：都会造成内存泄漏：这块堆内存会一直被占用，程序结束前系统无法回收；如果反复泄漏，程序占用内存会越来越大，最后可能崩溃。
   new分配的对象如果有析构函数，忘记delete会导致析构函数不执行，可能漏释放内部资源（如文件、网络连接）。
   malloc只分配内存free只释放内存，没有额外的资源清理逻辑。
    */
//写法对比：
    /*语法：
    int *p_new=new int(0);//申请int，初始值为0
    int *p_malloc=(int*)malloc(sizeof(int));//申请sizeof(int)大小内存，返回void*，强转化为int*
    */

int main() {
    //new
    int* arr=new int[5];//直接写个数
    arr[0]=10;
    delete[] arr;//固定写法
    return 0;

    //malloc
    /*
    int* arr=(int*)malloc(5*sizeof(int));//自己算总字节，强类型转化
    if (arr==NULL) return 1;
    arr[0]=10;//自己初始化
    free(arr);
    return 0;
    */
}