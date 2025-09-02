
#include "util/base_op_util.h"
#include "cos_sys_config.h"
#include "util/codec_util.h"
#include "util/simple_dns_cache.h"
#include <thread>

namespace qcloud_cos {
SimpleDnsCache& GetGlobalDnsCacheInstance() {
  static SimpleDnsCache dns_cache(CosSysConfig::GetDnsCacheSize(),
                                  CosSysConfig::GetDnsCacheExpireSeconds());
  return dns_cache;
}

bool BaseOpUtil::ShouldChangeBackupDomain(const CosResult &result, const uint32_t &request_num, const bool is_ci_req) const {
  if (is_ci_req) {
    // 请求到万象的, 不切域名
    return false;
  }
  if (!CosSysConfig::GetRetryChangeDomain()) {
    // 没开启开关, 不切域名
    return false;
  }
  if (!UseDefaultDomain()) {
    // 未使用默认域名, 不切域名
    return false;
  }
  if (!result.GetXCosRequestId().empty()) {
    // 响应中有x-cos-request-id, 说明请求到了 COS, 不切域名
    return false;
  }
  const int statusCode = result.GetHttpStatus();
  if (statusCode == 301 || statusCode == 302 || statusCode == 307) {
    // 3xx 的响应码只要满足切换条件就切换域名, 不需要等最后一次重试
    return true;
  }
  if (request_num + 1 < m_config->GetMaxRetryTimes()) {
    // 还没到最大重试次数, 不切域名
    return false;
  }
  return statusCode >= 500;
}

bool BaseOpUtil::UseDefaultDomain() const {
  if (m_config && m_config->GetSetIntranetOnce() && m_config->IsUseIntranet() && !m_config->GetIntranetAddr().empty()
    || CosSysConfig::IsUseIntranet() && !CosSysConfig::GetIntranetAddr().empty()
    || (!m_config->GetDestDomain().empty())
    || !CosSysConfig::GetDestDomain().empty()
    || m_config->GetRegion() == "accelerate") {
    return false;
  }
  return true;
}

std::string BaseOpUtil::GetRealUrl(const std::string& host, const std::string& path,
                               bool is_https, bool is_generate_presigned_url) const {
                                // 1. host优先级，私有ip > 自定义域名 > DNS cache > 默认域名
  std::string dest_uri;
  std::string dest_host = host;
  std::string dest_path = path;
  std::string dest_protocal = "http://"; // NOCA:HttpHardcoded(ignore)
  if (is_https) {
    dest_protocal = "https://";
  }

  if (dest_path.empty() || '/' != dest_path[0]) {
    dest_path = "/" + dest_path;
  }

  if (m_config &&
      m_config->GetSetIntranetOnce() &&
      m_config->IsUseIntranet() &&
      !m_config->GetIntranetAddr().empty() && !is_generate_presigned_url) {
    dest_host = m_config->GetIntranetAddr();
  } else if (CosSysConfig::IsUseIntranet() &&
      !CosSysConfig::GetIntranetAddr().empty() && !is_generate_presigned_url) {
    dest_host = CosSysConfig::GetIntranetAddr();
  } else if (m_config &&
             (!m_config->GetDestDomain().empty())) {
    dest_host = m_config->GetDestDomain();
  } else if (!CosSysConfig::GetDestDomain().empty()) {
    dest_host = CosSysConfig::GetDestDomain();
  } else if (CosSysConfig::GetUseDnsCache() && !is_generate_presigned_url) {
    dest_host = GetGlobalDnsCacheInstance().Resolve(host);
  }

  dest_uri = dest_protocal + dest_host + CodecUtil::EncodeKey(dest_path);
  SDK_LOG_DBG("dest_uri: %s", dest_uri.c_str());
  return dest_uri;
}

uint64_t BaseOpUtil::GetMaxRetryTimes() const
{
  return m_config->GetMaxRetryTimes();
}

void BaseOpUtil::SleepBeforeRetry(const uint32_t& request_num) const {
    const uint32_t interval_ms = m_config->GetRetryIntervalMs() * (request_num + 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
}

std::string BaseOpUtil::ChangeHostSuffix(const std::string& host) {
    const std::string old_suffix = ".myqcloud.com";
    const std::string new_suffix = ".tencentcos.cn";

    const size_t suffix_pos = host.rfind(old_suffix);
    if (suffix_pos != std::string::npos) {
        std::string new_host = host.substr(0, suffix_pos) + new_suffix;
        return new_host;
    }
    return host;
}
} // namespace qcloud_cos