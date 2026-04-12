#include "Buffer.h"
#include <unistd.h>
#include <sys/uio.h>
#include <errno.h>
#include <cstring>

// 构造函数：初始化读写指针
Buffer::Buffer()
    : readIndex_(kCheapPrepend),
      writeIndex_(kCheapPrepend)
{
    buffer_.resize(kInitialSize);
}

// 向缓冲区追加数据
void Buffer::append(const char* data, size_t len)
{
    buffer_.insert(buffer_.end(), data, data + len);
    writeIndex_ += len;
}

/**
 * @brief 跨平台 ET 模式读（Linux + macOS 通用）
 * 采用 readv 分散读，一次性读满所有数据，避免ET模式漏读
 */
ssize_t Buffer::readFd(int fd)
{
    // 临时栈缓冲区：64K，用于存放溢出数据（高效）
    char extrabuf[65536];
    // 分散读结构体：两块缓冲区同时接收数据
    struct iovec vec[2];

    // 第一块：缓冲区自身剩余空间
    size_t writable = writableBytes();
    vec[0].iov_base = begin() + writeIndex_;
    vec[0].iov_len = writable;

    // 第二块：临时栈空间，防止缓冲区满了放不下
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    // 执行分散读（系统调用，跨平台通用）
    ssize_t n = readv(fd, vec, 2);

    // 读取出错处理
    if (n < 0)
    {
        // EAGAIN：非阻塞模式下无数据可读，正常情况
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;
        // 真正的错误
        return -1;
    }

    // 数据全部写入缓冲区自身
    if (static_cast<size_t>(n) <= writable)
    {
        writeIndex_ += n;
    }
    // 数据一部分写入缓冲区，一部分写入临时栈，再追加到缓冲区
    else
    {
        // 先填满缓冲区
        writeIndex_ = buffer_.size();
        // 再把临时栈的数据追加进来
        append(extrabuf, n - writable);
    }

    return n;
}

/**
 * @brief 跨平台写数据（简单稳定版，保证HTTP响应完整发送）
 * 直接将缓冲区所有可读数据写入socket
 */
ssize_t Buffer::writeFd(int fd)
{
    // 从套接字写入所有可读数据
    ssize_t n = ::write(fd, peek(), readableBytes());

    // 写入成功：移动读指针，释放已发送的数据空间
    if (n > 0)
    {
        retrieve(n);
    }

    return n;
}