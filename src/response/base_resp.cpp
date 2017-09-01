// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/17/17
// Description:

#include "response/base_resp.h"

#include <iostream>

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

#include "cos_params.h"
#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

std::string BaseResp::GetHeader(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator itr = m_headers.find(key);
    if (itr != m_headers.end()) {
        return itr->second;
    }

    return "";
}

void BaseResp::ParseFromHeaders(const std::map<std::string, std::string>& headers) {
    m_headers = headers;
    std::map<std::string, std::string>::const_iterator itr;
    itr = headers.find(kReqHeaderContentLen);
    if (headers.end() != itr) {
        m_content_length = StringUtil::StringToUint64(itr->second);
    }

    itr = headers.find(kReqHeaderContentType);
    if (headers.end() != itr) {
        m_content_type = itr->second;
    }

    itr = headers.find(kReqHeaderEtag);
    if (headers.end() != itr) {
        m_etag = itr->second;
    }

    itr = headers.find(kReqHeaderConnection);
    if (headers.end() != itr) {
        m_connection = itr->second;
    }

    itr = headers.find(kReqHeaderDate);
    if (headers.end() != itr) {
        m_date = itr->second;
    }

    itr = headers.find(kReqHeaderServer);
    if (headers.end() != itr) {
        m_server = itr->second;
    }

    itr = headers.find(kReqHeaderXCosReqId);
    if (headers.end() != itr) {
        m_x_cos_request_id = itr->second;
    }

    itr = headers.find(kReqHeaderXCosTraceId);
    if (headers.end() != itr) {
        m_x_cos_trace_id = itr->second;
    }
}

} // namespace qcloud_cos
