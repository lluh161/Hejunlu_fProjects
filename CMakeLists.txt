cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(EchoServer
        VERSION 1.0.0
        LANGUAGES CXX
        DESCRIPTION "C++17 High-Performance Cross-Platform Echo Server"
)

# ====================== C++17 标准强制设置 ======================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 禁用GNU扩展，保证跨平台兼容性

# ====================== 平台区分与编译选项 ======================
# 区分Linux/macOS，适配epoll/kqueue
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-D__linux__)
    # Linux开启epoll相关优化
    add_compile_options(-O2 -pthread)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions(-D__APPLE__)
    # macOS适配kqueue
    add_compile_options(-O2)
endif()

# 通用编译警告
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    # Debug模式：开启调试信息，无优化
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address")
    # Release模式：最高优化，去除调试信息
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# ====================== 源文件收集 ======================
# 递归查找src目录下所有.cpp，自动包含子目录，新增文件自动识别
file(GLOB_RECURSE SRC_FILES
    CONFIGURE_DEPENDS
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/**/*.cpp
)

# ====================== 头文件路径 ======================
# 只需要包含根目录的include，所有头文件都在这里，结构清晰
include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

# ====================== 生成可执行文件 ======================
add_executable(echo_server ${SRC_FILES})

# ====================== 线程库（网络代码必须，跨平台写法） ======================
find_package(Threads REQUIRED)
target_link_libraries(echo_server PRIVATE Threads::Threads)

# ====================== 输出目录（统一到bin，方便运行） ======================
set_target_properties(echo_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# ====================== 调试信息打印（确认文件是否找到） ======================
message(STATUS "=====================================")
message(STATUS "项目名称: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "系统平台: ${CMAKE_SYSTEM_NAME}")
message(STATUS "C++标准: C++${CMAKE_CXX_STANDARD}")
message(STATUS "编译器: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "找到的源文件数量: ${SRC_FILES}")
message(STATUS "=====================================")