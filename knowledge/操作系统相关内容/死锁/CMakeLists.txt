cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(DeadLock 死锁.cpp)
target_link_libraries(DeadLock pthread)

add_executable(BankerAlgorithm 银行家算法.cpp)
target_link_libraries(BankerAlgorithm pthread)