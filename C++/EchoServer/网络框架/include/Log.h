#pragma once
#include <string>
#include <fstream>
#include <mutex>

class Log{
public:
    static Log& instance(); 
    void log(const std::string& msg);
    void logInfo(const std::string& msg);
    void logError(const std::string& msg);
private:
    Log();
    ~Log();
    std::ofstream ofs_;
    std::mutex mtx_;
};

#define LOG_INFO(msg) Log::instance().logInfo(msg)
#define LOG_ERROR(msg) Log::instance().logError(msg)