#ifndef RETRT_UTIL_H
#define RETRT_UTIL_H
#include "op/cos_result.h"

namespace qcloud_cos {

class RetryUtil {
 public:
  static bool ShouldRetryWithChangeDomain(CosResult result);
};
}  // namespace qcloud_cos

#endif