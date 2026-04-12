#pragma once
#include <vector>
#include <string>

/**
 * @brief 网络数据读写缓冲区
 * 用于解决TCP粘包/半包问题，统一管理读写指针
 * 跨平台：Linux/macOS 通用
 */
class Buffer
{
public:
    // 缓冲区前置预留空间（用于在头部插入协议数据）
    static const size_t kCheapPrepend = 8;
    // 缓冲区初始化大小
    static const size_t kInitialSize = 1024;

    Buffer();

    /**
     * @brief 从文件描述符fd读取数据到缓冲区
     * @param fd 套接字fd
     * @return 读取到的字节数，-1表示出错
     */
    ssize_t readFd(int fd);

    /**
     * @brief 将缓冲区数据写入文件描述符fd
     * @param fd 套接字fd
     * @return 写入的字节数，-1表示出错
     */
    ssize_t writeFd(int fd);

    // ========== 缓冲区状态查询 ==========
    // 可读字节数（已接收、待处理的数据）
    size_t readableBytes() const { return writeIndex_ - readIndex_; }
    // 可写字节数（缓冲区剩余空间）
    size_t writableBytes() const { return buffer_.size() - writeIndex_; }

    // ========== 数据操作 ==========
    // 获取可读数据的起始指针
    const char* peek() const { return begin() + readIndex_; }
    // 释放len长度的已读数据（移动读指针）
    void retrieve(size_t len) { readIndex_ += len; }
    // 向缓冲区追加数据
    void append(const char* data, size_t len);

private:
    // 获取缓冲区底层数组起始地址
    char* begin() { return buffer_.data(); }
    const char* begin() const { return buffer_.data(); }

    std::vector<char> buffer_;  // 底层存储数组
    size_t readIndex_;          // 读指针：下一次要读取的位置
    size_t writeIndex_;         // 写指针：下一次要写入的位置
};