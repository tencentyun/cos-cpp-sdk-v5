// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/17/17
// Description:

#include "response/bucket_resp.h"

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

#include "cos_params.h"
#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

bool GetBucketResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(body, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kGetBucketRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=ListBucketResult, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string node_name = node->name();
        if (node_name == kGetBucketName) {
            m_name = node->value();
        } else if (node_name == kGetBucketEncodingType) {
            m_encoding_type = node->value();
        } else if (node_name == kGetBucketNextMarker) {
            m_next_marker = node->value();
        } else if (node_name == kGetBucketDelimiter) {
            m_delimiter = node->value();
        } else if (node_name == kGetBucketPrefix) {
            m_prefix = node->value();
        } else if (node_name == kGetBucketMarker) {
            m_marker = node->value();
        } else if (node_name == kGetBucketMaxKeys) {
            m_max_keys = StringUtil::StringToUint64(node->value());
        } else if (node_name == kGetBucketIsTruncated) {
            m_is_truncated = ("true" == node->value()) ? true : false;
        } else if (node_name == kGetBucketCommonPrefixes) {
            rapidxml::xml_node<>* common_prefix_node = node->first_node();
            for (; common_prefix_node != NULL;
                 common_prefix_node = common_prefix_node->next_sibling()) {
                m_common_prefixes.push_back(node->value());
            }
        } else if (node_name == kGetBucketContents) {
            rapidxml::xml_node<>* contents_node = node->first_node();
            Content cnt;
            for (; contents_node != NULL; contents_node = contents_node->next_sibling()) {
                const std::string name = contents_node->name();
                if (name == kGetBucketContentsKey) {
                    cnt.m_key = contents_node->value();
                } else if (name == kGetBucketContentsLastModified) {
                    cnt.m_last_modified = contents_node->value();
                } else if (name == kGetBucketContentsETag) {
                    cnt.m_etag = contents_node->value();
                } else if (name == kGetBucketContentsSize) {
                    cnt.m_size = contents_node->value();
                } else if (name == kGetBucketContentsStorageClass) {
                    cnt.m_storage_class = contents_node->value();
                } else if (name == kGetBucketContentsOwner) {
                    rapidxml::xml_node<>* id_node = contents_node->first_node();
                    for (; id_node != NULL; id_node = id_node->next_sibling()) {
                        if (id_node->name() != kGetBucketContentsOwnerID) {
                            continue;
                        }
                        cnt.m_owner_ids.push_back(id_node->value());
                    }
                } else {
                    SDK_LOG_WARN("Unknown field in content node, field_name=%s, xml_body=%s",
                                name.c_str(), body.c_str());
                }
            }
            m_contents.push_back(cnt);
        } else {
            SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s",
                         node_name.c_str(), body.c_str());
        }
    }
    return true;
}

bool GetBucketReplicationResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(body, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kBucketReplicationRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=BucketReplicationRoot, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string node_name = node->name();
        if (node_name == kBucketReplicationRole) {
            m_role = node->value();
        } else if (node_name == kBucketReplicationRule) {
            rapidxml::xml_node<>* rule_node = node->first_node();
            ReplicationRule rule;
            for (; rule_node != NULL; rule_node = rule_node->next_sibling()) {
                const std::string& rule_node_name = rule_node->name();
                if (rule_node->first_node() && rule_node->first_node()->type() == rapidxml::node_data) {
                    if (rule_node_name == kBucketReplicationStatus) {
                        rule.m_is_enable = (rule_node->value() == "Enabled") ? true : false;
                    } else if (rule_node_name == kBucketReplicationID) {
                        rule.m_id = rule_node->value();
                    } else if (rule_node_name == kBucketReplicationPrefix) {
                        rule.m_prefix = rule_node->value();
                    } else {
                        SDK_LOG_WARN("Unknown field in rule node, field_name=[%s]",
                                     rule_node_name.c_str());
                    }
                } else if (rule_node_name == kBucketReplicationDestination) {
                    rapidxml::xml_node<>* dest_node = rule_node->first_node();
                    for (; dest_node != NULL; dest_node = dest_node->next_sibling()) {
                        const std::string dest_node_name = dest_node->name();
                        if (dest_node_name == kBucketReplicationBucket) {
                            rule.m_dest_bucket = dest_node->value();
                        } else if (dest_node_name == kBucketReplicationStorageClass) {
                            rule.m_dest_storage_class = dest_node->value();
                        } else {
                            SDK_LOG_WARN("Unknown field in destionation node, field_name=%s",
                                         dest_node_name.c_str());
                        }
                    }
                } else {
                    SDK_LOG_WARN("Unknown field in rule node, field_name=%s",
                                 rule_node_name.c_str());
                }
            }
            m_rules.push_back(rule);
        } else {
            SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s",
                         node_name.c_str(), body.c_str());
        }
    }
    return true;
}

} // namespace qcloud_cos
