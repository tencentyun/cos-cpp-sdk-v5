#ifndef COS_CONFIG_H
#define COS_CONFIG_H

#include <stdint.h>

#include <string>

namespace qcloud_cos{

class CosConfig{
public:
    /// \brief CosConfig构造函数
    ///
    /// \param config_file 配置文件所在路径
    explicit CosConfig(const std::string& config_file);

    /// \brief CosConfig构造函数
    CosConfig() : m_app_id(0), m_access_key(""), m_secret_key("") {}

    /// \brief CosConfig构造函数
    ///
    /// \param t_appid      开发者访问 COS 服务时拥有的用户维度唯一资源标识，用以标识资源
    /// \param t_access_key 开发者拥有的项目身份识别 ID，用以身份认证
    /// \param t_secret_key 开发者拥有的项目身份密钥
    CosConfig(uint64_t t_appid,
              const std::string& t_access_key,
              const std::string& t_secret_key)
        : m_app_id(t_appid), m_access_key(t_access_key), m_secret_key(t_secret_key){}

    /// \brief CosConfig复制构造函数
    ///
    /// \param config
    CosConfig(const CosConfig& config) {
        m_app_id = config.m_app_id;
        m_access_key = config.m_access_key;
        m_secret_key = config.m_secret_key;
    }

    /// \brief CosConfig赋值构造函数
    ///
    /// \param config
    CosConfig& operator=(const CosConfig& config) {
        m_app_id = config.m_app_id;
        m_access_key = config.m_access_key;
        m_secret_key = config.m_secret_key;
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

private:
    uint64_t m_app_id;
    std::string m_access_key;
    std::string m_secret_key;
};

} // namespace qcloud_cos
#endif
