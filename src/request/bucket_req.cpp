// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 08/25/17
// Description:

#include <sstream>
#include "request/bucket_req.h"

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

#include "cos_sys_config.h"

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
                                    "NoncurrentDays", doc.allocate_string(days.c_str())));
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
                                    "NoncurrentDays", doc.allocate_string(days.c_str())));
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

// PutBucketLogging需要设置TargetBucket和Targetprefix
bool PutBucketLoggingReq::GenerateRequestBody(std::string* body) const {

    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element, 
                                      doc.allocate_string("BucketLoggingStatus"), NULL);
    doc.append_node(root_node);
    
    if(HasLoggingEnabled()) {
        rapidxml::xml_node<>* LoggingEnabled_node = doc.allocate_node(rapidxml::node_element, 
                                            doc.allocate_string("LoggingEnabled"), NULL);
        if(m_rules.HasTargetBucket()) {
            LoggingEnabled_node->append_node(doc.allocate_node(rapidxml::node_element,
                            doc.allocate_string("TargetBucket"), 
                            doc.allocate_string(m_rules.GetTargetBucket().c_str())));
        }
        if(m_rules.HasTargetPrefix()) {
            LoggingEnabled_node->append_node(doc.allocate_node(rapidxml::node_element,
                            doc.allocate_string("TargetPrefix"),
                            doc.allocate_string(m_rules.GetTargetPrefix().c_str())));
        }
        root_node->append_node(LoggingEnabled_node);    
    }
    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();
    return true;      
}

// PutBucketDomain需要设置status, name, type和forcereplacement.
bool PutBucketDomainReq::GenerateRequestBody(std::string* body) const {

    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element, 
                                            doc.allocate_string("DomainConfiguration"), NULL);
    doc.append_node(root_node);

    rapidxml::xml_node<>* DomainRule_node = doc.allocate_node(rapidxml::node_element, 
                                            doc.allocate_string("DomainRule"),  NULL);
    if(m_rules.HasStatus()) {
        DomainRule_node->append_node(doc.allocate_node(rapidxml::node_element,
                                     doc.allocate_string("Status"), 
                                     doc.allocate_string(m_rules.GetStatus().c_str())));
    } else {
        SDK_LOG_ERR("PutBucketDomain need to set Status.");
    }
    
    if(m_rules.HasName()) {
        DomainRule_node->append_node(doc.allocate_node(rapidxml::node_element,
                                doc.allocate_string("Name"),
                                doc.allocate_string(m_rules.GetName().c_str())));
    } else {
        SDK_LOG_ERR("PutBucketDomain need to set Name.");
    }
    
    if(m_rules.HasType()) {
        DomainRule_node->append_node(doc.allocate_node(rapidxml::node_element,
                                doc.allocate_string("Type"),
                                doc.allocate_string(m_rules.GetType().c_str())));
    } else {
        SDK_LOG_ERR("PutBucketDomain need to set Type.");
    }
    if(m_rules.HasForcedrePlacement()) {
        DomainRule_node->append_node(doc.allocate_node(rapidxml::node_element,
                                doc.allocate_string("ForceReplacement"),
                                doc.allocate_string(m_rules.GetForcedReplacement().c_str())));
    }
    root_node->append_node(DomainRule_node);    
    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();
    return true;       
}

