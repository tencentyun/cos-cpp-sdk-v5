#ifndef COS_CONFIG_H
#define COS_CONFIG_H

#include <stdint.h>

#include <memory>
#include <mutex>
#include <string>

#include "Poco/JSON/Parser.h"
#include "util/log_util.h"

namespace qcloud_cos {
class CosConfig {
 public:
  /// \brief CosConfig构造函数
  ///
  /// \param config_file 配置文件所在路径
  explicit CosConfig(const std::string& config_file);

  /// \brief CosConfig构造函数
  CosConfig()
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
        m_is_domain_same_to_host_enable(false),
        m_config_parsed(false) {}

  /// \brief CosConfig构造函数
  ///
  /// \param appid      开发者访问 COS
  /// 服务时拥有的用户维度唯一资源标识，用以标识资源 \param access_key
  /// 开发者拥有的项目身份识别 ID，用以身份认证 \param secret_key
  /// 开发者拥有的项目身份密钥
  CosConfig(uint64_t appid, const std::string& access_key,
            const std::string& secret_key, const std::string& region)
      : m_app_id(appid),
        m_access_key(access_key),
        m_secret_key(secret_key),
        m_region(region),
        m_tmp_token(""),
        m_set_intranet_once(false),
        m_is_use_intranet(false),
        m_intranet_addr(""),
        m_dest_domain(""),
        m_is_domain_same_to_host(false),
        m_is_domain_same_to_host_enable(false),
        m_config_parsed(false) {}

  /// \brief CosConfig构造函数
  ///
  /// \param appid      开发者访问 COS
  /// 服务时拥有的用户维度唯一资源标识，用以标识资源 \param access_key
  /// 开发者拥有的项目身份识别 ID，用以身份认证 \param secret_key
  /// 开发者拥有的项目身份密钥
  CosConfig(uint64_t appid, const std::string& access_key,
            const std::string& secret_key, const std::string& region,
            const std::string& tmp_token)
      : m_app_id(appid),
        m_access_key(access_key),
        m_secret_key(secret_key),
        m_region(region),
        m_tmp_token(tmp_token),
        m_set_intranet_once(false),
        m_is_use_intranet(false),
        m_intranet_addr(""),
        m_dest_domain(""),
        m_is_domain_same_to_host(false),
        m_is_domain_same_to_host_enable(false),
        m_config_parsed(false) {}

  /// \brief CosConfig复制构造函数
  ///
  /// \param config
  CosConfig(const CosConfig& config) {
    m_app_id = config.m_app_id;
    m_access_key = config.m_access_key;
    m_secret_key = config.m_secret_key;
    m_region = config.m_region;
    m_tmp_token = config.m_tmp_token;
    m_set_intranet_once = config.m_set_intranet_once;
    m_is_use_intranet = config.m_is_use_intranet;
    m_intranet_addr = config.m_intranet_addr;
    m_dest_domain = config.m_dest_domain;
    m_is_domain_same_to_host = config.m_is_domain_same_to_host;
    m_is_domain_same_to_host_enable = config.m_is_domain_same_to_host;
    m_config_parsed = config.m_config_parsed;
  }

  /// \brief CosConfig赋值构造函数
  ///
  /// \param config
  CosConfig& operator=(const CosConfig& config) {
    m_app_id = config.m_app_id;
    m_access_key = config.m_access_key;
    m_secret_key = config.m_secret_key;
    m_region = config.m_region;
    m_tmp_token = config.m_tmp_token;
    m_set_intranet_once = config.m_set_intranet_once;
    m_is_use_intranet = config.m_is_use_intranet;
    m_intranet_addr = config.m_intranet_addr;
    m_dest_domain = config.m_dest_domain;
    m_is_domain_same_to_host = config.m_is_domain_same_to_host;
    m_is_domain_same_to_host_enable = config.m_is_domain_same_to_host;
    m_config_parsed = config.m_config_parsed;
    return *this;
  }

  /// \brief 根据配置文件进行初始化
  ///
  /// \param config_file 配置文件所在路径
  ///
  /// \return 初始化成功则返回true，否则返回false
  bool InitConf(const std::string& config_file);

  /// \brief 获取AppID
  ///
  /// \return appid
  uint64_t GetAppId() const;

  /// \brief 获取AccessKey
  ///
  /// \return access_key
  std::string GetAccessKey() const;

  /// \brief 获取SecretKey
  ///
  /// \return secret_key
  std::string GetSecretKey() const;

  /// \brief 操作的Region
  ///        region的有效值参见https://cloud.tencent.com/document/product/436/6224
  ///
  /// \return region
  std::string GetRegion() const;

  /// \brief 获取临时密钥
  std::string GetTmpToken() const;

  /// \brief 设置AppID
  void SetAppId(uint64_t app_id) { m_app_id = app_id; }

  /// \brief 设置AccessKey
  void SetAccessKey(const std::string& access_key) {
    m_access_key = access_key;
  }

  /// \brief 设置SecreteKey
  void SetSecretKey(const std::string& secret_key) {
    m_secret_key = secret_key;
  }

  /// \brief 设置操作的Region
  ///        region的有效值参见https://cloud.tencent.com/document/product/436/6224
  void SetRegion(const std::string& region) { m_region = region; }

  /// \brief 设置临时密钥
  void SetTmpToken(const std::string& tmp_token) { m_tmp_token = tmp_token; }

  /// \brief 更新临时密钥
  void SetConfigCredentail(const std::string& access_key,
                           const std::string& secret_key,
                           const std::string& tmp_token);

  /// \brief 设置是否使用自定义ip和端口号
  void SetIsUseIntranetAddr(bool is_use_intranet);

  bool IsUseIntranet();

  /// \berief 设置自定义ip和端口号
  void SetIntranetAddr(const std::string& intranet_addr);

  std::string GetIntranetAddr();

  bool GetSetIntranetOnce() const {return m_set_intranet_once;}

  void SetDestDomain(const std::string& domain);

  const std::string& GetDestDomain() const;

  bool IsDomainSameToHost() const;

  void SetDomainSameToHost(bool is_domain_same_to_host);

  bool IsDomainSameToHostEnable() const;

  // void SetDomainSameToHostEnable(bool is_domain_same_to_host_enable);

  /// \brief 设置日志回调
  void SetLogCallback(const LogCallback log_callback);

  static bool JsonObjectGetStringValue(
      const Poco::JSON::Object::Ptr& json_object, const std::string& key,
      std::string* value);
  static bool JsonObjectGetIntegerValue(
      const Poco::JSON::Object::Ptr& json_object, const std::string& key,
      uint64_t* value);
  static bool JsonObjectGetBoolValue(const Poco::JSON::Object::Ptr& json_object,
                                     const std::string& key, bool* value);

 private:
  mutable std::mutex m_lock;
  uint64_t m_app_id;
  std::string m_access_key;
  std::string m_secret_key;
  std::string m_region;
  std::string m_tmp_token;

  bool m_set_intranet_once;
  bool m_is_use_intranet;
  std::string m_intranet_addr;
  std::string m_dest_domain;
  bool m_is_domain_same_to_host;
  bool m_is_domain_same_to_host_enable;
  bool m_config_parsed;
};

typedef std::shared_ptr<CosConfig> SharedConfig;
}  // namespace qcloud_cos
#endif
