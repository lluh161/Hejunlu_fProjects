cmake_minimum_required(VERSION 3.16)
project(EchoServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 头文件路径
include_directories(网络框架/include)

# 自动找到所有 cpp
file(GLOB SRC_FILES
    网络框架/src/*.cpp
    网络框架/src/*/*.cpp
)

# 编译
add_executable(echo_server ${SRC_FILES})

# 线程库
target_link_libraries(echo_server pthread)