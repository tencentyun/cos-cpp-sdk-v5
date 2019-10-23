﻿// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:

#ifndef COS_RESULT_H
#define COS_RESULT_H
#pragma once

#include <map>
#include <string>

namespace qcloud_cos {

// 封装HTTP状态码：3XX，4XX，5XX 的返回结果
// 详见官网链接: https://www.qcloud.com/document/product/436/7730
class CosResult {
public:
    CosResult()
        : m_is_succ(false), m_http_status(-1), m_error_info(""), m_err_code(""),
          m_err_msg(""), m_resource_addr(""), m_x_cos_request_id(""), m_x_cos_trace_id("") {}

    ~CosResult() {}

    CosResult(const CosResult& other) {
        m_is_succ = other.m_is_succ;
        m_http_status = other.m_http_status;
        m_error_info = other.m_error_info;
        m_err_code = other.m_err_code;
        m_err_msg = other.m_err_msg;
        m_resource_addr = other.m_resource_addr;
        m_x_cos_request_id = other.m_x_cos_request_id;
        m_x_cos_trace_id = other.m_x_cos_trace_id;
    }

    CosResult& operator=(const CosResult& other) {
        if (this != &other) {
            m_is_succ = other.m_is_succ;
            m_http_status = other.m_http_status;
            m_error_info = other.m_error_info;
            m_err_code = other.m_err_code;
            m_err_msg = other.m_err_msg;
            m_resource_addr = other.m_resource_addr;
            m_x_cos_request_id = other.m_x_cos_request_id;
            m_x_cos_trace_id = other.m_x_cos_trace_id;
        }
        return *this;
    }

    bool IsSucc() const { return m_is_succ; }
    void SetSucc() { m_is_succ = true; }
    void SetFail() { m_is_succ = false; }
    void Clear() {
        m_is_succ = false;
        m_http_status = -1;
        m_error_info = "";
        m_err_code = "";
        m_err_msg = "";
        m_resource_addr = "";
        m_x_cos_request_id = "";
        m_x_cos_trace_id = "";
    }

    // 解析xml string
    bool ParseFromHttpResponse(const std::map<std::string, std::string>& headers,
                               const std::string& body);

    // Getter
    std::string GetErrorInfo() const { return m_error_info; }
    int GetHttpStatus() const { return m_http_status; }
    std::string GetErrorCode() const { return m_err_code; }
    std::string GetErrorMsg() const { return m_err_msg; }
    std::string GetResourceAddr() const { return m_resource_addr; }
    std::string GetXCosRequestId() const { return m_x_cos_request_id; }
    std::string GetXCosTraceId() const { return m_x_cos_trace_id; }

    // Setter
    void SetErrorInfo(const std::string& result) { m_error_info = result; }
    void SetHttpStatus(int http_status) { m_http_status = http_status; }
    void SetErrorCode(const std::string& err_code) { m_err_code = err_code; }
    void SetErrorMsg(const std::string& err_msg) { m_err_msg = err_msg; }
    void SetResourceAddr(const std::string& resource_addr) {
        m_resource_addr = resource_addr;
    }
    void SetXCosRequestId(const std::string& x_cos_request_id) {
        m_x_cos_request_id = x_cos_request_id;
    }
    void SetXCosTraceId(const std::string& x_cos_trace_id) {
        m_x_cos_trace_id = x_cos_trace_id;
    }

    /// \brief 输出Result的具体信息
    std::string DebugString() const;

private:
    bool m_is_succ; // 标识HTTP调用是否成功

    int m_http_status; // http状态码
    std::string m_error_info; // 错误的具体信息, 存放sdk内部错误/调用失败时无法解析的返回信息

    // COS返回的错误信息
    std::string m_err_code;
    std::string m_err_msg;
    std::string m_resource_addr;
    std::string m_x_cos_request_id;
    std::string m_x_cos_trace_id;
};

} // namespace qcloud_cos
#endif // COS_RESULT_H