// PutBucketWebsite需要设置Suffix.
bool PutBucketWebsiteReq::GenerateRequestBody(std::string* body) const {
    
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element, 
                                      doc.allocate_string("WebsiteConfiguration"), NULL);
    doc.append_node(root_node);

    if(HasSuffix()) {
        rapidxml::xml_node<>* IndexDocument_node = doc.allocate_node(rapidxml::node_element, 
                                                   doc.allocate_string("IndexDocument"), NULL);
        IndexDocument_node->append_node(doc.allocate_node(rapidxml::node_element,
                                        doc.allocate_string("Suffix"),
                                        doc.allocate_string(GetSuffix().c_str())));
        root_node->append_node(IndexDocument_node); 
        if(HasProtocol()) {
            rapidxml::xml_node<>* RedirectAllRequestsTo_node = doc.allocate_node(rapidxml::node_element, 
                                                    doc.allocate_string("RedirectAllRequestsTo"), NULL);    
            RedirectAllRequestsTo_node->append_node(doc.allocate_node(rapidxml::node_element,
                                                    doc.allocate_string("Protocol"),
                                                    doc.allocate_string(GetProtocol().c_str())));
            root_node->append_node(RedirectAllRequestsTo_node);              
        }
        if(HasKey()) {
            
            rapidxml::xml_node<>* ErrorDocument_node = doc.allocate_node(rapidxml::node_element, 
                                                doc.allocate_string("ErrorDocument"), NULL);                                        
            ErrorDocument_node->append_node(doc.allocate_node(rapidxml::node_element,
                           doc.allocate_string("Key"), doc.allocate_string(GetKey().c_str())));         
            root_node->append_node(ErrorDocument_node); 
        }
        if(HasRoutingRules()) {
            rapidxml::xml_node<>* RoutingRules_node = doc.allocate_node(rapidxml::node_element, 
                                            doc.allocate_string("RoutingRules"), NULL); 
            for(std::vector<RoutingRule>::const_iterator c_itr = m_routingrules.begin();
                c_itr != m_routingrules.end(); ++c_itr) {
                rapidxml::xml_node<>* routerule_node = doc.allocate_node(rapidxml::node_element, 
                                            doc.allocate_string("RoutingRule"), NULL);
                
                if(c_itr->HasCondition()) {
                    rapidxml::xml_node<>* condition_node = doc.allocate_node(rapidxml::node_element, 
                                            doc.allocate_string("Condition"), NULL);
                    if(c_itr->GetCondition().HasHttpErrorCodeReturnedEquals()) {
                        std::string http_codestr = StringUtil::IntToString(c_itr->GetCondition().GetHttpErrorCodeReturnedEquals());
                        condition_node->append_node(doc.allocate_node(rapidxml::node_element,
                                    doc.allocate_string("HttpErrorCodeReturnedEquals"),
                                    doc.allocate_string(http_codestr.c_str())));
                    }
                    
                    if(c_itr->GetCondition().HasKeyPrefixEquals()) {
                        condition_node->append_node(doc.allocate_node(rapidxml::node_element,
                                    doc.allocate_string("KeyPrefixEquals"),
                        doc.allocate_string(c_itr->GetCondition().GetKeyPrefixEquals().c_str())));  
                    }
                    routerule_node->append_node(condition_node);
                }
                if(c_itr->HasRedirect()) {
                    rapidxml::xml_node<>* redirect_node = doc.allocate_node(rapidxml::node_element, 
                                doc.allocate_string("Redirect"), NULL);
                        
                    if(c_itr->GetRedirect().HasProtocol()) {
                        redirect_node->append_node(doc.allocate_node(rapidxml::node_element,
                                doc.allocate_string("Protocol"),
                                doc.allocate_string(c_itr->GetRedirect().GetProtocol().c_str())));          
                    }
                    if(c_itr->GetRedirect().HasReplaceKeyWith()) {
                        redirect_node->append_node(doc.allocate_node(rapidxml::node_element, 
                                doc.allocate_string("ReplaceKeyWith"),
                                doc.allocate_string(c_itr->GetRedirect().GetReplaceKeyWith().c_str())));    
                    }
                    if(c_itr->GetRedirect().HasReplaceKeyPrefixWith() && c_itr->GetCondition().HasKeyPrefixEquals()) {
                        redirect_node->append_node(doc.allocate_node(rapidxml::node_element, 
                                doc.allocate_string("ReplaceKeyPrefixWith"),
                                doc.allocate_string(c_itr->GetRedirect().GetReplaceKeyPrefixWith().c_str())));
                    }
                    routerule_node->append_node(redirect_node);
                }
                RoutingRules_node->append_node(routerule_node); 
            }
            root_node->append_node(RoutingRules_node);
        }   
    } else {
        SDK_LOG_ERR("PutBucketWebsite need to set Suffix.");
        doc.clear();
        return false;   
    }                   
    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();
    return true;        
}

// 设置标签集合.
bool PutBucketTaggingReq::GenerateRequestBody(std::string* body) const {
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element,
                                      doc.allocate_string("Tagging"), NULL);
    doc.append_node(root_node);
    rapidxml::xml_node<>* TagSet_node = doc.allocate_node(rapidxml::node_element, 
                                        doc.allocate_string("TagSet"), NULL);
    
    for(std::vector<Tag>::const_iterator c_itr = m_tagset.begin(); 
        c_itr != m_tagset.end(); ++c_itr) {         
        rapidxml::xml_node<>* tag_node = doc.allocate_node(rapidxml::node_element, 
                                    doc.allocate_string("Tag"), NULL);
        if(c_itr->HasKey()) {               
            tag_node->append_node(doc.allocate_node(rapidxml::node_element,
                doc.allocate_string("Key"), doc.allocate_string(c_itr->GetKey().c_str())));
        } else {
            SDK_LOG_ERR("PutBucketTagging need to set Key.");       
        }
        if(c_itr->HasValue()) {         
            tag_node->append_node(doc.allocate_node(rapidxml::node_element,
                doc.allocate_string("Value"), doc.allocate_string(c_itr->GetValue().c_str())));
        } else {
            SDK_LOG_ERR("PutBucketTagging need to set Value.");
        }
        TagSet_node->append_node(tag_node);
    }
    root_node->append_node(TagSet_node);
    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();
    return true;
}


