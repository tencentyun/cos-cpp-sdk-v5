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
#include "cos_params.h"
#include "util/string_util.h"

namespace qcloud_cos {

typedef std::map<std::string, std::string> StringMap;
typedef StringMap::const_iterator citerator;
// 封装HTTP调用成功时的返回信息, 包括header和body
// CosResult::IsSucc返回true时, 调用其子类的Get方法获取具体字段
class BaseResp {
 public:
  BaseResp()
      : m_content_length(0), m_x_cos_storage_class(kStorageClassStandard) {}

  BaseResp(const BaseResp& rhs) { InternalCopyFrom(rhs); }

  BaseResp& operator=(const BaseResp& rhs) {
    if (&rhs != this) {
      InternalCopyFrom(rhs);
    }
    return *this;
  }

  virtual ~BaseResp() {}

  // debug使用
  virtual std::string DebugString() const {
    std::string ret;
    for (std::map<std::string, std::string>::const_iterator c_itr =
             m_headers.begin();
         c_itr != m_headers.end(); ++c_itr) {
      ret += c_itr->first + " = " + c_itr->second + "\n";
    }
    ret += m_body_str;
    return ret;
  }

  std::string GetHeader(const std::string& key) const;
  std::map<std::string, std::string> GetHeaders() const { return m_headers; }
  std::map<std::string, std::string>* GetHeadersPtr() { return &m_headers; }
  void SetHeaders(const std::map<std::string, std::string>& headers) {
    m_headers = headers;
  }

  // ==========================头部相关==============================
  // TODO 头部可以不需要解析
  virtual void ParseFromHeaders(
      const std::map<std::string, std::string>& headers);
  uint64_t GetContentLength() const {
    citerator cit = m_headers.find(kHttpHeaderContentLength);
    if (m_headers.end() != cit) {
      return StringUtil::StringToUint64(cit->second);
    } else {
      return 0;
    }
  }
  std::string GetContentRange() const {
    return GetHeader(kHttpHeaderContentRange);
  }
  std::string GetContentType() const {
    return GetHeader(kHttpHeaderContentType);
  }
  std::string GetEtag() const {
    std::string etag;
    citerator cit = m_headers.find(kHttpHeaderEtag);
    if (m_headers.end() != cit) {
      etag = StringUtil::Trim(cit->second, "\"");
    } else {
      // 某些代理软件可能会修改HTTP Header，比如把ETag改成Etag
      // 此处找不到ETag，再尝试查找Etag
      cit = m_headers.find(kHttpHeaderLowerCaseEtag);
      if (m_headers.end() != cit) {
        etag = StringUtil::Trim(cit->second, "\"");
      }
    }
    return etag;
  }
  std::string GetConnection() const { return GetHeader(kHttpHeaderConnection); }
  std::string GetDate() const { return GetHeader(kHttpHeaderDate); }
  std::string GetServer() const { return GetHeader(kHttpHeaderServer); }
  std::string GetXCosTaggingCount() const {
    return GetHeader(kRespHeaderXCosTaggingCount);
  }
  std::string GetContentDisposition() const {
    return GetHeader(kHttpHeaderContentDisposition);
  }
  std::string GetContentEncoding() const {
    return GetHeader(kHttpHeaderContentEncoding);
  }
  std::string GetCacheControl() const {
    return GetHeader(kHttpHeaderCacheControl);
  }
  std::string GetExpires() const { return GetHeader(kHttpHeaderExpires); }
  std::string GetXCosRequestId() const {
    return GetHeader(kRespHeaderXCosReqId);
  }
  std::string GetXCosTraceId() const {
    return GetHeader(kRespHeaderXCosTraceId);
  }
  std::string GetXCosStorageTier() const {
    return GetHeader(kRespHeaderXCosStorageTier);
  }
  /// \brief 获取Object 的存储类型
  /// 枚举值：MAZ_STANDARD，MAZ_STANDARD_IA，INTELLIGENT_TIERING，MAZ_INTELLIGENT_TIERING，STANDARD_IA，ARCHIVE，DEEP_ARCHIVE
  std::string GetXCosStorageClass() const {
    return GetHeader(kRespHeaderXCosStorageClass);
  }
  std::string GetXCosHashCrc64Ecma() const {
    return GetHeader(kRespHeaderXCosHashCrc64Ecma);
  }
  /// \brief 获取object最后被修改的时间, 字符串格式Date, 类似"Wed, 28 Oct 2014
  /// 20:30:00 GMT"
  std::string GetLastModified() const {
    return GetHeader(kRespHeaderLastModified);
  }

