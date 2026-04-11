#pragma once
#include <string>
#include <fstream>
#include <mutex>

class Log{//日志
public:
    static Log& instance(); //获取单例实例
    void log(const std::string& msg);//日志输出
    void logInfo(const std::string& msg);//信息级别日志
    void logError(const std::string& msg);//错误级别日志

    //禁用拷贝和赋值
    Log(const Log&)=delete;
    Log& operator=(const Log&)=delete;

private:
    Log();
    ~Log();
    std::ofstream ofs_;//文件输出流
    std::mutex mtx_;//互斥锁
};

#define LOG_INFO(msg) Log::instance().logInfo(msg)
#define LOG_ERROR(msg) Log::instance().logError(msg)