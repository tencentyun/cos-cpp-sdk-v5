// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/17/17
// Description:

#include "response/bucket_resp.h"

#include <stdio.h>
#include <string.h>
#include <iostream>

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

#include "cos_params.h"
#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

bool GetBucketResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kGetBucketRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=kGetBucketRoot, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
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
            m_is_truncated = ("true" == std::string(node->value())) ? true : false;
        } else if (node_name == kGetBucketCommonPrefixes) {
            rapidxml::xml_node<>* common_prefix_node = node->first_node();
            for (; common_prefix_node != NULL;
                 common_prefix_node = common_prefix_node->next_sibling()) {
                m_common_prefixes.push_back(std::string(common_prefix_node->value()));
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
                    cnt.m_etag = StringUtil::Trim(contents_node->value(), "\"");
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
                        cnt.m_owner_ids.push_back(std::string(id_node->value()));
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
    delete[] cstr;
    return true;
}

bool ListMultipartUploadResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kListMultipartUploadRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=kListMultipartUploadRoot, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == kListMultipartUploadBucket) {
            m_name = node->value();
        } else if (node_name == kGetBucketEncodingType) {
            m_encoding_type = node->value();
        } else if (node_name == kListMultipartUploadMarker) {
            m_marker = node->value();
        } else if (node_name == kListMultipartUploadIdMarker) {
            m_uploadid_marker = node->value();
        } else if (node_name == kListMultipartUploadNextKeyMarker) {
            m_nextkey_marker = node->value();
        } else if (node_name == kListMultipartUploadNextUploadIdMarker) {
            m_nextuploadid_marker = node->value();
        } else if (node_name == kListMultipartUploadMaxUploads) {
            // Notice the qcloud.com gives the string type
            m_max_uploads = node->value();
        } else if (node_name == kGetBucketDelimiter) {
            m_delimiter = node->value();
        } else if (node_name == kGetBucketPrefix) {
            m_prefix = node->value();
        } else if (node_name == kGetBucketIsTruncated) {
            m_is_truncated = ("true" == std::string(node->value())) ? true : false;
        } else if (node_name == kGetBucketCommonPrefixes) {
            rapidxml::xml_node<>* common_prefix_node = node->first_node();
            for (; common_prefix_node != NULL;
                 common_prefix_node = common_prefix_node->next_sibling()) {
                m_common_prefixes.push_back(std::string(common_prefix_node->value()));
            }
        } else if (node_name == kListMultipartUploadUpload) {
            rapidxml::xml_node<>* upload_node = node->first_node();
            Upload cnt;
            for (; upload_node != NULL; upload_node = upload_node->next_sibling()) {
                const std::string name = upload_node->name();
                if (name == kListMultipartUploadKey) {
                    cnt.m_key = upload_node->value();
                } else if (name == kListMultipartUploadId) {
                    cnt.m_uploadid = upload_node->value();
                } else if (name == kListMultipartUploadStorageClass) {
                    cnt.m_storage_class = upload_node->value();
                } else if (name == kListMultipartUploadInitiator) {
                    // push the owner struct, m_initator
                    rapidxml::xml_node<>* id_node = upload_node->first_node();
                    for (; id_node != NULL; id_node = id_node->next_sibling()) {
                        Owner own;
                        if (id_node->name() == kListMultipartUploadID) {
                            own.m_id = id_node->value();
                        } else if (id_node->name() == kListMultipartUploadDisplayName) {
                            own.m_display_name = id_node->value();
                        } else {
                            SDK_LOG_WARN("Unknown field in KListMultipartUploadInitiator node.");
                        }
                        cnt.m_initator.push_back(own);
                    }

                } else if (name == kListMultipartUploadOwner) {
                    // push the owner struct, m_owner
                    rapidxml::xml_node<>* id_node = upload_node->first_node();
                    for (; id_node != NULL; id_node = id_node->next_sibling()) {
                        Owner own;
                        if (id_node->name() == kListMultipartUploadID) {
                            own.m_id = id_node->value();
                        } else if (id_node->name() == kListMultipartUploadDisplayName) {
                            own.m_display_name = id_node->value();
                        } else {
                            SDK_LOG_WARN("Unknown field in KListMultipartUploadOwner node.");
                        }
                        cnt.m_owner.push_back(own);
                    }
                } else if (name == kListMultipartUploadInitiated) {
                    cnt.m_initiated = upload_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in content node, field_name=%s, xml_body=%s",
                                name.c_str(), body.c_str());
                }
            }
            m_upload.push_back(cnt);
        } else {
            SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s",
                         node_name.c_str(), body.c_str());
        }
    }
    delete[] cstr;
    return true;
}