  void SetContentLength(uint64_t content_length) {
    m_headers[kHttpHeaderContentLength] =
        StringUtil::Uint64ToString(content_length);
  }
  void SetContentType(const std::string& content_type) {
    m_headers[kHttpHeaderContentType] = content_type;
  }
  void SetEtag(const std::string& etag) { m_headers[kHttpHeaderEtag] = etag; }
  void SetConnection(const std::string& conn) {
    m_headers[kHttpHeaderConnection] = conn;
  }
  void SetDate(const std::string& date) { m_headers[kHttpHeaderDate] = date; }
  void SetServer(const std::string& server) {
    m_headers[kHttpHeaderServer] = server;
  }
  void SetContentDisposition(const std::string& content_disposition) {
    m_headers[kHttpHeaderContentDisposition] = content_disposition;
  }
  void SetContentEncoding(const std::string& content_encoding) {
    m_headers[kHttpHeaderContentEncoding] = content_encoding;
  }
  void SetCacheControl(const std::string& cache_control) {
    m_headers[kHttpHeaderCacheControl] = cache_control;
  }
  void SetExpires(const std::string& expires) {
    m_headers[kHttpHeaderExpires] = expires;
  }
  void SetXCosTraceId(const std::string& trace_id) {
    m_headers[kRespHeaderXCosTraceId] = trace_id;
  }
  void SetXCosRequestId(const std::string& request_id) {
    m_headers[kRespHeaderXCosReqId] = request_id;
  }
  void SetXCosStorageTier(const std::string& storage_tier) {
    m_headers[kRespHeaderXCosStorageTier] = storage_tier;
  }
  void SetXCosStorageClass(const std::string& storage_class) {
    m_headers[kRespHeaderXCosStorageClass] = storage_class;
  }
  void SetXCosHashCrc64Ecma(const std::string& crc64) {
    m_headers[kRespHeaderXCosHashCrc64Ecma] = crc64;
  }
  void SetLastModified(const std::string& last_modified) {
    m_headers[kRespHeaderLastModified] = last_modified;
  }

  // ==========================响应体================================
  // 解析响应体的XML字符串
  virtual bool ParseFromXmlString(const std::string& body) {
    m_body_str = body;
    return true;
  }
  void SetBody(const std::string& body) { m_body_str = body; }
  const std::string& GetBody() const { return m_body_str; }
  std::string* GetBodyPtr() { return &m_body_str; }

  void CopyFrom(const BaseResp& resp) { InternalCopyFrom(resp); }

 protected:
  std::string GetHeader(const std::string& key) {
    citerator cit = m_headers.find(key);
    if (m_headers.end() != cit) {
      return cit->second;
    }
    return "";
  }
  bool ParseFromACLXMLString(const std::string& body, std::string* owner_id,
                             std::string* owner_display_name,
                             std::vector<Grant>* acl);

  void InternalCopyFrom(const BaseResp& resp);

 private:
  std::map<std::string, std::string> m_headers;
  std::string m_body_str;
  // 公共头部字段
  uint64_t m_content_length;
  std::string m_content_range;
  std::string m_content_type;
  std::string m_etag;
  std::string m_connection;
  std::string m_date;
  std::string m_server;
  std::string m_content_disposition;
  std::string m_content_encoding;
  std::string m_cache_control;
  std::string m_expires;
  std::string m_last_modified;
  // cos响应头部
  std::string m_x_cos_request_id;
  std::string m_x_cos_trace_id;
  std::string m_x_cos_storage_tier;
  std::string m_x_cos_storage_class;
  std::string m_x_cos_hash_crc64ecma;
};

}  // namespace qcloud_cos

#endif  // BASERESP_H
