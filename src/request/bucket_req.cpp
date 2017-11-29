// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 08/25/17
// Description:

#include "request/bucket_req.h"

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

#include "cos_sys_config.h"
#include <iostream>

namespace qcloud_cos {

bool PutBucketReplicationReq::GenerateRequestBody(std::string* body) const {
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element,
                                                doc.allocate_string("ReplicationConfiguration"),
                                                NULL);
    doc.append_node(root_node);

    if (!m_role.empty()) {
        root_node->append_node(doc.allocate_node(rapidxml::node_element, "Role",
                                            doc.allocate_string(m_role.c_str())));
    }

    if (m_rules.empty()) {
        SDK_LOG_ERR("Replication rule is empty.");
        return false;
    }

    for (size_t idx = 0; idx < m_rules.size(); ++idx) {
        rapidxml::xml_node<>* xml_rule = doc.allocate_node(rapidxml::node_element, "Rule", NULL);
        root_node->append_node(xml_rule);
        const ReplicationRule& rule = m_rules[idx];
        if (rule.m_is_enable) {
            xml_rule->append_node(doc.allocate_node(rapidxml::node_element, "Status", "Enabled"));
        } else {
            xml_rule->append_node(doc.allocate_node(rapidxml::node_element, "Status", "Disabled"));
        }

        if (!rule.m_id.empty()) {
            xml_rule->append_node(doc.allocate_node(rapidxml::node_element, "ID",
                                        doc.allocate_string(rule.m_id.c_str())));
        }

        xml_rule->append_node(doc.allocate_node(rapidxml::node_element, "Prefix",
                                        doc.allocate_string(rule.m_prefix.c_str())));

        rapidxml::xml_node<>* dest = doc.allocate_node(rapidxml::node_element,
                                                       "Destination", NULL);
        xml_rule->append_node(dest);

        if (rule.m_dest_bucket.empty()) {
            SDK_LOG_ERR("Dest bucket of replication rule is empty.");
            return false;
        }
        dest->append_node(doc.allocate_node(rapidxml::node_element, "Bucket",
                                    doc.allocate_string(rule.m_dest_bucket.c_str())));
        if (!rule.m_dest_storage_class.empty()) {
            dest->append_node(doc.allocate_node(rapidxml::node_element,
                    "StorageClass", doc.allocate_string(rule.m_dest_storage_class.c_str())));
        }
    }

    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();

    return true;
}

bool PutBucketACLReq::GenerateRequestBody(std::string* body) const {
    return GenerateAclRequestBody(m_owner, m_acl, body);
}

