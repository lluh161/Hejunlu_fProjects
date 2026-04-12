#ifndef HTTP_H
#define HTTP_H

#include "Buffer.h"
#include <string>

class Http{
public:
    //解析HTTP请求
    static bool parseRequest(Buffer* buf, std::string& method, std::string& path, std::string& version);
    //组装HTTP响应
    static std::string makeResponse(int statusCode, const std::string& body = "", const std::string& contentType = "text/html");
};
#endif