bool GetBucketReplicationResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kBucketReplicationRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=BucketReplicationRoot, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == kBucketReplicationRole) {
            m_role = node->value();
        } else if (node_name == kBucketReplicationRule) {
            rapidxml::xml_node<>* rule_node = node->first_node();
            ReplicationRule rule;
            for (; rule_node != NULL; rule_node = rule_node->next_sibling()) {
                const std::string& rule_node_name = rule_node->name();
                if (rule_node->first_node() && rule_node->first_node()->type() == rapidxml::node_data) {
                    if (rule_node_name == kBucketReplicationStatus) {
                        rule.m_is_enable = (std::string(rule_node->value()) == "Enabled") ? true : false;
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
    delete[] cstr;
    return true;
}

bool GetBucketLifecycleResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("LifecycleConfiguration");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=LifecycleConfiguration, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("Rule" == node_name) {
            rapidxml::xml_node<>* rule_node = node->first_node();
            LifecycleRule rule;
            for (; rule_node != NULL; rule_node = rule_node->next_sibling()) {
                const std::string& rule_node_name = rule_node->name();
                if ("ID" == rule_node_name) {
                    rule.SetId(rule_node->value());
                } else if ("Prefix" == rule_node_name) {
                    // Prefix可以直接作为Rule的子节点
                    LifecycleFilter filter = rule.GetFilter();
                    filter.SetPrefix(rule_node->value());
                    rule.SetFilter(filter);
                } else if ("Filter" == rule_node_name) {
                    rapidxml::xml_node<>* filter_node = rule_node->first_node();
                    LifecycleFilter filter = rule.GetFilter();
                    for (; filter_node != NULL; filter_node = filter_node->next_sibling()) {
                        const std::string& filter_node_name = filter_node->name();
                        if ("And" == filter_node_name) {
                            rapidxml::xml_node<>* and_node = filter_node->first_node();
                            for (; and_node != NULL; and_node = and_node->next_sibling()) {
                                const std::string& and_node_name = and_node->name();
                                if ("Prefix" == and_node_name) {
                                    filter.SetPrefix(and_node->value());
                                } else if ("Tag" == and_node_name) {
                                    rapidxml::xml_node<>* tag_node = and_node->first_node();
                                    LifecycleTag tag;
                                    for (; tag_node != NULL; tag_node = tag_node->next_sibling()) {
                                        const std::string& tag_node_name = tag_node->name();
                                        if ("Key" == tag_node_name) {
                                            tag.key = tag_node->value();
                                        } else if ("Value" == tag_node_name) {
                                            tag.value = tag_node->value();
                                        } else {
                                            SDK_LOG_WARN("Unknown field in tag node, field_name=%s",
                                                         tag_node_name.c_str());
                                        }
                                    }
                                    filter.AddTag(tag);
                                } else {
                                    SDK_LOG_WARN("Unknown field in and node, field_name=%s",
                                                 and_node_name.c_str());
                                }
                            }
                        } else if ("Prefix" == filter_node_name) {
                            filter.SetPrefix(filter_node->value());
                        } else if ("Tag" == filter_node_name) {
                            rapidxml::xml_node<>* tag_node = filter_node->first_node();
                            LifecycleTag tag;
                            for (; tag_node != NULL; tag_node = tag_node->next_sibling()) {
                                const std::string& tag_node_name = tag_node->name();
                                if ("Key" == tag_node_name) {
                                    tag.key = tag_node->value();
                                } else if ("Value" == tag_node_name) {
                                    tag.value = tag_node->value();
                                } else {
                                    SDK_LOG_WARN("Unknown field in tag node, field_name=%s",
                                                 tag_node_name.c_str());
                                }
                            }
                            filter.AddTag(tag);
                        } else {
                            SDK_LOG_WARN("Unknown field in filter node, field_name=%s",
                                         filter_node_name.c_str());
                        }
                    }
                    rule.SetFilter(filter);
                } else if ("Status" == rule_node_name) {
                    rule.SetIsEnable(("Enabled" == std::string(rule_node->value())) ? true : false);
                } else if ("Transition" == rule_node_name) {
                    rapidxml::xml_node<>* trans_node = rule_node->first_node();
                    LifecycleTransition transition;
                    for (; trans_node != NULL; trans_node = trans_node->next_sibling()) {
                        const std::string& trans_node_name = trans_node->name();
                        if ("Days" == trans_node_name) {
                            transition.SetDays(StringUtil::StringToUint64(trans_node->value()));
                        } else if ("Date" == trans_node_name) {
                            transition.SetDate(trans_node->value());
                        } else if ("StorageClass" == trans_node_name) {
                            transition.SetStorageClass(trans_node->value());
                        } else {
                            SDK_LOG_WARN("Unknown field in transition node, field_name=%s",
                                         trans_node_name.c_str());
                        }
                    }
                    rule.AddTransition(transition);
                } else if ("Expiration" == rule_node_name) {
                    rapidxml::xml_node<>* expir_node = rule_node->first_node();
                    LifecycleExpiration expiration;
                    for (; expir_node != NULL; expir_node = expir_node->next_sibling()) {
                        const std::string& expir_node_name = expir_node->name();
                        if ("Days" == expir_node_name) {
                            expiration.SetDays(StringUtil::StringToUint64(expir_node->value()));
                        } else if ("Date" == expir_node_name) {
                            expiration.SetDate(expir_node->value());
                        } else if ("ExpiredObjectDeleteMarker" == expir_node_name) {
                            expiration.SetExpiredObjDelMarker(
                                (std::string(expir_node->value()) == "true") ? true : false);
                        } else {
                            SDK_LOG_WARN("Unknown field in expiration node, field_name=%s",
                                         expir_node_name.c_str());
                        }
                    }
                    rule.SetExpiration(expiration);
                } else if ("NoncurrentVersionTransition" == rule_node_name) {
                    rapidxml::xml_node<>* trans_node = rule_node->first_node();
                    LifecycleNonCurrTransition transition;
                    for (; trans_node != NULL; trans_node = trans_node->next_sibling()) {
                        const std::string& trans_node_name = trans_node->name();
                        if ("NoncurrentDays" == trans_node_name) {
                            transition.SetDays(StringUtil::StringToUint64(trans_node->value()));
                        } else if ("StorageClass" == trans_node_name) {
                            transition.SetStorageClass(trans_node->value());
                        } else {
                            SDK_LOG_WARN("Unknown field in NoncurrentVersionTransition node,"
                                         "field_name=%s", trans_node_name.c_str());
                        }
                    }
                    rule.SetNonCurrTransition(transition);
                } else if ("NoncurrentVersionExpiration" == rule_node_name) {
                    rapidxml::xml_node<>* expir_node = rule_node->first_node();
                    LifecycleNonCurrExpiration expiration;
                    for (; expir_node != NULL; expir_node = expir_node->next_sibling()) {
                        const std::string& expir_node_name = expir_node->name();
                        if ("NoncurrentDays" == expir_node_name) {
                            expiration.SetDays(StringUtil::StringToUint64(expir_node->value()));
                        } else {
                            SDK_LOG_WARN("Unknown field in NoncurrentVersionExpiration node,"
                                         "field_name=%s", expir_node_name.c_str());
                        }
                    }
                    rule.SetNonCurrExpiration(expiration);
                } else if ("AbortIncompleteMultipartUpload" == rule_node_name) {
                    rapidxml::xml_node<>* abort_node = rule_node->first_node();
                    AbortIncompleteMultipartUpload abort;
                    for (; abort_node != NULL; abort_node = abort_node->next_sibling()) {
                        const std::string& abort_node_name = abort_node->name();
                        if ("DaysAfterInitiation" == abort_node_name) {
                            abort.m_days_after_init =
                                StringUtil::StringToUint64(abort_node->value());
                        } else {
                            SDK_LOG_WARN("Unknown field in AbortIncompleteMultipartUpload node,"
                                         "field_name=%s", abort_node_name.c_str());
                        }
                    }
                    rule.SetAbortIncompleteMultiUpload(abort);
                } else {
                    SDK_LOG_WARN("Unknown field in Rule node, field_name=%s",
                                 rule_node_name.c_str());
                }
            }
            m_rules.push_back(rule);
        } else {
            SDK_LOG_WARN("Unknown field in LifecycleConfiguration node, field_name=%s",
                         node_name.c_str());
        }
    }

    delete[] cstr;
    return true;
}

bool GetBucketACLResp::ParseFromXmlString(const std::string& body) {
    return ParseFromACLXMLString(body, &m_owner_id, &m_owner_display_name, &m_acl);
}

bool GetBucketCORSResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    
    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("CORSConfiguration");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=CORSConfiguration, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("CORSRule" == node_name) {
            CORSRule rule;
            rapidxml::xml_node<>* rule_node = node->first_node();
            for (; rule_node != NULL; rule_node = rule_node->next_sibling()) {
                const std::string& rule_node_name = rule_node->name();
                if ("ID" == rule_node_name) {
                    rule.m_id = rule_node->value();
                } else if ("AllowedOrigin" == rule_node_name) {
                    rule.m_allowed_origins.push_back(rule_node->value());
                } else if ("AllowedMethod" == rule_node_name) {
                    rule.m_allowed_methods.push_back(rule_node->value());
                } else if ("AllowedHeader" == rule_node_name) {
                    rule.m_allowed_headers.push_back(rule_node->value());
                } else if ("MaxAgeSeconds" == rule_node_name) {
                    rule.m_max_age_secs = rule_node->value();
                } else if ("ExposeHeader" == rule_node_name) {
                    rule.m_expose_headers.push_back(rule_node->value());
                } else {
                    SDK_LOG_WARN("Unknown field in CORSRule node, field_name=%s",
                                 node_name.c_str());
                }
            }
            m_rules.push_back(rule);
        } else {
            SDK_LOG_WARN("Unknown field in CORSConfiguration node, field_name=%s",
                         node_name.c_str());
        }
    }

    delete[] cstr;
    return true;
}