bool PutBucketLifecycleReq::GenerateRequestBody(std::string* body) const {
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element,
                                                doc.allocate_string("LifecycleConfiguration"),
                                                NULL);
    doc.append_node(root_node);

    for (std::vector<LifecycleRule>::const_iterator c_itr = m_rules.begin();
         c_itr != m_rules.end(); ++c_itr) {
        const LifecycleRule& rule = *c_itr;
        rapidxml::xml_node<>* rule_node = doc.allocate_node(rapidxml::node_element,
                                                            "Rule", NULL);
        if (rule.HasId()) {
            rule_node->append_node(doc.allocate_node(rapidxml::node_element, "ID",
                                         doc.allocate_string(rule.GetId().c_str())));
        }

        if (rule.HasIsEnable()) {
            rule_node->append_node(doc.allocate_node(rapidxml::node_element, "Status",
                                             rule.IsEnable() ? "Enabled" : "Disabled"));
        }

        if (rule.HasFilter()) {
            rapidxml::xml_node<>* filter_node = doc.allocate_node(rapidxml::node_element,
                                                                  "Filter", NULL);
            LifecycleFilter filter = rule.GetFilter();
            std::vector<LifecycleTag> tags = filter.GetTags();
            int cnt = tags.size();
            if (filter.HasPrefix()) {
                ++cnt;
            }

            // Prefix/Tag的父节点可能是Filter/And(PutLifecycle不考虑直接作为Rule的子节点)
            rapidxml::xml_node<>* parent_node = filter_node;
            // 多个筛选条件需要用and
            if (cnt >= 2) {
                rapidxml::xml_node<>* and_node = doc.allocate_node(rapidxml::node_element,
                                                                   "And", NULL);
                filter_node->append_node(and_node);
                parent_node = and_node;
            }

            if (filter.HasPrefix()) {
                std::string prefix = filter.GetPrefix();
                rapidxml::xml_node<>* prefix_node = doc.allocate_node(rapidxml::node_element,
                                                "Prefix", doc.allocate_string(prefix.c_str()));
                parent_node->append_node(prefix_node);
            }

            for (std::vector<LifecycleTag>::const_iterator tag_itr = tags.begin();
                 tag_itr != tags.end(); ++tag_itr) {
                const LifecycleTag& tag = *tag_itr;
                rapidxml::xml_node<>* tag_node = doc.allocate_node(rapidxml::node_element,
                                                                   "Tag", NULL);
                tag_node->append_node(doc.allocate_node(rapidxml::node_element, "Key",
                                                doc.allocate_string(tag.key.c_str())));
                tag_node->append_node(doc.allocate_node(rapidxml::node_element, "Value",
                                                doc.allocate_string(tag.value.c_str())));
                parent_node->append_node(tag_node);
            }
            rule_node->append_node(filter_node);
        }

        // Expiration
        if (rule.HasExpiration()) {
            LifecycleExpiration expiration = rule.GetExpiration();
            rapidxml::xml_node<>* expir_node = doc.allocate_node(rapidxml::node_element,
                                                                 "Expiration", NULL);
            if (expiration.HasDays()) {
                std::string days = StringUtil::Uint64ToString(expiration.GetDays());
                expir_node->append_node(doc.allocate_node(rapidxml::node_element,
                                    "Days", doc.allocate_string(days.c_str())));
            } else if (expiration.HasDate()) {
                expir_node->append_node(doc.allocate_node(rapidxml::node_element,
                                    "Date", doc.allocate_string(expiration.GetDate().c_str())));
            }

            if (expiration.HasExpiredObjDelMarker()) {
                expir_node->append_node(doc.allocate_node(rapidxml::node_element,
                                    "ExpiredObjectDeleteMarker",
                                    expiration.IsExpiredObjDelMarker() ? "true" : "false"));
            }
            rule_node->append_node(expir_node);
        }

        // Transition
        if (rule.HasTransition()) {
            const std::vector<LifecycleTransition>& transitions = rule.GetTransitions();
            for (std::vector<LifecycleTransition>::const_iterator c_itr = transitions.begin();
                 c_itr != transitions.end(); ++c_itr) {
                LifecycleTransition transition = *c_itr;
                rapidxml::xml_node<>* trans_node = doc.allocate_node(rapidxml::node_element,
                                                                     "Transition", NULL);
                if (transition.HasDays()) {
                    std::string days = StringUtil::Uint64ToString(transition.GetDays());
                    trans_node->append_node(doc.allocate_node(rapidxml::node_element,
                                                              "Days", doc.allocate_string(days.c_str())));
                } else if (transition.HasDate()) {
                    trans_node->append_node(doc.allocate_node(rapidxml::node_element,
                                                              "Date", doc.allocate_string(transition.GetDate().c_str())));
                }

                if (transition.HasStorageClass()) {
                    trans_node->append_node(doc.allocate_node(rapidxml::node_element,
                                                              "StorageClass", doc.allocate_string(transition.GetStorageClass().c_str())));
                }
                rule_node->append_node(trans_node);
            }
        }

        // NonCurrTransition
        if (rule.HasNonCurrTransition()) {
            LifecycleNonCurrTransition transition = rule.GetNonCurrTransition();
            rapidxml::xml_node<>* trans_node = doc.allocate_node(rapidxml::node_element,
                                                         "NoncurrentVersionTransition", NULL);
            if (transition.HasDays()) {
                std::string days = StringUtil::Uint64ToString(transition.GetDays());
                trans_node->append_node(doc.allocate_node(rapidxml::node_element,
                                    "Days", doc.allocate_string(days.c_str())));
            }

            if (transition.HasStorageClass()) {
                trans_node->append_node(doc.allocate_node(rapidxml::node_element,
                    "StorageClass", doc.allocate_string(transition.GetStorageClass().c_str())));
            }
            rule_node->append_node(trans_node);
        }

        // NonCurrExpiration
        if (rule.HasNonCurrExpiration()) {
            LifecycleNonCurrExpiration expiration = rule.GetNonCurrExpiration();
            rapidxml::xml_node<>* expir_node = doc.allocate_node(rapidxml::node_element,
                                                         "NoncurrentVersionExpiration", NULL);
            if (expiration.HasDays()) {
                std::string days = StringUtil::Uint64ToString(expiration.GetDays());
                expir_node->append_node(doc.allocate_node(rapidxml::node_element,
                                    "Days", doc.allocate_string(days.c_str())));
            }
            rule_node->append_node(expir_node);
        }

        // AbortIncomMultiUpload
        if (rule.HasAbortIncomMultiUpload()) {
            rapidxml::xml_node<>* abort_node = doc.allocate_node(rapidxml::node_element,
                                                     "AbortIncompleteMultipartUpload", NULL);
            std::string days = StringUtil::Uint64ToString(
                rule.GetAbortIncompleteMultiUpload().m_days_after_init);
            abort_node->append_node(doc.allocate_node(rapidxml::node_element,
                                    "DaysAfterInitiation", doc.allocate_string(days.c_str())));
            rule_node->append_node(abort_node);
        }
        root_node->append_node(rule_node);
    }

    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();

    SDK_LOG_DBG("PutBucketLifecycle Request Body=[%s]", body->c_str());
    return true;
}

