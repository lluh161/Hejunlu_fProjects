//多路复用:一个线程，同时监听多个文件描述符（socket），等谁就绪就处理谁

//5种IO模型
/*
    阻塞IO（默认）        调用read/write时卡住，直到完成
    非阻塞IO             不卡住，但要不断轮询
    IO多路复用           select/poll/epoll一个线程监听多个fd。
    信号驱动IO           很少用
    异步IO              内核完成后通知你

*/

//select/poll/epoll对比
/*
    	        select	    poll	     epoll(高性能网络唯一选择)
    最大连接数	   1024      无上限	        无上限
    遍历方式	 遍历全部	  遍历全部	   只遍历就绪
    效率	      O(n)	     O(n)	      O(1)
    拷贝	    每次拷贝	 每次拷贝	    内核共享
    触发模式	    LT	      LT	      LT + ET
    使用难度	   复杂	       复杂	         简单
*/

//epoll详解及对比LTvsET
/*
    epoll:是Linux下IO多路复用的高性能实现，一个线程可以同时监听成千上万个socket，只处理就绪的，效率极高。
          1.无最大连接数限制
          2.只遍历就绪fd，效率O(1)
          3.无需每次复制fd集合到内核
          4.支持ET高性能模式
          5.支持百万级并发连接
    三大核心API详解:创建epoll句柄 /int epfd=epoll_create1(0);/返回一个文件描述符
                  管理要监听的fd（增/删/改）/epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event);/
                                        EPOLL_CTL_ADD：添加 fd
                                        EPOLL_CTL_DEL：删除 fd
                                        EPOLL_CTL_MOD：修改事件
                  等待事件发生 /int n=epoll_wait(epfd,events,maxevents,timeout);/阻塞等待，只返回已经就绪的fd列表->不用遍历所有 fd → O (1) 效率
    触发模式对比:
        特性	        LT水平触发	              ET边缘触发
        触发时机	缓冲区有数据就一直触发	仅状态变化（空→有数据）时触发一次
        读取要求	    可分多次读取	        必须一次性读完所有数据
        非阻塞要求	        可选	            必须配合非阻塞IO
        性能	           一般	                    极高
        易用性	          简单安全	            需注意细节，易丢数据
*/

//关于maOS的IO多路复用kqueue
/*
1.对应Linux的epoll，是macOS/BSD系统的高性能IO接口
2.核心API：
   -kqueue()：创建实例
   -EV_SET()：构造事件
   -kevent()：注册/等待事件
3.支持：socket、文件、信号、定时器等多种事件
4.为LT水平触发模式（默认，安全易用）
*/

//Echo Server（为Linux版）
/*#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> //用于打印客户端IP

using namespace std;

#define MAX_EVENTS 1024
#define PORT 8888

int main()
{
    //1.创建监听socket
    int listen_fd=socket(AF_INET,SOCK_STREAM,0);
    if(listen_fd<0){
        perror("socket failed");
        return 1;
    }

    //2.绑定地址端口
    sockaddr_in server_addr{};
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(PORT);
    server_addr.sin_addr.s_addr=INADDR_ANY;//监听所有网卡

    if(bind(listen_fd,(sockaddr *)&server_addr,sizeof(server_addr))<0){
        perror("bind failed");
        close(listen_fd);
        return 1;
    }

    //3.开始监听
    if(listen(listen_fd, 5)<0){
        perror("listen failed");
        close(listen_fd);
        return 1;
    }

    //4.创建 epoll 实例
    int epfd=epoll_create1(0);
    if(epfd<0){
        perror("epoll_create1 failed");
        close(listen_fd);
        return 1;
    }

    //5.将监听socket加入epoll（LT 水平触发）
    epoll_event ev{};
    ev.events = EPOLLIN;//监听读事件
    ev.data.fd = listen_fd;
    if(epoll_ctl(epfd,EPOLL_CTL_ADD,listen_fd,&ev)<0){
        perror("epoll_ctl add listen_fd failed");
        close(listen_fd);
        close(epfd);
        return 1;
    }

    epoll_event events[MAX_EVENTS];
    cout<<"Epoll Echo Server启动，监听端口："<<PORT<<endl;

    //6.事件循环（核心）
    while (1){
        //阻塞等待就绪事件，只返回有事件的fd
        int n=epoll_wait(epfd,events,MAX_EVENTS,-1);
        if(n<0){
            perror("epoll_wait failed");
            break;
        }

        for(int i=0;i<n;i++){
            int fd=events[i].data.fd;

            //情况1：监听fd就绪->新客户端连接
            if(fd==listen_fd){
                sockaddr_in client_addr{};
                socklen_t client_len=sizeof(client_addr);
                int client_fd=accept(listen_fd, (sockaddr *)&client_addr,&client_len);
                if(client_fd<0){
                    perror("accept failed");
                    continue;
                }

                //将新客户端fd加入epoll（LT 模式）
                ev.events=EPOLLIN;
                ev.data.fd=client_fd;
                if(epoll_ctl(epfd,EPOLL_CTL_ADD,client_fd, &ev)<0){
                    perror("epoll_ctl add client_fd failed");
                    close(client_fd);
                    continue;
                }

                cout<<"新客户端连接："<<inet_ntoa(client_addr.sin_addr) 
                    <<":"<<ntohs(client_addr.sin_port)<<endl;
            }
            //情况2：客户端fd就绪->有数据可读
            else{
                char buf[1024]={0};
                ssize_t len=read(fd,buf,sizeof(buf)-1);

                if(len<=0){
                    //连接断开或出错
                    epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr);
                    close(fd);
                    cout<<"客户端断开连接："<<fd<<endl;
                }
                else{
                    cout<<"收到客户端 "<<fd<<"数据："<<buf;
                    write(fd,buf,len);//Echo回显：原样发回
                    memset(buf,0,sizeof(buf));
                }
            }
        }
    }

    //资源清理
    close(listen_fd);
    close(epfd);
    return 0;
}
*/

