#include "cos_sys_config.h"

#include <stdint.h>
#include <iostream>

#include "cos_defines.h"
#include "util/string_util.h"

namespace qcloud_cos {

//上传文件分片大小,默认10M
uint64_t CosSysConfig::m_upload_part_size = kPartSize1M * 10;
//上传复制文件分片大小,默认20M
uint64_t CosSysConfig::m_upload_copy_part_size = kPartSize1M * 20;
//签名超时时间,默认60秒
uint64_t CosSysConfig::m_expire_in_s = 60;
//HTTP连接/接收时间(秒)
uint64_t CosSysConfig::m_conn_timeout_in_ms = 5 * 1000;
uint64_t CosSysConfig::m_recv_timeout_in_ms = 5 * 1000;

std::string CosSysConfig::m_dest_domain = "";
unsigned CosSysConfig::m_threadpool_size = kDefaultThreadPoolSizeUploadPart;
unsigned CosSysConfig::m_asyn_threadpool_size = kDefaultPoolSize;

//日志输出
LOG_OUT_TYPE CosSysConfig::m_log_outtype = COS_LOG_STDOUT;
LOG_LEVEL CosSysConfig::m_log_level = COS_LOG_DBG;

//下载文件到本地线程池大小
unsigned CosSysConfig::m_down_thread_pool_max_size = 10;
//下载文件到本地,每次下载字节数
unsigned CosSysConfig::m_down_slice_size = 4*1024*1024;

// 长连接
bool CosSysConfig::m_keep_alive = false;
int64_t CosSysConfig::m_keep_idle = 20;
int64_t CosSysConfig::m_keep_intvl = 5;
bool CosSysConfig::m_is_check_md5 = false;

void CosSysConfig::PrintValue() {
    std::cout << "upload_part_size:" << m_upload_part_size << std::endl;
    std::cout << "upload_copy_part_size:" << m_upload_copy_part_size << std::endl;
    std::cout << "expire_in_s:" << m_expire_in_s << std::endl;
    std::cout << "conn_timeout_in_ms:" << m_conn_timeout_in_ms << std::endl;
    std::cout << "recv_timeout_in_ms:" << m_recv_timeout_in_ms << std::endl;
    std::cout << "threadpool_size:" << m_threadpool_size << std::endl;
    std::cout << "asyn_threadpool_size:" << m_asyn_threadpool_size << std::endl;
    std::cout << "log_outtype:" << m_log_outtype << std::endl;
    std::cout << "log_level:" << m_log_level << std::endl;
    std::cout << "down_thread_pool_max_size:" << m_down_thread_pool_max_size << std::endl;
    std::cout << "down_slice_size:" << m_down_slice_size << std::endl;
    std::cout << "keepalive:" << m_keep_alive << std::endl;
    std::cout << "keepidle:" << m_keep_idle << std::endl;
    std::cout << "keepintvl:" << m_keep_intvl << std::endl;
}

void CosSysConfig::SetKeepAlive(bool keep_alive) {
    m_keep_alive = keep_alive;
}

void CosSysConfig::SetKeepIdle(int64_t keep_idle) {
    m_keep_idle = keep_idle;
}

void CosSysConfig::SetKeepIntvl(int64_t keep_intvl) {
    m_keep_intvl = keep_intvl;
}

void CosSysConfig::SetUploadPartSize(uint64_t part_size) {
    m_upload_part_size = part_size;
}

void CosSysConfig::SetUploadCopyPartSize(uint64_t part_size) {
    m_upload_copy_part_size = part_size;
}

void CosSysConfig::SetDownThreadPoolMaxSize(unsigned size) {
    if (size > 10) {
        m_down_thread_pool_max_size = 10;
    } else if (size < 1) {
        m_down_thread_pool_max_size = 1;
    } else {
        m_down_thread_pool_max_size = size;
    }
}

void CosSysConfig::SetDownSliceSize(unsigned slice_size) {
    if (slice_size < 4 * 1024) {
        m_down_slice_size = 4* 1024;
    } else if (slice_size > 20 * 1024 * 1024) {
        m_down_slice_size = 20 * 1024 * 1024;
    } else {
        m_down_slice_size = slice_size;
    }
}

void CosSysConfig::SetDestDomain(const std::string& dest_domain) {
    m_dest_domain = dest_domain;
}

unsigned CosSysConfig::GetDownThreadPoolMaxSize() {
    return m_down_thread_pool_max_size;
}

unsigned CosSysConfig::GetDownSliceSize() {
    return m_down_slice_size;
}

bool CosSysConfig::GetKeepAlive() {
    return m_keep_alive;
}

int64_t CosSysConfig::GetKeepIdle() {
    return m_keep_idle;
}

int64_t CosSysConfig::GetKeepIntvl() {
    return m_keep_intvl;
}

void CosSysConfig::SetAuthExpiredTime(uint64_t time) {
    m_expire_in_s = time;
}

void CosSysConfig::SetConnTimeoutInms(uint64_t time) {
    m_conn_timeout_in_ms = time;
}

void CosSysConfig::SetRecvTimeoutInms(uint64_t time) {
    m_recv_timeout_in_ms = time;
}

void CosSysConfig::SetUploadThreadPoolSize(unsigned size)
{
    if (size > kMaxThreadPoolSizeUploadPart) {
        m_threadpool_size = kMaxThreadPoolSizeUploadPart;
    } else if (size < kMinThreadPoolSizeUploadPart) {
        m_threadpool_size = kMinThreadPoolSizeUploadPart;
    } else {
        m_threadpool_size = size;
    }
}

void CosSysConfig::SetAsynThreadPoolSize(unsigned size)
{
    // 异步线程池不设置上限
    if (size < kMinThreadPoolSizeUploadPart) {
        m_asyn_threadpool_size = kMinThreadPoolSizeUploadPart;
        return;
    }
    m_asyn_threadpool_size = size;
}

unsigned CosSysConfig::GetAsynThreadPoolSize()
{
    return m_asyn_threadpool_size;
}

unsigned CosSysConfig::GetUploadThreadPoolSize() {
    return m_threadpool_size;
}

void CosSysConfig::SetLogOutType(LOG_OUT_TYPE log) {
    m_log_outtype = log;
}

void CosSysConfig::SetLogLevel(LOG_LEVEL level) {
    m_log_level = level;
}

int CosSysConfig::GetLogOutType() {
    return (int)m_log_outtype;
}

int CosSysConfig::GetLogLevel() {
    return (int)m_log_level;
}

uint64_t CosSysConfig::GetUploadPartSize() {
    return m_upload_part_size;
}

uint64_t CosSysConfig::GetUploadCopyPartSize() {
    return m_upload_copy_part_size;
}

uint64_t CosSysConfig::GetAuthExpiredTime() {
    return m_expire_in_s;
}

uint64_t CosSysConfig::GetConnTimeoutInms() {
    return m_conn_timeout_in_ms;
}

uint64_t CosSysConfig::GetRecvTimeoutInms() {
    return m_recv_timeout_in_ms;
}

bool CosSysConfig::IsCheckMd5() {
    return m_is_check_md5;
}

void CosSysConfig::SetCheckMd5(bool is_check_md5) {
    m_is_check_md5 = is_check_md5;
}

std::string CosSysConfig::GetHost(uint64_t app_id,
                                  const std::string& region,
                                  const std::string& bucket_name) {
    std::string format_region("");
    if (region == "cn-east" || region == "cn-north" || region == "cn-south"
        || region == "cn-southwest" || region == "cn-south-2" || region == "sg"
        || StringUtil::StringStartsWith(region, "cos.")) {
        format_region = region;
    } else {
        format_region = "cos." + region;
    }

    std::string app_id_suffix = "-" + StringUtil::Uint64ToString(app_id);
    if (app_id == 0 || StringUtil::StringEndsWith(bucket_name, app_id_suffix)) {
        return bucket_name + "." + format_region + ".myqcloud.com";
    }

    return bucket_name + app_id_suffix + "." + format_region + ".myqcloud.com";
}

std::string CosSysConfig::GetDestDomain() {
    return m_dest_domain;
}

}
