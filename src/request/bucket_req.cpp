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
        root_node->append_node(doc.allocate_node(rapidxml::node_element, "Role", m_role.c_str()));
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
            xml_rule->append_node(doc.allocate_node(rapidxml::node_element,
                                                    "ID", rule.m_id.c_str()));
        }

        xml_rule->append_node(doc.allocate_node(rapidxml::node_element, "Prefix",
                                                rule.m_prefix.c_str()));

        rapidxml::xml_node<>* dest = doc.allocate_node(rapidxml::node_element,
                                                       "Destination", NULL);
        xml_rule->append_node(dest);

        if (rule.m_dest_bucket.empty()) {
            SDK_LOG_ERR("Dest bucket of replication rule is empty.");
            return false;
        }
        dest->append_node(doc.allocate_node(rapidxml::node_element, "Bucket",
                                            rule.m_dest_bucket.c_str()));
        if (!rule.m_dest_storage_class.empty()) {
            dest->append_node(doc.allocate_node(rapidxml::node_element,
                                            "StorageClass", rule.m_dest_storage_class.c_str()));
        }
    }

    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();

    return true;
}

} // namespace qcloud_cos
