#include "Logger.h"

namespace logging {

Logger::Logger(LogLevel logLevel_):
    logLevel { logLevel_ },
    outStream { std::cerr } {
}

Logger::Logger(LogLevel logLevel_, std::ostream& outStream_):
    logLevel { logLevel_ },
    outStream { outStream_ } {
}

void Logger::Debug(const std::string& msg) {
    if (logLevel <= LogLevel::DEBUG) {
        std::string logMsg = CreateLogMessage("DEBUG", msg);
        Log(logMsg);
    }
}

void Logger::Info(const std::string& msg) {
    if (logLevel <= LogLevel::INFO) {
        std::string logMsg = CreateLogMessage("INFO", msg);
        Log(logMsg);
    }
}

void Logger::Warn(const std::string& msg) {
    if (logLevel <= LogLevel::WARN) {
        std::string logMsg = CreateLogMessage("WARN", msg);
        Log(logMsg);
    }
}

void Logger::Error(const std::string& msg) {
    if (logLevel <= LogLevel::ERROR) {
        std::string logMsg = CreateLogMessage("ERROR", msg);
        Log(logMsg);
    }
}

std::string Logger::CreateLogMessage(const std::string& level, const std::string& msg) {
    time_t rawTime;
    struct tm localTime;
    size_t len = msg.length() + level.length() + 26;
    std::string logMsg(len, 0);

    time(&rawTime);
    #ifdef _WIN32
    localtime_s(&localTime, &rawTime);
    #endif
    #ifdef __linux__
    localtime_r(&rawTime, &localTime);
    #endif

    len = snprintf(&logMsg[0], len, "[%04d-%02d-%02d %02d:%02d:%02d] %s - %s",
        localTime.tm_year + 1900,
        localTime.tm_mon + 1,
        localTime.tm_mday,
        localTime.tm_hour,
        localTime.tm_min,
        localTime.tm_sec,
        level.c_str(),
        msg.c_str());

    logMsg.resize(len);

    return std::move(logMsg);
}

void Logger::Log(const std::string& msg) {
    std::cerr << msg << std::endl;
}

}
