#include "FileLogger.h"

namespace logging {

FileLogger::FileLogger(LogLevel logLevel_, const std::string& filename):
    Logger(logLevel_),
    file { filename } {
}

FileLogger::~FileLogger() {
    file.close();
}

void FileLogger::Log(const std::string& msg) {
    file << msg << std::endl;
}

}
