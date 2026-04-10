//智能指针
/*是什么？
  自动管理内存的 “安全版指针”，让你告别手动 new/delete，写出更健壮的代码。
 
  有什么用？不用担心忘记释放、重复释放或野指针问题; 

  特点：
  自动释放：离开作用域/异常/提前return时，都会自动释放内存，不会内存泄漏
  生命周期管理: unique_ptr：独占所有权；不能拷贝，只能移交所有权；一个对象一个管理者，性能最高
              shared_ptr：共享所有权；引用计数；支持拷贝，拷贝时计数+1，销毁时-1；多个管理者一个对象
              weak_ptr：无所有权；解决shared_ptr循环引用，不增加引用计数；观察对象，不能访问、不销毁
  安全可靠：避免野指针、重复释放等常见内存错误
*/

#include <iostream>
#include <memory>//智能指针头文件
#include <string>
#include <cstdio>
using namespace std;
/*
//-------------------------unique_ptr
{
    //创建unique_ptr，管理int内存
    unique_ptr<int> p1(new int(100));
    cout<<*p1<<endl;//输出100

    //不能拷贝，只能转移所有权（move）
    unique_ptr<int> p2=move(p1);

    //p1为空，p2接管内存
    if(!p1) cout<<"p1已空"<<endl;//p1不再拥有内存
    cout<<*p2<<endl;//输出100
    
    }//p2离开作用域，自动delete内存


//--------------------------shared_ptr
{
    //创建shared_ptr，引用计数=1
    shared_ptr<int> p3(new int(200));
    cout<<"p3引用计数："<<p3.use_count()<<endl;//输出1

    //拷贝，引用计数+1
    shared_ptr<int> p4=p3;
    cout<<"p4引用计数："<<p4.use_count()<<endl;//输出2
    cout<<"p3引用计数："<<p3.use_count()<<endl;//输出2

    //p2销毁，计数-1
    {
        shared_ptr<int> p5=p3;
        cout<<"p1引用计数："<<p3.use_count()<< endl;//输出3
    }
    //作用域结束，p5释放，计数-1
    cout<<"p1引用计数："<<p3.use_count()<<endl;//输出2

    //最后一个shared_ptr销毁时，内存才释放
}

//--------------------------weak_ptr
class B;//前向声明

class A{
public:
    weak_ptr<B> b;//用weak_ptr避免循环引用：A不持有B，只观察，可以释放AB
    ~A() {cout<<"A被销毁"<<endl;}
};

class B{
public:
    shared_ptr<A> a;
    ~B() {cout<<"B被销毁"<<endl;}
};

int main(){
    shared_ptr<A> a=make_shared<A>();
    shared_ptr<B> b=make_shared<B>();
    a->b=b;
    b->a=a;
    //正常释放，不会内存泄漏，AB归0，释放内存
    return 0;
}
*/

//用shared_ptr改写RALL类
/*完全RAII:构造打开->析构自动关闭
  用shared_ptr管理文件:安全、自动、不怕异常
  支持拷贝:多个对象共用同一个文件,最后一个用完才关闭
  不用写fclose():永远不会忘
*/
class Myfile{
private:
    //FILE* m_fp;
    //改用share_ptr管理，自动关闭文件
    shared_ptr<FILE> m_fp;

public:
    //构造：自动打开文件
    Myfile(const string& path, const string& mode="r") {
        FILE* temp_fp=fopen(path.c_str(), mode.c_str());
        //必须加c_str():文件操作、字符串操作函数、格式化输出、系统/文件路径、网络编程、动态内存

        if(!temp_fp){
            cout<<"打开文件失败！"<< endl;
            throw runtime_error("文件打开失败");
        }

        //交给shared_ptr管理，并传入lambda删除器，自定义关闭方式
        m_fp=shared_ptr<FILE>(temp_fp,[](FILE* fp){
            if(fp){
                fclose(fp);
                cout<<"文件自动关闭"<<endl;
            }
        });

        cout<<"文件已打开"<<endl;
    }

    //不需要 析构：自动关闭文件
    /*~Myfile(){
        if(m_fp){
            fclose(m_fp);
            cout<<"文件已自动关闭"<<endl;
        }
    }*/

    //写文件
    void write(const string& content) {
        fputs(content.c_str(), m_fp.get());//get获取原始指针
    }

    //读文件
    void read(){
        char buf[1024];//读文件缓冲区
        while(fgets(buf,sizeof(buf),m_fp.get())) {
            cout<<buf;
        }
    }
};

// 测试
int main() {
    //try/catch代码放try里，一旦出错不崩溃程序，跳到catch中
    try{
        //创建对象w
        Myfile file("test.txt", "w");
        file.write("Hello RAII!\n");
        file.write("这是自动管理的文件！");

        //读取文件r
        Myfile readFile("test.txt", "r");
        cout<<"\n读取内容：\n";
        readFile.read();
        cout<<endl;

        //追加内容a
        Myfile appendFile("test.txt", "a");
        appendFile.write("\n这是追加的内容！");
    }
    catch(...){
        cout<<"出错了"<<endl;
    }

    //离开大括号
    //file对象销毁->自动调用析构->自动关闭文件
    return 0;
}
//资源管理方式：
/*管理堆内存→智能指针
  管理文件/句柄→RAII类/Lambda删除器
  管理锁→lock_guard/unique_lock
  管理普通对象→栈上创建
  管理动态数组→vector
*/