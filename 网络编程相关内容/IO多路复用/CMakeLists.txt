set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(epoll_lt_echo 
    "基础知识+简单epoll.cpp"
)
target_link_libraries(epoll_lt_echo PRIVATE pthread)

add_executable(epoll_et_echo 
    "ET epoll.cpp"
)
target_link_libraries(epoll_et_echo PRIVATE pthread)