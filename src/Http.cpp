#include "Http.h"
#include <unistd.h>

// 用你原有的retrieveAllAsString()解析请求
bool Http::parseRequest(Buffer* buf){
    // 直接获取缓冲区完整数据
    std::string request = buf->retrieveAllAsString();
    // 简单判断：只要是GET请求就认为合法
    return request.find("GET") != std::string::npos;
}

// 生成标准HTTP响应
std::string Http::makeResponse(){
    return "HTTP/1.1 200 OK\r\n"
           "Content-Type: text/html; charset=utf-8\r\n"
           "Content-Length: 20\r\n"
           "Connection: close\r\n\r\n"
           "Hello HTTP Server!";
}