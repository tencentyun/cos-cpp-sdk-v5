// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/15/17
// Description:
#pragma once

#include <stdint.h>

#include <map>
#include <string>

#include "util/string_util.h"

namespace qcloud_cos {

/// \brief 封装Http请求
class BaseReq {
 public:
  typedef std::map<std::string, std::string> Str2StrMap;

 public:
  BaseReq();
  virtual ~BaseReq() {}

  /// \brief 设置请求头部
  void AddHeader(const std::string& key, const std::string& value);
  void AddHeaders(const Str2StrMap& user_headers);

  /// \brief 获取请求头部
  std::string GetHeader(const std::string& key) const;
  const Str2StrMap& GetHeaders() const { return m_headers_map; }

  /// \brief 清空请求头部
  void ClearHeaders() { m_headers_map.clear(); }

  /// \brief 设置请求参数
  void AddParam(const std::string& key, const std::string& value);
  void AddParams(const Str2StrMap& user_params);

  /// \brief 获取请求参数
  std::string GetParam(const std::string& key) const;
  const Str2StrMap& GetParams() const { return m_params_map; };

  /// \brief 清空请求参数
  void ClearParams() { m_params_map.clear(); }

  /// \brief 获取请求的http方法
  std::string GetMethod() const { return m_method; }

  /// \brief 设置请求的http方法
  void SetMethod(const std::string& method) {
    m_method = StringUtil::StringToUpper(method);
  }

  /// \brief 获取请求的body
  std::string GetBody() const { return m_body; }

  /// \brief 设置请求的body
  void SetBody(const std::string& body) { m_body = body; }

  /// \brief 获取Path
  std::string GetPath() const { return m_path; }

  /// \brief 设置Path
  void SetPath(const std::string& path) { m_path = path; }

  void SetConnTimeoutInms(uint64_t conn_timeout_in_ms) {
    m_conn_timeout_in_ms = conn_timeout_in_ms;
  }

  uint64_t GetConnTimeoutInms() const { return m_conn_timeout_in_ms; }

  void SetRecvTimeoutInms(uint64_t recv_timeout_in_ms) {
    m_recv_timeout_in_ms = recv_timeout_in_ms;
  }

  uint64_t GetRecvTimeoutInms() const { return m_recv_timeout_in_ms; }

  /// \brief 设置当前请求是否使用https
  void SetHttps() { m_is_https = true; }
  bool IsHttps() const { return m_is_https; }

  /// \brief set whether check content md5 each request, used for close range
  /// download check
  void SetCheckMD5(bool check_md5) { mb_check_md5 = check_md5; }
  bool CheckMD5() const { return mb_check_md5; }

  void SetCheckCRC64(bool check_crc64) { mb_check_crc64 = check_crc64; }
  bool CheckCRC64() const { return mb_check_crc64; }

  void SetCaLocation(const std::string& ca_location) { m_ca_location = ca_location; }
  const std::string& GetCaLocation() const { return m_ca_location; }

  void SetVerifyCert(bool verify_cert) { mb_verify_cert = verify_cert; }
  bool GetVerifyCert() const { return mb_verify_cert; }

  /// \brief 输出请求的header和param信息
  std::string DebugString() const;

  /// \brief 是否生成对header host
  /// 是否对host进行签名，默认为true，您也可以选择不签入host，
  /// 但可能导致请求失败或安全漏洞
  void SetSignHeaderHost(bool sign_header_host) {
    m_sign_header_host = sign_header_host;
  }
  bool SignHeaderHost() const { return m_sign_header_host; }

 protected:
  // acl相关的请求，供PutObjectACL和PutBucketACL使用
  bool GenerateAclRequestBody(const Owner& owner, const std::vector<Grant>& acl,
                              std::string* body) const;

 protected:
  Str2StrMap m_headers_map;
  Str2StrMap m_params_map;
  std::string m_path;
  std::string m_method;
  std::string m_body;

  uint64_t m_conn_timeout_in_ms;
  uint64_t m_recv_timeout_in_ms;
  bool m_is_https;
  bool mb_check_md5;    // default is true
  bool mb_check_crc64;  // default is false

  bool mb_verify_cert;  // default is true
  bool m_sign_header_host;  // 是否对header host进行签名
  std::string m_ca_location;
};

}  // namespace qcloud_cos
