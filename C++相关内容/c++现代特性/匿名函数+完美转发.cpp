//lambda匿名函数:没有名字，就地定义，用完就丢的小函数
/*
    格式:[捕获列表] (参数)->返回值{
            ...函数体
        }

    =====简单lambda：
        auto func = [](){
            cout<<"我是匿名函数"<<endl;
        };
        func();//调用
    =====含参数：
        auto add=[](int a, int b){
            return a + b;
        };
        cout<<add(2,3);//输出5
    =====捕获外部变量：
        int x=10;
        //[=]值捕获：拷贝一份x
        auto f1=[=](){cout<<x;};
        //[&]引用捕获：用原来的 x
        auto f2=[&](){x=20;};

        ！！！//[=]/[x]只捕获x，加mutable内变外不变；不加内外都不变
        ！！！//[&]/[&x]只引用捕获x，可以修改外部变量x,里外一起改
    =====配合sort：
        vector<int> v={3,1,4,2};

        sort(v.begin(),v.end(),[](int a, int b){
            return a<b;//从小到大
        });
*/

//forward完美转发：原封不动地把参数的“左值/右值”传给下一个函数
/*
    作用：写模板函数时，参数传进来可能是：左值（变量）右值（临时值）
        forward能保证：传进来是左值→传出去还是左值；传进来是右值→传出去还是右值
        forward会自动保持它的属性：传左值→调用拷贝构造；传右值→调用移动构造
    核心作用：std::move强制转右值，用完就不用了
            forward保持原来的类型不变，把参数转手给别人，不要自己处理了
    写法：template<typename... Args>//1.可变参模板
         返回类型 函数名(Args&&... args){//2.万能引用参数包
            return 实际调用的函数(std::forward<Args>(args)...);//3.完美转发展开
         }
    用法:String s1 = make_string("hello"); //普通构造
        String s2 = make_string(s1);      //拷贝构造
        String s3 = make_string(move(s1));//移动构造
*/  

//改写移动语意中的String类
#include <iostream>
#include <cstring>
#include <vector>
#include <utility>//move+forward
using namespace std;

class String{
private:
    char* _data=nullptr;//指向字符串真实内存
    int   _len=0;//字符串长度

public:
    //1.普通构造:接收一个字符串->开辟内存->把内容复制进去
    String(const char* str=""){
        _len=strlen(str);//计算长度
        _data=new char[_len+1];//申请内存
        strcpy(_data,str);
        cout<<"构造:"<<_data<<endl;
    }

    //2.析构函数:对象销毁时自动释放内存->防止内存泄漏
    ~String() {
        if(_data){
            cout<<"析构:"<<_data<<endl;
            delete[] _data;
            _data=nullptr;
        }
    }

    //3.拷贝构造（深拷贝）:用一个旧对象，创建一个新对象->重新开辟内存 → 复制数据->两个对象各有各的内存，互不影响
    String(const String& other){
        _len=other._len;
        _data=new char[_len + 1];
        strcpy(_data,other._data);
        cout<<"拷贝构造:拷贝了数据"<<endl;
    }

    //4.拷贝赋值（深拷贝）:对象已经存在->把另一个对象的值赋值给它->必须先释放自己，再拷贝
    String& operator=(const String& other){
        if(this==&other) return *this;

        // 释放自己
        delete[] _data;

        // 重新分配并拷贝
        _len=other._len;
        _data=new char[_len + 1];
        strcpy(_data,other._data);
        cout<<"拷贝赋值:拷贝了数据"<<endl;
        return *this;
    }

    // ===================== 移动语义 =====================
    //5.移动构造（偷资源）:创建新对象时,不拷贝内存->直接偷别人的内存->把对方指针置空，防止重复释放
    String(String&& other) noexcept {
        //直接拿指针
        _data=other._data;
        _len=other._len;

        //把对方置空，防止重复释放
        other._data=nullptr;
        other._len=0;

        cout<<"移动构造:偷资源"<<endl;
    }

    //6.移动赋值（偷资源）:已有对象，重新赋值->先释放自己->再偷对方资源
    String& operator=(String&& other) noexcept{
        if(this==&other) return *this;

        //先释放自己
        delete[] _data;

        //偷对方
        _data=other._data;
        _len=other._len;

        //对方置空
        other._data=nullptr;
        other._len=0;

        cout<<"移动赋值: 偷资源"<<endl;
        return *this;
    }

    //辅助打印:有数据 → 返回字符串;没数据 → 返回 (null)
    const char* c_str() const{
        return _data?_data:"(null)";
    }

    int length() const{return _len;}//给新增lambda/范围for使用
};

//完美转发
template<typename... Args>
String make_string(Args&&... args) {
    return String(std::forward<Args>(args)...);
}

int main() {
    //auto:
    auto s1=String("hello");

    //move+移动构造:
    String s2=std::move(s1);
    cout<<"s2="<<s2.c_str()<<endl;
    cout<<"s1="<<s1.c_str()<<endl<<endl;

    String s3("world");
    //移动赋值:
    s3=String("temp");
    cout<<"s3="<<s3.c_str()<<endl;

    //forward
    auto s4=make_string("forward-test");
    cout<<"s4="<<s4.c_str()<<endl<<endl;

    //decltype
    decltype(s4) s5=std::move(s4);
    cout<<"s5="<<s5.c_str()<<endl;
    cout<<"s4="<<s4.c_str()<<endl<<endl;

    //范围for+lambda
    String strs[]={"A","B","C"};
    int total=0;

    //lambda捕获
    auto func=[&total](const String& s){
        total+=s.length();
    };
    //范围for
    for(const auto& s:strs){
        func(s);
    }
    cout<<"总长度："<<total<<endl;
}