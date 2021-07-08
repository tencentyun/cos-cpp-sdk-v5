// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/26/17
// Description:

#include "op/cos_result.h"

#include <stdio.h>
#include <string.h>

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

#include "cos_params.h"
#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

bool CosResult::ParseFromHttpResponse(const std::map<std::string, std::string>& headers,
                                      const std::string& body) {
    std::map<std::string, std::string>::const_iterator c_itr;
    c_itr = headers.find("x-cos-request-id");
    if (c_itr != headers.end()) {
        m_x_cos_request_id = c_itr->second;
    }

    c_itr = headers.find("x-cos-trace-id");
    if (c_itr != headers.end()) {
        m_x_cos_trace_id = c_itr->second;
    }

    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_INFO("Parse string to xml doc error, xml_body=%s", body.c_str());
        SetErrorMsg(body);
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kErrorRoot.c_str());
    if (NULL == root) {
        //SDK_LOG_INFO("Miss root node=Error, xml_body=%s", body.c_str());
        SetErrorMsg(body);
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string node_name = node->name();
        if (node_name == kErrorCode) {
            m_err_code = node->value();
        } else if (node_name == kErrorMessage) {
            m_err_msg = node->value();
        } else if (node_name == kErrorResource) {
            m_resource_addr = node->value();
        } else if (node_name == kErrorRequestId) {
            m_x_cos_request_id = node->value();
        } else if (node_name == kErrorTraceId) {
            m_x_cos_trace_id = node->value();
        } else if (node_name == kErrorServerTime) {
            m_x_cos_server_time = node->value();
        } else {
            SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s",
                         node_name.c_str(), body.c_str());
        }
    }
    delete []cstr;
    return true;
}

std::string CosResult::DebugString() const {
    std::string ret = "";
    ret = "IsSucc=" + std::string(m_is_succ ? "true" : "false") + "\n"
        + "HttpStatus=" + StringUtil::IntToString(m_http_status) + "\n"
        + "InternalErrorInfo=" + m_error_info + "\n"
        + "RemoteReturn=[\n"
        + "\tErrorCode=" + m_err_code
        + "\tErrorMsg=" + m_err_msg
        + "\tResourceAddr=" + m_resource_addr
        + "\tXCosRequestId=" + m_x_cos_request_id
        + "\tXCosTraceId=" + m_x_cos_trace_id
        + "]";
    return ret;
}

} // namespace qcloud_cos