bool GetBucketVersioningResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("VersioningConfiguration");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=VersioningConfiguration, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == "Status") {
            std::string status(node->value());
            SDK_LOG_DBG("status value=%s", status.c_str());
            if (status == "Enabled") {
                m_status = 1;
            } else if (status == "Suspended") {
                m_status = 2;
            } else {
                m_status = 0;
            }
        } else {
            SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s",
                         node_name.c_str(), body.c_str());
        }
    }
    delete[] cstr;
    return true;
}

bool GetBucketLocationResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("LocationConstraint");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=LocationConstraint, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    m_location = root->value();

    delete[] cstr;
    return true;
}

bool GetBucketObjectVersionsResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("ListVersionsResult");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=ListVersionsResult, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("Name" == node_name) {
            m_bucket_name = node->value();
        } else if ("Prefix" == node_name) {
            m_prefix = node->value();
        } else if ("KeyMarker" == node_name) {
            m_key_marker = node->value();
        } else if ("VersionIdMarker" == node_name) {
            m_version_id_marker = node->value();
        } else if ("MaxKeys" == node_name) {
            m_max_keys = StringUtil::StringToUint64(node->value());
        } else if ("IsTruncated" == node_name) {
            m_is_truncated = ("true" == std::string(node->value())) ? true : false;
        } else if ("Encoding-Type" == node_name) {
            m_encoding_type = node->value();
        } else if ("NextKeyMarker" == node_name) {
            m_next_key_marker = node->value();
        } else if ("NextVersionIdMarker" == node_name) {
            m_next_version_id_marker = node->value();
        } else if ("DeleteMarker" == node_name || "Version" == node_name) {
            COSVersionSummary summary;
            summary.m_is_delete_marker = ("DeleteMarker" == node_name) ? true : false;
            rapidxml::xml_node<>* result_node = node->first_node();
            for (; result_node != NULL; result_node = result_node->next_sibling()) {
                const std::string& result_node_name = result_node->name();
                if ("Key" == result_node_name) {
                    summary.m_key = result_node->value();
                } else if ("VersionId" == result_node_name) {
                    summary.m_version_id = result_node->value();
                } else if ("IsLatest" == result_node_name) {
                    summary.m_is_latest = ("true" == result_node->value()) ? true : false;
                } else if ("LastModified" == result_node_name) {
                    summary.m_last_modified = result_node->value();
                } else if ("Owner" == result_node_name) {
                    rapidxml::xml_node<>* owner_node = result_node->first_node();
                    for (; owner_node != NULL; owner_node = owner_node->next_sibling()) {
                        const std::string& owner_node_name = owner_node->name();
                        if ("DisplayName" == owner_node_name) {
                            summary.m_owner.m_display_name = owner_node->value();
                        } else if ("ID" == owner_node_name) {
                            summary.m_owner.m_id = owner_node->value();
                        } else {
                            SDK_LOG_WARN("Unknown field in owner node, field_name=%s.",
                                    owner_node_name.c_str());
                        }
                    }
                } else if ("ETag" == result_node_name) {
                    summary.m_etag = StringUtil::Trim(result_node->value(), "\"");
                } else if  ("Size" == result_node_name) {
                    summary.m_size = StringUtil::StringToUint64(result_node->value());
                } else if ("StorageClass" == result_node_name) {
                    summary.m_storage_class = result_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in DeleteMarker/Version node, field_name=%s.",
                            result_node_name.c_str());
                }
            }

            m_summaries.push_back(summary);
        } else {
            SDK_LOG_WARN("Unknown field in ListVersionsResult node, field_name=%s.",
                    node_name.c_str());
        }
    }

    delete[] cstr;
    return true;
}

