#ifndef COS_CPP_SDK_V5_INCLUDE_UTIL_ILLEGAL_INTERCEPT_H_
#define COS_CPP_SDK_V5_INCLUDE_UTIL_ILLEGAL_INTERCEPT_H_
#include <string>
namespace qcloud_cos {

class IllegalIntercept {
 public:
  static bool ObjectKeySimplifyCheck(const std::string& path);
  static bool CheckBucket(const std::string& path);
  static bool isAlnum(char c);
};
}  // namespace qcloud_cos

#endif  // COS_CPP_SDK_V5_INCLUDE_UTIL_ILLEGAL_INTERCEPT_H_