#ifndef LOG_H
#define LOG_H

#include <string>

class Log{
public:
    static std::string getCurrentTime();
    static void log(const std::string& msg);
};

#endif 