#ifndef UTIL_AUTHTOOl_H
#define UTIL_AUTHTOOl_H

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "request/base_req.h"
#include "util/noncopyable.h"

namespace qcloud_cos {

class AuthTool : private NonCopyable {
public:
    /// \brief 返回签名，可以在指定的有效期内(通过CosSysConfig设置, 默认60s)使用
    ///
    /// \param secret_id   开发者拥有的项目身份识别 ID，用以身份认证
    /// \param secret_key  开发者拥有的项目身份密钥
    /// \param http_method http方法,如POST/GET/HEAD/PUT等, 传入大小写不敏感
    /// \param in_uri      http uri
    /// \param headers     http header的键值对
    /// \param params      http params的键值对
    ///
    /// \return 字符串形式的签名，返回空串代表失败
    static std::string Sign(const std::string& secret_id,
                            const std::string& secret_key,
                            const std::string& http_method,
                            const std::string& in_uri,
                            const std::map<std::string, std::string>& headers,
                            const std::map<std::string, std::string>& params);

    /// \brief 返回签名，可以在指定的有效期内使用
    ///
    /// \param secret_id   开发者拥有的项目身份识别 ID，用以身份认证
    /// \param secret_key  开发者拥有的项目身份密钥
    /// \param http_method http方法,如POST/GET/HEAD/PUT等, 传入大小写不敏感
    /// \param in_uri      http uri
    /// \param headers     http header的键值对
    /// \param params      http params的键值对
    ///
    /// \return 字符串形式的签名，返回空串代表失败
    static std::string Sign(const std::string& secret_id,
                            const std::string& secret_key,
                            const std::string& http_method,
                            const std::string& in_uri,
                            const std::map<std::string, std::string>& headers,
                            const std::map<std::string, std::string>& params,
                            uint64_t start_time_in_s,
                            uint64_t end_time_in_s);

    /// \brief get rtmp signature
    static std::string RtmpSign(const std::string& secret_id,
                        const std::string& secret_key,
                        const std::string& token,
                        const std::string& bucket,
                        const std::string& channel,
                        const std::map<std::string, std::string>& params,
                        uint64_t expire);
private:
    /// \brief 把params中的数据，转小写，正排,key放在param_list key=value放param_value_list
    /// \param params 参数
    /// \param key_encode key是否进行uri编码
    /// \param value_encode value是否进行uri编码
    /// \param value_lower value是否小写
    /// \param param_list 参数名列表，以;分隔
    /// \param param_value_list 参数键值对列表,以&分隔
    /// \retval     无
    static void FillMap(const std::map<std::string,std::string> &params,
                        bool key_encode,
                        bool value_encode,
                        bool value_lower,
                        std::string* param_list,
                        std::string* param_value_list);

    /// \brief 找出需要鉴权的头部，并设置,目前host conent-type 还有x开头的都要鉴权
    /// \param hedaers 头部的kv对
    /// \param filted_req_headers 需要鉴权的头部
    /// \retval 无
    static void FilterAndSetSignHeader(const std::map<std::string,std::string>& headers,
                                       std::map<std::string, std::string>* filted_req_headers);
};

} // namespace qcloud_cos

#endif // AUTHTOOL_H
