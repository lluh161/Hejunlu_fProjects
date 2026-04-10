//1、什么是多态：同一个调用，表现出不同的状态；=虚指针+虚函数表+运行时查表

//2、虚函数是什么？语法？
/*
    是什么？
    被virtual修饰的成员函数好，用于开启多态
    相比与普通函数编译时确定调用谁，虚函数在运行时才确定调用谁
    基类指针指向谁就调用谁的函数

    构造函数：不能是虚函数
    析构函数：最好是虚函数
    普通成员函数：可以是虚函数

    语法口诀：基类：virtual开头；子类：同名同参同返回，override结尾
    重写:基类必须带virtual
        子类不用再写virtual（会自动继承）
        必须加override更安全
        名字、参数、返回值必须一模一样
*/

//3、多态的触发条件：
/*
    1、基类中有虚函数（virtual）
    2、派生类重写这个虚函数（函数名、参数、返回值完全一样）
    3、有基类指针/引用派生类对象
    4、通过这个指针调用虚函数

class Base{
public:
    //1、基类有虚函数
    virtual void show(){
        cout<<"Base"<<endl;
    }
};

class Derived:public Base{
public:
    //2、派生类重写
    void show() override{
        cout<<"Derived"<<endl;
    }
};

int main(){
    //3、基类指针指向派生类
    Base* p=new Derived();

    //4、调用虚函数，多态生效，输出Derived
    p->show();

    delete p;
    return 0;
}
*/


//4、虚析构函数：只要基类里有虚函数，析构函数就必须写成虚析构
/*
    语法：基类的虚构函数前面加virtual：virtual ～类名(){...}
    场景：用基类指针指向派生类对象，最后delete这个指针
    如果不是虚析构函数，派生类析构不执行，派生类申请的内存资源不释放，内存泄漏
    先析构子类，再析构父类

class Base{
public:
    virtual ~Base(){//虚析构
        cout<<"Base析构"<<endl;
    }
};

class Derived:public Base{
public:
    ~Derived(){
        cout<<"Derived析构"<<endl;
    }
};

int main(){
    Base* p=new Derived();
    delete p;
    //输出： Derived析构
            Base析构
}
*/

//5、纯虚函数&抽象类
/*
    纯虚函数：没有声明，一般没有函数体，在末尾=0
            virtual void speak()=0;
    抽象类：包含至少一个纯虚函数的类，不能创建对象/实例化，智能指针继承，用于做接口/规范
    子类必须实现纯虚函数，子类如果不实现，子类也变成抽象类，也不能创建对象

    纯虚函数：只定规矩，不实现
    抽象类：不能造对象，只能当父类  
    子类：必须实现纯虚函数，否则也是抽象类
*/

//6、虚函数表vtable原理
/*
    1、只要类里有虚函数，编译器就会生成一张虚函数表vtable；
    2、每个对象里会多一个隐藏指针虚指针vptr，指向这张表；
    3、调用虚函数时：通过vptr查表，找到对应函数

    基类vtable：存基类虚函数地址
    派生类vtable：存重写后的函数地址
    基类指针指向谁，就用谁的vtable，调用谁的函数
*/

//7、对比：虚函数、虚析构、纯虚函数
/*
    虚函数：用于实现多态，有函数体;
           virtual void func(){...}有函数体；
           可以子类重写，实现多态；
           可实例化；
           子类可重写可不重写；
    虚析构：用于安全删除，防止派生类泄漏，必须加；
           virtual ~类名(){...清理资源...}
           用基类指针删派生对象，保证子类析构被调用；
           可实例化；
           可重写子类可不重写，会自动生成，但必须重写情况：子类自己new了内存、打开了文件、占用了资源；
    纯虚函数：用于定接口，强制实现，只定接口，没有实现（=0）；
            virtual void func()=0;没有函数体或很少写
            强制子类必须实现，定义接口规范；
            抽象类，不可实例化；
            必须写，否则子类也是抽象类；
*/

/*8、内存布局结构图：
对象内存起始：                                   Derived 对象内存：
  +------------------+                           +------------------+
  |  vptr 虚指针      |  → 指向 虚函数表 vtable     |  vptr            |  → 指向 Derived 的 vtable
  +------------------+                           +------------------+
  |  成员变量        |                             |  成员变量         |
  +------------------+                           +------------------+

虚函数表 vtable（每个类一张）                      Derived 的 vtable：
  +------------------+                           +------------------+
  |  &Base::func1    |                           |  &Derived::func1 |  ←--- 被重写了
  +------------------+                           +------------------+
  |  &Base::func2    |                           |  &Base::func2    |  ←--- 没重写，用父类的
  +------------------+                           +------------------+
  |  &Base::~Base    |  虚析构函数                 |&Derived::~Derived|
  +------------------+                           +------------------+
                vptr指向谁的表，就调用谁的函数->这就是多态
  */

#include <iostream>
using namespace std;

class Base{
public:
    //虚函数
    virtual void func1(){
        cout<<"Base::func1"<<endl;
    }
    virtual void func2(){
        cout<<"Base::func2"<<endl;
    }
    //虚析构
    virtual ~Base(){
        cout<<"Base析构"<<endl;
    }
    //纯虚函数
    virtual void func()=0;
};

class Derived:public Base{
public:
    //重写func1，不重写fun2
    void func1() override{
        cout<<"Derived::func1"<<endl;
    }
    //重写析构
    ~Derived() override{
        cout<<"Derived析构"<<endl;
    }
    //子类必须实现纯虚函数
    void func() override{
        cout<<"Derived::func实现纯虚"<<endl;
    }
};

int main(){
    Base* p=new Derived();

    p->func1();//走Derived::func1
    p->func2();//走Base::func2
    delete p;//虚析构，先走子类再走父类

    return 0;
}