// 解析GetBucketLoggingResponse body
bool GetBucketLoggingResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    
    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("BucketLoggingStatus");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=BucketLoggingConfiguration, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* loggingenabled_node = root->first_node("LoggingEnabled");
    if(NULL == loggingenabled_node) {
        SDK_LOG_ERR("Miss node=LoggingEnableConfiguration, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }
    rapidxml::xml_node<>* node = loggingenabled_node->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == "TargetBucket") {
            std::string targetbucket(node->value());
            m_rules.SetTargetBucket(targetbucket);
            SDK_LOG_DBG("TargetBucket value=%s", targetbucket.c_str());
           
        } else if(node_name == "TargetPrefix") {
            std::string targetprefix(node->value());
            m_rules.SetTargetPrefix(targetprefix);
            SDK_LOG_DBG("TargePrefix value=%s", targetprefix.c_str());

        } else {
            SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s",
                         node_name.c_str(), body.c_str());
        }
    }
    delete[] cstr;
    return true;
}

// 解析PutBucketDomainResponse body
bool PutBucketDomainResp::ParseFromXmlString(const std::string& body) {

    //绑定自定义域名，设置成功，返回http 200 OK，无body返回.
    if(body.empty()) {
        return true;
    }
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    
    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }
    
    rapidxml::xml_node<>* root = doc.first_node("Error");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=Error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == "Code") {
            std::string code(node->value());
            m_rules.SetCode(code);
            SDK_LOG_DBG("Code value=%s", code.c_str())
       
        } else if(node_name == "Message") {
            std::string message(node->value());
            m_rules.SetMessage(message);
            SDK_LOG_DBG("Message value=%s", message.c_str());

        } else if(node_name == "Resource") {
            std::string resource(node->value());
            m_rules.SetResource(resource);
            SDK_LOG_DBG("Resource value=%s", resource.c_str());

        } else if(node_name == "RequestId") {
            std::string requestid(node->value());
            m_rules.SetRequestid(requestid);
            SDK_LOG_DBG("RequestId value=%s", requestid.c_str());

        } else if(node_name == "TraceId") {
            std::string traceid(node->value());
            m_rules.SetTraceid(traceid);
            SDK_LOG_DBG("traceId value=%s", traceid.c_str());
        } else {
            SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s",
                          node_name.c_str(), body.c_str());
        }
    }
    delete[] cstr;
    return true;
}

