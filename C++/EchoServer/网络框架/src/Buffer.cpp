#include "Buffer.h"
#include <sys/uio.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>

void Buffer::append(const char* data,size_t len){
    buf_.insert(buf_.end(),data,data+len);
}

void Buffer::append(const std::string& str) {
    append(str.data(),str.size());
}

ssize_t Buffer::readFd(int fd){//ET模式专用：从fd读取数据到缓冲区
    char extra_buf[65536];
    struct iovec vec[2];
    
    //第一块：缓冲区剩余空间
    vec[0].iov_base=buf_.data()+buf_.size();
    vec[0].iov_len=buf_.capacity()-buf_.size();
    //第二块：临时栈空间（防止缓冲区不够）
    vec[1].iov_base=extra_buf;
    vec[1].iov_len=sizeof(extra_buf);

    const ssize_t n=readv(fd,vec,2);
    if(n<0){
        return n;
    }else if(static_cast<size_t>(n)<=vec[0].iov_len){
        //数据全部读到了buf_里
        buf_.resize(buf_.size()+n);
    }else{
        //一部分数据在buf_，一部分在extra_buf
        buf_.resize(buf_.capacity());
        append(extra_buf,n-vec[0].iov_len);
    }
    return n;
}

ssize_t Buffer::writeFd(int fd){//向fd写缓冲区数据
    ssize_t n=::write(fd,peek(),readableBytes());
    if(n>0){
        retrieve(n);
    }
    return n;
}

const char* Buffer::peek() const{return buf_.data();}//获取可读数据起始地址

size_t Buffer::readableBytes() const{return buf_.size();}//可读字节数

void Buffer::retrieve(size_t len){//回收len字节数据
    if (len < readableBytes()){
        buf_.erase(buf_.begin(), buf_.begin() + len);
    } else {
        retrieveAll();
    }
}

void Buffer::retrieveAll(){buf_.clear();}//回收所有数据

std::string Buffer::retrieveAllAsString(){//读取所有数据并转为string
    std::string str(peek(),readableBytes());
    retrieveAll();
    return str;
}