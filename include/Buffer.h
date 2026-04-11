#pragma once
#include <vector>
#include <string>
#include <cstddef>

class Buffer{//读写缓冲区
private:
    std::vector<char> buf_;//存数据
    static const size_t kCheapPrepend=8;//预留空间
public:
    void append(const char* data,size_t len);//将指定长度的裸字符数据追加到缓冲区
    void append(const std::string& str);//将字符串直接追加到缓冲区
    ssize_t readFd(int fd);//ET模式下从socket读取数据到缓冲区，返回读写长度
    ssize_t writeFd(int fd);//将缓冲区数据写入socket，返回写入长度
    const char* peek() const;//缓冲区可读数据的首地址，只查看不取出
    size_t readableBytes() const;//缓冲区当前可读的总字节数
    void retrieve(size_t len);//释放缓冲区中指定长度的已读数据
    void retrieveAll();//清空缓冲区所有数据
    std::string retrieveAllAsString();//缓冲区所有数据转为字符串返回，并清空缓冲区
};
