//在Linux下 LT改为ET：事件加EPOLLET、所有fd必须设置为非阻塞、读数据必须用while循环读到EAGAIN(循环accpt/read)
//在macOS下 LT改为ET：只需要加一个标志 /EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, nullptr);/
//一次性读完

//epoll ET边缘触发+非阻塞IO版本
/*
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>     //非阻塞需要
#include <errno.h>     //错误码需要
using namespace std;

#define MAX_EVENTS 1024
#define PORT 8888
#define BUF_SIZE 1024

// ======================
// 工具函数：设置非阻塞IO（ET 必须）
// ======================
void set_nonblock(int fd){
    int flag=fcntl(fd,F_GETFL);
    fcntl(fd,F_SETFL,flag | O_NONBLOCK);
}

int main()
{
    //1.创建监听socket
    int listen_fd=socket(AF_INET,SOCK_STREAM,0);
    if(listen_fd<0){
        perror("socket failed");
        return 1;
    }

    // ======================
    // LT→ET 改动 1：监听fd设为非阻塞
    // ======================
    set_nonblock(listen_fd);

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
    // ======================
    // LT→ET 改动 2：添加 EPOLLET 标志
    // ======================
    ev.events = EPOLLIN | EPOLLET;  //监听读事件+边缘触发
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
                //======循环 accept，处理所有等待的连接（ET 模式必须）
                while (true) {
                    sockaddr_in client_addr{};
                    socklen_t client_len=sizeof(client_addr);
                    int client_fd=accept(listen_fd, (sockaddr *)&client_addr,&client_len);
                    if(client_fd<0){
                        //=====没有更多连接，退出循环
                        if(errno==EAGAIN||errno==EWOULDBLOCK){
                            break;
                        }else{
                            perror("accept failed");
                            break;
                    }

                    // ======================
                    // LT → ET 改动 3：客户端 fd 设为非阻塞
                    // ======================
                    set_nonblock(client_fd);

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
            }
            //情况2：客户端fd就绪->有数据可读
            else{
                // ======================
                // LT→ET 改动 4：循环读，直到读完所有数据（ET 必须）
                // ======================
                char buf[BUF_SIZE] = {0};
                while (true) {
                    ssize_t len=read(fd,buf,sizeof(buf)-1);

                    if(len>0){
                        //回显数据
                        cout<<"收到客户端 "<<fd<<"数据："<<buf;
                        write(fd,buf,len);
                        memset(buf,0,sizeof(buf));
                    }else if(len==0){
                        //客户端断开连接
                        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr);
                        close(fd);
                        cout<<"客户端断开连接："<<fd<<endl;
                        break;
                    }else{
                        //数据读完或出错
                        if(errno==EAGAIN||errno==EWOULDBLOCK){
                            //数据已全部读完，正常退出循环
                            break;
                        }else{
                            //其他错误，断开连接
                            perror("read failed");
                            epoll_ctl(epfd,EPOLL_CTL_DEL,fd,nullptr);
                            close(fd);
                            break;
                        }
                    }
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

// kqueue ET 边缘触发版本（macOS 原生，等价 epoll ET）
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/event.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

#define PORT 8888
#define BUF_SIZE 1024
#define MAX_EVENTS 1024

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

// ======================
// 工具函数：设置非阻塞（ET 必须）
// ======================
void setnonblock(int fd){
    int flag=fcntl(fd,F_GETFL);
    fcntl(fd,F_SETFL,flag | O_NONBLOCK);
}

int main(){
    //1.创建监听 socket
    int listen_fd=socket(AF_INET,SOCK_STREAM,0);
    if(listen_fd<0){
        perror("socket failed");
        return 1;
    }

    // ======================
    // LT → ET 改动 1：监听 fd 设为非阻塞
    // ======================
    setnonblock(listen_fd);

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
    // ======================
    // LT → ET 改动 2：加 EV_CLEAR（边缘触发）
    // ======================
    EV_SET(&ev, listen_fd,EVFILT_READ,EV_ADD | EV_CLEAR,0,0,nullptr);

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
                // ======================
                // LT → ET 改动 3：循环 accept（ET必须）
                // ======================
                while(true){
                    sockaddr_in client_addr{};
                    socklen_t client_len=sizeof(client_addr);
                    int client_fd=accept(listen_fd,(sockaddr*)&client_addr,&client_len);
                    
                    if(client_fd<0){
                        //没有新连接了，退出
                        if(errno==EAGAIN||errno==EWOULDBLOCK){
                            break;
                        }else{
                            perror("accept failed");
                            break;
                        }
                    }
                    // ======================
                    // LT → ET 改动 4：客户端也设为非阻塞
                    // ======================
                    setnonblock(client_fd);

                    //注册客户端socket的读事件
                    // LT → ET 改动 5：客户端也加 EV_CLEAR
                    // ======================
                    EV_SET(&ev,client_fd,EVFILT_READ,EV_ADD|EV_CLEAR,0,0,nullptr);
                    if(kevent(kq,&ev,1,nullptr,0,nullptr)<0){
                        perror("kevent add client_fd failed");
                        close(client_fd);
                        continue;
                    }

                    cout<<"新客户端连接："<<inet_ntoa(client_addr.sin_addr)
                        <<":"<<ntohs(client_addr.sin_port)<<endl;
                }
            }
            //情况2：客户端fd就绪→有数据可读
            else{
                // ======================
                // LT → ET 改动 6：循环 read 直到读完（ET必须）
                // ======================
                char buf[BUF_SIZE];
                while(true){
                    ssize_t len=read(fd,buf,BUF_SIZE-1);
                    if(len>0){
                        buf[len]=0;
                        cout<<"收到客户端 "<<fd<<" 数据："<<buf;
                        write(fd,buf,len);//Echo回显
                    }
                    else if(len==0){
                        //断开连接
                        EV_SET(&ev,fd,EVFILT_READ,EV_DELETE,0,0,nullptr);
                        kevent(kq,&ev,1,nullptr,0,nullptr);
                        close(fd);
                        cout<<"客户端断开连接："<<fd<<endl;
                        break;
                    }
                    else{
                        //读完了（EAGAIN），退出循环
                        if(errno==EAGAIN||errno==EWOULDBLOCK){
                            break;
                        }
                        //其他错误
                        else{
                            perror("read error");
                            EV_SET(&ev,fd,EVFILT_READ,EV_DELETE,0,0,nullptr);
                            kevent(kq,&ev,1,nullptr,0,nullptr);
                            close(fd);
                            break;
                        }
                    }
                }
            }
        }
    }

    //资源清理
    close(listen_fd);
    close(kq);
    return 0;
}