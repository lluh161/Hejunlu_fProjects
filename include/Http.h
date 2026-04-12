#ifndef HTTP_H
#define HTTP_H

#include "Buffer.h"
#include <string>

class Http{
public:
    //解析HTTP请求
    static bool parseRequest(Buffer* buf);
    //组装HTTP响应
    static std::string makeResponse();
};
#endif