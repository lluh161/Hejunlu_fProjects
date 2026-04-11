cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(StackHeapDiff 堆与栈的区别.cpp)
target_link_libraries(StackHeapDiff pthread)

add_executable(VirtualMem 虚拟内存与地址映射.cpp)
target_link_libraries(VirtualMem pthread)

add_executable(StackOverflow 栈溢出.cpp)
target_link_libraries(StackOverflow pthread)