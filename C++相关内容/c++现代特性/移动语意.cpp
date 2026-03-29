//1、左值、右值、右值引用&&
/*
    左值：有名字，能取地址，持久存在（变量）
        int a=5;（a）
    右值：临时值，用完即销毁（字面量、表达式结果）
        int b=a+5;(a+5)
    右值引用：专门绑定临时对象，延长临时对象的生命周期，提高效率
        int&& r=123;
        普通引用&只能接左值（变量）
        以前：完整拷贝一份，慢，费内存
            string a="hello";
            string b=a;
        右值引用：不拷贝，直接把a的内存“偷”过来，
            string a="hello";
            string b=std::move(a); 
*/

//2、移动构造函数
/*：
    用右值（临时对象）构造新对象，不拷贝数据，把对方的内存 “拿过来”，再把对方指针置空
//移动构造：对象正在出生，直接拿资源
//加noexcept，告诉编译器这里不会抛异常，这样STL容器（vector等）才敢放心使用移动，而不是拷贝
MyString(MyString&& other) noexcept{
    //偷资源
    m_data=other.m_data;

    //让对方不再持有这块内存（避免重复释放）
    other.m_data=nullptr;

    cout<<"调用了移动构造"<<endl;
}
*/

//3、移动赋值运算符
/*
//移动赋值：对象已存在，先扔旧的，再拿新的
MyString& operator=(MyString&& other) noexcept{
    if(this==&other) return *this;

    //先释放自己原来的内存
    delete[] m_data;

    //偷对方的资源
    m_data=other.m_data;
    other.m_data=nullptr;

    cout<<"调用了移动赋值"<<endl;
    return *this;
}
*/

//4、std::move的作用
/*
    不移动数据，只转类型
    作用：强迫编译器使用移动语义，而不是拷贝
    后果：被move的对象变成无效状态，不要再用,被移动过的对象要置空，避免析构时重复释放
    
    MyString s1("hello");
    MyString s2=std::move(s1);
    这里s1已经被掏空，指针是nullptr，再用s1会直接崩溃！
*/

//5、编写支持移动的String类
#include <iostream>
#include <cstring>
#include <utility>//为了std：：move
using namespace std;

class String{
private:
    char* _data;
    int   _len;

public:
    //1.普通构造
    String(const char* str=""){
        _len=strlen(str);
        _data=new char[_len+1];
        strcpy(_data,str);
        cout<<"构造:"<<_data<<endl;
    }

    //2.析构函数
    ~String() {
        if(_data){
            cout<<"析构:"<<_data<<endl;
            delete[] _data;
            _data=nullptr;
        }
    }

    //3.拷贝构造（深拷贝）
    String(const String& other){
        _len=other._len;
        _data=new char[_len + 1];
        strcpy(_data,other._data);
        cout<<"拷贝构造:拷贝了数据"<<endl;
    }

    //4.拷贝赋值（深拷贝）
    String& operator=(const String& other) {
        if (this == &other) return *this;

        // 释放自己
        delete[] _data;

        // 重新分配并拷贝
        _len = other._len;
        _data = new char[_len + 1];
        strcpy(_data, other._data);
        cout << "拷贝赋值: 拷贝了数据" << endl;
        return *this;
    }

    // ===================== 移动语义 =====================
    // 5. 移动构造（偷资源）
    String(String&& other) noexcept {
        // 直接拿指针
        _data = other._data;
        _len  = other._len;

        // 把对方置空，防止重复释放
        other._data = nullptr;
        other._len  = 0;

        cout << "移动构造: 偷资源" << endl;
    }

    // 6. 移动赋值（偷资源）
    String& operator=(String&& other) noexcept {
        if (this == &other) return *this;

        // 先释放自己
        delete[] _data;

        // 偷对方
        _data = other._data;
        _len  = other._len;

        // 对方置空
        other._data = nullptr;
        other._len  = 0;

        cout << "移动赋值: 偷资源" << endl;
        return *this;
    }

    //辅助打印
    const char* c_str() const{
        return _data?_data:"(null)";
    }
};

int main() {
    String s1("hello");

    //移动构造：创建新对象
    String s2=move(s1);
    cout<<"s2="<<s2.c_str()<<endl;
    cout<<"s1="<<s1.c_str()<<endl<<endl;

    String s3("world");
    //移动赋值：给已有对象赋值
    s3=String("temp");
    cout<<"s3="<<s3.c_str()<<endl;
}