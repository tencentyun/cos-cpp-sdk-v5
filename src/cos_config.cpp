#include "cos_config.h"

#include <fstream>
#include <iostream>
#include <string>

#include "json/json.h"

#include "cos_sys_config.h"

namespace qcloud_cos {
CosConfig::CosConfig(const std::string& config_file) :
    m_app_id(0), m_access_key(""), m_secret_key(""), m_region(""), m_tmp_token("") {
    InitConf(config_file);
}

bool CosConfig::InitConf(const std::string& config_file) {
    Json::Value root;
    Json::Reader reader;
    std::ifstream is(config_file.c_str(), std::ios::in);
    if (!is || !is.is_open()) {
        std::cout << "open config file fail " << config_file << std::endl;
        return false;
    }

    if (!reader.parse(is, root, false)) {
        std::cout << "parse config file fail " << config_file << std::endl;
        is.close();
        return false;
    }

    is.close();

    if (root.isMember("AppID")) {
        m_app_id = root["AppID"].asUInt64();
    }

    if (root.isMember("AccessKey")) {
        m_access_key = root["AccessKey"].asString();
    }

    if (root.isMember("SecretId")) {
        m_access_key = root["SecretId"].asString();
    }

    if (root.isMember("SecretKey")) {
        m_secret_key = root["SecretKey"].asString();
    }

    //设置cos区域和下载域名:cos,cdn,innercos,自定义,默认:cos
    if (root.isMember("Region")){
        m_region = root["Region"].asString();
    }

    //设置签名超时时间,单位:秒
    if (root.isMember("SignExpiredTime")) {
        CosSysConfig::SetAuthExpiredTime(root["SignExpiredTime"].asInt64());
    }

    //设置连接超时时间,单位:豪秒
    if (root.isMember("ConnectTimeoutInms")) {
        CosSysConfig::SetConnTimeoutInms(root["ConnectTimeoutInms"].asInt64());
    }

    //设置超时时间,单位:豪秒
    if (root.isMember("ReceiveTimeoutInms")) {
        CosSysConfig::SetRecvTimeoutInms(root["ReceiveTimeoutInms"].asInt64());
    }

    //设置上传分片大小,默认:10M
    if (root.isMember("UploadPartSize")) {
        CosSysConfig::SetUploadPartSize(root["UploadPartSize"].asInt64());
    }

    //设置上传分片大小,默认:20M
    if (root.isMember("UploadCopyPartSize")) {
        CosSysConfig::SetUploadCopyPartSize(root["UploadCopyPartSize"].asInt64());
    }

    //设置单文件分片并发上传的线程池大小
    if (root.isMember("UploadThreadPoolSize")) {
        CosSysConfig::SetUploadThreadPoolSize(root["UploadThreadPoolSize"].asInt());
    }

    //异步上传下载的线程池大小
    if (root.isMember("AsynThreadPoolSize")) {
        CosSysConfig::SetAsynThreadPoolSize(root["AsynThreadPoolSize"].asInt());
    }

    //设置log输出,0:不输出, 1:屏幕,2:syslog,,默认:0
    if (root.isMember("LogoutType")) {
        CosSysConfig::SetLogOutType((LOG_OUT_TYPE)(root["LogoutType"].asInt64()));
    }

    // 设置日志级别
    if (root.isMember("LogLevel")) {
        CosSysConfig::SetLogLevel((LOG_LEVEL)(root["LogLevel"].asInt64()));
    }

    if (root.isMember("down_thread_pool_max_size")) {
        CosSysConfig::SetDownThreadPoolMaxSize((root["down_thread_pool_max_size"].asUInt()));
    }

    if (root.isMember("down_slice_size")) {
        CosSysConfig::SetDownSliceSize((root["down_slice_size"].asUInt()));
    }

    // 长连接相关
    if (root.isMember("keepalive_mode")) {
        bool keepalive_mode = (root["keepalive_mode"].asInt() == 0 ? false : true);
        CosSysConfig::SetKeepAlive(keepalive_mode);
    }
    if (root.isMember("keepalive_idle_time")) {
        CosSysConfig::SetKeepIdle(root["keepalive_idle_time"].asInt());
    }
    if (root.isMember("keepalive_interval_time")) {
        CosSysConfig::SetKeepIntvl(root["keepalive_interval_time"].asInt());
    }

    if (root.isMember("IsCheckMd5")) {
        CosSysConfig::SetCheckMd5(root["IsCheckMd5"].asBool());
    }
    
    if (root.isMember("DestDomain")) {
        CosSysConfig::SetDestDomain(root["DestDomain"].asString());
    }
     
    if (root.isMember("IsDomainSameToHost")) {
        CosSysConfig::SetDomainSameToHost(root["IsDomainSameToHost"].asBool());
    }
    
    CosSysConfig::PrintValue();
    return true;
}

uint64_t CosConfig::GetAppId() const {
    return m_app_id;
}

std::string CosConfig::GetAccessKey() const {
	boost::shared_lock<boost::shared_mutex> lock(m_lock);
	return m_access_key;
}

std::string CosConfig::GetSecretKey() const {
	boost::shared_lock<boost::shared_mutex> lock(m_lock);
	return m_secret_key;
}

std::string CosConfig::GetRegion() const {
    return m_region;
}

std::string CosConfig::GetTmpToken() const {
	boost::shared_lock<boost::shared_mutex> lock(m_lock);
	return m_tmp_token;
}

void CosConfig::SetConfigCredentail(const std::string& access_key, const std::string& secret_key, const std::string& tmp_token) {
	// get upgradable access
	boost::upgrade_lock<boost::shared_mutex> lock(m_lock);
	// get exclusive access
	boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    m_access_key = access_key;
    m_secret_key = secret_key;
    m_tmp_token  = tmp_token;
}

void CosConfig::SetLogCallback(const LogCallback log_callback) {
    CosSysConfig::SetLogCallback(log_callback);
}


} // qcloud_cos