// 解析GetBucketDomainResponse body
bool GetBucketDomainResp::ParseFromXmlString(const std::string& body) {

    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    
    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }
    
    rapidxml::xml_node<>* root = doc.first_node("DomainConfiguration");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=DomainConfiguration, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* domainrule_node = root->first_node("DomainRule");
    if(NULL == domainrule_node) {
        SDK_LOG_WARN("Miss node=DomainRule, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = domainrule_node->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == "Status") {
            std::string status(node->value());
            SetStatus(status);
            SDK_LOG_DBG("Status value=%s", status.c_str())
       
        } else if(node_name == "Name") {
            std::string name(node->value());
            SetName(name);
            SDK_LOG_DBG("Name value=%s", name.c_str());

        } else if(node_name == "Type") {
            std::string type(node->value());
            SetType(type);
            SDK_LOG_DBG("Type value=%s", type.c_str());
        } else {
            SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s",
                          node_name.c_str(), body.c_str());
        }
    }
    delete[] cstr;
    return true;
}


bool GetBucketWebsiteResp::ParseFromXmlRoutingRule(rapidxml::xml_node<>* node, RoutingRule& tmp_routingrule) {
    rapidxml::xml_node<>* rule_node = node->first_node();
    for( ; rule_node != NULL; rule_node = rule_node->next_sibling()) {
        const std::string& rule_node_name = rule_node->name();
        if(rule_node_name == "Condition") { 
            Condition temp_condition;                           
            rapidxml::xml_node<>* condition_node = rule_node->first_node();
            for(; condition_node != NULL; condition_node = condition_node->next_sibling()) {
                const std::string& condition_name = condition_node->name();
                if(condition_name == "HttpErrorCodeReturnedEquals") {
                    try {
                        std::string httperrorcodereturnequals(condition_node->value()); 
                        int httpcode = StringUtil::StringToInt(httperrorcodereturnequals);
                        temp_condition.SetHttpErrorCodeReturnedEquals(httpcode);
                    } catch(std::exception& e) {
                        continue;
                    }
                } else if(condition_name == "KeyPrefixEquals") {
                    std::string keyprefixequals(condition_node->value());
                    temp_condition.SetKeyPrefixEquals(keyprefixequals); 
                } else {
                    SDK_LOG_WARN("Condition have no contents, condition_name = %s.", 
                                            condition_name.c_str());
                    continue;
                }
            }
            tmp_routingrule.SetCondition(temp_condition);
        } else if(rule_node_name == "Redirect") {
            rapidxml::xml_node<>* redirect_node = rule_node->first_node();
            Redirect temp_redirect;
            for(; redirect_node != NULL; redirect_node = redirect_node->next_sibling()) {
                const std::string& redirect_node_name = redirect_node->name();
                if(redirect_node_name == "Protocol") {
                    std::string protocol(redirect_node->value());
                    temp_redirect.SetProtocol(protocol);
                } else if(redirect_node_name == "ReplaceKeyWith") {
                    std::string replacekeywith(redirect_node->value());
                    temp_redirect.SetReplaceKeyWith(replacekeywith);        
                } else if(redirect_node_name == "ReplaceKeyPrefixWith") {
                    std::string replacekeyprefixwith(redirect_node->value());
                    temp_redirect.SetReplaceKeyPrefixWith(replacekeyprefixwith);            
                } else {
                    SDK_LOG_WARN("Redirect have no contents, redirect_node_name = %s", 
                                                redirect_node_name.c_str());
                    continue;
                }
            }
            tmp_routingrule.SetRedirect(temp_redirect);
         } else {
            SDK_LOG_WARN("RoutingRule have no contents, rule_node_name = %s.", 
                                          rule_node_name.c_str());
            continue;
        }
    }
    return true;
}

