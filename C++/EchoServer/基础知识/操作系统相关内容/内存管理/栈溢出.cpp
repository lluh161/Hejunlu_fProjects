//栈溢出
/*
    概念：程序在栈上分配的内存超过了栈的最大容量，导致栈空间耗尽，触发段错误崩溃。
    溢出原因:无限递归：函数递归调用没有终止条件，每次调用都在栈上分配栈帧，栈空间持续耗尽
            栈上超大数组：在函数内定义超大局部数组（如char buf[1024*1024*10]），直接占满栈空间
            函数调用层级过深：正常递归但层级过深，栈空间不足
    危害:程序直接崩溃，无法正常运行;安全漏洞：黑客可利用栈溢出覆盖函数返回地址，执行恶意代码（缓冲区溢出攻击）
    如何避免栈溢出:避免无限递归，设置递归终止条件
                避免在栈上分配超大数组，大内存用堆分配（new/malloc）
                优化函数调用层级，避免过深递归

*/

#include <iostream>
#include <unistd.h>
using namespace std;

//无限递归：每次调用都在栈上分配1KB数组，栈空间持续耗尽
void stack_overflow(int depth){
    char buf[1024];//栈上分配1KB内存
    printf("递归深度: %d，当前栈指针: %p\n",depth,buf);
    stack_overflow(depth+1);//无终止条件，无限递归
}

int main() {
    cout<<"=====栈溢出演示====="<<endl;
    cout<<"开始无限递归，栈空间将耗尽..."<<endl;

    stack_overflow(0);//调用后程序会直接崩溃，报Segmentation fault

    return 0;
}