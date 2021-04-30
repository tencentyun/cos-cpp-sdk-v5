// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/17/17
// Description:

#include "request/base_req.h"
#include "cos_sys_config.h"

namespace qcloud_cos {

#define COS_CPP_SDK_HTTP_HEADER_USER_AGENT "cos-cpp-sdk-" COS_CPP_SDK_VERSON

BaseReq::BaseReq() : m_is_https(false), mb_check_md5(true), mb_check_crc(false) {
    m_recv_timeout_in_ms = CosSysConfig::GetRecvTimeoutInms();
    m_conn_timeout_in_ms = CosSysConfig::GetConnTimeoutInms();
    AddHeader("User-Agent", COS_CPP_SDK_HTTP_HEADER_USER_AGENT);
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

bool BaseReq::GenerateAclRequestBody(const Owner& owner,
                                     const std::vector<Grant>& acl,
                                     std::string* body) const {
    if (acl.empty() || owner.m_id.empty() || owner.m_display_name.empty()) {
        SDK_LOG_ERR("Owner id or access control list is empty.");
        return false;
    }

    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element,
                                                        doc.allocate_string("AccessControlPolicy"),
                                                        NULL);
    doc.append_node(root_node);

    // 1. 添加ownerid节点
    rapidxml::xml_node<>* owner_node = doc.allocate_node(rapidxml::node_element,
                                                         "Owner", NULL);
    owner_node->append_node(doc.allocate_node(rapidxml::node_element, "ID",
                                          doc.allocate_string(owner.m_id.c_str())));
    if (!owner.m_display_name.empty()) {
        owner_node->append_node(doc.allocate_node(rapidxml::node_element, "DisplayName",
                                          doc.allocate_string(owner.m_display_name.c_str())));
    }
    root_node->append_node(owner_node);

    // 2. 添加AccessControlList
    rapidxml::xml_node<>* acl_node = doc.allocate_node(rapidxml::node_element,
                                                       "AccessControlList", NULL);
    for (std::vector<Grant>::const_iterator c_itr = acl.begin(); c_itr != acl.end(); ++c_itr) {
        const Grant& grant = *c_itr;
        rapidxml::xml_node<>* grant_node = doc.allocate_node(rapidxml::node_element,
                                                             "Grant", NULL);
        rapidxml::xml_node<>* grantee_node = doc.allocate_node(rapidxml::node_element,
                                "Grantee", NULL);
        grantee_node->append_attribute(doc.allocate_attribute("xmlns:xsi",
                                    "http://www.w3.org/2001/XMLSchema-instance"));
        grantee_node->append_attribute(doc.allocate_attribute("xsi:type",
                                    doc.allocate_string(grant.m_grantee.m_type.c_str())));

        if (!grant.m_grantee.m_id.empty()) {
            grantee_node->append_node(doc.allocate_node(rapidxml::node_element, "ID",
                            doc.allocate_string(grant.m_grantee.m_id.c_str())));
        }

        if (!grant.m_grantee.m_display_name.empty()) {
            grantee_node->append_node(doc.allocate_node(rapidxml::node_element, "DisplayName",
                            doc.allocate_string(grant.m_grantee.m_display_name.c_str())));
        }

        if (!grant.m_grantee.m_uri.empty()) {
            grantee_node->append_node(doc.allocate_node(rapidxml::node_element, "URI",
                            doc.allocate_string(grant.m_grantee.m_uri.c_str())));
        }
        grant_node->append_node(grantee_node);
        grant_node->append_node(doc.allocate_node(rapidxml::node_element, "Permission",
                                        doc.allocate_string(grant.m_perm.c_str())));

        acl_node->append_node(grant_node);
    }
    root_node->append_node(acl_node);

    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();

    return true;
}

std::string BaseReq::DebugString() const {
    std::string ret = "";
    ret += "\nMethod=" + m_method + ", Path=" + m_path + "\n";
    ret += "Headers={\n";
    Str2StrMap::const_iterator c_itr = m_headers_map.begin();
    for (; c_itr != m_headers_map.end(); ++c_itr) {
        ret += "\tkey=[" + c_itr->first + "], value=[" + c_itr->second + "]\n";
    }
    ret += "}\n";

    ret += "Params={\n";
    c_itr = m_params_map.begin();
    for (; c_itr != m_params_map.end(); ++c_itr) {
        ret += "\tkey=[" + c_itr->first + "], value=[" + c_itr->second + "]\n";
    }
    ret += "}\n";
    return ret;
}

} // namespace qcloud_cos
