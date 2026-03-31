//vector扩容机制
/*
    本质：vector是动态数组，容量不足时会自动重新分配更大内存→拷贝旧数据→释放旧内存
    常见扩容倍数：GCC 2倍、MSVC 1.5倍
    旧迭代器、指针、引用全失效，内存地址全变了，需要重新获取
*/

#include <iostream>
#include <vector>
using namespace std;

int main(){
    vector<int> v;
    cout<<"初始capacity:"<<v.capacity()<<endl;

    for(int i=0;i<20;++i){
        v.push_back(i);
        cout<<"size:"<<v.size() 
            <<"|capacity:"<<v.capacity()<<endl;
    }
    return 0;
}
//size和capacity的区别
/*
    概念:size:现在有多少
        capacity:最多能装多少且不扩容
    size==capacity，再push_back就会扩容，永远size<=capacity
    扩容=换更大内存->所有迭代器失效
    vector/string:有size，有capacity
    map:只有size，无capacity
    unordered_map:size+bucket_count
*/

