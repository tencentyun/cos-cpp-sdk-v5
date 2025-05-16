#ifndef COS_CPP_SDK_V5_INCLUDE_UTIL_LOG_UTIL_H_
#define COS_CPP_SDK_V5_INCLUDE_UTIL_LOG_UTIL_H_

#include <stdint.h>

#include <string>

#include "cos_defines.h"

namespace qcloud_cos {

typedef void (*LogCallback)(const std::string& logstr);

class LogUtil {
 public:
  static std::string GetLogPrefix(int level);
  static std::string FormatLog(int level, const char* fmt, ...);
  static void Syslog(int level, const char* fmt, ...);
};

}  // namespace qcloud_cos

#endif  // COS_CPP_SDK_V5_INCLUDE_UTIL_LOG_UTIL_H_