//采用替代的kqueue
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/event.h>//kqueue核心头文件
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
using namespace std;

#define MAX_EVENTS 1024
#define BUF_SIZE 1024
#define PORT 8888

int main(){
    //1.创建监听 socket
    int listen_fd=socket(AF_INET,SOCK_STREAM,0);
    if(listen_fd<0){
        perror("socket failed");
        return 1;
    }

    //2.绑定地址端口
    sockaddr_in server_addr{};
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(PORT);
    server_addr.sin_addr.s_addr=INADDR_ANY;

    if (bind(listen_fd,(sockaddr*)&server_addr,sizeof(server_addr))<0){
        perror("bind failed");
        close(listen_fd);
        return 1;
    }

    //3.开始监听
    if(listen(listen_fd,5)<0){
        perror("listen failed");
        close(listen_fd);
        return 1;
    }

    //4.创建kqueue实例（对应 epoll_create1）
    int kq=kqueue();
    if(kq<0){
        perror("kqueue failed");
        close(listen_fd);
        return 1;
    }

    //5.注册监听socket的读事件（对应 epoll_ctl ADD）
    struct kevent ev;
    EV_SET(&ev, listen_fd,EVFILT_READ,EV_ADD,0,0,nullptr);
    if(kevent(kq,&ev,1,nullptr,0,nullptr)<0){
        perror("kevent add listen_fd failed");
        close(listen_fd);
        close(kq);
        return 1;
    }

    struct kevent events[MAX_EVENTS];
    cout<<"Kqueue Echo Server 启动，监听端口："<<PORT<<endl;

    //6.事件循环（对应 epoll_wait）
    while(true){
        //阻塞等待就绪事件
        int n=kevent(kq,nullptr,0,events,MAX_EVENTS,nullptr);
        if(n<0){
            perror("kevent wait failed");
            break;
        }

        for(int i=0;i<n;++i){
            int fd=(int)events[i].ident;

            //情况1：监听fd就绪→新客户端连接
            if(fd==listen_fd){
                sockaddr_in client_addr{};
                socklen_t client_len=sizeof(client_addr);
                int client_fd=accept(listen_fd,(sockaddr*)&client_addr,&client_len);
                if(client_fd<0){
                    perror("accept failed");
                    continue;
                }

                //注册客户端socket的读事件
                EV_SET(&ev,client_fd,EVFILT_READ,EV_ADD,0,0,nullptr);
                if(kevent(kq,&ev,1,nullptr,0,nullptr)<0){
                    perror("kevent add client_fd failed");
                    close(client_fd);
                    continue;
                }

                cout<<"新客户端连接："<<inet_ntoa(client_addr.sin_addr)
                    <<":"<<ntohs(client_addr.sin_port)<<endl;
            }
            //情况2：客户端fd就绪→有数据可读
            else{
                char buf[BUF_SIZE]={0};
                ssize_t len=read(fd,buf,BUF_SIZE-1);

                if(len<=0){
                    //连接断开，从kqueue中删除
                    EV_SET(&ev,fd,EVFILT_READ,EV_DELETE,0,0,nullptr);
                    kevent(kq,&ev,1,nullptr,0,nullptr);
                    close(fd);
                    cout<<"客户端断开连接："<<fd<<endl;
                }else{
                    cout<<"收到客户端 "<<fd<<" 数据："<<buf;
                    write(fd,buf,len);//Echo回显
                    memset(buf,0,BUF_SIZE);
                }
            }
        }
    }

    //资源清理
    close(listen_fd);
    close(kq);
    return 0;
}