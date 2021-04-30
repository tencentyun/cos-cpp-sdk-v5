// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/18/17
// Description:

#ifndef BASERESP_H
#define BASERESP_H
#pragma once

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "cos_defines.h"

namespace qcloud_cos {

// 封装HTTP调用成功时的返回信息, 包括header和body
// CosResult::IsSucc返回true时, 调用其子类的Get方法获取具体字段
class BaseResp {
public:
    BaseResp() {}
    virtual ~BaseResp() {}

    // debug使用
    virtual std::string DebugString() const {
        std::string ret;
        for (std::map<std::string, std::string>::const_iterator c_itr = m_headers.begin();
             c_itr != m_headers.end(); ++c_itr) {
            ret += c_itr->first + " = " + c_itr->second + "\n";
        }
        ret += m_body_str;
        return ret;
    }

    std::string GetHeader(const std::string& key) const;
    std::map<std::string, std::string> GetHeaders() const { return m_headers; }
    std::map<std::string, std::string>* GetHeadersPtr() { return &m_headers; }
    void SetHeaders(const std::map<std::string, std::string>& headers) { m_headers = headers; }

    // ==========================头部相关==============================
    //TODO 头部可以不需要解析
    virtual void ParseFromHeaders(const std::map<std::string, std::string>& headers);
    uint64_t GetContentLength() const { return m_content_length; }
    std::string GetContentType() const { return m_content_type; }
    std::string GetEtag() const { return m_etag; }
    std::string GetCrc64Ecma() const { return m_crc64ecma; }
    std::string GetConnection() const { return m_connection; }
    std::string GetDate() const { return m_date; }
    std::string GetServer() const { return m_server; }
    std::string GetXCosRequestId() const { return m_x_cos_request_id; }
    std::string GetXCosTraceId() const { return m_x_cos_trace_id; }

    void SetContentLength(uint64_t content_length) { m_content_length = content_length; }
    void SetContentType(const std::string& content_type) { m_content_type = content_type; }
    void SetEtag(const std::string& etag);
    void SetConnection(const std::string& conn) { m_connection = conn; }
    void SetDate(const std::string& date) { m_date = date; }
    void SetServer(const std::string& server) { m_server = server; }
    void SetXCosRequestId(const std::string& id) { m_x_cos_request_id = id; }
    void SetXCosTraceId(const std::string& id) { m_x_cos_trace_id = id; }

    // ==========================响应体================================
    // 解析响应体的XML字符串
    virtual bool ParseFromXmlString(const std::string& body) { return true; }
    void SetBody(const std::string& body) { m_body_str = body; }
    const std::string& GetBody() const { return m_body_str; }
    std::string* GetBodyPtr() { return &m_body_str; }

    void CopyFrom(const BaseResp& resp) { InternalCopyFrom(resp);} 

protected:
    bool ParseFromACLXMLString(const std::string& body,
                               std::string* owner_id,
                               std::string* owner_display_name,
                               std::vector<Grant>* acl);

    void InternalCopyFrom(const BaseResp& resp);

private:
    std::map<std::string, std::string> m_headers;
    std::string m_body_str;

    // 公共头部字段
    uint64_t m_content_length;
    std::string m_content_type;
    std::string m_etag;
    std::string m_crc64ecma;
    std::string m_connection;
    std::string m_date;
    std::string m_server;
    std::string m_x_cos_request_id;
    std::string m_x_cos_trace_id;
};

} // namespace qcloud_cos

#endif // BASERESP_H
