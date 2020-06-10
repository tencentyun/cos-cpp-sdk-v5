// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "response/object_resp.h"

#include <stdio.h>
#include <string.h>

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

#include "cos_params.h"
#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

bool InitMultiUploadResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kInitiateMultipartUploadRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=InitiateMultipartUploadResult, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == kInitiateMultipartUploadBucket) {
            m_bucket = node->value();
        } else if (node_name == kInitiateMultipartUploadKey) {
            m_key = node->value();
        } else if (node_name == kInitiateMultipartUploadId) {
            m_upload_id = node->value();
        }
    }

    delete[] cstr;
    return true;
}

bool CompleteMultiUploadResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=[%s]", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kCompleteMultiUploadRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=ListBucketsResult, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if (node_name == kCompleteMultiUploadLocation) {
            m_location = node->value();
        } else if (node_name == kCompleteMultiUploadBucket) {
            m_bucket = node->value();
        } else if (node_name == kCompleteMultiUploadKey) {
            m_key = node->value();
        } else if (node_name == kCompleteMultiUploadETag) {
            SetEtag(StringUtil::Trim(node->value(), "\""));
        }
    }

    delete[] cstr;
    return true;
}

void GetObjectResp::ParseFromHeaders(const std::map<std::string, std::string>& headers) {
    BaseResp::ParseFromHeaders(headers);
    std::map<std::string, std::string>::const_iterator itr;
    itr = headers.find(kRespHeaderLastModified);
    if (headers.end() != itr) {
        m_last_modified = itr->second;
    }

    itr = headers.find(kRespHeaderXCosObjectType);
    if (headers.end() != itr) {
        m_x_cos_object_type = itr->second;
    }

    itr = headers.find(kRespHeaderXCosStorageClass);
    if (headers.end() != itr) {
        m_x_cos_storage_class = itr->second;
    }
}

void HeadObjectResp::ParseFromHeaders(const std::map<std::string, std::string>& headers) {
    BaseResp::ParseFromHeaders(headers);
    std::map<std::string, std::string>::const_iterator itr;
    for (itr = headers.begin(); itr != headers.end(); ++itr) {
        const std::string& key = itr->first;
        // TODO 可以直接get header
        if (key == kRespHeaderLastModified) {
            m_last_modified = itr->second;
        } else if (key == kRespHeaderXCosObjectType) {
            m_x_cos_object_type = itr->second;
        } else if (key == kRespHeaderXCosStorageClass) {
            m_x_cos_storage_class = itr->second;
        } else if (StringUtil::StringStartsWith(key, kXCosMetaPrefix)) {
            std::string remove_prefix = StringUtil::StringRemovePrefix(key, kXCosMetaPrefix);
            m_x_cos_metas[remove_prefix] = itr->second;
        }
    }
}

void MultiUploadObjectResp::CopyFrom(const InitMultiUploadResp& resp) {
    m_resp_tag = "Init";
    InternalCopyFrom(resp);
    m_upload_id = resp.GetUploadId();
    m_bucket = resp.GetBucket();
    m_key = resp.GetKey();
}

// TODO(sevenyou)
void MultiUploadObjectResp::CopyFrom(const UploadPartDataResp& resp) {
    m_resp_tag = "Upload";
    InternalCopyFrom(resp);
}

void MultiUploadObjectResp::CopyFrom(const CompleteMultiUploadResp& resp) {
    m_resp_tag = "Complete";
    InternalCopyFrom(resp);
    m_location = resp.GetLocation();
    m_bucket = resp.GetBucket();
    m_key = resp.GetKey();
}

bool ListPartsResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("ListPartsResult");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=ListPartsResult, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("Bucket" == node_name) {
            m_bucket = node->value();
        } else if ("EncodingType" == node_name) {
            m_encoding_type = node->value();
        } else if ("Encoding-type" == node_name) {
	    m_encoding_type = node->value();
	} else if ("Key" == node_name) {
            m_key = node->value();
        } else if ("UploadId" == node_name) {
            m_upload_id = node->value();
        } else if ("Initiator" == node_name) {
            rapidxml::xml_node<>* init_node = node->first_node();
            for (; init_node != NULL; init_node = init_node->next_sibling()) {
                const std::string& init_node_name = init_node->name();
                if ("ID" == init_node_name) {
                    m_initiator.m_id = init_node->value();
                } else if ("DisplyName" == init_node_name) {
                    m_initiator.m_display_name = init_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in Initiator node, field_name=%s",
                                 init_node_name.c_str());
                }
            }
        } else if ("Owner" == node_name) {
            rapidxml::xml_node<>* owner_node = node->first_node();
            for (; owner_node != NULL; owner_node = owner_node->next_sibling()) {
                const std::string& owner_node_name = owner_node->name();
                if ("ID" == owner_node_name) {
                    m_owner.m_id = owner_node->value();
                } else if ("DisplyName" == owner_node_name) {
                    m_owner.m_display_name = owner_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in Owner node, field_name=%s",
                                 owner_node_name.c_str());
                }
            }
        } else if ("PartNumberMarker" == node_name) {
            m_part_number_marker = StringUtil::StringToUint64(node->value());
        } else if ("Part" == node_name) {
            rapidxml::xml_node<>* part_node = node->first_node();
            Part part;
            for (; part_node != NULL; part_node = part_node->next_sibling()) {
                const std::string& part_node_name = part_node->name();
                if ("PartNumber" == part_node_name) {
                    part.m_part_num = StringUtil::StringToUint64(part_node->value());
                } else if ("LastModified" == part_node_name) {
                    part.m_last_modified = part_node->value();
                } else if ("ETag" == part_node_name) {
                    part.m_etag = StringUtil::Trim(part_node->value(), "\"");
                } else if ("Size" == part_node_name) {
                    part.m_size = StringUtil::StringToUint64(part_node->value());
                } else {
                    SDK_LOG_WARN("Unknown field in Part node, field_name=%s",
                                 part_node_name.c_str());
                }
            }
            m_parts.push_back(part);
        } else if ("NextPartNumberMarker" == node_name) {
            m_next_part_number_marker = StringUtil::StringToUint64(node->value());
        } else if ("StorageClass" == node_name) {
            m_storage_class = node->value();
        } else if ("MaxParts" == node_name) {
            m_max_parts = StringUtil::StringToUint64(node->value());
        } else if ("IsTruncated" == node_name) {
            m_is_truncated = (std::string(node->value()) == "true") ? true : false;
        } else {
            SDK_LOG_WARN("Unknown field in ListPartsResult node, field_name=%s",
                         node_name.c_str());
        }
    }

    delete[] cstr;
    return true;
}

