#ifndef COS_DEFINE_H
#define COS_DEFINE_H
#include <stdint.h>
#include <stdio.h>
#include <syslog.h>

#include <vector>
#include <string>

namespace qcloud_cos{

/// 路径分隔符
const std::string kPathDelimiter = "/";
/// 路径分隔符
const unsigned char kPathDelimiterChar = '/';

/// 分块上传时，失败的最大重试次数
const int kMaxRetryTimes = 3;

/// 默认线程池大小
const int kDefaultPoolSize = 2;

/// 分块上传的线程池默认大小
const int kDefaultThreadPoolSizeUploadPart = 5;
/// 分块上传的线程池最大数目
const int kMaxThreadPoolSizeUploadPart = 10;
/// 分块上传的线程池最小数目
const int kMinThreadPoolSizeUploadPart = 1;

/// 分块大小1M
const uint64_t kPartSize1M = 1 * 1024 * 1024;
/// 分块大小5G
const uint64_t kPartSize5G = (uint64_t)5 * 1024 * 1024 * 1024;

typedef enum log_out_type {
    COS_LOG_NULL = 0,
    COS_LOG_STDOUT,
    COS_LOG_SYSLOG
} LOG_OUT_TYPE;

typedef enum cos_log_level {
    COS_LOG_ERR  = 1,          // LOG_ERR
    COS_LOG_WARN = 2,          // LOG_WARNING
    COS_LOG_INFO = 3,          // LOG_INFO
    COS_LOG_DBG  = 4          // LOG_DEBUG
} LOG_LEVEL;

#define LOG_LEVEL_STRING(level) \
        ( (level == COS_LOG_DBG) ? "[DBG] " :    \
          (level == COS_LOG_INFO) ? "[INFO] " :  \
          (level == COS_LOG_WARN) ? "[WARN] " :  \
          (level == COS_LOG_ERR) ? "[ERR] " : "[CRIT]")

#define COS_LOW_LOGPRN(level, fmt, ...) \
    if (level <= CosSysConfig::GetLogLevel()) { \
        if (CosSysConfig::GetLogOutType()== COS_LOG_STDOUT) { \
           fprintf(stdout,"%s:%s(%d) " fmt "%s\n", LOG_LEVEL_STRING(level),__func__,__LINE__, __VA_ARGS__); \
        }else if (CosSysConfig::GetLogOutType() == COS_LOG_SYSLOG){ \
           syslog(LOG_INFO,"%s:%s(%d) " fmt "%s\n", LOG_LEVEL_STRING(level),__func__,__LINE__, __VA_ARGS__); \
        } else { \
        } \
    } else { \
    } \


#define SDK_LOG_DBG(fmt, ...)           COS_LOW_LOGPRN(COS_LOG_DBG,  fmt, ##__VA_ARGS__, "")
#define SDK_LOG_INFO(fmt, ...)          COS_LOW_LOGPRN(COS_LOG_INFO,  fmt, ##__VA_ARGS__, "")
#define SDK_LOG_WARN(fmt, ...)          COS_LOW_LOGPRN(COS_LOG_WARN,  fmt, ##__VA_ARGS__, "")
#define SDK_LOG_ERR(fmt, ...)           COS_LOW_LOGPRN(COS_LOG_ERR,  fmt, ##__VA_ARGS__, "")
#define SDK_LOG_COS(level, fmt, ...)    COS_LOW_LOGPRN(level,  fmt, ##__VA_ARGS__, "")

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct Content {
    std::string m_key; // Object 的 Key
    std::string m_last_modified; // Object 最后被修改时间
    std::string m_etag; // 文件的 MD-5 算法校验值
    std::string m_size; // 文件大小，单位是 Byte
    std::vector<std::string> m_owner_ids; // Bucket 持有者信息
    std::string m_storage_class; // Object 的存储级别，枚举值：STANDARD，STANDARD_IA，NEARLINE
};

struct ReplicationRule {
    bool m_is_enable;
    std::string m_id; // 非必须
    std::string m_prefix;
    std::string m_dest_bucket;
    std::string m_dest_storage_class; // 非必须

    ReplicationRule() {}
    ReplicationRule(const std::string& prefix,
                    const std::string& dest_bucket,
                    const std::string& storage_class = "",
                    const std::string& id = "",
                    bool is_enable = true) {
        m_is_enable = is_enable;
        m_id = id;
        m_prefix = prefix;
        m_dest_bucket = dest_bucket;
        m_dest_storage_class = storage_class;
    }
};

struct LifecycleTag {
    std::string key;
    std::string value;
};

struct LifecycleFilter {
    std::string prefix;
    std::vector<LifecycleTag> tags;
};

struct LifecycleTransition {
    uint64_t days;
    std::string date;
    std::string storage_class;
};

struct LifecycleExpiration {
    uint64_t days;
    std::string date;
    bool expired_obj_del_marker;
};

struct LifecycleNonCurrentTransition {
    uint64_t days;
    std::string storage_class;
};

struct LifecycleNonCurrentExpiration {
    uint64_t days;
};

struct LifecycleRule {
    bool is_enable;
    std::string id;
    std::string filter;
    LifecycleTransition transition;
    LifecycleExpiration expiration;
    LifecycleTransition non_curr_transition;
    LifecycleExpiration non_curr_expiration;

};

} // namespace qcloud_cos
#endif