// 解析GetBucketWebsiteResponse body
bool GetBucketWebsiteResp::ParseFromXmlString(const std::string& body) {

    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    
    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }
    
    rapidxml::xml_node<>* root = doc.first_node("WebsiteConfiguration");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=WebsiteConfiguration, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    
    for(; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if(node_name == "IndexDocument") {
            rapidxml::xml_node<>* suffix_node = node->first_node();
            const std::string& suffix_node_name = suffix_node->name();
            if(suffix_node_name == "Suffix") {
                std::string suffix_name(suffix_node->value());
                SetSuffix(suffix_name);
                SDK_LOG_DBG("suffix value=%s", suffix_name.c_str())
            } 
        } else if(node_name == "RedirectAllRequestsTo") {
            rapidxml::xml_node<>* protocol_node = node->first_node();
            const std::string& protocol_node_name = protocol_node->name();
            if(protocol_node_name == "Protocol") {
                std::string protocol_name(protocol_node->value());
                SetProtocol(protocol_name);
                SDK_LOG_DBG("Protocol value=%s", protocol_name.c_str())
            } 
        } else if(node_name == "ErrorDocument") {
            rapidxml::xml_node<>* key_node = node->first_node();
            const std::string& key_node_name = key_node->name();
            if(key_node_name == "Key") {
                std::string key_name(key_node->value());
                SetKey(key_name);
                SDK_LOG_DBG("key_name value=%s", key_name.c_str())
            } 
        } else if(node_name == "RoutingRules") {    
            rapidxml::xml_node<>* routingrules_node = node->first_node();
            for(; routingrules_node != NULL; routingrules_node = routingrules_node->next_sibling()) {
                const std::string& routingrules_node_name = routingrules_node->name();
                if(routingrules_node_name == "RoutingRule") {
                    RoutingRule tmp_routingrule;
                    ParseFromXmlRoutingRule(routingrules_node, tmp_routingrule);
                    AddRoutingRule(tmp_routingrule);        
                }
            }
        } else {    
            SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s", node_name.c_str(), body.c_str());
            continue;
        }
    }
    delete[] cstr;
    return true;
}

