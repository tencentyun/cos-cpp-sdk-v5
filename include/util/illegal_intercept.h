#ifndef ILLEGAL_INTERCEPT_H
#define ILLEGAL_INTERCEPT_H
#include <string>
namespace qcloud_cos {

class IllegalIntercept {
 public:
  static bool ObjectKeySimplifyCheck(const std::string& path);
  static bool CheckBucket(const std::string& path);
  static bool isAlnum(char c);
};
}  // namespace qcloud_cos

#endif