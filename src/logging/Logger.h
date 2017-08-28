#pragma once

//for localtime_s usage
#ifdef _WIN32
#define __STDC_WANT_LIB_EXT1__
#endif

#include <iostream>
#include <ctime>
#include <string>
#include <memory>

namespace logging {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class Logger {

protected:
    LogLevel        logLevel;
    std::ostream&   outStream;

    virtual void    Log(const std::string& msg);
    std::string     CreateLogMessage(const std::string& level, const std::string& msg);

public:
     explicit       Logger(LogLevel logLevel_);
                    Logger(LogLevel logLevel_, std::ostream& outStream_);

     void           Debug(const std::string& msg);
     void           Info(const std::string& msg);
     void           Warn(const std::string& msg);
     void           Error(const std::string& msg);
};

}
