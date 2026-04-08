cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MultiThread 多线程.cpp)
target_link_libraries(MultiThread pthread)

add_executable(Distinguish 区别.cpp)
target_link_libraries(Distinguish pthread)

add_executable(StateSwitch 状态切换.cpp)
target_link_libraries(StateSwitch pthread)