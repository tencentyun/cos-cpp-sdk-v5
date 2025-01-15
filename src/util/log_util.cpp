#include "util/log_util.h"

#include <stdarg.h>

#include <chrono>
#include <ctime>
#include <sstream>

#if !defined(_WIN32)
#include <syslog.h>
#endif

namespace qcloud_cos {

std::string LogUtil::GetLogPrefix(int level) {
  std::stringstream ss;
  char buf[64];
  std::time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::strftime(buf, 64, "%Y-%m-%d %H:%M:%S", std::localtime(&now));
  ss << buf;
  ss << " " << LOG_LEVEL_STRING(level);
  return ss.str();
}

// std::string LogUtil::FormatLog(int level, const char* fmt, ...) {
//   std::stringstream ss;
//   ss << GetLogPrefix(level);
//   char buf[1024];
//   va_list ap;
//   va_start(ap, fmt);
//   vsnprintf(buf, 1024, fmt, ap);
//   va_end(ap);
//   ss << buf;
//   return ss.str();
// }

std::string LogUtil::FormatLog(int level, const char* fmt, ...) {
  std::stringstream ss;
  ss << GetLogPrefix(level);
  std::vector<char> buf(1024);
  va_list ap;

  while (true) {
    va_start(ap, fmt);
    int needed = vsnprintf(buf.data(), buf.size(), fmt, ap);
    va_end(ap);
    if (needed > -1 && static_cast<size_t>(needed) < buf.size()) {
      break;
    }
    buf.resize(needed > -1 ? needed + 1 : buf.size() * 2);
  }
  ss << buf.data();
  return ss.str();
}

void LogUtil::Syslog(int level, const char* fmt, ...) {
#if !defined(_WIN32)
  va_list ap;
  va_start(ap, fmt);
  syslog(level, fmt, ap);
  va_end(ap);
#endif
  // for windows , do nothing
}

}  // namespace qcloud_cos
