#pragma once

#include <memory>

#include "Logger.h"

namespace logging {

extern const std::unique_ptr<Logger> stdLogger;

}

#define LOG_LEVEL INFO

#if LOG_LEVEL == DEBUG
    #define DEBUG(msg) logging::stdLogger->Debug(msg)
    #define INFO(msg) logging::stdLogger->Info(msg)
    #define WARN(msg) logging::stdLogger->Warn(msg)
    #define ERROR(msg) logging::stdLogger->Error(msg)
#elif LOG_LEVEL == INFO
    #define DEBUG(msg)
    #define INFO(msg) logging::stdLogger->Info(msg)
    #define WARN(msg) logging::stdLogger->Warn(msg)
    #define ERROR(msg) logging::stdLogger->Error(msg)
#elif LOG_LEVEL == WARN
    #define DEBUG(msg)
    #define INFO(msg)
    #define WARN(msg) logging::stdLogger->Warn(msg)
    #define ERROR(msg) logging::stdLogger->Error(msg)
#elif LOG_LEVEL == ERROR
    #define DEBUG(msg)
    #define INFO(msg)
    #define WARN(msg)
    #define ERROR(msg) logging::stdLogger->Error(msg)
#endif
