# 📚 EchoServer 项目模块功能拆解
这是一个基于 **C++ 实现的高并发网络服务器**
逐个拆解每个头文件对应的类的核心职责、需要实现的功能，以及它们之间的协作关系。

---

## 1. 核心入口：`EchoServer.h`
### 核心定位
整个服务器的**业务入口与总控类**，是用户直接交互的顶层类，负责串联所有底层模块，实现「回显」业务逻辑。
### 实现的功能
1.  **服务器生命周期管理**
    - 初始化：绑定端口、启动事件循环、初始化线程池等
    - 启动/停止：`start()` 启动服务，`stop()` 优雅关闭所有连接、释放资源
2.  **业务逻辑实现（回显核心）**
    - 接收客户端发来的消息，原封不动（或加工后）发回给客户端
    - 处理新连接接入、连接断开、异常断开等场景
3.  **模块串联**
    - 持有 `EventLoop`、`ThreadPool`、`Socket` 等模块的实例，将网络事件回调到业务层
    - 绑定 `Channel` 的读/写/错误回调，实现「网络事件→业务处理」的流转
4.  **配置管理**
    - 监听端口、线程池大小、超时时间等可配置参数的加载与管理

---

## 2. 事件驱动核心：`EventLoop.h` / `Epoll.h` / `Channel.h`
这三个是**Reactor 事件驱动模型**的核心，是高并发服务器的基石，三者强绑定。

### 2.1 `EventLoop.h`
### 核心定位
Reactor 模型的**事件循环主体**，是每个线程的「事件调度中心」，负责驱动整个线程的网络事件处理。
### 实现的功能
1.  **事件循环主逻辑**
    - `loop()`：无限循环，调用 `Epoll` 等待事件发生，分发事件到对应 `Channel`
    - `quit()`：安全退出事件循环，处理未完成的事件
2.  **Channel 管理**
    - 新增/更新/删除 `Channel`：将 `Channel` 注册到 `Epoll`，更新监听事件（读/写/错误）
    - 维护 `Channel` 映射表：通过文件描述符（fd）快速找到对应 `Channel`
3.  **跨线程任务投递（核心）**
    - `runInLoop()`：将任务投递到当前 EventLoop 线程执行（解决多线程安全问题）
    - `queueInLoop()`：异步投递任务到 EventLoop 队列，唤醒 EventLoop 执行
    - 实现「唤醒机制」：通过 eventfd/pipe 等机制，唤醒阻塞在 `epoll_wait` 的线程
4.  **定时器管理（可选增强）**
    - 维护定时任务队列，处理超时连接、心跳检测等

### 2.2 `Epoll.h`
### 核心定位
**Linux I/O 多路复用的封装类**，对 `epoll` 系统调用做面向对象封装，是 EventLoop 的底层依赖。
### 必须实现的功能
1.  **epoll 基础操作封装**
    - `epoll_create()`：创建 epoll 实例
    - `epoll_ctl()`：添加/修改/删除监听的文件描述符（fd）
    - `epoll_wait()`：等待事件发生，返回就绪的事件列表
2.  **事件监听管理**
    - 封装 `EPOLLIN`（读就绪）、`EPOLLOUT`（写就绪）、`EPOLLERR`（错误）等事件的添加/移除
    - 支持边缘触发（ET）/水平触发（LT）的配置（高并发服务器通常用 ET）
3.  **错误处理**
    - 封装系统调用错误的捕获与日志打印，保证服务器稳定性

### 2.3 `Channel.h`
### 核心定位
**文件描述符（fd）的封装类**，是「事件」和「回调」的桥梁，每个 socket/事件句柄对应一个 Channel。
### 必须实现的功能
1.  **fd 与事件绑定**
    - 持有 fd，以及需要监听的事件类型（读/写/错误）
    - `setReadCallback()` / `setWriteCallback()` / `setErrorCallback()`：绑定事件触发时的回调函数
2.  **事件处理**
    - `handleEvent()`：根据 epoll 返回的事件类型，调用对应的回调函数
    - 区分读事件、写事件、错误事件、关闭事件的不同处理逻辑
3.  **事件更新**
    - `enableReading()` / `disableReading()` / `enableWriting()` / `disableWriting()`：更新监听的事件
    - 通知 `EventLoop` 更新 epoll 监听状态
4.  **生命周期管理**
    - 绑定所属 `EventLoop`，确保 Channel 只在所属线程操作，保证线程安全

---

## 3. 网络基础封装：`Socket.h` / `InetAddress.h` / `Buffer.h`
这三个是**网络通信的底层工具类**，封装了 socket 编程的系统调用与数据结构，向上提供易用接口。

