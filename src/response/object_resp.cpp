// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "response/object_resp.h"

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

#include "cos_params.h"
#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

bool InitMultiUploadResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(body, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kInitiateMultipartUploadRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=InitiateMultipartUploadResult, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string node_name = node->name();
        if (node_name == kInitiateMultipartUploadBucket) {
            m_bucket = node->value();
        } else if (node_name == kInitiateMultipartUploadKey) {
            m_key = node->value();
        } else if (node_name == kInitiateMultipartUploadId) {
            m_upload_id = node->value();
        }
    }
}

bool CompleteMultiUploadResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(body, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node(kCompleteMultiUploadRoot.c_str());
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=ListBucketsResult, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string node_name = node->name();
        if (node_name == kCompleteMultiUploadLocation) {
            m_location = node->value();
        } else if (node_name == kCompleteMultiUploadBucket) {
            m_bucket = node->value();
        } else if (node_name == kCompleteMultiUploadKey) {
            m_key = node->value();
        } else if (node_name == kCompleteMultiUploadETag) {
            SetEtag(node->value());
        }
    }
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

void MultiUploadObjectResp::InternalCopyFrom(const BaseResp& resp) {
    SetContentLength(resp.GetContentLength());
    SetContentType(resp.GetContentType());
    SetEtag(resp.GetEtag());
    SetConnection(resp.GetConnection());
    SetDate(resp.GetDate());
    SetServer(resp.GetServer());
    SetXCosRequestId(resp.GetXCosRequestId());
    SetXCosTraceId(resp.GetXCosTraceId());

    SetHeaders(resp.GetHeaders());
    SetBody(resp.GetBody());
}

} // namespace qcloud_cos
