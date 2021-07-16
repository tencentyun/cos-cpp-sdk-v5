#ifndef COS_SYS_CONF_H
#define COS_SYS_CONF_H
#include <stdint.h>

#include "cos_defines.h"
#include "util/log_util.h"

namespace qcloud_cos{

class CosSysConfig {
public:
    /// \brief 设置签名超时时间,单位:秒
    static void SetAuthExpiredTime(uint64_t time);

    /// \brief 设置本地时间与网络时间差值
    static void SetTimeStampDelta(int64_t dela);

    /// \brief 设置连接超时时间,单位:毫秒
    static void SetConnTimeoutInms(uint64_t time);

    /// \brief 设置接收超时时间,单位:毫秒
    static void SetRecvTimeoutInms(uint64_t time);

    /// \brief 设置上传分片大小,单位:字节,默认:10M
    static void SetUploadPartSize(uint64_t part_size);

    /// \brief 获取上传复制分片大小,单位:字节,默认: 20M
    static void SetUploadCopyPartSize(uint64_t part_size);

    /// \brief 设置文件分片并发上传线程池大小,默认: 5
    static void SetUploadThreadPoolSize(unsigned size);

    /// \brief 设置异步上传下载线程池大小,默认: 2
    static void SetAsynThreadPoolSize(unsigned size);

    /// \brief 设置log输出,1:屏幕,2:syslog,3:不输出,默认:1
    static void SetLogOutType(LOG_OUT_TYPE log);

    /// \brief 设置log输出等级,COS_LOG_ERR/WRAN/INFO/DBG
    static void SetLogLevel(LOG_LEVEL level);

    /// \brief 设置下载线程池的大小
    static void SetDownThreadPoolSize(unsigned size);

    /// \brief 设置下载分片的大小
    static void SetDownSliceSize(unsigned slice_size);

    /// \brief 设置长连接的参数
    static void SetKeepAlive(bool keepalive);

    /// \brief 设置长连接的参数
    static void SetKeepIdle(int64_t keepidle);

    /// \brief 设置长连接的参数
    static void SetKeepIntvl(int64_t keepintvl);

    static void SetDestDomain(const std::string& dest_domain);

    /// \brief 获取签名超时时间,单位秒
    static uint64_t GetAuthExpiredTime();

    /// \brief 获取本地时间与网络时间差值
    static int64_t GetTimeStampDelta();

    /// \brief 获取连接超时时间,单位:毫秒
    static uint64_t GetConnTimeoutInms();

    /// \brief 获取接收超时时间,单位:毫秒
    static uint64_t GetRecvTimeoutInms();

    /// \brief 获取上传分片大小,单位:字节
    static uint64_t GetUploadPartSize();

    /// \brief 获取上传复制分片大小,单位:字节
    static uint64_t GetUploadCopyPartSize();

    /// \brief 获取上传线程池大小
    static unsigned GetUploadThreadPoolSize();

    /// \brief 获取异步线程池大小
    static unsigned GetAsynThreadPoolSize();

    /// \brief 获取日志输出类型,默认输出到屏幕
    static int GetLogOutType();

    /// \brief 获取日志输出等级
    static int GetLogLevel();

    /// \brief 打印CosSysConfig的配置详情
    static void PrintValue();

    /// \brief 获取下载分片大小
    static unsigned GetDownSliceSize();

    /// \brief 获取下载线程池大小
    static unsigned GetDownThreadPoolSize();

    /// \brief 获取keepalive参数
    static bool GetKeepAlive();
    static int64_t GetKeepIdle();
    static int64_t GetKeepIntvl();

    /// \brief 下载过程中是否检查MD5
    static bool IsCheckMd5();

    /// \brief 设置下载过程中检查MD5
    static void SetCheckMd5(bool is_check_md5);

    static bool IsDomainSameToHost();

    static void SetDomainSameToHost(bool is_domain_same_to_host);

    /// \brief 根据传入appid、region、bucket_name返回对应的hostname
    static std::string GetHost(uint64_t app_id, const std::string& region,
                               const std::string& bucket_name);

    static std::string GetDestDomain();

    /// \brief 获取是否使用特定ip和端口号
    static bool IsUseIntranet();
   
    static void SetIsUseIntranet(bool is_use_interanet);

    static void SetIntranetAddr(const std::string& intranet_addr);
    
    /// \brief 获取特定ip和端口号
    static std::string GetIntranetAddr();

    /// \brief 日志回调
    static LogCallback GetLogCallback();

    static void SetLogCallback(const LogCallback log_callback);
private:
    // 打印日志:0,不打印,1:打印到屏幕,2:打印到syslog
    static LOG_OUT_TYPE m_log_outtype;
    // 日志级别:1: ERR, 2: WARN, 3:INFO, 4:DBG
    static LOG_LEVEL m_log_level;
    // 上传分片大小
    static uint64_t m_upload_part_size;
    // 上传分片大小
    static uint64_t m_upload_copy_part_size;
    // 本地时间戳与网络时间服务器时间戳的差值，计算方法：
    // delta = network_ts - local_ts
    static int64_t m_timestamp_delta;
    // 签名超时时间(秒)
    static uint64_t m_sign_expire_in_s;
    // Http连接超时时间(毫秒)
    static uint64_t m_conn_timeout_in_ms;
    // Http接收超时时间(毫秒)
    static uint64_t m_recv_timeout_in_ms;
    // 单文件分片并发上传线程池大小(每个文件一个)
    static unsigned m_threadpool_size;
    // 异步上传下载线程池大小(全局就一个)
    static unsigned m_asyn_threadpool_size;
    // 下载文件到本地线程池大小
    static unsigned m_down_thread_pool_size;
    // 下载文件到本地,每次下载字节数
    static unsigned m_down_slice_size;
    // 是否开启长连接
    static bool m_keep_alive;
    // 空闲多久后，发送keepalive探针，单位s
    static int64_t m_keep_idle;
    // 每个keepalive探针时间间隔，单位s
    static int64_t m_keep_intvl;
    // 下载时是否检查md5
    static bool m_is_check_md5;
    
    static std::string m_dest_domain;

    static bool m_is_domain_same_to_host;

    static bool m_is_use_intranet;

    static std::string m_intranet_addr;
    // 日志回调
    static LogCallback m_log_callback;
};
} // namespace qcloud_cos
#endif
