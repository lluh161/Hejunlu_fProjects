#include "Log.h"

inline Log& Log::instance() {
    static Log logInstance;  // 静态局部变量，C++11 后线程安全
    return logInstance;
}

inline Log::Log() {
    // 打开日志文件（追加模式，不存在则创建）
    ofs_.open("system.log", std::ios::app | std::ios::out);
    if (!ofs_.is_open()) {
        std::cerr << "日志文件打开失败！" << std::endl;
    }
}

inline Log::~Log() {
    // 关闭文件流
    if (ofs_.is_open()) {
        ofs_.close();
    }
}

inline std::string Log::getCurrentTime() {
    time_t now = time(nullptr);
    char buf[64] = {0};
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buf);
}

inline void Log::log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtx_);  // 自动加锁/解锁

    if (ofs_.is_open()) {
        ofs_ << msg << std::endl;
        ofs_.flush();  // 立即写入文件，不缓存
    }
    // 同时输出到控制台
    std::cout << msg << std::endl;
}

inline void Log::logInfo(const std::string& msg) {
    std::string timeMsg = "[" + getCurrentTime() + "] [INFO] " + msg;
    log(timeMsg);
}

inline void Log::logError(const std::string& msg) {
    std::string timeMsg = "[" + getCurrentTime() + "] [ERROR] " + msg;
    log(timeMsg);
}