//迭代器失效
/*
    失效场景:push_back触发扩容，全失效
                vector<int> v={1,2,3};
                auto it=v.begin();//指向1
                v.push_back(4);//可能触发扩容
                // *it=100;//未定义行为！迭代器已失效
            insert/erase部分失效
                insert(pos,val):pos及之后的迭代器失效
                    vector<int> v={1,2,3,4};
                    auto it=v.begin()+2;
                    //v.erase(it);失效
                    it=v.erase(it);用erase的返回值更新迭代器
                erase(pos):pos及之后的迭代器失效
    安全规则:1/不在push_back/insert/erase后继续使用旧迭代器;用v.begin()、v.end()或函数返回值重新获取
            2/对于inser/erase，遍历+删除时，在pos及之后的用返回值，更新迭代器it=v.erase(it);
            3/对于push_back，提前reserve容量,v.reserve(100)
                            用下标，不用迭代器
                            重新获取 vector<int> v={1,2,3};
                                    auto it=v.begin() + 1;
                                    v.push_back(4);//扩容→it废了
                                    it=v.begin() + 1;//旧地址作废，全部重新获取
*/

#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> v={1,2,3};
    auto it=v.begin();
    cout<<"扩容前:"<<*it<<endl;
    
    v.push_back(4);//触发扩容
    //cout<<"扩容后:"<<*it<<endl;//解开注释，观察崩溃/乱码
    return 0;
}