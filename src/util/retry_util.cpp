#include "util/retry_util.h"
#include "cos_sys_config.h"

namespace qcloud_cos {


bool RetryUtil::ShouldRetryWithChangeDomain(CosResult result) {
    if ((result.GetHttpStatus() != 200 && result.GetXCosRequestId().empty()) && CosSysConfig::GetRetryChangeDomain())
    {
        return true;
    }
    return false;
}

}  // namespace qcloud_cos