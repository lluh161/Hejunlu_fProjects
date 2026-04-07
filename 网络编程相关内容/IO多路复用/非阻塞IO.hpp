//非阻塞IO:默认socket是阻塞IO,阻塞：没数据就卡住不动,非阻塞：没数据就立刻返回，不卡住

/*
    为什么要用非阻塞IO:单线程能同时处理很多连接
                    配合epoll ET/kqueue EV_CLEAR必须用
                    高并发服务器标配
    怎么设置非阻塞IO:只要给fd加上O_NONBLOCK，它就变成非阻塞
                    #include <fcntl.h>
                    void set_nonblock(int fd) {
                        int flag = fcntl(fd, F_GETFL);
                        fcntl(fd, F_SETFL, flag | O_NONBLOCK);
                    }
    非阻塞read的返回规则 len>0：读到数据
                        len==0：客户端断开
                        len==-1：
                         errno==EAGAIN：没数据，正常
                         其他：真的出错
    非阻塞+ET为什么必须一起用:ET/EV_CLEAR只触发一次必须一次性把数据读完
                          必须循环读,循环读必须用非阻塞，否则最后一次会卡住
*/
/*
char buf[1024];

while(1){
    int len=read(fd,buf,1024);

    if(len>0){
        //处理数据
    }else if(len==0){
        close(fd);
        break;
    }else if(errno==EAGAIN){
        //读完了
        break;
    }
}
*/