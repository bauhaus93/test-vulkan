#include "StdLogger.h"

namespace logging {

const std::unique_ptr<Logger> stdLogger = std::make_unique<Logger>(LogLevel::LOG_LEVEL);

}