bool PutBucketCORSReq::GenerateRequestBody(std::string* body) const {
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element,
                                                doc.allocate_string("CORSConfiguration"),
                                                NULL);
    doc.append_node(root_node);

    for (std::vector<CORSRule>::const_iterator c_itr = m_rules.begin();
         c_itr != m_rules.end(); ++c_itr) {
        const CORSRule& rule = *c_itr;
        rapidxml::xml_node<>* rule_node = doc.allocate_node(rapidxml::node_element,
                                                            "CORSRule", NULL);
        // ID,AllowedHeader,MaxAgeSeconds,ExposeHeader为非必选项
        if (!rule.m_id.empty()) {
            rule_node->append_node(doc.allocate_node(rapidxml::node_element, "ID",
                                                 doc.allocate_string(rule.m_id.c_str())));
        }

        if (!rule.m_max_age_secs.empty()) {
            rule_node->append_node(doc.allocate_node(rapidxml::node_element, "MaxAgeSeconds",
                                     doc.allocate_string(rule.m_max_age_secs.c_str())));
        }

        for (std::vector<std::string>::const_iterator c_itr = rule.m_allowed_headers.begin();
             c_itr != rule.m_allowed_headers.end(); ++c_itr) {
            rule_node->append_node(doc.allocate_node(rapidxml::node_element, "AllowedHeader",
                                     doc.allocate_string(c_itr->c_str())));
        }


        for (std::vector<std::string>::const_iterator c_itr = rule.m_expose_headers.begin();
             c_itr != rule.m_expose_headers.end(); ++c_itr) {
            rule_node->append_node(doc.allocate_node(rapidxml::node_element, "ExposeHeader",
                                     doc.allocate_string(c_itr->c_str())));
        }

        for (std::vector<std::string>::const_iterator c_itr = rule.m_allowed_origins.begin();
             c_itr != rule.m_allowed_origins.end(); ++c_itr) {
            rule_node->append_node(doc.allocate_node(rapidxml::node_element, "AllowedOrigin",
                                     doc.allocate_string(c_itr->c_str())));
        }

        for (std::vector<std::string>::const_iterator c_itr = rule.m_allowed_methods.begin();
             c_itr != rule.m_allowed_methods.end(); ++c_itr) {
            rule_node->append_node(doc.allocate_node(rapidxml::node_element, "AllowedMethod",
                                     doc.allocate_string(c_itr->c_str())));
        }

        root_node->append_node(rule_node);
    }

    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();

    return true;
}

bool PutBucketVersioningReq::GenerateRequestBody(std::string* body) const {
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element,
                                                doc.allocate_string("VersioningConfiguration"),
                                                NULL);
    doc.append_node(root_node);

    std::string status = m_status ? "Enabled" : "Suspended";
    root_node->append_node(doc.allocate_node(rapidxml::node_element, "Status",
                                             doc.allocate_string(status.c_str())));
    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();

    return true;
}

} // namespace qcloud_cos
