#ifndef COS_CPP_SDK_V5_INCLUDE_UTIL_RETRY_UTIL_H_
#define COS_CPP_SDK_V5_INCLUDE_UTIL_RETRY_UTIL_H_
#include "op/cos_result.h"

namespace qcloud_cos {

class RetryUtil {
 public:
  static bool ShouldRetryWithChangeDomain(CosResult result);
};
}  // namespace qcloud_cos

#endif  // COS_CPP_SDK_V5_INCLUDE_UTIL_RETRY_UTIL_H_