# 【第一行必须是版本声明，绝对不能改顺序】
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# 总项目声明
project(Hejunlu_fProjects VERSION 1.0 LANGUAGES C CXX)

# 全局C++标准，所有子项目继承
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译警告配置（可选，提升代码质量）
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 【核心】添加子项目：EchoServer
# 路径严格对应你的目录：C++/EchoServer
add_subdirectory(C++/EchoServer)

# 后续添加其他子项目，直接在这里加一行即可
# add_subdirectory(Web)
# add_subdirectory(Python)