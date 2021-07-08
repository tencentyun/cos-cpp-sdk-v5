// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "response/object_resp.h"

#include <stdio.h>
#include <string.h>
#include <bitset>
#include <fstream>

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"
#include "Poco/Checksum.h"

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
                } else if ("DisplayName" == init_node_name) {
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
                } else if ("DisplayName" == owner_node_name) {
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
        } else if ("CRC64" == node_name) {
            m_crc64 = node->value();
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

// @brief parse body
// message1
// message2
// mesages3
// ...
//
// messageN = prelude + message_data + CRC32(prelude + message_data)(4 byte)
// prelude = total_byte_len(4 byte) + header_byte_len(4 byte) + CRC32(total_byte_len + header_byte_len)(4 byte)
// message_data = header + payload
bool SelectObjectContentResp::ParseFromXmlString(const std::string& body) {
    size_t body_length = body.length();
    const char * body_start = body.data();
    const char * message_start = nullptr;
    const char * header_start = nullptr;
    int body_cursor = 0;
    int message_cursor = 0;
    int header_cursor = 0;
    uint32_t total_byte_length;
    uint32_t header_byte_length;
    uint32_t crc_expect;

    const int bytes_8 = 8;
    const int bytes_4 = 4;

    SDK_LOG_DBG("body_length:%u", (unsigned int)body_length); 
    while (body_cursor < body_length) {
        message_start = body_start + body_cursor;
        message_cursor = 0;
    
        // calc prelude crc
        Poco::Checksum prelude_crc;
        prelude_crc.update(message_start, bytes_8);
        
        // get total byte length
        total_byte_length = StringUtil::GetUint32FromStrWithBigEndian(message_start);
        if (total_byte_length < 56) { // 16 + 40
            SDK_LOG_ERR("Invalid total_byte_length:%u", total_byte_length);
            return false;
        }
        SDK_LOG_DBG("total_byte_length:%u", total_byte_length);

        // calc message crc
        Poco::Checksum message_crc;
        message_crc.update(message_start, total_byte_length - 4);
        crc_expect = StringUtil::GetUint32FromStrWithBigEndian(message_start + total_byte_length - 4);
        if (message_crc.checksum() != crc_expect) {
            SDK_LOG_ERR("Message data crc check faield, crc=%u, expected=%u", message_crc.checksum(), crc_expect);
            return false;
        }

        // forward message cursor
        message_cursor += bytes_4;
        // get header byte length
        header_byte_length = StringUtil::GetUint32FromStrWithBigEndian(message_start + message_cursor);
        SDK_LOG_DBG("header_byte_length:%u", header_byte_length);
        if (header_byte_length > total_byte_length || header_byte_length  < 40) {
            SDK_LOG_ERR("Invalid header_byte_length:%u", header_byte_length);
            return false;
        }

        // forward message cursor
        message_cursor += bytes_4;
        // get and check prelude crc
        crc_expect = StringUtil::GetUint32FromStrWithBigEndian(message_start + message_cursor);
        if (prelude_crc.checksum() != crc_expect) {
            SDK_LOG_ERR("Prelude crc check faield, crc=%u, expected=%u", prelude_crc.checksum(), crc_expect);
            return false;
        }
        // forward message cursor
        message_cursor += bytes_4;

        // start to parse header
        header_cursor = 0;
        header_start = message_start + message_cursor;
        std::map<std::string, std::string> header_map;
        while (header_cursor < header_byte_length) {
            std::bitset<8> bs = header_start[header_cursor];
            uint8_t header_name_len = header_start[header_cursor];
            SDK_LOG_DBG("header_name_len:%u", header_name_len);
            header_cursor++;
            std::string header_name(header_start + header_cursor, header_name_len);
            SDK_LOG_DBG("header_name:%s", header_name.c_str());
            header_cursor += header_name_len;
            char header_value_type = header_start[header_cursor];
            SDK_LOG_DBG("header_value_type:%u", header_value_type);
            if (header_value_type != 7) {
                SDK_LOG_ERR("Invalid header value type:%u, expect 7", header_value_type);
                return false;
            }
            header_cursor++;
            uint16_t header_value_len = StringUtil::GetUint16FromStrWithBigEndian(header_start + header_cursor);
            SDK_LOG_DBG("header_value_len:%u", header_value_len);
            header_cursor += 2;
            std::string header_value(header_start + header_cursor, header_value_len);
            SDK_LOG_DBG("header_value:%s", header_value.c_str());
            header_map.emplace(std::move(header_name), std::move(header_value));
            header_cursor += header_value_len;
        }
        
        // forward message cursor
        message_cursor += header_byte_length; 
        int payload_length = total_byte_length - header_byte_length - 16;
        if (header_map.count(":message-type") > 0) {
            if (header_map[":message-type"] == "event") {
                std::string event_type = header_map[":event-type"];
                std::string content_type = header_map[":content-type"];
                if (event_type == "End") {
                    // end of response
                    SDK_LOG_DBG("Get event End, finish parse body"); 
                    return true;
                } else {
                    if (event_type == "Records" || event_type == "Progress" || event_type == "Stats") {
                        // new message block
                        SelectMessage msg;
                        msg.m_event_type = std::move(event_type);
                        msg.m_content_type = std::move(content_type);
                        msg.payload = std::move(std::string(message_start + message_cursor, payload_length));
                        SDK_LOG_DBG("Get event:%s, content_type:%s, payload:%s", 
                                    msg.m_event_type.c_str(), msg.m_content_type.c_str(), msg.payload.c_str());
                        resp_data.emplace_back(std::move(msg));
                    } else if (event_type == "Cont") {
                        SDK_LOG_DBG("Get event Continue"); 
                    } else {
                        SDK_LOG_ERR("Invalid event:%s", event_type.c_str());
                        return false;
                    }
                }
            } 
            else if (header_map[":message-type"] == "error") {
                if (header_map.count(":error-message")) {
                    error_message = header_map[":error-message"];
                }
                if (header_map.count(":error-code")) {
                    error_code = header_map[":error-code"];
                }
                SDK_LOG_ERR("Get error-message:%s, error-code:%s", error_message.c_str(), error_code.c_str());
                return false;
            }else {
                SDK_LOG_ERR("Unknown message type:%s", header_map[":message-type"].c_str());
                return false;
            }
        }    
        else {
            SDK_LOG_ERR("No message type in header");
            return false;
        }

        // forward body cursor
        body_cursor += total_byte_length;
    }

    SDK_LOG_DBG("Succefully parse body");
    return true;
}

void SelectObjectContentResp::PrintResult() const {
    if (!error_message.empty() || !error_code.empty()) {
        std::cout << "Error code:" << error_code << ", error message:" << error_message << std::endl;

        return;
    }
    for (size_t i = 0; i < resp_data.size(); ++i) {
        if (resp_data[i].m_event_type == "Records") {
            std::cout << resp_data[i].payload;
        }
    }
}

int SelectObjectContentResp::WriteResultToLocalFile(const std::string& file) {
    if (!error_message.empty() || !error_code.empty()) {
        return -1;
    }

    std::ofstream ofs(file, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        SDK_LOG_ERR("Failed to open file:%s, error info:%s", file.c_str(), strerror(errno));
        return -1;
    }
    for (size_t i = 0; i < resp_data.size(); ++i) {
        if (resp_data[i].m_event_type == "Records") {
           ofs << resp_data[i].payload;
        }
    }
    ofs.close();
    return 0;
}

bool PutLiveChannelResp::ParseFromXmlString(const std::string& body) {
    std::string tmp_body = body;
    rapidxml::xml_document<> doc;

    // StringUtil::StringToXml(const_cast<char *>(tmp_body.c_str()), &doc) not working
    if (!StringUtil::StringToXml(&tmp_body[0], &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }
    
    rapidxml::xml_node<>* root = doc.first_node("CreateLiveChannelResult");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node=CreateLiveChannelResult, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* publish_url_node = root->first_node("PublishUrls");
    if(NULL == publish_url_node) {
        SDK_LOG_WARN("Miss node PublishUrls, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = publish_url_node->first_node();

    if ("Url" == node->name()) {
        SDK_LOG_WARN("Miss node PublishUrls Url, xml_body=%s", body.c_str());
        return false;
    }
    m_publish_url = node->value();

    rapidxml::xml_node<>* play_url_node = root->first_node("PlayUrls");
    if(NULL == play_url_node) {
        SDK_LOG_WARN("Miss node PlayUrls, xml_body=%s", body.c_str());
        return false;
    }
    node = play_url_node->first_node();
    if ("Url" == node->name()) {
        SDK_LOG_WARN("Miss node PlayUrls, xml_body=%s", body.c_str());
        return false;
    }
    m_play_url = node->value();

    return true;
}

bool GetLiveChannelResp::ParseFromXmlString(const std::string& body) {
    std::string tmp_body = body;
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(&tmp_body[0], &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("LiveChannelConfiguration");
    if (NULL == root) {
        SDK_LOG_ERR("Missing root node LiveChannelConfiguration, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("Description" == node_name) {
            m_chan_conf.SetDescription(node->value());
        } else if ("Switch" == node_name) {
            m_chan_conf.SetSwitch(node->value());
        } else if ("Target" == node_name) {
            rapidxml::xml_node<>* target_node = node->first_node();
            for (; target_node != NULL; target_node = target_node->next_sibling()) {
                const std::string& node_name = target_node->name();
                if ("Type" == node_name) {
                    m_chan_conf.SetType(target_node->value());
                } else if ("FragDuration" == node_name) {
                    m_chan_conf.SetFragDuration(StringUtil::StringToInt(target_node->value()));
                } else if ("FragCount" == node_name) {
                    m_chan_conf.SetFragCount(StringUtil::StringToInt(target_node->value()));
                } else if ("PlaylistName" == node_name) {
                    m_chan_conf.SetPlaylistName(target_node->value());
                } else if ("PublishUrls" == node_name) {
                    rapidxml::xml_node<>* publish_url_node = target_node->first_node();
                    if ("Url" == publish_url_node->name()) {
                        SDK_LOG_WARN("Missing node PublishUrls Url, xml_body=%s", body.c_str());
                        return false;
                    }
                    m_chan_conf.SetPublishUrl(publish_url_node->value());
                } else if ("PlayUrls" == node_name) {
                    rapidxml::xml_node<>* play_url_node = target_node->first_node();
                    if ("Url" == play_url_node->name()) {
                        SDK_LOG_WARN("Missing node PlayUrls Url, xml_body=%s", body.c_str());
                        return false;
                    }
                    m_chan_conf.SetPlayUrl(play_url_node->value());
                } else {
                    SDK_LOG_WARN("Unknown field in Target, field_name=%s", node_name.c_str());
                    return false;
                }
            }
        } else {
            SDK_LOG_WARN("Unknown field in LiveChannelConfiguration, field_name=%s", node_name.c_str());
            return false;
        }
    }

    return true;
}

bool GetLiveChannelHistoryResp::ParseFromXmlString(const std::string& body) {
    rapidxml::xml_document<> doc;
    char* cstr = new char[body.size() + 1];
    strcpy(cstr, body.c_str());
    cstr[body.size()] = '\0';
    if (!StringUtil::StringToXml(cstr, &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("LiveChannelHistory");
    if (NULL == root) {
        SDK_LOG_ERR("Missing root node LiveChannelHistory, xml_body=%s", body.c_str());
        delete[] cstr;
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("LiveRecord" != node_name) {
            SDK_LOG_WARN("Unknown field in LiveChannelHistory, field_name=%s", node_name.c_str());
            delete[] cstr;
            return false;
        }
        LiveRecord r;
        rapidxml::xml_node<>* record_node = node->first_node();
        for (; record_node != NULL; record_node = record_node->next_sibling()) {
            const std::string& node_name = record_node->name();
            if ("StartTime" == node_name) {
                r.m_start_time = record_node->value();
            } else if ("EndTime" == node_name) {
                r.m_end_time = record_node->value();
            } else if ("RemoteAddr" == node_name) {
                r.m_remote_addr = record_node->value();
            } else if ("RequestId" == node_name) {
                r.m_request_id = record_node->value();
            } else {
                SDK_LOG_WARN("Unknown field in LiveRecord, field_name=%s", node_name.c_str());
                delete[] cstr;
                return false;
            }
        }
        m_history.push_back(r);
    }

    delete[] cstr;
    return true;
}

bool GetLiveChannelStatusResp::ParseFromXmlString(const std::string& body) {
    std::string tmp_body = body;
    rapidxml::xml_document<> doc;
    if (!StringUtil::StringToXml(&tmp_body[0], &doc)) {
        SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* root = doc.first_node("LiveChannelStatus");
    if (NULL == root) {
        SDK_LOG_ERR("Miss root node LiveChannelStatus, xml_body=%s", body.c_str());
        return false;
    }

    rapidxml::xml_node<>* node = root->first_node();
    for (; node != NULL; node = node->next_sibling()) {
        const std::string& node_name = node->name();
        if ("Status" == node_name) {
            const std::string& status = node->value();
            if (status != "Idle" && status != "Live") {
                SDK_LOG_WARN("Unknown Status in LiveChannelStatus, Status:%s", status.c_str());
                return false;
            }
            m_livechan_status.m_status = status;
        } else if ("ConnectedTime" == node_name) {
            m_livechan_status.m_connected_time = node->value();
        } else if ("RemoteAddr" == node_name) {
            m_livechan_status.m_remote_addr = node->value();
        } else if ("RequestId" == node_name) {
            m_livechan_status.m_request_id = node->value();
        } else if ("Video" == node_name) {
            m_livechan_status.m_has_video = true;
            rapidxml::xml_node<>* video_node = node->first_node();
            for (; video_node != NULL; video_node = video_node->next_sibling()) {
                const std::string& node_name = video_node->name();
                if ("Width" == node_name) {
                    m_livechan_status.m_video.m_width = video_node->value();
                } else if ("Height" == node_name) {
                    m_livechan_status.m_video.m_heigh = video_node->value();
                } else if ("FrameRate" == node_name) {
                    m_livechan_status.m_video.m_framerate = video_node->value();
                } else if ("Bandwidth" == node_name) {
                    m_livechan_status.m_video.m_bandwidth = video_node->value();
                } else if ("Codec" == node_name) {
                    m_livechan_status.m_video.m_codec = video_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in Video, field_name=%s", node_name.c_str());
                    return false;
                }
            }
        }else if ("Audio" == node_name) {
            m_livechan_status.m_has_audio = true;
            rapidxml::xml_node<>* audio_node = node->first_node();
            for (; audio_node != NULL; audio_node = audio_node->next_sibling()) {
                const std::string& node_name = audio_node->name();
                if ("Bandwidth" == node_name) {
                    m_livechan_status.m_audio.m_bandwidth = audio_node->value();
                } else if ("SampleRate" == node_name) {
                    m_livechan_status.m_audio.m_samplerate = audio_node->value();
                } else if ("Codec" == node_name) {
                    m_livechan_status.m_audio.m_codec = audio_node->value();
                } else {
                    SDK_LOG_WARN("Unknown field in Audio, field_name=%s", node_name.c_str());
                    return false;
                }
            }
        } else {
            SDK_LOG_WARN("Unknown field in LiveChannelStatus, field_name=%s", node_name.c_str());
            return false;
        }
    }

    return true;
}

int GetLiveChannelVodPlaylistResp::WriteResultToFile(const std::string& file) {
    std::ofstream ofs(file, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        SDK_LOG_ERR("Failed to open file:%s, error info:%s", file.c_str(), strerror(errno));
        return -1;
    }
    ofs << GetBody();
    ofs.close();
    return 0;
}
} // namespace qcloud_cos
