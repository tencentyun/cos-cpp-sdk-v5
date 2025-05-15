﻿#include "cos_config.h"

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#include "cos_sys_config.h"
#include "util/string_util.h"
#include "util/illegal_intercept.h"

namespace qcloud_cos {
CosConfig::CosConfig(const std::string& config_file)
    : m_app_id(0),
      m_access_key(""),
      m_secret_key(""),
      m_region(""),
      m_tmp_token(""),
      m_set_intranet_once(false),
      m_is_use_intranet(false),
      m_intranet_addr(""),
      m_dest_domain(""),
      m_is_domain_same_to_host(false),
      m_config_parsed(false) {
  if (InitConf(config_file)) {
    m_config_parsed = true;
  }
}

bool CosConfig::JsonObjectGetStringValue(
    const Poco::JSON::Object::Ptr& json_object, const std::string& key,
    std::string* value) {
  if (json_object->has(key)) {
    Poco::Dynamic::Var value_get = json_object->get(key);
    if (value_get.isString()) {
      (*value).clear();
      *value = value_get.convert<std::string>();
      return true;
    } else {
      std::cerr << "failed to parse config file, " << key
                << " should be interger" << std::endl;
    }
  }
  return false;
}

bool CosConfig::JsonObjectGetIntegerValue(
    const Poco::JSON::Object::Ptr& json_object, const std::string& key,
    uint64_t* value) {
  if (json_object->has(key)) {
    Poco::Dynamic::Var value_get = json_object->get(key);
    if (value_get.isInteger()) {
      *value = value_get;
      return true;
    } else {
      std::cerr << "failed to parse config file, " << key
                << " should be unsigned interger" << std::endl;
    }
  }
  return false;
}

bool CosConfig::JsonObjectGetBoolValue(
    const Poco::JSON::Object::Ptr& json_object, const std::string& key,
    bool* value) {
  if (json_object->has(key)) {
    Poco::Dynamic::Var value_get = json_object->get(key);
    if (value_get.isBoolean()) {
      *value = value_get;
      return true;
    } else {
      std::cerr << "failed to parse config file, " << key
                << " should be boolean" << std::endl;
    }
  }
  return false;
}

bool CosConfig::InitConf(const std::string& config_file) {
  Poco::JSON::Parser parser;
  std::ifstream ifs(config_file.c_str(), std::ios::in);
  if (!ifs || !ifs.is_open()) {
    std::cerr << "failed to open config file " << config_file << std::endl;
    return false;
  }

  std::istream& is = ifs;
  Poco::Dynamic::Var result;
  try {
    result = parser.parse(is);
  } catch (Poco::JSON::JSONException& jsone) {
    std::cerr << "failed to parse config file, " << jsone.message()
              << std::endl;
    return false;
  }
  if (result.type() != typeid(Poco::JSON::Object::Ptr)) {
    std::cerr << "failed to parse config file " << config_file << std::endl;
    ifs.close();
    return false;
  }

  Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

  JsonObjectGetIntegerValue(object, "AppID", &m_app_id);
  JsonObjectGetStringValue(object, "AccessKey", &m_access_key);
  JsonObjectGetStringValue(object, "SecretId", &m_access_key);
  JsonObjectGetStringValue(object, "SecretKey", &m_secret_key);
  if (m_access_key.empty() || m_secret_key.empty()) {
    std::cerr << "warnning, access_key or serete_key not exists" << std::endl;
  }
  m_access_key = StringUtil::Trim(m_access_key);
  m_secret_key = StringUtil::Trim(m_secret_key);
  //设置cos区域和下载域名:cos,cdn,innercos,自定义,默认:cos
  JsonObjectGetStringValue(object, "Region", &m_region);
  m_region = StringUtil::Trim(m_region);

  uint64_t integer_value;

  //设置签名超时时间,单位:秒
  if (JsonObjectGetIntegerValue(object, "SignExpiredTime", &integer_value)) {
    CosSysConfig::SetAuthExpiredTime(integer_value);
  }

  //设置连接超时时间,单位:毫秒
  if (JsonObjectGetIntegerValue(object, "ConnectTimeoutInms", &integer_value)) {
    CosSysConfig::SetConnTimeoutInms(integer_value);
  }

  //设置接收超时时间,单位:毫秒
  if (JsonObjectGetIntegerValue(object, "ReceiveTimeoutInms", &integer_value)) {
    CosSysConfig::SetRecvTimeoutInms(integer_value);
  }

  //设置上传分片大小,默认:10M
  if (JsonObjectGetIntegerValue(object, "UploadPartSize", &integer_value)) {
    CosSysConfig::SetUploadPartSize(integer_value);
  }

  //设置单文件分片并发上传的线程池大小
  if (JsonObjectGetIntegerValue(object, "UploadThreadPoolSize",
                                &integer_value)) {
    CosSysConfig::SetUploadThreadPoolSize((unsigned)integer_value);
  }

  //异步上传下载的线程池大小
  if (JsonObjectGetIntegerValue(object, "AsynThreadPoolSize", &integer_value)) {
    CosSysConfig::SetAsynThreadPoolSize((unsigned)integer_value);
  }

  //设置log输出,0:不输出, 1:屏幕,2:syslog,默认:0
  if (JsonObjectGetIntegerValue(object, "LogoutType", &integer_value)) {
    CosSysConfig::SetLogOutType((LOG_OUT_TYPE)integer_value);
  }

  // 设置日志级别
  if (JsonObjectGetIntegerValue(object, "LogLevel", &integer_value)) {
    CosSysConfig::SetLogLevel((LOG_LEVEL)integer_value);
  }

  if (JsonObjectGetIntegerValue(object, "DownloadThreadPoolSize",
                                &integer_value)) {
    CosSysConfig::SetDownThreadPoolSize((unsigned)integer_value);
  }

  if (JsonObjectGetIntegerValue(object, "DownloadSliceSize", &integer_value)) {
    CosSysConfig::SetDownSliceSize((unsigned)integer_value);
  }

  bool bool_value;
// 长连接相关
#if 0
    if (JsonObjectGetBoolValue(object, "keepalive_mode", &bool_value)) {
        CosSysConfig::SetKeepAlive(bool_value);
    }
    if (JsonObjectGetIntegerValue(object, "keepalive_idle_time", &integer_value)) {
        CosSysConfig::SetKeepIdle(integer_value);
    }
    if (JsonObjectGetIntegerValue(object, "keepalive_interval_time", &integer_value)) {
        CosSysConfig::SetKeepIntvl(integer_value);
    }
#endif
  if (JsonObjectGetBoolValue(object, "IsCheckMd5", &bool_value)) {
    CosSysConfig::SetCheckMd5(bool_value);
  }

  std::string str_value;
  if (JsonObjectGetStringValue(object, "DestDomain", &str_value)) {
    CosSysConfig::SetDestDomain(str_value);
    m_dest_domain = str_value;
  }

  if (JsonObjectGetBoolValue(object, "IsDomainSameToHost", &bool_value)) {
    CosSysConfig::SetDomainSameToHost(bool_value);
  }

  if (JsonObjectGetBoolValue(object, "IsUseIntranet", &bool_value)) {
    CosSysConfig::SetIsUseIntranet(bool_value);
    m_is_use_intranet = bool_value;
    m_set_intranet_once = true;
  }

  if (JsonObjectGetStringValue(object, "IntranetAddr", &str_value)) {
    CosSysConfig::SetIntranetAddr(str_value);
    m_intranet_addr = str_value;
    m_set_intranet_once = true;
  }

  CosSysConfig::PrintValue();
  return true;
}

uint64_t CosConfig::GetAppId() const { return m_app_id; }

std::string CosConfig::GetAccessKey() const {
  std::lock_guard<std::mutex> lock(m_lock);
  std::string ak = m_access_key;
  return ak;
}

std::string CosConfig::GetSecretKey() const {
  std::lock_guard<std::mutex> lock(m_lock);
  std::string sk = m_secret_key;
  return sk;
}

std::string CosConfig::GetRegion() const { return m_region; }

std::string CosConfig::GetTmpToken() const {
  std::lock_guard<std::mutex> lock(m_lock);
  std::string token = m_tmp_token;
  return token;
}

void CosConfig::SetConfigCredentail(const std::string& access_key,
                                    const std::string& secret_key,
                                    const std::string& tmp_token) {
  std::lock_guard<std::mutex> lock(m_lock);
  m_access_key = access_key;
  m_secret_key = secret_key;
  m_tmp_token = tmp_token;
}

void CosConfig::SetIsUseIntranetAddr(bool is_use_intranet) {
  CosSysConfig::SetIsUseIntranet(is_use_intranet);
  m_is_use_intranet = is_use_intranet;

  m_set_intranet_once = true;
}

bool CosConfig::IsUseIntranet() {
  return m_is_use_intranet;
}

void CosConfig::SetIntranetAddr(const std::string& intranet_addr) {
  CosSysConfig::SetIntranetAddr(intranet_addr);
  m_intranet_addr = intranet_addr;

  m_set_intranet_once = true;
}

std::string CosConfig::GetIntranetAddr() {
  return m_intranet_addr;
}

void CosConfig::SetDestDomain(const std::string& domain) {
  CosSysConfig::SetDestDomain(domain);
  m_dest_domain = domain; 
}

const std::string& CosConfig::GetDestDomain() const { 
  return m_dest_domain; 
}

bool CosConfig::IsDomainSameToHost() const {
  return m_is_domain_same_to_host;
}

void CosConfig::SetDomainSameToHost(bool is_domain_same_to_host) {
  m_is_domain_same_to_host = is_domain_same_to_host;
  m_is_domain_same_to_host_enable = true;
}

bool CosConfig::IsDomainSameToHostEnable() const {
  return m_is_domain_same_to_host_enable;
}

void CosConfig::SetLogCallback(const LogCallback log_callback) {
  CosSysConfig::SetLogCallback(log_callback);
}

bool CosConfig::CheckRegion() {
  // 检查 region 是否符合规范
  if (m_region.empty() || !IllegalIntercept::isAlnum(m_region.front()) || !IllegalIntercept::isAlnum(m_region.back())) {
    return false;
  }
  for (size_t i = 1; i < m_region.size() - 1; ++i) {
    char c = m_region[i];
    if (!IllegalIntercept::isAlnum(c) && c != '.' && c != '-') {
      return false;
    }
  }
  return true;
}

}  // namespace qcloud_cos