bool GetObjectACLResp::ParseFromXmlString(const std::string& body) {
    return ParseFromACLXMLString(body, &m_owner_id, &m_owner_display_name, &m_acl);
}

bool PutObjectCopyResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("CopyObjectResult");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=CopyObjectResult, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("ETag" == node_name) {
            m_etag = StringUtil::Trim(node->value(), "\"");
        } else if ("LastModified" == node_name) {
            m_last_modified = node->value();
        } else if ("VersionId" == node_name) {
            m_version_id = node->value();
        } else {
            SDK_LOG_WARN("Unknown field in CopyObjectResult node, field_name=%s",
                         node_name.c_str());
        }
    }
    delete[] cstr;
    return true;
}

bool UploadPartCopyDataResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("CopyPartResult");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=CopyObjectResult, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("ETag" == node_name) {
            m_etag = StringUtil::Trim(node->value(), "\"");
        } else if ("LastModified" == node_name) {
            m_last_modified = node->value();
        } else {
            SDK_LOG_WARN("Unknown field in CopyPartResult node, field_name=%s",
                         node_name.c_str());
        }
    }
    delete[] cstr;
    return true;
}

void CopyResp::CopyFrom(const PutObjectCopyResp& resp) {
    Clear();
    InternalCopyFrom(resp);
    m_resp_tag = "PutObjectCopy";
    m_etag = resp.GetEtag();
    m_last_modified = resp.GetLastModified();
    m_version_id = resp.GetVersionId();
}

void CopyResp::CopyFrom(const CompleteMultiUploadResp& resp) {
    Clear();
    InternalCopyFrom(resp);
    m_resp_tag = "Complete";
    m_location = resp.GetLocation();
    m_bucket = resp.GetBucket();
    m_key = resp.GetKey();
}

bool DeleteObjectsResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';

    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("DeleteResult");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=DeleteResult, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("Deleted" == node_name) {
            DeletedInfo info;
            rapidxml::xml_node<>* deleted_node = node->first_node();
            for (; deleted_node != NULL; deleted_node = deleted_node->next_sibling()) {
                const std::string& deleted_node_name = deleted_node->name();
                if ("Key" == deleted_node_name) {
                    info.m_key = deleted_node->value();
                } else if ("DeleteMarker" == deleted_node_name) {
                    info.m_delete_marker = std::string(deleted_node->value()) == "true" ? true : false;
                } else if ("DeleteMarkerVersionId" == deleted_node_name) {
                    info.m_delete_marker_version_id = deleted_node->value();
                } else if ("VersionId" == deleted_node_name) {
                    info.m_version_id = deleted_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in Deleted, field_name=%s",
                            deleted_node_name.c_str());
                }
            }
            m_deleted_infos.push_back(info);
        } else if ("Error" == node_name) {
            ErrorInfo info;
            rapidxml::xml_node<>* error_node = node->first_node();
            for (; error_node != NULL; error_node = error_node->next_sibling()) {
                const std::string& error_node_name = error_node->name();

                if ("Key" == error_node_name) {
                    info.m_key = error_node->value();
                } else if ("Code" == error_node_name) {
                    info.m_code = error_node->value();
                } else if ("Message" == error_node_name) {
                    info.m_message = error_node->value();
                } else if ("VersionId" == error_node_name) {
                    info.m_version_id = error_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in Error, field_name=%s",
                            error_node_name.c_str());
                }
            }
            m_error_infos.push_back(info);
        } else {
            SDK_LOG_WARN("Unknown field in DeletResultnode, field_name=%s",
                         node_name.c_str());
        }
    }
    delete[] cstr;
    return true;
}

} // namespace qcloud_cos