### 3.1 `Socket.h`
### 核心定位
**socket 文件描述符的封装类**，封装所有 socket 相关的系统调用，是网络连接的底层载体。
### 必须实现的功能
1.  **socket 基础操作**
    - `socket()`：创建 socket 实例（TCP/UDP，这里是 TCP）
    - `bind()`：绑定 IP 和端口
    - `listen()`：监听端口，设置 backlog
    - `accept()`：接受新连接，返回新的 socket fd
    - `connect()`：客户端连接服务器（客户端模式用）
    - `close()`：关闭 socket，释放资源
2.  **TCP 选项配置**
    - `setReuseAddr()` / `setReusePort()`：地址/端口复用，解决端口占用问题
    - `setTcpNoDelay()`：禁用 Nagle 算法，降低延迟
    - `setKeepAlive()`：开启 TCP 保活，检测死连接
    - `setNonBlocking()`：设置 socket 为非阻塞（I/O 多路复用的核心要求）
3.  **数据收发**
    - `read()` / `recv()`：从 socket 读取数据
    - `write()` / `send()`：向 socket 发送数据
    - 处理 `EAGAIN` / `EWOULDBLOCK` 等非阻塞错误
4.  **连接状态管理**
    - 获取对端地址、本端地址，判断连接是否断开

### 3.2 `InetAddress.h`
### 核心定位
**IP 地址与端口的封装类**，封装 `sockaddr_in` 等网络地址结构，简化地址操作。
### 必须实现的功能
1.  **地址结构封装**
    - 封装 `sockaddr_in`（IPv4）/ `sockaddr_in6`（IPv6，可选）结构
    - 支持通过「IP 字符串 + 端口」、「端口」（监听所有地址）、`sockaddr` 结构体构造
2.  **地址转换**
    - `toIp()`：将网络地址转换为 IP 字符串
    - `toPort()`：获取端口号
    - `toIpPort()`：获取「IP:端口」格式的字符串
    - `getSockAddr()`：获取原生 `sockaddr` 指针，用于 socket 系统调用
3.  **地址校验**
    - 校验 IP 地址格式是否合法，支持 IPv4/IPv6 区分

### 3.3 `Buffer.h`
### 核心定位
**应用层读写缓冲区**，解决「TCP 粘包问题」和「非阻塞读写的不完整性」，是数据收发的缓冲载体。
### 必须实现的功能
1.  **缓冲区基础操作**
    - 自动扩容的动态缓冲区（基于 vector/链表，通常用 vector）
    - `readFd()`：从 socket fd 一次性读取所有数据到缓冲区（处理 ET 模式的读事件）
    - `writeFd()`：将缓冲区数据一次性写入 socket（处理写事件）
2.  **数据读写接口**
    - `append()`：向缓冲区追加数据
    - `retrieve()`：从缓冲区取出数据（支持按长度、按行、按指定分隔符读取，解决粘包）
    - `readableBytes()`：获取可读数据长度
    - `peek()`：查看缓冲区数据但不取出
3.  **内存管理**
    - 自动扩容、缩容，避免内存泄漏
    - 支持缓冲区清零、重置等操作
4.  **粘包处理（核心）**
    - 提供按行读取、按长度读取等接口，让业务层可以正确拆分完整的数据包

---

## 4. 业务辅助模块：`Http.h` / `Log.h` / `ThreadPool.h`
### 4.1 `Http.h`
### 核心定位
**HTTP 协议解析与封装类**（如果是纯 Echo 服务器，也可能是扩展 HTTP 服务），用于将 Echo 服务器升级为 HTTP 服务器。
### 必须实现的功能
1.  **HTTP 请求解析**
    - 解析请求行（方法、URL、HTTP 版本）
    - 解析请求头（Host、Content-Length 等）
    - 解析请求体（POST 数据）
    - 处理 GET/POST 等常用方法
2.  **HTTP 响应封装**
    - 构造响应行、响应头、响应体
    - 支持 200 OK、404 Not Found、500 Internal Server Error 等状态码
    - 支持长连接（Connection: keep-alive）、短连接
3.  **HTTP 会话管理**
    - 绑定 `Buffer`，从缓冲区读取完整 HTTP 请求，写入响应
    - 处理 HTTP 1.1 分块传输、管线化等特性（可选）

### 4.2 `Log.h`
### 核心定位
**日志系统封装类**，是服务器的「调试与运维工具」，用于记录运行状态、错误信息。
### 必须实现的功能
1.  **多级别日志**
    - 支持 DEBUG、INFO、WARN、ERROR、FATAL 等日志级别
    - 可配置日志输出级别，过滤低级别日志
