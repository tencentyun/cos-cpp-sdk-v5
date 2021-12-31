// Copyright (c) 2017, Tencent Inc.
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
      : m_is_succ(false),
        m_http_status(-1),
        m_err_code(""),
        m_err_msg(""),
        m_resource_addr(""),
        m_x_cos_request_id(""),
        m_x_cos_trace_id("") {}

  ~CosResult() {}

  CosResult(const CosResult& other) {
    m_is_succ = other.m_is_succ;
    m_http_status = other.m_http_status;
    m_err_code = other.m_err_code;
    m_err_msg = other.m_err_msg;
    m_resource_addr = other.m_resource_addr;
    m_x_cos_request_id = other.m_x_cos_request_id;
    m_x_cos_trace_id = other.m_x_cos_trace_id;
    m_real_byte = other.m_real_byte;
  }

  CosResult& operator=(const CosResult& other) {
    if (this != &other) {
      m_is_succ = other.m_is_succ;
      m_http_status = other.m_http_status;
      m_err_code = other.m_err_code;
      m_err_msg = other.m_err_msg;
      m_resource_addr = other.m_resource_addr;
      m_x_cos_request_id = other.m_x_cos_request_id;
      m_x_cos_trace_id = other.m_x_cos_trace_id;
      m_real_byte = other.m_real_byte;
    }
    return *this;
  }

  bool IsSucc() const { return m_is_succ; }
  void SetSucc() { m_is_succ = true; }
  void SetFail() { m_is_succ = false; }
  void Clear() {
    m_is_succ = false;
    m_http_status = -1;
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
  int GetHttpStatus() const { return m_http_status; }
  std::string GetErrorCode() const { return m_err_code; }
  std::string GetErrorMsg() const { return m_err_msg; }
  std::string GetResourceAddr() const { return m_resource_addr; }
  std::string GetXCosRequestId() const { return m_x_cos_request_id; }
  std::string GetXCosTraceId() const { return m_x_cos_trace_id; }
  std::string GetXCosServerTime() const { return m_x_cos_server_time; }
  uint64_t GetRealByte() const { return m_real_byte; }

  // Setter
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

  void SetRealByte(uint64_t real_byte) { m_real_byte = real_byte; }
  /// \brief 输出Result的具体信息
  std::string DebugString() const;

  std::string GetInitMpRequestId() const { return m_init_mp_request_id; }
  void SetInitMpRequestId(const std::string& request_id) {
    m_init_mp_request_id = request_id;
  }

 private:
  bool m_is_succ;  // 标识HTTP调用是否成功

  int m_http_status;  // http状态码
  // COS返回的错误信息
  std::string m_err_code;
  std::string m_err_msg;
  std::string m_resource_addr;
  std::string m_x_cos_request_id;
  std::string m_x_cos_trace_id;
  std::string m_x_cos_server_time;
  uint64_t m_real_byte;
  // MultiUploadObject接口中封装了init mp/upload part/complete，该成员保存init
  // mp的request id 如果是断点续传，则该reqeust id为空
  std::string m_init_mp_request_id;
};

}  // namespace qcloud_cos
#endif  // COS_RESULT_H
