#ifndef COS_DEFINE_H
#define COS_DEFINE_H
#include <stdint.h>
#include <stdio.h>

#include <vector>
#include <string>

#include "util/log_util.h"

namespace qcloud_cos{

#define COS_CPP_SDK_VERSON "v5.5.0"

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

/// 最大分开数量
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

typedef enum file_type {
    CSV = 0,
    JSON
} SELECT_FILE_TYPE;

typedef enum compress_type {
    COMPRESS_NONE = 0,
    COMPRESS_GZIP,
    COMPRESS_BZIP2
} SELECT_COMPRESS_TYPE;

#define LOG_LEVEL_STRING(level) \
        ( (level == COS_LOG_DBG) ? "[DBG] " :    \
          (level == COS_LOG_INFO) ? "[INFO] " :  \
          (level == COS_LOG_WARN) ? "[WARN] " :  \
          (level == COS_LOG_ERR) ? "[ERR] " : "[CRIT]")

#define COS_LOW_LOGPRN(level, fmt, ...) \
    if (level <= CosSysConfig::GetLogLevel()) { \
        if (CosSysConfig::GetLogOutType()== COS_LOG_STDOUT) { \
           fprintf(stdout,"%s:%s(%d) " fmt "\n", LOG_LEVEL_STRING(level),__func__,__LINE__, ##__VA_ARGS__); \
        } else if (CosSysConfig::GetLogOutType() == COS_LOG_SYSLOG){ \
           LogUtil::Syslog(level,"%s:%s(%d) " fmt "\n", LOG_LEVEL_STRING(level),__func__,__LINE__, ##__VA_ARGS__); \
        } else { \
        } \
    } else { \
    } \
    { \
        auto log_callback = CosSysConfig::GetLogCallback(); \
        if (log_callback) { \
            std::string logstr = LogUtil::FormatLog(level, "%s:%s(%d) " fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
	        log_callback(logstr); \
        } \
    } \

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

class LoggingEnabled {
public:
    LoggingEnabled():
        m_mask(0x00000000u),m_targetbucket(""), m_targetprefix("") {
    }

    LoggingEnabled& operator=(const LoggingEnabled& obj) {
        if(this == &obj) {
	   return *this;
        } else {
           m_mask = obj.GetMask();
           m_targetbucket = obj.GetTargetBucket(); 
           m_targetprefix = obj.GetTargetPrefix();
           return *this;
        }
    }	

    void SetTargetBucket(const std::string &targetbucket) {
        m_mask = m_mask | 0x00000001u;
        m_targetbucket = targetbucket;
    }

    void SetTargetPrefix(const std::string &targetprefix) {
        m_mask = m_mask | 0x00000002u;
        m_targetprefix = targetprefix;
    }

    uint64_t GetMask() const {
        return m_mask;
    }

    std::string GetTargetBucket() const {
        return m_targetbucket;
    }

    std::string GetTargetPrefix() const {
	return m_targetprefix;
    }
	
    bool HasTargetBucket() const {
	return (m_mask & 0x00000001u) != 0;
    }
	
    bool HasTargetPrefix() const {
	return (m_mask & 0x00000002u) != 0;	
    } 
 
    virtual ~LoggingEnabled() {}

private:
    uint64_t m_mask;
    std::string m_targetbucket;
    std::string m_targetprefix;
};

class DomainRule {
public:
    DomainRule(): m_mask(0x00000000u),  m_status(""), m_name(""), m_type(""),
       m_forcedreplacement("") {}
    
    DomainRule& operator=(const DomainRule& obj) {
        if(this == &obj) {
	   return *this;
        } else {
	   m_name = obj.GetName();
	   m_type = obj.GetType();
	   m_status = obj.GetStatus();
	   m_mask = obj.GetMask();
	   m_forcedreplacement = obj.GetForcedReplacement();
	   return *this;
        }
    }

    virtual ~DomainRule() {}
	
    void SetStatus(const std::string &status) {
	m_mask = m_mask | 0x00000001u;
        m_status = status;
    }

    void SetName(const std::string &name) {
	m_mask = m_mask | 0x00000002u;
        m_name = name;
    }

    void SetType(const std::string &type) {
	m_mask = m_mask | 0x00000004u;
        m_type = type;
    }

    void SetForcedReplacement(const std::string &forcedreplacement) {
	m_mask = m_mask | 0x00000008u;
        m_forcedreplacement = forcedreplacement;
    } 

    std::string GetStatus() const {
        return m_status;
    }

    std::string GetName() const {
        return m_name;
    }

    std::string GetType() const {
        return m_type;
    }

    std::string GetForcedReplacement() const {
        return m_forcedreplacement;
    } 
	
    uint64_t GetMask() const {
	return m_mask;
    }
	
    bool HasStatus() const {
	return (m_mask & 0x00000001u) != 0; 
    }
	
    bool HasName() const {
	return (m_mask & 0x00000002u) != 0;
    }
	
    bool HasType() const {
	return (m_mask & 0x00000004u) != 0;
    }
	
    bool HasForcedrePlacement() const {
	return (m_mask & 0x00000008u) != 0;
    }
    
private:
    uint64_t m_mask;
    std::string m_status;
    std::string m_name;
    std::string m_type;
    std::string m_forcedreplacement;
};


class DomainErrorMsg {
public:
    DomainErrorMsg(): m_code(""), m_message(""), m_resource(""),
      m_traceid("") {}
    
    virtual ~DomainErrorMsg() {}
	
    void SetCode(const std::string &code) {
        m_code = code;
    }
    
    void SetMessage(const std::string &message) {
        m_message = message;
    }

    void SetResource(const std::string &resource) {
        m_resource = resource;
    }
   
    void SetRequestid(const std::string &requestid) {
        m_requestid = requestid;
    }

    void SetTraceid(const std::string &traceid) {
        m_traceid = traceid;
    }

    std::string GetCode() const {
        return m_code;
    }

    std::string GetMessage() const {
        return m_message;
    }

    std::string GetResource() const {
        return m_resource;
    }
   
    std::string GetRequestid() const {
        return m_requestid;
    }
    
    std::string GetTraceid() const {
        return m_traceid;
    } 
    
private:
    std::string m_code;
    std::string m_message;
    std::string m_resource;
    std::string m_requestid;
    std::string m_traceid;
};

class Condition {
public:
    Condition(): m_mask(0x00000000u), m_httperrorcodereturnedequals(404),
		 m_keyprefixequals("") {}	
		
    Condition& operator=(const Condition& obj) {
	if(this == &obj) {
            return *this;
	} else {
	    m_mask = obj.GetMask();
	    m_keyprefixequals = obj.GetKeyPrefixEquals();
	    m_httperrorcodereturnedequals = obj.GetHttpErrorCodeReturnedEquals();
	    return *this;
	}
    }
	 
    void SetHttpErrorCodeReturnedEquals(const int &httpcode) {
	m_mask = m_mask | 0x00000001u;
	m_httperrorcodereturnedequals = httpcode;
    }
	
    void SetKeyPrefixEquals(const std::string& keyprefixequals) {
	m_mask = m_mask | 0x00000002u;
	m_keyprefixequals = keyprefixequals;
    }
	
    bool HasHttpErrorCodeReturnedEquals() const {
	return (m_mask&0x00000001u) != 0;
    }
	
    bool HasKeyPrefixEquals() const {
	return (m_mask&0x00000002u) != 0;
    }
	
    uint64_t GetMask() const {
	return m_mask;	
    }
	
    int GetHttpErrorCodeReturnedEquals() const {
	return m_httperrorcodereturnedequals;
    }
	
    std::string GetKeyPrefixEquals() const {
	return m_keyprefixequals;
    }

    virtual ~Condition() {}
	
private:
    uint64_t m_mask;
    int m_httperrorcodereturnedequals;
    std::string m_keyprefixequals;
};

class Redirect {
public:
    Redirect(): m_mask(0x00000000u),  m_protocol(""), 
  	m_replacekeywith(""), m_replacekeyprefixwith("") {}
		
    Redirect& operator=(const Redirect& obj) {
        if(this == &obj) {
    	   return *this;
        } else {
	   m_mask = obj.GetMask();		
           m_protocol = obj.GetProtocol();
	   m_replacekeywith = obj.GetReplaceKeyWith();
	   m_replacekeyprefixwith = obj.GetReplaceKeyPrefixWith();
	   return *this;
        } 
    }
	
    void SetProtocol(const std::string protocol) {
      	m_mask = m_mask | 0x00000001u;
	m_protocol = protocol;
    }
	
    void SetReplaceKeyWith(const std::string replacekeywith) {
    	m_mask = m_mask | 0x00000002u;
	m_replacekeywith = replacekeywith;
    }
	
    void SetReplaceKeyPrefixWith(const std::string replacekeyprefixwith) {
  	m_mask = m_mask | 0x00000004u;
	m_replacekeyprefixwith = replacekeyprefixwith;
    }	
	
    uint64_t GetMask() const {
	return m_mask;
    }
	
    std::string GetProtocol() const {
	return m_protocol;
    }
	
    std::string GetReplaceKeyWith() const {
	return m_replacekeywith;
    }
	
    std::string GetReplaceKeyPrefixWith() const {	
	return m_replacekeyprefixwith;
    }
	
    bool HasProtocol() const {
	return (m_mask & 0x00000001u) != 0;
    }
	
    bool HasReplaceKeyWith() const {
	return (m_mask & 0x00000002u) != 0;
    }
	
    bool HasReplaceKeyPrefixWith() const {
	return (m_mask & 0x00000004u) != 0;
    }
	
    virtual ~Redirect() {}

private:
    uint64_t m_mask;
    std::string m_protocol;
    std::string m_replacekeywith;
    std::string m_replacekeyprefixwith;
};

class RoutingRule {
public:
    RoutingRule() : m_mask(0x00000000u), m_condition(),m_redirect() {}
	
    RoutingRule(const RoutingRule& obj) {
        m_mask = obj.m_mask;
	m_condition = obj.GetCondition();
	m_redirect = obj.GetRedirect();
    } 
	
    RoutingRule& operator=(const RoutingRule& obj) {
        if(this == &obj) {
	    return *this;
	} else {
	    m_mask = obj.GetMask();
	    m_condition = obj.GetCondition();
	    m_redirect = obj.GetRedirect();
	    return *this;
	}
    }		
		
    void SetCondition(const Condition& condition) {
 	m_mask = m_mask | 0x00000001u;
	m_condition = condition;
    }
	
    void SetRedirect(const Redirect& redirect) {
	m_mask = m_mask | 0x00000002u;
	m_redirect = redirect;
    } 

    uint64_t GetMask() const {
	return m_mask;	
    }

    const Condition& GetCondition() const {
	return m_condition;
    }
	
    const Redirect& GetRedirect() const {
	return m_redirect;
    }
	
    bool HasCondition() const {
	return (m_mask & 0x00000001u) != 0;	
    }
	
    bool HasRedirect() const {
	return (m_mask & 0x00000002u) != 0; 
    }
	
    virtual ~RoutingRule() {}

private:
    uint64_t m_mask;
    Condition m_condition;
    Redirect m_redirect;
};

class Tag {
public:
    Tag() : m_mask(0x00000000u), m_key(""), m_value("") {}
	
    void SetKey(const std::string key) {
	m_mask = m_mask | 0x00000001u;
	m_key = key;
    }
	
    void SetValue(const std::string value) {
	m_mask = m_mask | 0x00000002u;
	m_value = value;
    }
	
    uint64_t GetMask() const {
	return m_mask;
    }
	
    std::string GetKey() const {
	return m_key;
    } 
	
    std::string GetValue() const {
	return m_value;
    } 
	
    bool HasKey() const {
	return (m_mask & 0x00000001u) != 0;	
    }
	
    bool HasValue() const {
	return (m_mask & 0x00000002u) != 0;
    }
			
private:
    uint64_t m_mask;
    std::string m_key;
    std::string m_value;
};

class COSBucketDestination {
public:
    COSBucketDestination(): m_mask(0x00000000u), m_format(""),
	m_accountId(""), m_bucket(""), m_prefix(""), m_encryption(false) {}
	
    void SetFormat(const std::string& format) {
	m_mask = m_mask | 0x00000001u;
	m_format = format;
    }
	
    void SetAccountId(const std::string& accountId) {
	m_mask = m_mask | 0x00000002u;
	m_accountId = accountId;
    }
	
    void SetBucket(const std::string& bucket) {
	m_mask = m_mask | 0x00000004u;
	m_bucket = bucket;
    }

    void SetPrefix(const std::string& prefix) {
	m_mask = m_mask | 0x00000008u;
	m_prefix = prefix;
    }
	
    void SetEncryption(const bool encryption) {
	m_mask = m_mask | 0x000000010u;
	m_encryption = encryption;
    }
	
    uint64_t GetMask() const {
	return m_mask;
    }
	
    std::string GetFormat() const {
	return m_format;
    }
	
    std::string GetAccountId() const {
	return m_accountId;
    }
	
    std::string GetBucket() const {
	return m_bucket;
    }
	
    std::string GetPrefix() const {
	return m_prefix;
    }
	
    bool GetEncryption() const {
	return m_encryption;
    }
	
    bool HasFormat() const {
	return (m_mask & 0x00000001u) != 0;	
    }
	
    bool HasAccountId() const {
	return (m_mask & 0x00000002u) != 0;
    }
	
    bool HasBucket() const {
	return (m_mask & 0x00000004u) != 0;
    }
	
    bool HasPrefix() const {
	return (m_mask & 0x00000008u) != 0;
    }
	
    bool HasEncryption() const {
	return (m_mask & 0x00000010u) != 0;
    }

private:
    uint64_t m_mask;
    std::string m_format;
    std::string m_accountId;
    std::string m_bucket;
    std::string m_prefix;
    bool m_encryption;	
};

class OptionalFields {
public:
    OptionalFields(): m_mask(0x00000000u), m_is_size(false), m_is_etag(false),
	m_is_last_modified(false), m_is_storage_class(false), 
        m_is_replication_status(false), m_is_multipart_uploaded(false) {}
	
    void SetIsSize(const bool size) {
	m_mask = m_mask | 0x00000001u;
	m_is_size = size;
    }
	
    void SetIsLastModified(const bool last_modified) {
	m_mask = m_mask | 0x00000002u;
	m_is_last_modified = last_modified;
    }
	
    void SetIsStorageClass(const bool storage_class) {
	m_mask = m_mask | 0x00000004u;
	m_is_storage_class = storage_class;
    }
	
    void SetIsMultipartUploaded(const bool ismultipart_uploaded) {
	m_mask = m_mask | 0x00000008u;
	m_is_multipart_uploaded = ismultipart_uploaded;
    }
	
    void SetIsReplicationStatus(const bool replication_status) {
	m_mask = m_mask | 0x000000010u;
	m_is_replication_status = replication_status;
    }
	
    void SetIsEtag(const bool is_etag) {
	m_mask = m_mask | 0x000000020u;
	m_is_etag = is_etag;
    }
	
    uint64_t GetMask() const {
	return m_mask;
    }
	
    bool GetIsSize() const {
	return m_is_size;
    }
	
    bool GetIsLastModified() const {
	return m_is_last_modified;
    }
	
    bool GetIsStorageClass() const {
	return m_is_storage_class;
    }
		
    bool GetIsReplicationStatus() const {
	return m_is_replication_status;
    }
	
    bool GetIsMultipartUploaded() const {
	return m_is_multipart_uploaded;
    }
	
    bool GetIsETag() const {
	return m_is_etag;
    }
	
    bool HasIsSize() const {
	return (m_mask & 0x00000001u) != 0;	
    }
	
    bool HasIsLastModified() const {
	return (m_mask & 0x00000002u) != 0;
    }
	
    bool HasIsStorageClass() const {
	return (m_mask & 0x00000004u) != 0;
    }
	
    bool HasIsReplicationStatus() const {
	return (m_mask & 0x00000008u) != 0;
    }
	
    bool HasIsMultipartUploaded() const {
	return (m_mask & 0x00000010u) != 0;
    }
	
    bool HasIsETag() const {
	return (m_mask & 0x00000020u) != 0;	
    }

private:
    uint64_t m_mask;
    bool m_is_size;
    bool m_is_etag;
    bool m_is_last_modified;
    bool m_is_storage_class;
    bool m_is_replication_status;
    bool m_is_multipart_uploaded;
};

class Inventory {
public:
    Inventory(): m_mask(0x00000000u), m_id(""), m_is_enabled(true),
	m_included_objectversions(""), m_filter(""), m_frequency("") {}
	
    Inventory& operator=(const Inventory& obj) {
        if(this == &obj) {
	   return *this;
	} else {
	   m_mask = obj.GetMask();
	   m_id = obj.GetId();
	   m_included_objectversions = obj.GetIncludedObjectVersions();
	   m_filter = obj.GetFilter();
	   m_is_enabled = obj.GetIsEnable();
	   m_destination = obj.GetCOSBucketDestination();
	   m_fields = obj.GetOptionalFields();	
	   m_frequency = obj.GetFrequency();
	   return *this;
	}	
    }
    void SetId(const std::string& id) {
	m_mask = m_mask | 0x00000001u;
        m_id = id;
    }
    void SetIsEnable(bool is_enabled) {
	m_mask = m_mask | 0x00000002u;
	m_is_enabled = is_enabled;
    }
	
    void SetIncludedObjectVersions(const std::string& included_objectversions) {
	m_mask = m_mask | 0x00000004u;
	m_included_objectversions = included_objectversions;
    }
	
    void SetFilter(const std::string& filter) {
        m_mask = m_mask | 0x00000008u;
        m_filter = filter;
    }
	
    void SetCOSBucketDestination(const COSBucketDestination& destination) {
        m_mask = m_mask | 0x00000010u;
        m_destination = destination;
    }
	
    void SetOptionalFields(const OptionalFields& fields) {
	m_mask = m_mask | 0x00000020u;
	m_fields = fields;	
    }
	
    void SetFrequency(const std::string& frequency) {
	if(frequency == "Daily" || frequency == "Weekly") {
   	    m_mask = m_mask | 0x00000040u;
	    m_frequency	= frequency;
	} else {
	    m_frequency = "";
	}
    }
	
    uint64_t GetMask() const {
	return m_mask;
    }
	
    std::string GetId() const {
	return m_id;
    }
	
    bool GetIsEnable() const {
	return m_is_enabled;
    }
	
    std::string GetIncludedObjectVersions() const {
	return m_included_objectversions;
    }
	
    std::string GetFilter() const {
	return m_filter;	
    } 
	
    const COSBucketDestination& GetCOSBucketDestination() const {
	return m_destination;
    }
	
    const OptionalFields& GetOptionalFields() const {
	return m_fields;
    }
	
    std::string GetFrequency() const {
	return m_frequency;	
    }
	
    bool HasId() const {
	return (m_mask & 0x00000001u) != 0;
    }
	
    bool HasIsEnable() const {
	return (m_mask & 0x00000002u) != 0;
    }
	
    bool HasIncludedObjectVersions() const {
	return (m_mask & 0x00000004u) != 0;
    }
	
    bool HasFilter() const {
	return (m_mask & 0x00000008u) != 0;
    }
	
    bool HasCOSBucketDestination() const {
	return (m_mask & 0x000000010u) != 0;
    }
	
    bool HasOptionalFields() const {
	return (m_mask & 0x00000020u) != 0;
    }	
	
    bool HasFrequency() const {
	return (m_mask & 0x00000040u) != 0;
    }
	
private:
    uint64_t m_mask;
    std::string m_id;
    std::string m_included_objectversions;
    std::string m_filter;
    std::string m_frequency;
    bool m_is_enabled;
    COSBucketDestination m_destination;
    OptionalFields m_fields;
};

struct SelectMessage {
    std::string m_event_type;
    std::string m_content_type;
    //bool m_has_payload;
    std::string payload;
    //uint32_t m_payload_offset;  // offset in body
    //uint32_t m_payload_len;  // length
};

class LiveChannelConfiguration {
  public:
    LiveChannelConfiguration() {}
    LiveChannelConfiguration(const std::string& desc, const std::string& switch_info,
                            const std::string& type, int frag_duartion,
                            int frag_count, const std::string& playlist_name) :
        m_desc(desc)
        , m_switch(switch_info)
        , m_type(type)
        , m_frag_duartion(frag_duartion)
        , m_frag_count(frag_count)
        , m_playlist_name(playlist_name) {}

    ~LiveChannelConfiguration() {}

    void SetDescription(const std::string& desc) {
        m_desc = desc;
    }

    std::string GetDescription() const {
        return m_desc;
    }

    /// brief: "Enabled" or "Disabled"
    void SetSwitch(const std::string& switch_info) {
        m_switch = switch_info;
    }

    std::string GetSwitch() const {
        return m_switch;
    }

    /// brief: only support "HLS" for now
    void SetType(const std::string& type) {
        m_type = type;
    }

    std::string GetType() const {
        return m_type;
    }

    void SetFragDuration(const int frag_duartion) {
        m_frag_duartion = frag_duartion;
    }

    int GetFragDuration() const {
        return m_frag_duartion;
    }

    void SetFragCount(const int frag_count) {
        m_frag_count = frag_count;
    }

    int GetFragCount() const {
        return m_frag_count;
    }

    void SetPlaylistName(const std::string& playlist_name) {
        m_playlist_name = playlist_name;
    }

    std::string GetPlaylistName() const {
        return m_playlist_name;
    }

    void SetPublishUrl(const std::string& url) {
        m_publish_url = url;
    }

    std::string GetPublishUrl() const {
        return m_publish_url;
    }

    void SetPlayUrl(const std::string& url) {
        m_play_url = url;
    }

    std::string GetPlayUrl() const {
        return m_play_url;
    }

  private:
    std::string m_desc;  // 通道描述信息
    std::string m_switch;  // "Enabled" or "Disabled"
    std::string m_type;  // 目前仅支持"HLS"
    int m_frag_duartion;  // [1, 100]
    int m_frag_count;  // [1, 100]
    std::string m_playlist_name;
    std::string m_publish_url;
    std::string m_play_url;
};


/// @brief 直播通道推流历史记录
struct LiveRecord {
    std::string m_start_time;  // 推流开始时间
    std::string m_end_time;  // 推流结束时间
    std::string m_remote_addr;  // 客户端ip地址和端口
    std::string m_request_id;  // 请求ID
};

/// @brief 直播通道推流的视频信息
struct LiveChannelVideoInfo {
    std::string m_width;
    std::string m_heigh;
    std::string m_framerate;
    std::string m_bandwidth;
    std::string m_codec;
};

/// @brief 直播通道推流的音频信息
struct LiveChannelAudioInfo {
    std::string m_bandwidth;
    std::string m_samplerate;
    std::string m_codec;
};

/// @brief 直播通道推流的状态
struct LiveChannelStatus {
    LiveChannelStatus() : m_has_video(false), m_has_audio(false) {}
    std::string m_status;  // 流当前状态
    std::string m_connected_time;  // 推流开始时间
    std::string m_remote_addr;  // 客户端ip地址和端口
    std::string m_request_id;  // 请求ID
    bool m_has_video;
    LiveChannelVideoInfo m_video;
    bool m_has_audio;
    LiveChannelAudioInfo m_audio;
};

/// @brief 列举得到的通道信息
struct LiveChannel {
    std::string m_name;  // 通道名
    std::string m_last_modified;  // 上次修改时间
};

/// @brief 列举通道结果
struct ListLiveChannelResult {
    std::string m_max_keys;  // 单次响应返回结果的最大条目数量，对应请求中的 max-keys 参数
    std::string m_marker;  // 起始通道标记，从该标记之后（不含）按照 UTF-8 字典序返回通道，对应请求中的marker参数
    std::string m_prefix;  // 通道名匹配前缀，限定响应中只包含指定前缀的通道名
    std::string m_is_truncated; // 响应条目是否被截断，布尔值，例如 true 或 false	
    std::string m_next_marker; // 仅当响应条目有截断（IsTruncated 为 true）才会返回该节点，
                               // 该节点的值为当前响应条目中的最后一个通道名，当需要继续请求后续条目时，
                               // 将该节点的值作为下一次请求的 marker 参数传入
    std::vector<LiveChannel> m_channels;
    void Clear() {
        m_max_keys = "";
        m_marker = "";
        m_prefix = "";
        m_is_truncated = "";
        m_next_marker = "";
        m_channels.clear();
    }
};

} // namespace qcloud_cos
#endif
