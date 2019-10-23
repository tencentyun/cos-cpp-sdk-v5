﻿#include "util/auth_tool.h"

#include <cstdio>
#include <cstdlib>
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
        if( (itr->first[0] == 'x' || itr->first[0] == 'X')
            || !strcasecmp(itr->first.c_str(), "content-type")
            || !strcasecmp(itr->first.c_str(), "host")) {
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
        std::string key = itr->first;
        std::string value;

        if(value_encode) {
            value = CodecUtil::UrlEncode(itr->second);
        } else {
            value = itr->second;
        }

        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        if(value_lower) {
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        }
        trim_params[key] = value;
    }

    itr = trim_params.begin();

    if (key_encode) {
        param_list->append(CodecUtil::UrlEncode(itr->first));
        *param_value_list += CodecUtil::UrlEncode(itr->first) + "=" + itr->second;
    } else {
        param_list->append(itr->first);
        *param_value_list += itr->first + "=" + itr->second;
    }

    for (++itr; itr != trim_params.end(); ++itr) {
        if(key_encode) {
            *param_list += ";" + CodecUtil::UrlEncode(itr->first);
            *param_value_list += "&" + CodecUtil::UrlEncode(itr->first) + "=" + itr->second;
        } else {
            *param_list += ";" + itr->first;
            *param_value_list += "&" + itr->first + "=" + itr->second;
        }
    }
}

std::string AuthTool::Sign(const std::string& access_key, const std::string& secret_key,
                           const std::string& http_method, const std::string& in_uri,
                           const std::map<std::string, std::string>& headers,
                           const std::map<std::string, std::string>& params) {
    uint64_t expired_time_in_s = CosSysConfig::GetAuthExpiredTime();
    uint64_t start_time_in_s = HttpSender::GetTimeStampInUs() / 1000000;
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

} // namespace qcloud_cos
