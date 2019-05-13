#ifndef COS_DEFINE_H
#define COS_DEFINE_H
#include <stdint.h>
#include <stdio.h>

#if !defined(_WIN32)
#include <syslog.h>
#endif

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
const int kMaxThreadPoolSizeUploadPart = 100;
/// 分块上传的线程池最小数目
const int kMinThreadPoolSizeUploadPart = 1;

const int kMaxPartNumbers = 10000;

/// 分块大小1M
const uint64_t kPartSize1M = 1 * 1024 * 1024;
/// 分块大小5G
const uint64_t kPartSize5G = (uint64_t)5 * 1024 * 1024 * 1024;

typedef enum log_out_type {
    COS_LOG_NULL = 0,
    COS_LOG_STDOUT,
    COS_LOG_SYSLOG
} LOG_OUT_TYPE;

typedef enum {
    HTTP_HEAD,
    HTTP_GET,
    HTTP_PUT,
    HTTP_POST,
    HTTP_DELETE,
    HTTP_OPTIONS
} HTTP_METHOD;

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


#if defined(_WIN32)
#define COS_LOW_LOGPRN(level, fmt, ...) \
    if (level <= CosSysConfig::GetLogLevel()) { \
        if (CosSysConfig::GetLogOutType()== COS_LOG_STDOUT) { \
			fprintf(stdout,"%s:%s(%d) " fmt "\n", LOG_LEVEL_STRING(level),__func__,__LINE__, ##__VA_ARGS__); \
        }else if (CosSysConfig::GetLogOutType() == COS_LOG_SYSLOG){ \
        } else { \
        } \
    } else { \
    }
#else
#define COS_LOW_LOGPRN(level, fmt, ...) \
    if (level <= CosSysConfig::GetLogLevel()) { \
        if (CosSysConfig::GetLogOutType()== COS_LOG_STDOUT) { \
           fprintf(stdout,"%s:%s(%d) " fmt "\n", LOG_LEVEL_STRING(level),__func__,__LINE__, ##__VA_ARGS__); \
        }else if (CosSysConfig::GetLogOutType() == COS_LOG_SYSLOG){ \
           syslog(LOG_INFO,"%s:%s(%d) " fmt "\n", LOG_LEVEL_STRING(level),__func__,__LINE__, ##__VA_ARGS__); \
        } else { \
        } \
    } else { \
    }
#endif



// For now just support the std output log for windows
#define SDK_LOG_DBG(fmt, ...)           COS_LOW_LOGPRN(COS_LOG_DBG,  fmt, ##__VA_ARGS__)
#define SDK_LOG_INFO(fmt, ...)          COS_LOW_LOGPRN(COS_LOG_INFO,  fmt, ##__VA_ARGS__)
#define SDK_LOG_WARN(fmt, ...)          COS_LOW_LOGPRN(COS_LOG_WARN,  fmt, ##__VA_ARGS__)
#define SDK_LOG_ERR(fmt, ...)           COS_LOW_LOGPRN(COS_LOG_ERR,  fmt, ##__VA_ARGS__)
#define SDK_LOG_COS(level, fmt, ...)    COS_LOW_LOGPRN(level,  fmt, ##__VA_ARGS__)

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct Content {
    std::string m_key; // Object 的 Key
    std::string m_last_modified; // Object 最后被修改时间
    std::string m_etag; // 文件的 MD-5 算法校验值
    std::string m_size; // 文件大小，单位是 Byte
    std::vector<std::string> m_owner_ids; // Bucket 持有者信息
    std::string m_storage_class; // Object 的存储级别，枚举值：STANDARD，STANDARD_IA
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

class LifecycleFilter {
public:
    LifecycleFilter() : m_mask(0x00000000u), m_prefix("") {}

    std::string GetPrefix() const {
        return m_prefix;
    }

    std::vector<LifecycleTag> GetTags() const {
        return m_tags;
    }

    void SetPrefix(const std::string& prefix) {
        m_mask |= 0x00000001u;
        m_prefix = prefix;
    }

    void SetTags(const std::vector<LifecycleTag>& tags) {
        m_mask |= 0x00000002u;
        m_tags = tags;
    }

    void AddTag(const LifecycleTag& tag) {
        m_mask |= 0x00000002u;
        m_tags.push_back(tag);
    }

    bool HasPrefix() const {
        return (m_mask & 0x00000001u) != 0;
    }

    bool HasTags() const {
        return (m_mask & 0x00000002u) != 0;
    }

private:
    uint64_t m_mask;
    std::string m_prefix;
    std::vector<LifecycleTag> m_tags;
};

class LifecycleTransition {
public:
    LifecycleTransition()
        : m_mask(0x00000000u), m_days(0), m_date(""), m_storage_class("") {
    }

    uint64_t GetDays() const {
        return m_days;
    }

    std::string GetDate() const {
        return m_date;
    }

    std::string GetStorageClass() const {
        return m_storage_class;
    }

    void SetDays(uint64_t days) {
        m_mask |= 0x00000001u;
        m_days = days;
    }

    void SetDate(const std::string& date) {
        m_mask |= 0x00000002u;
        m_date = date;
    }

    void SetStorageClass(const std::string& storage_class) {
        m_mask |= 0x00000004u;
        m_storage_class = storage_class;
    }

    bool HasDays() const {
        return (m_mask & 0x00000001u) != 0;
    }

    bool HasDate() const {
        return (m_mask & 0x00000002u) != 0;
    }

    bool HasStorageClass() const {
        return (m_mask & 0x00000004u) != 0;
    }

private:
    uint64_t m_mask;
    // 不能在同一规则中同时使用Days和Date
    uint64_t m_days; // 有效值是非负整数
    std::string m_date;
    std::string m_storage_class;
};

class LifecycleExpiration {
public:
    LifecycleExpiration()
        : m_mask(0x00000000u), m_days(0), m_date(""), m_expired_obj_del_marker(false) {
    }

    uint64_t GetDays() const {
        return m_days;
    }

    std::string GetDate() const {
        return m_date;
    }

    bool IsExpiredObjDelMarker() const {
        return m_expired_obj_del_marker;
    }

    void SetDays(uint64_t days) {
        m_mask |= 0x00000001u;
        m_days = days;
    }

    void SetDate(const std::string& date) {
        m_mask |= 0x00000002u;
        m_date = date;
    }

    void SetExpiredObjDelMarker(bool marker) {
        m_mask |= 0x00000004u;
        m_expired_obj_del_marker = marker;
    }

    bool HasDays() const {
        return (m_mask & 0x00000001u) != 0;
    }

    bool HasDate() const {
        return (m_mask & 0x00000002u) != 0;
    }

    bool HasExpiredObjDelMarker() const {
        return (m_mask & 0x00000004u) != 0;
    }

private:
    uint64_t m_mask;
    // 不能在同一规则中同时使用Days和Date
    uint64_t m_days; // 有效值为正整数
    std::string m_date;
    bool m_expired_obj_del_marker;
};

class LifecycleNonCurrTransition {
public:
    LifecycleNonCurrTransition()
        : m_mask(0x00000000u), m_days(0), m_storage_class("") {
    }

    uint64_t GetDays() const {
        return m_days;
    }

    std::string GetStorageClass() const {
        return m_storage_class;
    }

    void SetDays(uint64_t days) {
        m_mask |= 0x00000001u;
        m_days = days;
    }

    void SetStorageClass(const std::string& storage_class) {
        m_mask |= 0x00000002u;
        m_storage_class = storage_class;
    }

    bool HasDays() const {
        return (m_mask & 0x00000001u) != 0;
    }

    bool HasStorageClass() const {
        return (m_mask & 0x00000002u) != 0;
    }

private:
    uint64_t m_mask;
    uint64_t m_days;
    std::string m_storage_class;
};

class LifecycleNonCurrExpiration {
public:
    LifecycleNonCurrExpiration()
        : m_mask(0x00000000u), m_days(0) {
    }

    uint64_t GetDays() const {
        return m_days;
    }

    void SetDays(uint64_t days) {
        m_mask |= 0x00000001u;
        m_days = days;
    }

    bool HasDays() const {
        return (m_mask & 0x00000001u) != 0;
    }

private:
    uint64_t m_mask;
    uint64_t m_days;
};

struct AbortIncompleteMultipartUpload {
    uint64_t m_days_after_init;
};

class LifecycleRule {
public:
    LifecycleRule()
        : m_mask(0x00000000u), m_is_enable(false), m_id("") {
    }

    void SetIsEnable(bool is_enable) {
        m_mask |= 0x00000001u;
        m_is_enable = is_enable;
    }

    void SetId(const std::string& id) {
        m_mask |= 0x00000002u;
        m_id = id;
    }

    void SetFilter(const LifecycleFilter& filter) {
        m_mask |= 0x00000004u;
        m_filter = filter;
    }

    void AddTransition(const LifecycleTransition& rh) {
        m_mask |= 0x00000008u;
        m_transitions.push_back(rh);
    }

    void SetExpiration(const LifecycleExpiration& rh) {
        m_mask |= 0x00000010u;
        m_expiration = rh;
    }

    void SetNonCurrTransition(const LifecycleNonCurrTransition& rh) {
        m_mask |= 0x00000020u;
        m_non_curr_transition = rh;
    }

    void SetNonCurrExpiration(const LifecycleNonCurrExpiration& rh) {
        m_mask |= 0x00000040u;
        m_non_curr_expiration = rh;
    }

    void SetAbortIncompleteMultiUpload(const AbortIncompleteMultipartUpload& rh) {
        m_mask |= 0x00000080u;
        m_abort_multi_upload = rh;
    }

    bool IsEnable() const {
        return m_is_enable;
    }

    std::string GetId() const {
        return m_id;
    }

    LifecycleFilter GetFilter() const {
        return m_filter;
    }

    std::vector<LifecycleTransition> GetTransitions() const {
        return m_transitions;
    }

    LifecycleExpiration GetExpiration() const {
        return m_expiration;
    }

    LifecycleNonCurrTransition GetNonCurrTransition() const {
        return m_non_curr_transition;
    }

    LifecycleNonCurrExpiration GetNonCurrExpiration() const {
        return m_non_curr_expiration;
    }

    AbortIncompleteMultipartUpload GetAbortIncompleteMultiUpload() const {
        return m_abort_multi_upload;
    }

    bool HasIsEnable() const {
        return (m_mask & 0x00000001u) != 0;
    }

    bool HasId() const {
        return (m_mask & 0x00000002u) != 0;
    }

    bool HasFilter() const {
        return (m_mask & 0x00000004u) != 0;
    }

    bool HasTransition() const {
        return (m_mask & 0x00000008u) != 0;
    }

    bool HasExpiration() const {
        return (m_mask & 0x00000010u) != 0;
    }

    bool HasNonCurrTransition() const {
        return (m_mask & 0x00000020u) != 0;
    }

    bool HasNonCurrExpiration() const {
        return (m_mask & 0x00000040u) != 0;
    }

    bool HasAbortIncomMultiUpload() const {
        return (m_mask & 0x00000080u) != 0;
    }

private:
    uint64_t m_mask;
    bool m_is_enable;
    std::string m_id;
    LifecycleFilter m_filter;
    std::vector<LifecycleTransition> m_transitions;
    LifecycleExpiration m_expiration;
    LifecycleNonCurrTransition m_non_curr_transition;
    LifecycleNonCurrExpiration m_non_curr_expiration;
    AbortIncompleteMultipartUpload m_abort_multi_upload;
};

struct Grantee {
    std::string m_type;
    std::string m_id;
    std::string m_display_name;
    std::string m_uri;
};

struct Grant {
    Grantee m_grantee;
    std::string m_perm;
};

struct CORSRule {
    std::string m_id;
    std::string m_max_age_secs;
    std::vector<std::string> m_allowed_headers;
    std::vector<std::string> m_allowed_methods;
    std::vector<std::string> m_allowed_origins;
    std::vector<std::string> m_expose_headers;
};

struct Initiator {
    std::string m_id;
    std::string m_display_name;
};

struct Owner {
    std::string m_id;
    std::string m_display_name;
};

// The result of the listmultiparts
struct Upload {
    std::string m_key;
    std::string m_uploadid;
    std::string m_storage_class;
    std::vector<Owner> m_initator;
    std::vector<Owner> m_owner;
    std::string m_initiated;
};

struct Part {
    uint64_t m_part_num;
    uint64_t m_size;
    std::string m_etag;
    std::string m_last_modified;
};

// 描述单个 Bucket 的信息
struct Bucket {
    std::string m_name; // Bucket 名称
    std::string m_location; // Bucket 所在地域
    std::string m_create_date; // Bucket 创建时间。ISO8601 格式，例如 2016-11-09T08:46:32.000Z
};

struct ObjectVersionPair {
    std::string m_object_name;
    std::string m_version_id;

    ObjectVersionPair() {
        m_object_name = "";
        m_version_id = "";
    }

    ObjectVersionPair(const std::string& obj_name,
                      const std::string& version_id) {
        m_object_name = obj_name;
        m_version_id = version_id;
    }
};

struct DeletedInfo {
    DeletedInfo() :
        m_key(""), m_version_id(""), m_delete_marker(false),
        m_delete_marker_version_id("") {
    }

    std::string m_key;
    std::string m_version_id;
    bool m_delete_marker;
    std::string m_delete_marker_version_id;
};

struct ErrorInfo {
    ErrorInfo() :
        m_key(""), m_code(""), m_message(""),
        m_version_id("") {
    }

    std::string m_key;
    std::string m_code;
    std::string m_message;
    std::string m_version_id;
};

struct COSVersionSummary {
    bool m_is_delete_marker;
    std::string m_etag;
    uint64_t m_size;
    std::string m_storage_class;
    bool m_is_latest;
    std::string m_key;
    std::string m_last_modified;
    Owner m_owner;
    std::string m_version_id;
};

} // namespace qcloud_cos
#endif