bool PutBucketInventoryReq::GenerateRequestBody(std::string* body) const {
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element,
                    doc.allocate_string("InventoryConfiguration"), NULL);
    doc.append_node(root_node);
    
    if(HasInventory()) {
        const Inventory& temp_inventory = GetInventory();
        if(temp_inventory.HasId()) {
            rapidxml::xml_node<>* Id_node = doc.allocate_node(rapidxml::node_element,
                    doc.allocate_string("Id"), doc.allocate_string(temp_inventory.GetId().c_str()));
            root_node->append_node(Id_node);
        } else {
            SDK_LOG_ERR("This inventory neet to set Id.");
            doc.clear();
            return false;
        }
        
        if(temp_inventory.HasIsEnable()) {
            const std::string is_enabled_str = temp_inventory.GetIsEnable()? "True":"False";
            rapidxml::xml_node<>* IsEnable_node = doc.allocate_node(rapidxml::node_element,
                                doc.allocate_string("IsEnabled"),
                                doc.allocate_string(is_enabled_str.c_str()));
            root_node->append_node(IsEnable_node);
            
        } else {
            SDK_LOG_ERR("This inventory neet to set IsEnabled.");
            doc.clear();
            return false;   
        }
        
        rapidxml::xml_node<>* Destination_node = doc.allocate_node(rapidxml::node_element,
                        doc.allocate_string("Destination"), NULL);
        
        if(temp_inventory.HasCOSBucketDestination()) {
            
            rapidxml::xml_node<>* COSBucketDestination_node = doc.allocate_node(rapidxml::node_element,
                        doc.allocate_string("COSBucketDestination"), NULL);
                                                
            if(temp_inventory.GetCOSBucketDestination().HasFormat()) {
                COSBucketDestination_node->append_node(doc.allocate_node(rapidxml::node_element,
                        doc.allocate_string("Format"), 
                        doc.allocate_string(temp_inventory.GetCOSBucketDestination().GetFormat().c_str())));
            } else {
                SDK_LOG_ERR("This COSBucketDestination neet to set Format.");
                doc.clear();
                return false;   
            }       
            if(temp_inventory.GetCOSBucketDestination().HasAccountId()) {
                COSBucketDestination_node->append_node(doc.allocate_node(rapidxml::node_element,
                    doc.allocate_string("AccountId"), 
                    doc.allocate_string(temp_inventory.GetCOSBucketDestination().GetAccountId().c_str())));
            }
            
            if(temp_inventory.GetCOSBucketDestination().HasBucket()) {
                COSBucketDestination_node->append_node(doc.allocate_node(rapidxml::node_element,
                    doc.allocate_string("Bucket"), 
                    doc.allocate_string(temp_inventory.GetCOSBucketDestination().GetBucket().c_str())));
            } else {
                SDK_LOG_ERR("This COSBucketDestination neet to set Bucket.");
                doc.clear();
                return false;   
            }
            
            if(temp_inventory.GetCOSBucketDestination().HasPrefix()) {
                COSBucketDestination_node->append_node(doc.allocate_node(rapidxml::node_element,
                    doc.allocate_string("Prefix"), 
                    doc.allocate_string(temp_inventory.GetCOSBucketDestination().GetPrefix().c_str())));
            }
            
            if(temp_inventory.GetCOSBucketDestination().HasEncryption()) {
                if(temp_inventory.GetCOSBucketDestination().GetEncryption()) {
                    rapidxml::xml_node<>* Encryption_node =  doc.allocate_node(rapidxml::node_element,
                                doc.allocate_string("Encryption"), NULL);
                    Encryption_node->append_node(doc.allocate_node(rapidxml::node_element, 
                                            doc.allocate_string("SSE-COS"), doc.allocate_string(" ")));
                    COSBucketDestination_node->append_node(Encryption_node);                                    
                }       
            }   
            Destination_node->append_node(COSBucketDestination_node);   
        } else {
            SDK_LOG_ERR("This inventory neet to set COSBucketDestination.");
            doc.clear();
            return false;       
        }
        root_node->append_node(Destination_node);   
        
        if(temp_inventory.HasFrequency()) {
            rapidxml::xml_node<>* Schedule_node = doc.allocate_node(rapidxml::node_element,
                            doc.allocate_string("Schedule"), NULL);
            
            Schedule_node->append_node(doc.allocate_node(rapidxml::node_element,
                            doc.allocate_string("Frequency"), 
                            doc.allocate_string(temp_inventory.GetFrequency().c_str())));
            root_node->append_node(Schedule_node);                                          
        } else {    
            SDK_LOG_ERR("This inventory neet to set Schedule Frequency.");
            doc.clear();
            return false;
        }
        
        if(temp_inventory.HasFilter()) {
            rapidxml::xml_node<>* Filter_node = doc.allocate_node(rapidxml::node_element,
                                            doc.allocate_string("Filter"), NULL);
            
            Filter_node->append_node(doc.allocate_node(rapidxml::node_element,
                                doc.allocate_string("Prefix"), 
                                doc.allocate_string(temp_inventory.GetFilter().c_str())));
                                                
            root_node->append_node(Filter_node);            
        }
        
        if(temp_inventory.HasIncludedObjectVersions()) {
            root_node->append_node(doc.allocate_node(rapidxml::node_element,
                    doc.allocate_string("IncludedObjectVersions"), 
                    doc.allocate_string(temp_inventory.GetIncludedObjectVersions().c_str())));      
        } else {
            SDK_LOG_ERR("This inventory neet to set IncludedObjectVersions.");
            doc.clear();
            return false;
        }
        
        if(temp_inventory.HasOptionalFields()) {
            rapidxml::xml_node<>* OptionalFields_node = doc.allocate_node(rapidxml::node_element,
                            doc.allocate_string("OptionalFields"), NULL);
            if(temp_inventory.GetOptionalFields().HasIsSize()
                && temp_inventory.GetOptionalFields().GetIsSize()) {
                OptionalFields_node->append_node(doc.allocate_node(rapidxml::node_element,
                        doc.allocate_string("Field"), doc.allocate_string("Size")));
            }
            
            if(temp_inventory.GetOptionalFields().HasIsLastModified() 
                && temp_inventory.GetOptionalFields().GetIsLastModified()) {
                OptionalFields_node->append_node(doc.allocate_node(rapidxml::node_element,
                                            doc.allocate_string("Field"),
                                            doc.allocate_string("LastModifiedDate")));
            }
            
            if(temp_inventory.GetOptionalFields().HasIsETag() 
                && temp_inventory.GetOptionalFields().GetIsETag()) {
                OptionalFields_node->append_node(doc.allocate_node(rapidxml::node_element,
                        doc.allocate_string("Field"), 
                        doc.allocate_string("ETag")));
            }
            
            if(temp_inventory.GetOptionalFields().HasIsStorageClass()
               && temp_inventory.GetOptionalFields().GetIsStorageClass()) {
                OptionalFields_node->append_node(doc.allocate_node(rapidxml::node_element,
                                            doc.allocate_string("Field"),
                                            doc.allocate_string("StorageClass")));
            }
            
            if(temp_inventory.GetOptionalFields().HasIsMultipartUploaded() 
                && temp_inventory.GetOptionalFields().GetIsMultipartUploaded()) {
                OptionalFields_node->append_node(doc.allocate_node(rapidxml::node_element,
                                            doc.allocate_string("Field"),
                                            doc.allocate_string("IsMultipartUploaded")));
            }
            
            if(temp_inventory.GetOptionalFields().HasIsReplicationStatus() 
                && temp_inventory.GetOptionalFields().GetIsReplicationStatus()) {
                OptionalFields_node->append_node(doc.allocate_node(rapidxml::node_element,
                                            doc.allocate_string("Field"), 
                                            doc.allocate_string("ReplicationStatus")));
            }   
            root_node->append_node(OptionalFields_node);        
        }   
    } else {    
        SDK_LOG_ERR("PutBucketInventory neet to set Inventory.");
        doc.clear();
        return false;
    }
    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();
    return true;    
}

bool PutBucketIntelligentTieringReq::GenerateRequestBody(std::string* body) const {
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element,
                                                doc.allocate_string("IntelligentTieringConfiguration"),
                                                NULL);
    doc.append_node(root_node);

    std::string status = m_status ? "Enabled" : "Suspended";
    root_node->append_node(doc.allocate_node(rapidxml::node_element, "Status",
                                             doc.allocate_string(status.c_str())));

    rapidxml::xml_node<>* transition_node = doc.allocate_node(rapidxml::node_element,
                                                        doc.allocate_string("Transition"), NULL);
    transition_node->append_node(doc.allocate_node(rapidxml::node_element, doc.allocate_string("Days"),
                                  doc.allocate_string(StringUtil::IntToString(m_days).c_str())));
    transition_node->append_node(doc.allocate_node(rapidxml::node_element, doc.allocate_string("RequestFrequent"),
                                    doc.allocate_string("1")));
    root_node->append_node(transition_node);
    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();

    return true;
}

} // namespace qcloud_cos
