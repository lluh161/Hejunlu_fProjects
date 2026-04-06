# 最低版本
cmake_minimum_required(VERSION 3.16)

# 项目名称
project(CppStudy LANGUAGES CXX)

# 强制C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 强制Debug模式
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

# 核心指令
add_subdirectory("C++相关内容")
add_subdirectory("操作系统相关内容")
add_subdirectory("网络编程相关内容")