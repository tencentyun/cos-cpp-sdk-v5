#ifndef COS_CPP_SDK_V5_INCLUDE_UTIL_BASE_OP_UTIL_H_
#define COS_CPP_SDK_V5_INCLUDE_UTIL_BASE_OP_UTIL_H_

#include <utility>

#include "cos_config.h"
#include "op/cos_result.h"

namespace qcloud_cos {
class BaseOpUtil {
public:
    explicit BaseOpUtil(SharedConfig  cos_conf) : m_config(std::move(cos_conf)) {}

    BaseOpUtil() = default;

    bool ShouldChangeBackupDomain(const CosResult &result, const uint32_t &request_num, bool is_ci_req = false) const;

    void SleepBeforeRetry(const uint32_t &request_num) const;

    std::string GetRealUrl(const std::string& host, const std::string& path, bool is_https, bool is_generate_presigned_url = false) const;

    uint64_t GetMaxRetryTimes() const;

    static std::string ChangeHostSuffix(const std::string& host);

private:
    SharedConfig m_config;

    bool UseDefaultDomain() const;
};
} // namespace qcloud_cos
#endif  // COS_CPP_SDK_V5_INCLUDE_UTIL_BASE_OP_UTIL_H_