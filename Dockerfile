# 基础镜像：Ubuntu 22.04（纯Linux环境，自带epoll头文件）
FROM ubuntu:22.04

# 避免交互安装
ENV DEBIAN_FRONTEND=noninteractive

# 安装编译环境
RUN apt update && apt install -y \
    g++ \
    cmake \
    make \
    net-tools \
    && rm -rf /var/lib/apt/lists/*

# 工作目录
WORKDIR /app

# 复制整个项目
COPY . .

# 编译项目
RUN mkdir -p build && cd build \
    && cmake .. \
    && make -j8

# 暴露8888端口
EXPOSE 8888

# 启动服务器
CMD ["./build/echo_server"]