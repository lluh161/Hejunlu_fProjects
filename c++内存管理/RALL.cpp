//RALL
//以前有new就有delete，现在用RAII：创建对象→自动new销毁对象→自动delete不用管释放
/*构造函数：对象出生→申请资源（new）
  析构函数：对象死亡→自动释放delete）
  不可以拷贝，防止重复释放，程序崩溃
    Myint a(10);
    Myint b=a;拷贝了
*/

#include <iostream>
#include <cstdio>//文件操作
#include <string>//字符串
using namespace std;

/*
打开文件:FILE* fp=fopen("test.txt","w/r/a/r+");
写文件:fputs/fwrite("写的内容",fp);
读文件:fgets/fread(buffer存放读的内容，大小，fp); char buffer[1024];
关闭文件:fclose(fp)
*/

//RALL封装文件句柄的类
class Myfile{
private:
    FILE* m_fp;//文件指针

public:
    //构造：自动打开文件
    Myfile(const string& path, const string& mode="r") {
        m_fp=fopen(path.c_str(), mode.c_str());
        //必须加c_str():文件操作、字符串操作函数、格式化输出、系统/文件路径、网络编程、动态内存

        if(!m_fp){
            cout<<"打开文件失败！"<< endl;
            throw runtime_error("文件打开失败");
        }
        cout<<"文件已打开"<<endl;
    }

    //析构：自动关闭文件
    ~Myfile(){
        if(m_fp){
            fclose(m_fp);
            cout<<"文件已自动关闭"<<endl;
        }
    }

    //写文件
    void write(const string& content) {
        fputs(content.c_str(), m_fp);
    }

    //读文件
    void read(){
        char buf[1024];//读文件缓冲区
        while(fgets(buf,sizeof(buf),m_fp)) {
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