// 解析GetBucketTagging Response
bool GetBucketTaggingResp::ParseFromXmlString(const std::string &body) {
    
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    
    if (!StringUtil::StringToXml(cstr, &doc)) {
       SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
       delete[] cstr;
       return false;
    }
    
    rapidxml::xml_node<>* root = doc.first_node("Tagging");
    if(NULL == root) {
        SDK_LOG_ERR("Miss root node = Tagging, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;   
    }
    
    rapidxml::xml_node<>* TagSet_node = root->first_node("TagSet");
    if(NULL == TagSet_node) {
        SDK_LOG_WARN("Miss node = TagSet, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;   
    }
    
    rapidxml::xml_node<>* tag_node = TagSet_node->first_node();
    for(; tag_node != NULL; tag_node = tag_node->next_sibling()) {
        const std::string& tage_node_name = tag_node->name();
        if(tage_node_name == "Tag") {
            Tag temp_tag;
            rapidxml::xml_node<>*node = tag_node->first_node();
            for(; node != NULL; node = node->next_sibling()) {
                const std::string& node_name = node->name();
                if(node_name == "Key") {
                    std::string key(node->value());
                    temp_tag.SetKey(key);
                } else if(node_name == "Value") {
                    std::string value(node->value());
                    temp_tag.SetValue(value);
                } else {
                    continue;
                    SDK_LOG_WARN("Unknown field, field_name=%s, xml_body=%s",
                                  node_name.c_str(), body.c_str());
                }
            }
            AddTag(temp_tag);
        } else {
            continue;
        }   
    }   
    delete[] cstr;
    return true;            
}


bool GetBucketInventoryResp::ParseFromXmlCOSBucketDestination(rapidxml::xml_node<>* node, Inventory &temp_inventory) {
    rapidxml::xml_node<>* destination_node = node->first_node();
    COSBucketDestination temp_destination;
    for(; destination_node != NULL; destination_node = destination_node->next_sibling()) {
        const std::string& destination_node_name = destination_node->name();
        std::string destination_value = std::string(destination_node->value());
        if(destination_node_name == "Format") { 
            temp_destination.SetFormat(destination_value);      
        } else if(destination_node_name == "AccountId") {
            temp_destination.SetAccountId(destination_value);   
        } else if(destination_node_name == "Bucket") {
            temp_destination.SetBucket(destination_value);
        } else if(destination_node_name == "Prefix") {
            temp_destination.SetPrefix(destination_value);
        } else if(destination_node_name == "Encryption") {
            temp_destination.SetEncryption(true);
        }       
    }
    temp_inventory.SetCOSBucketDestination(temp_destination);   
    return true;
}

bool GetBucketInventoryResp::ParseFromXmlOptionalFields(rapidxml::xml_node<>* node, Inventory &temp_inventory) {
    rapidxml::xml_node<>* Field_node = node->first_node();
    OptionalFields temp_fields;
    for(; Field_node != NULL; Field_node = Field_node->next_sibling()) {
        std::string Field_value(Field_node->value());
        if(Field_value == "Size") {
            temp_fields.SetIsSize(true);
        } else if(Field_value == "LastModifiedDate") {
            temp_fields.SetIsLastModified(true);
        } else if(Field_value == "ETag") {
            temp_fields.SetIsEtag(true);
        } else if(Field_value == "StorageClass") {
            temp_fields.SetIsStorageClass(true);
        } else if(Field_value == "IsMultipartUploaded") {
            temp_fields.SetIsMultipartUploaded(true);
        } else if(Field_value == "ReplicationStatus") {
            temp_fields.SetIsReplicationStatus(true);
        } else {
            continue;
        }
    }
    temp_inventory.SetOptionalFields(temp_fields);
    return true;
}


bool GetBucketInventoryResp::ParseFromXmlInventoryConfiguration(rapidxml::xml_node<>* root, Inventory &temp_inventory) {
    rapidxml::xml_node<>* node = root->first_node();
    for(; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        std::string node_value = std::string(node->value());
        if(node_name == "Id") {
            std::string Id(node_value);
            temp_inventory.SetId(Id);
        } else if(node_name == "IsEnabled") {
            std::string IsEnabled(node_value);
            if(StringUtil::StringToLower(IsEnabled) == StringUtil::StringToLower("True"))
                temp_inventory.SetIsEnable(true);
            else 
                temp_inventory.SetIsEnable(false);
        } else if(node_name == "Destination") {
            rapidxml::xml_node<>* COSBucketDestination_node = node->first_node();
            if(std::string(COSBucketDestination_node->name()) == "COSBucketDestination") {
                ParseFromXmlCOSBucketDestination(COSBucketDestination_node, temp_inventory);
            }   
        } else if(node_name == "Schedule") {
            rapidxml::xml_node<>* Frequency_node = node->first_node();
            if(std::string(Frequency_node->name()) == "Frequency") {
                temp_inventory.SetFrequency(std::string(Frequency_node->value()));
            }
        } else if(node_name == "Filter") {
            rapidxml::xml_node<>* Prefix_node = node->first_node();
            if(std::string(Prefix_node->name()) == "Prefix") {
                temp_inventory.SetFilter(std::string(Prefix_node->value()));
            }
        } else if(node_name == "IncludedObjectVersions") {
            temp_inventory.SetIncludedObjectVersions(node_value);
        } else if(node_name == "OptionalFields") {
            ParseFromXmlOptionalFields(node, temp_inventory);
        } else {
            continue;
        }
    }
    return true;
}

// GetBucketInventory Response
bool GetBucketInventoryResp::ParseFromXmlString(const std::string &body) {
    
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    
    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }
    rapidxml::xml_node<>* root = doc.first_node("InventoryConfiguration");
    if(NULL == root) {
        SDK_LOG_ERR("Miss root node = InventoryConfiguration, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;   
    } else {
        Inventory temp_inventory;
        ParseFromXmlInventoryConfiguration(root, temp_inventory);
        SetInventory(temp_inventory);
    }
    delete[] cstr;
    return true;            
}

// ListBucketInventoryConfigurations Response
bool ListBucketInventoryConfigurationsResp::ParseFromXmlString(const std::string &body) {
    
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    
    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }
    
    rapidxml::xml_node<>* root = doc.first_node("ListInventoryConfigurationResult");
    if(NULL == root) {
        SDK_LOG_ERR("Miss root node = ListInventoryConfigurationResult, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;   
    }   
    rapidxml::xml_node<>* InventoryConfiguration_node = root->first_node();
    for(; InventoryConfiguration_node != NULL; 
        InventoryConfiguration_node = InventoryConfiguration_node->next_sibling()) {
        
        const std::string& InventoryConfiguration_node_name = InventoryConfiguration_node->name();
        if(InventoryConfiguration_node_name == "InventoryConfiguration") {      
            Inventory temp_inventory;
            ParseFromXmlInventoryConfiguration(InventoryConfiguration_node, temp_inventory);
            AddInventory(temp_inventory);               
        } else if(InventoryConfiguration_node_name == "IsTruncated") {
            std::string IsTruncated(InventoryConfiguration_node->value());
            if(StringUtil::StringToLower(IsTruncated) == StringUtil::StringToLower("true"))
                SetIsTruncated(true);
            else 
                SetIsTruncated(false);  
        } else if(InventoryConfiguration_node_name == "ContinuationToken") {
            SetContinuationToken(std::string(InventoryConfiguration_node->value()));    
        } else if(InventoryConfiguration_node_name == "NextContinuationToken") {
            SetNextContinuationToken(std::string(InventoryConfiguration_node->value()));
        } else {
            continue;
        }
    }
    delete[] cstr;
    return true;            
}

bool ListLiveChannelResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("ListLiveChannelResult");
    if (NULL == root) {
        SDK_LOG_ERR("Missing root node ListLiveChannelResult, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("MaxKeys" == node_name) {
            result.m_max_keys = node->value();
        } else if ("Marker" == node_name) {
            result.m_marker = node->value();
        } else if ("Prefix" == node_name) {
            result.m_prefix = node->value();
        } else if ("IsTruncated" == node_name) {
            result.m_is_truncated = node->value();
        } else if ("NextMarker" == node_name) {
            result.m_next_marker = node->value();
        } else if ("LiveChannel" == node_name) {
            LiveChannel channel;
            rapidxml::xml_node<>* live_channel_node = node->first_node();
            for (; live_channel_node != NULL; live_channel_node = live_channel_node->next_sibling()) {
                const std::string& node_name = live_channel_node->name();
                if ("Name" == node_name) {
                    channel.m_name = live_channel_node->value();
                } else if ("LastModified" == node_name) {
                    channel.m_last_modified = live_channel_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in LiveChannel, field_name=%s", node_name.c_str());
                    delete[] cstr;
                    return false;
                }
            }
            result.m_channels.push_back(channel);
        } else {
            SDK_LOG_WARN("Unknown field in ListLiveChannelResult, field_name=%s", node_name.c_str());
            delete[] cstr;
            return false;
        }
    }
    delete[] cstr;
    return true;
}

bool GetBucketIntelligentTieringResp::ParseFromXmlString(const std::string& body) {
    std::string tmp_body = body;
    rapidxml::xml_document<> doc;

    if (!StringUtil::StringToXml(&tmp_body[0], &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }
    
    rapidxml::xml_node<>* root = doc.first_node("IntelligentTieringConfiguration");
    if (NULL == root) {
        SDK_LOG_ERR("Missing root node IntelligentTieringConfiguration, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* intel_tiering_node = root->first_node();
    for (; intel_tiering_node != NULL; intel_tiering_node = intel_tiering_node->next_sibling()) {
        const std::string& node_name = intel_tiering_node->name();
        if("Status" == node_name) {
            m_status = intel_tiering_node->value();
        } else if("Transition" == node_name) {
            rapidxml::xml_node<>* trans_node = intel_tiering_node->first_node();
            for (; trans_node != NULL; trans_node = trans_node->next_sibling()) {
                const std::string& node_name = trans_node->name();
                if ("Days" == node_name) {
                    std::string days_str = trans_node->value();
                    m_days = StringUtil::StringToInt(days_str);
                } else if ("RequestFrequent" == node_name) {
                    std::string freq_str = trans_node->value();
                    m_freq = StringUtil::StringToInt(freq_str);
                } else {
                    SDK_LOG_WARN("Unknown field in Transition, field_name=%s", node_name.c_str());
                    return false;
                }
            }
        } else {
            SDK_LOG_WARN("Unknown field in IntelligentTieringConfiguration, field_name=%s", node_name.c_str());
            return false;
        }
    }
    return true;
}

} // namespace qcloud_cos

