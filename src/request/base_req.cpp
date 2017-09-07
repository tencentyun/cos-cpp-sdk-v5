// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/17/17
// Description:

#include "request/base_req.h"

#include "cos_sys_config.h"

namespace qcloud_cos {

BaseReq::BaseReq() : m_is_https(false) {
    m_recv_timeout_in_ms = CosSysConfig::GetRecvTimeoutInms();
    m_conn_timeout_in_ms = CosSysConfig::GetConnTimeoutInms();
    AddHeader("User-Agent", "cos-cpp-sdk-v5");
}

void BaseReq::AddHeader(const std::string& key, const std::string& value) {
    m_headers_map[key] = value;
}

void BaseReq::AddHeaders(const Str2StrMap& user_headers) {
    Str2StrMap::const_iterator itr = user_headers.begin();
    for (; itr != user_headers.end(); ++itr) {
        m_headers_map[itr->first] = itr->second;
    }
}

std::string BaseReq::GetHeader(const std::string& key) const {
    Str2StrMap::const_iterator c_itr = m_headers_map.find(key);
    if (c_itr != m_headers_map.end()) {
        return c_itr->second;
    }

    return "";
}

void BaseReq::AddParam(const std::string& key, const std::string& value) {
    m_params_map[key] = value;
}

void BaseReq::AddParams(const Str2StrMap& user_params) {
    Str2StrMap::const_iterator itr = user_params.begin();
    for (; itr != user_params.end(); ++itr) {
        m_params_map[itr->first] = itr->second;
    }
}

std::string BaseReq::GetParam(const std::string& key) const {
    Str2StrMap::const_iterator c_itr = m_params_map.find(key);
    if (c_itr != m_params_map.end()) {
        return c_itr->second;
    }

    return "";
}

} // namespace qcloud_cos
