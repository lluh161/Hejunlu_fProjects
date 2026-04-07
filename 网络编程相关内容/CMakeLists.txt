cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 网络编程相关内容下的子模块
add_subdirectory("TCP协议")
add_subdirectory("IO多路复用")
