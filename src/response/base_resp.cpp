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

bool BaseResp::ParseFromACLXMLString(const std::string& body,
                                     std::string* owner_id,
                                     std::string* owner_display_name,
                                     std::vector<Grant>* acl) {
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(body, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("AccessControlPolicy");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=AccessControlPolicy, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string node_name = node->name();
        if ("Owner" == node_name) {
            rapidxml::xml_node<>* owner_node = node->first_node();
            for (; owner_node != NULL; owner_node = owner_node->next_sibling()) {
                const std::string owner_node_name = owner_node->name();
                if ("ID" == owner_node_name) {
                    *owner_id = owner_node->value();
                } else if ("DisplayName" == owner_node_name) {
                    *owner_display_name = owner_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in owner node, field_name=%s",
                                 owner_node_name.c_str());
                }
            }
        } else if ("AccessControlList" == node_name) {
            rapidxml::xml_node<>* acl_node = node->first_node();
            for (; acl_node != NULL; acl_node = acl_node->next_sibling()) {
                const std::string acl_node_name = acl_node->name();
                if ("Grant" == acl_node_name) {
                    Grant grant;
                    rapidxml::xml_node<>* grant_node = acl_node->first_node();
                    for (; grant_node != NULL; grant_node = grant_node->next_sibling()) {
                        const std::string& grant_node_name = grant_node->name();
                        if ("Grantee" == grant_node_name) {
                            grant.m_grantee.m_type = acl_node->value();
                            rapidxml::xml_node<>* grantee_node = grant_node->first_node();
                            for (; grantee_node != NULL; grantee_node = grantee_node->next_sibling()) {
                                const std::string& grantee_node_name = grantee_node->name();
                                if ("ID" == grantee_node_name) {
                                    grant.m_grantee.m_id = grantee_node->value();
                                } else if ("DisplayName" == grantee_node_name) {
                                    grant.m_grantee.m_display_name = grantee_node->value();
                                } else if ("URI" == grantee_node_name) {
                                    // TODO(sevenyou) 公有读写才返回URI
                                    grant.m_grantee.m_uri = grantee_node->value();
                                } else {
                                    SDK_LOG_WARN("Unknown field in grantee node, field_name=%s",
                                                 grantee_node_name.c_str());
                                }
                            }
                        } else if ("Permission" == grant_node_name) {
                            grant.m_perm = grant_node->value();
                        } else {
                            SDK_LOG_WARN("Unknown field in grant node, field_name=%s",
                                         grant_node_name.c_str());
                        }
                    }
                    acl->push_back(grant);
                } else {
                    SDK_LOG_WARN("Unknown field in AccessControlList node, field_name=%s",
                                 acl_node_name.c_str());
                }
            }
        } else {
            SDK_LOG_WARN("Unknown field in AccessControlPolicy node, field_name=%s",
                         node_name.c_str());
        }
    }

    return true;
}

} // namespace qcloud_cos
