// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 11/14/17
// Description:

#include "response/service_resp.h"

#include <stdio.h>
#include <string.h>

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

#include "cos_params.h"
#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

bool GetServiceResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("ListAllMyBucketsResult");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=ListAllMyBucketsResult, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == "Owner") {
            rapidxml::xml_node<>* owner_node = node->first_node();
            for (; owner_node != NULL; owner_node = owner_node->next_sibling()) {
                const std::string& owner_node_name = owner_node->name();
                if ("ID" == owner_node_name) {
                    m_owner.m_id = owner_node->value();
                } else if ("DisplayName" == owner_node_name) {
                    m_owner.m_display_name = owner_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in Owner node, field_name=%s",
                                 owner_node_name.c_str());
                }
            }
        } else if (node_name == "Buckets") {
            rapidxml::xml_node<>* buckets_node = node->first_node();
            for (; buckets_node != NULL; buckets_node = buckets_node->next_sibling()) {
                const std::string& name = buckets_node->name();
                if (name == "Bucket") {
                    Bucket bucket;
                    rapidxml::xml_node<>* bucket_node = buckets_node->first_node();
                    for (; bucket_node != NULL; bucket_node = bucket_node->next_sibling()) {
                        const std::string& bucket_node_name = bucket_node->name();
                        if ("Name" == bucket_node_name) {
                            bucket.m_name = bucket_node->value();
                        } else if ("Location" == bucket_node_name) {
                            bucket.m_location = bucket_node->value();
                        } else if ("CreationDate" == bucket_node_name) {
                            bucket.m_create_date = bucket_node->value();
                        } else {
                            SDK_LOG_WARN("Unknown field in Bucket node, field_name=%s, xml_body=%s",
                                         bucket_node_name.c_str(), body.c_str());
                        }
                    }
                    m_buckets.push_back(bucket);
                } else {
                    SDK_LOG_WARN("Unknown field in Buckets node, field_name=%s, xml_body=%s",
                                name.c_str(), body.c_str());
                }
            }
        } else {
            SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s",
                         node_name.c_str(), body.c_str());
        }
    }
    delete[] cstr;
    return true;
}

} // namespace qcloud_cos
