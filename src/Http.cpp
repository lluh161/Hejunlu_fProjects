#include "Http.h"
#include <sstream>
#include <algorithm>

// 解析HTTP请求（只处理GET，兼容浏览器基础请求）
bool Http::parseRequest(Buffer* buf, std::string& method, std::string& path, std::string& version) {
    if (!buf || buf->readableBytes() == 0) return false;

    const char* data = buf->peek();
    size_t len = buf->readableBytes();
    std::string request(data, len);

    // 找请求行（第一行：GET / HTTP/1.1\r\n）
    size_t lineEnd = request.find("\r\n");
    if (lineEnd == std::string::npos) return false; // 没收到完整请求头

    std::string requestLine = request.substr(0, lineEnd);
    std::istringstream iss(requestLine);
    iss >> method >> path >> version;

    // 校验请求行格式
    if (method.empty() || path.empty() || version.find("HTTP/") != 0) {
        return false;
    }

    // 找到完整请求头结束（\r\n\r\n），释放已读数据
    size_t headerEnd = request.find("\r\n\r\n");
    if (headerEnd == std::string::npos) return false;
    buf->retrieve(headerEnd + 4); // 跳过\r\n\r\n

    return true;
}

// 组装标准HTTP响应
std::string Http::makeResponse(int statusCode, const std::string& body, const std::string& contentType) {
    std::string statusText;
    switch (statusCode) {
        case 200: statusText = "OK"; break;
        case 404: statusText = "Not Found"; break;
        default: statusText = "Internal Server Error";
    }

    std::ostringstream response;
    response << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
    response << "Content-Type: " << contentType << "; charset=utf-8\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Connection: close\r\n"; // 先做短连接，避免长连接问题（最小代价）
    response << "\r\n";
    response << body;

    return response.str();
}