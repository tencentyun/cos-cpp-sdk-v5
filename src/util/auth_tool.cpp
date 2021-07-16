#include "util/auth_tool.h"

#include <cstdio>
#include <cstdlib>
//#include <strings.h>
#include <string.h>

#include <iostream>
#include <algorithm>

#include "util/codec_util.h"
#include "util/sha1.h"
#include "util/string_util.h"
#include "util/http_sender.h"
#include "cos_sys_config.h"

#if defined(_WIN32)
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

namespace qcloud_cos {

void AuthTool::FilterAndSetSignHeader(const std::map<std::string,std::string> &headers,
                                      std::map<std::string, std::string>* filted_req_headers) {
    for(std::map<std::string, std::string>::const_iterator itr = headers.begin();
        itr != headers.end(); ++itr) {
        if( !strcasecmp(itr->first.c_str(), "host")
            || !strcasecmp(itr->first.c_str(), "content-type")
            || !strcasecmp(itr->first.c_str(), "content-md5")
            || !strcasecmp(itr->first.c_str(), "content-disposition")
            || !strcasecmp(itr->first.c_str(), "content-encoding")
            || !strcasecmp(itr->first.c_str(), "content-length")
            || !strcasecmp(itr->first.c_str(), "transfer-encoding")
            || !strcasecmp(itr->first.c_str(), "range")
            || !strncmp(itr->first.c_str(), "x-cos", 5)) {
            filted_req_headers->insert(std::make_pair(itr->first, itr->second));
        }
    }
}

void AuthTool::FillMap(const std::map<std::string,std::string> &params,
                       bool key_encode,
                       bool value_encode,
                       bool value_lower,
                       std::string* param_list,
                       std::string* param_value_list) {
    if (params.empty()) {
        return;
    }

    std::map<std::string, std::string>::const_iterator itr;
    std::map<std::string, std::string> trim_params;

    for (itr = params.begin(); itr != params.end(); ++itr) {
        std::string key, value;
        if (key_encode) {
            key = CodecUtil::UrlEncode(itr->first);
        } else {
            key = itr->first;
        }

        if(value_encode) {
            value = CodecUtil::UrlEncode(itr->second);
        } else {
            value = itr->second;
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        if (value_lower) {
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        }
        trim_params[key] = value;
    }

    itr = trim_params.begin();

    param_list->append(itr->first);
    *param_value_list += itr->first + "=" + itr->second;

    for (++itr; itr != trim_params.end(); ++itr) {
        *param_list += ";" + itr->first;
        *param_value_list += "&" + itr->first + "=" + itr->second;
    }
}

std::string AuthTool::Sign(const std::string& access_key, const std::string& secret_key,
                           const std::string& http_method, const std::string& in_uri,
                           const std::map<std::string, std::string>& headers,
                           const std::map<std::string, std::string>& params) {
    uint64_t expired_time_in_s = CosSysConfig::GetAuthExpiredTime();
    uint64_t start_time_in_s = time(NULL);
    // 如果本地时间需要调整，则调用CosSysConfig::SetTimeStampDelta设置本地与网络时间差
    int64_t timestamp_delta = CosSysConfig::GetTimeStampDelta();
    if (0 != timestamp_delta) {
        start_time_in_s += timestamp_delta;
    }
    uint64_t end_time_in_s = start_time_in_s + expired_time_in_s;

    return Sign(access_key, secret_key, http_method, in_uri, headers, params, start_time_in_s,
                end_time_in_s);
}

std::string AuthTool::Sign(const std::string& access_key, const std::string& secret_key,
                           const std::string& http_method, const std::string& in_uri,
                           const std::map<std::string, std::string>& headers,
                           const std::map<std::string, std::string>& params,
                           uint64_t start_time_in_s,
                           uint64_t end_time_in_s) {
    if (access_key.empty() || secret_key.empty()) {
        return "";
    }
    std::string start_end_time_str = StringUtil::Uint64ToString(start_time_in_s) + ";"
        + StringUtil::Uint64ToString(end_time_in_s);

    // 1. 获取签名所需的path/params/headers
    std::map<std::string, std::string> filted_req_headers;
    FilterAndSetSignHeader(headers, &filted_req_headers);

    // 2. 将header和params拼接为字符串
    std::string header_list, header_value_list;
    std::string param_list, param_value_list;

    FillMap(params, true, true, false, &param_list, &param_value_list);
    FillMap(filted_req_headers, false, true, false, &header_list, &header_value_list);

    std::string uri = in_uri;
    if (uri.empty()) {
        uri = "/";
    }

    // 3. format string
    std::string lower_method = StringUtil::StringToLower(http_method);
    std::string format_str= lower_method + "\n" + uri + "\n"
        + param_value_list + "\n" + header_value_list + "\n";

    // 4. StringToSign
    Sha1 sha1;
    sha1.Append(format_str.c_str(), format_str.size());
    std::string string_to_sign= "sha1\n" + start_end_time_str + "\n" + sha1.Final() + "\n";

    // 5. signature
    std::string sign_key = CodecUtil::HmacSha1Hex(start_end_time_str, secret_key);
    std::transform(sign_key.begin(), sign_key.end(), sign_key.begin(), ::tolower);

    std::string signature = CodecUtil::HmacSha1Hex(string_to_sign, sign_key);
    std::transform(signature.begin(), signature.end(), signature.begin(), ::tolower);

    // 6. 拼接
    std::string req_sign = "q-sign-algorithm=sha1&q-ak=" + access_key +
                           "&q-sign-time=" + start_end_time_str +
                           "&q-key-time=" + start_end_time_str +
                           "&q-header-list=" + header_list +
                           "&q-url-param-list=" + param_list +
                           "&q-signature=" + signature;

    return req_sign;
}

std::string AuthTool::RtmpSign(const std::string& secret_id,
                            const std::string& secret_key,
                            const std::string& token,
                            const std::string& bucket,
                            const std::string& channel,
                            const std::map<std::string, std::string>& params,
                            uint64_t expire) {
    std::string canonicalized_param;
    std::string rtmp_str;
    std::string str_to_sign;
    std::string signature;
    std::string rtmp_sign;

    std::map<std::string, std::string>::const_iterator it = params.begin();
    for(; it != params.end(); ++it) {
         canonicalized_param.append(it->first + "=" + it->second + "&");
    }
    if (!token.empty()) {
        canonicalized_param.append(token);
    }
    canonicalized_param = StringUtil::StringRemoveSuffix(canonicalized_param, "&");

    rtmp_str = "/" + bucket + "/" + channel + "\n";
    if (!canonicalized_param.empty()) {
        rtmp_str.append(canonicalized_param);
    }
    rtmp_str.append("\n");
    Sha1 sha1;
    sha1.Append(rtmp_str.c_str(), rtmp_str.size());

    time_t now = time(NULL);
    time_t end = now + expire;
    std::string time_str = StringUtil::IntToString(now) + ";" + StringUtil::IntToString(end);

    str_to_sign = "sha1\n" + time_str + "\n" + sha1.Final() + "\n";

    signature = CodecUtil::HmacSha1Hex(str_to_sign, secret_key);
    std::transform(signature.begin(), signature.end(), signature.begin(), ::tolower);
    rtmp_sign = "q-sign-algorithm=sha1&q-ak=" + secret_id +
                   "&q-sign-time=" + time_str +
                   "&q-key-time=" + time_str +
                   "&q-signature=" + signature;
    if (!canonicalized_param.empty()) {
        rtmp_sign.append("&");
        rtmp_sign.append(canonicalized_param);
    }
    return rtmp_sign;
}

} // namespace qcloud_cos