2.  **日志输出**
    - 支持控制台输出、文件输出（按大小/按天滚动切割日志文件）
    - 日志格式：时间、级别、文件名、行号、线程ID、日志内容
3.  **线程安全**
    - 多线程环境下的日志写入安全（加锁/无锁队列实现）
    - 异步日志（可选）：将日志写入队列，后台线程异步落盘，不阻塞业务线程
4.  **日志管理**
    - 日志文件清理、过期删除，避免磁盘占满

### 4.3 `ThreadPool.h`
### 核心定位
**线程池封装类**，用于将耗时的业务逻辑（如数据库操作、复杂计算）从 EventLoop 线程剥离，避免阻塞事件循环，提升并发能力。
### 必须实现的功能
1.  **线程池生命周期**
    - 初始化：指定线程数，创建工作线程
    - 启动/停止：启动线程池，优雅停止（等待所有任务完成后退出）
2.  **任务管理**
    - `addTask()`：向线程池提交任务（支持普通函数、lambda、成员函数）
    - 任务队列：线程安全的任务队列，工作线程从队列取任务执行
3.  **线程同步**
    - 条件变量 + 互斥锁实现任务队列的同步
    - 支持任务优先级（可选）
4.  **状态监控**
    - 获取当前线程数、任务队列长度等监控指标（可选）

---

## 5. 模块协作流程（Echo 服务器完整链路）
我们用一个「客户端连接 → 发消息 → 服务器回显」的完整流程，串起所有模块的协作：
1.  **启动阶段**
    - `EchoServer` 初始化：创建 `EventLoop`、`ThreadPool`、`Socket`
    - `Socket` 绑定端口、监听，创建 `Channel` 绑定 listen fd，注册到 `EventLoop`
    - `EventLoop` 启动 `loop()`，调用 `Epoll::wait()` 等待事件
2.  **新连接接入**
    - 客户端发起连接，`Epoll` 监听到 listen fd 的读事件
    - `EventLoop` 分发事件到 `Channel`，`Channel` 调用读回调
    - 回调触发 `EchoServer::onNewConnection()`，`Socket::accept()` 接受新连接，返回新的 client fd
    - 为 client fd 创建新的 `Channel`，绑定读/写/错误回调，注册到 `EventLoop`
3.  **客户端发消息**
    - 客户端发送数据，`Epoll` 监听到 client fd 的读事件
    - `EventLoop` 分发事件到 `Channel`，`Channel` 调用读回调
    - 回调触发 `EchoServer::onMessage()`，调用 `Buffer::readFd()` 从 socket 读取所有数据到缓冲区
    - `EchoServer` 从 `Buffer` 取出完整消息，将消息写回 `Buffer`（回显逻辑）
    - 注册 client fd 的写事件到 `Epoll`
4.  **服务器回显消息**
    - `Epoll` 监听到 client fd 的写事件
    - `EventLoop` 分发事件到 `Channel`，`Channel` 调用写回调
    - 回调触发 `EchoServer::onWriteComplete()`，调用 `Buffer::writeFd()` 将缓冲区数据写入 socket
    - 数据发送完成，注销写事件
5.  **连接断开**
    - 客户端断开连接，`Epoll` 监听到 client fd 的读/关闭事件
    - `Channel` 调用关闭回调，`EchoServer` 释放 `Channel`、`Socket` 资源，从 `EventLoop` 移除

---

## 6. 关键设计要点（避坑指南）
1.  **线程安全**
    - `EventLoop` 是线程专属的，所有 `Channel` 操作必须在所属 `EventLoop` 线程执行
    - `ThreadPool` 用于剥离耗时任务，绝对不能在 `EventLoop` 线程执行阻塞操作
2.  **非阻塞 I/O**
    - 所有 socket 必须设置为非阻塞，`Epoll` 用边缘触发（ET）模式，提升并发性能
    - `Buffer` 必须一次性读完 socket 数据（ET 模式要求），否则会丢失事件
3.  **粘包处理**
    - 必须通过 `Buffer` 做应用层缓冲，按业务协议（如行分隔、长度前缀）拆分完整数据包
4.  **优雅关闭**
    - 服务器关闭时，先停止 `EventLoop`，再关闭所有 socket，最后释放线程池，避免资源泄漏

---

## 7. 扩展方向
如果需要从 Echo 服务器升级为生产级服务器，可以基于这些模块扩展：
- 增加 `Timer.h` 定时器模块，处理超时连接、心跳检测
- 增加 `HttpRouter.h` 路由模块，实现 HTTP 接口服务
- 增加 `Config.h` 配置模块，支持配置文件热加载
- 增加 `Monitor.h` 监控模块，统计 QPS、连接数等指标
