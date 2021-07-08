#include "util/log_util.h"
#include <stdarg.h>
#include <sstream>
#include <chrono>
#include <ctime>

namespace qcloud_cos {

std::string LogUtil::GetLogPrefix(int level) {
    std::stringstream ss;
    char buf[64];
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::strftime(buf, 64, "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    ss << buf;
    ss << " " << LOG_LEVEL_STRING(level);
    return ss.str();
}

std::string LogUtil::FormatLog(int level, const char* fmt, ...) {
    std::stringstream ss;
    ss << GetLogPrefix(level);
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, 1024, fmt, ap);
    va_end(ap);
    ss << buf;
    return ss.str();
}

}
