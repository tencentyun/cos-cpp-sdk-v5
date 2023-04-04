// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#ifndef BUCKET_RESP_H
#define BUCKET_RESP_H
#pragma once

#include <vector>

#include "cos_config.h"
#include "cos_defines.h"
#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"
#include "response/base_resp.h"

namespace qcloud_cos {

class PutBucketResp : public BaseResp {
 public:
  PutBucketResp() {}
  virtual ~PutBucketResp() {}
};

class HeadBucketResp : public BaseResp {
 public:
  HeadBucketResp() {}
  virtual ~HeadBucketResp() {}
};

class GetBucketResp : public BaseResp {
 public:
  GetBucketResp() {}
  virtual ~GetBucketResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  /// \brief 获取Bucket中Object对应的元信息
  std::vector<Content> GetContents() const { return m_contents; }

  /// \brief Bucket名称
  std::string GetName() const { return m_name; }

  /// \brief 获取定界符
  std::string GetDelimiter() const { return m_delimiter; }

  /// \brief 编码格式
  std::string GetEncodingType() const { return m_encoding_type; }

  /// \brief 返回的文件前缀
  std::string GetPrefix() const { return m_prefix; }

  /// \brief 默认以UTF-8二进制顺序列出条目，所有列出条目从marker开始
  std::string GetMarker() const { return m_marker; }

  /// \brief 单次响应请求内返回结果的最大的条目数量
  uint64_t GetMaxKeys() const { return m_max_keys; }

  /// \brief 响应请求条目是否被截断，布尔值：true，false
  bool IsTruncated() const { return m_is_truncated; }

  /// \brief 假如返回条目被截断，则返回 NextMarker 就是下一个条目的起点
  std::string GetNextMarker() const { return m_next_marker; }

  /// \brief 将 Prefix 到 delimiter 之间的相同路径归为一类，定义为 Common Prefix
  std::vector<std::string> GetCommonPrefixes() const {
    return m_common_prefixes;
  }

 private:
  std::vector<Content> m_contents;
  std::string m_name;
  std::string m_encoding_type;
  std::string m_delimiter;
  std::string m_prefix;
  std::string m_marker;
  uint64_t m_max_keys;
  bool m_is_truncated;
  std::string m_next_marker;
  std::vector<std::string> m_common_prefixes;
};

class DeleteBucketResp : public BaseResp {
 public:
  DeleteBucketResp() {}
  virtual ~DeleteBucketResp() {}
};

class GetBucketVersioningResp : public BaseResp {
 public:
  GetBucketVersioningResp() : m_status(0) {}
  virtual ~GetBucketVersioningResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  /// \brief 返回bucket的版本状态,0: 从未开启版本管理, 1: 版本管理生效中, 2:
  /// 暂停 区别于PutBucketVersioning, 一个Bucket可能处于三种状态
  int GetStatus() const { return m_status; }

 private:
  int m_status;
};

class PutBucketVersioningResp : public BaseResp {
 public:
  PutBucketVersioningResp() {}
  virtual ~PutBucketVersioningResp() {}
};

class GetBucketReplicationResp : public BaseResp {
 public:
  GetBucketReplicationResp() {}
  virtual ~GetBucketReplicationResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  std::string GetRole() const { return m_role; }

  std::vector<ReplicationRule> GetRules() const { return m_rules; }

 private:
  std::string m_role;
  std::vector<ReplicationRule> m_rules;
};

class PutBucketReplicationResp : public BaseResp {
 public:
  PutBucketReplicationResp() {}
  virtual ~PutBucketReplicationResp() {}
};

class DeleteBucketReplicationResp : public BaseResp {
 public:
  DeleteBucketReplicationResp() {}
  virtual ~DeleteBucketReplicationResp() {}
};

class GetBucketLifecycleResp : public BaseResp {
 public:
  GetBucketLifecycleResp() {}
  virtual ~GetBucketLifecycleResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  std::vector<LifecycleRule> GetRules() const { return m_rules; }

 private:
  std::vector<LifecycleRule> m_rules;
};

class PutBucketLifecycleResp : public BaseResp {
 public:
  PutBucketLifecycleResp() {}
  virtual ~PutBucketLifecycleResp() {}
};

class DeleteBucketLifecycleResp : public BaseResp {
 public:
  DeleteBucketLifecycleResp() {}
  virtual ~DeleteBucketLifecycleResp() {}
};

class GetBucketACLResp : public BaseResp {
 public:
  GetBucketACLResp() {}
  virtual ~GetBucketACLResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  std::string GetOwnerID() const { return m_owner_id; }
  std::string GetOwnerDisplayName() const { return m_owner_display_name; }
  std::vector<Grant> GetAccessControlList() const { return m_acl; }

 private:
  std::string m_owner_id;
  std::string m_owner_display_name;
  std::vector<Grant> m_acl;
};

// TODO
class ListMultipartUploadResp : public BaseResp {
 public:
  ListMultipartUploadResp() {}
  virtual ~ListMultipartUploadResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  /// \brief 获取Bucket中Object对应的元信息
  std::vector<Upload> GetUpload() const { return m_upload; }

  /// \brief Bucket名称
  std::string GetName() const { return m_name; }

  /// \brief 编码格式
  std::string GetEncodingType() const { return m_encoding_type; }

  /// \brief 默认以UTF-8二进制顺序列出条目，所有列出条目从marker开始
  std::string GetMarker() const { return m_marker; }

  std::string GetUploadIdMarker() const { return m_uploadid_marker; }

  std::string GetNextKeyMarker() const { return m_nextkey_marker; }

  std::string GetNextUploadIdMarker() const { return m_nextuploadid_marker; }

  std::string GetMaxUploads() const { return m_max_uploads; }

  /// \brief 响应请求条目是否被截断，布尔值：true，false
  bool IsTruncated() const { return m_is_truncated; }

  /// \brief 返回的文件前缀
  std::string GetPrefix() const { return m_prefix; }

  /// \brief 获取定界符
  std::string GetDelimiter() const { return m_delimiter; }

  /// \brief 将 Prefix 到 delimiter 之间的相同路径归为一类，定义为 Common Prefix
  std::vector<std::string> GetCommonPrefixes() const {
    return m_common_prefixes;
  }

 private:
  std::vector<Upload> m_upload;
  std::string m_name;
  std::string m_encoding_type;
  std::string m_delimiter;
  std::string m_prefix;
  std::string m_marker;
  std::string m_uploadid_marker;
  std::string m_nextkey_marker;
  std::string m_nextuploadid_marker;
  std::string m_max_uploads;
  bool m_is_truncated;
  std::vector<std::string> m_common_prefixes;
};

class PutBucketACLResp : public BaseResp {
 public:
  PutBucketACLResp() {}
  virtual ~PutBucketACLResp() {}
};

class GetBucketPolicyResp : public BaseResp {
 public:
  GetBucketPolicyResp() {}
  virtual ~GetBucketPolicyResp() {}
  std::string GetPolicy() const { return GetBody(); }
};

class PutBucketPolicyResp : public BaseResp {
 public:
  PutBucketPolicyResp() {}
  virtual ~PutBucketPolicyResp() {}
};

class DeleteBucketPolicyResp : public BaseResp {
 public:
  DeleteBucketPolicyResp() {}
  virtual ~DeleteBucketPolicyResp() {}
};

class GetBucketCORSResp : public BaseResp {
 public:
  GetBucketCORSResp() {}
  virtual ~GetBucketCORSResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  std::vector<CORSRule> GetCORSRules() const { return m_rules; }

 private:
  std::vector<CORSRule> m_rules;
};

class PutBucketCORSResp : public BaseResp {
 public:
  PutBucketCORSResp() {}
  virtual ~PutBucketCORSResp() {}
};

class DeleteBucketCORSResp : public BaseResp {
 public:
  DeleteBucketCORSResp() {}
  virtual ~DeleteBucketCORSResp() {}
};

class GetBucketLocationResp : public BaseResp {
 public:
  GetBucketLocationResp() {}
  virtual ~GetBucketLocationResp() {}

  std::string GetLocation() const { return m_location; }

  virtual bool ParseFromXmlString(const std::string& body);

 private:
  std::string m_location;
};

class GetBucketObjectVersionsResp : public BaseResp {
 public:
  GetBucketObjectVersionsResp() {}
  virtual ~GetBucketObjectVersionsResp() {}

  /// \brief 编码格式
  std::string GetEncodingType() const { return m_encoding_type; }

  /// \brief 返回的文件前缀
  std::string GetPrefix() const { return m_prefix; }

  /// \brief 单次响应请求内返回结果的最大的条目数量
  uint64_t GetMaxKeys() const { return m_max_keys; }

  /// \brief 响应请求条目是否被截断，布尔值：true，false
  bool IsTruncated() const { return m_is_truncated; }

  /// \brief 假如返回条目被截断，则返回 NextKeyMarker 就是下一个条目的起点
  std::string GetNextKeyMarker() const { return m_next_key_marker; }

  std::string GetKeyMarker() const { return m_key_marker; }

  std::string GetBucketName() const { return m_bucket_name; }

  std::string GetVersionIdMarker() const { return m_version_id_marker; }

  std::vector<COSVersionSummary> GetVersionSummary() const {
    return m_summaries;
  }

  virtual bool ParseFromXmlString(const std::string& body);

 private:
  std::vector<COSVersionSummary> m_summaries;
  std::string m_encoding_type;
  bool m_is_truncated;
  uint64_t m_max_keys;
  std::string m_bucket_name;
  std::string m_key_marker;
  std::string m_prefix;
  std::string m_version_id_marker;
  std::string m_next_key_marker;
  std::string m_next_version_id_marker;
};

class PutBucketLoggingResp : public BaseResp {
 public:
  PutBucketLoggingResp() {}
  virtual ~PutBucketLoggingResp() {}
};

class GetBucketLoggingResp : public BaseResp {
 public:
  GetBucketLoggingResp() {}
  virtual ~GetBucketLoggingResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  LoggingEnabled GetLoggingEnabled() const { return m_rules; }

 private:
  LoggingEnabled m_rules;
};

class PutBucketDomainResp : public BaseResp {
 public:
  PutBucketDomainResp() {}
  virtual ~PutBucketDomainResp() {}
  virtual bool ParseFromXmlString(const std::string& body);
  DomainErrorMsg GetDomainErrorMsg() const { return m_rules; }

 private:
  DomainErrorMsg m_rules;
};

class GetBucketDomainResp : public BaseResp {
 public:
  GetBucketDomainResp() {}
  virtual ~GetBucketDomainResp() {}
  virtual bool ParseFromXmlString(const std::string& body);

  std::string GetStatus() const { return m_status; }

  std::string GetName() const { return m_name; }

  std::string GetType() const { return m_type; }

  void SetStatus(const std::string& status) { m_status = status; }

  void SetName(const std::string& name) { m_name = name; }

  void SetType(const std::string& type) { m_type = type; }

 private:
  std::string m_status;
  std::string m_name;
  std::string m_type;
};

class PutBucketWebsiteResp : public BaseResp {
 public:
  PutBucketWebsiteResp() {}
  virtual ~PutBucketWebsiteResp() {}
};

class GetBucketWebsiteResp : public BaseResp {
 public:
  GetBucketWebsiteResp() {}
  virtual ~GetBucketWebsiteResp() {}
  virtual bool ParseFromXmlString(const std::string& body);
  bool ParseFromXmlRoutingRule(rapidxml::xml_node<>* node,
                               RoutingRule& tmp_routingrule);

  void SetSuffix(const std::string& suffix) { m_suffix = suffix; }

  void SetProtocol(const std::string& protocol) { m_protocol = protocol; }

  void SetKey(const std::string& key) { m_key = key; }

  std::string GetSuffix() const { return m_suffix; }

  std::string GetProtocol() const { return m_protocol; }

  std::string GetKey() const { return m_key; }

  /// 设置重定向规则
  void SetRoutingRules(const std::vector<RoutingRule>& routingrules) {
    m_routingrules = routingrules;
  }

  std::vector<RoutingRule> GetRoutingRules() const { return m_routingrules; }

  /// 添加单个rule
  void AddRoutingRule(const RoutingRule& routingrule) {
    m_routingrules.push_back(routingrule);
  }

  /// 清空重定向规则
  void ClearRoutingRule() {
    std::vector<RoutingRule> tmp;
    m_routingrules.swap(tmp);
  }

 private:
  std::string m_suffix;
  std::string m_protocol;
  std::string m_key;
  std::vector<RoutingRule> m_routingrules;
};

class DeleteBucketWebsiteResp : public BaseResp {
 public:
  DeleteBucketWebsiteResp() {}
  virtual ~DeleteBucketWebsiteResp() {}
};

class PutBucketTaggingResp : public BaseResp {
 public:
  PutBucketTaggingResp() {}
  virtual ~PutBucketTaggingResp() {}
};

class GetBucketTaggingResp : public BaseResp {
 public:
  GetBucketTaggingResp() {}

  void SetTagSet(std::vector<Tag>& tagset) { m_tagset = tagset; }

  std::vector<Tag> GetTagSet() const { return m_tagset; }

  //清除tag规则.
  void ClearTagSet() {
    std::vector<Tag> temp;
    m_tagset.swap(temp);
  }

  /// 添加单个tag.
  void AddTag(const Tag& tag) { m_tagset.push_back(tag); }

  virtual bool ParseFromXmlString(const std::string& body);
  virtual ~GetBucketTaggingResp() {}

 private:
  std::vector<Tag> m_tagset;
};

class DeleteBucketTaggingResp : public BaseResp {
 public:
  DeleteBucketTaggingResp() {}
  virtual ~DeleteBucketTaggingResp() {}
};

class PutBucketInventoryResp : public BaseResp {
 public:
  PutBucketInventoryResp() {}
  virtual ~PutBucketInventoryResp() {}
};

class GetBucketInventoryResp : public BaseResp {
 public:
  GetBucketInventoryResp() : m_mask(0x00000001u) {}

  void SetInventory(const Inventory& inventory) {
    m_mask = m_mask | 0x00000001u;
    m_inventory = inventory;
  }

  bool HasInventory() const { return (m_mask & 0x00000001u) != 0; }

  const Inventory& GetInventory() const { return m_inventory; }

  bool ParseFromXmlOptionalFields(rapidxml::xml_node<>* node,
                                  Inventory& temp_inventory);
  bool ParseFromXmlCOSBucketDestination(rapidxml::xml_node<>* node,
                                        Inventory& temp_inventory);
  bool ParseFromXmlInventoryConfiguration(rapidxml::xml_node<>* node,
                                          Inventory& temp_inventory);

  virtual bool ParseFromXmlString(const std::string& body);
  virtual ~GetBucketInventoryResp() {}

 private:
  uint64_t m_mask;
  Inventory m_inventory;
};

class ListBucketInventoryConfigurationsResp : public GetBucketInventoryResp {
 public:
  ListBucketInventoryConfigurationsResp()
      : m_is_truncated(false),
        m_continuation_token(""),
        m_next_continuation_token("") {}

  virtual bool ParseFromXmlString(const std::string& body);
  virtual ~ListBucketInventoryConfigurationsResp() {}

  /// 添加单个Inventory.
  void AddInventory(const Inventory& inventory) {
    m_inventory_vec.push_back(inventory);
  }

  void SetInventoryVec(std::vector<Inventory>& inventory_vec) {
    m_inventory_vec = inventory_vec;
  }

  void SetIsTruncated(bool is_truncated) { m_is_truncated = is_truncated; }

  void SetContinuationToken(std::string continuation_token) {
    m_continuation_token = continuation_token;
  }

  void SetNextContinuationToken(std::string next_continuation_token) {
    m_next_continuation_token = next_continuation_token;
  }

  std::vector<Inventory> GetInventory() const { return m_inventory_vec; }

  bool GetIsTruncated() const { return m_is_truncated; }

  std::string GetContinuationToken() const { return m_continuation_token; }

  std::string GetNextContinuationToken() const {
    return m_next_continuation_token;
  }

 private:
  std::vector<Inventory> m_inventory_vec;
  bool m_is_truncated;
  std::string m_continuation_token;
  std::string m_next_continuation_token;
};

class DeleteBucketInventoryResp : public BaseResp {
 public:
  DeleteBucketInventoryResp() {}
  virtual ~DeleteBucketInventoryResp() {}
};

/// \brief: 列举直播通道响应
class ListLiveChannelResp : public BaseResp {
 public:
  ListLiveChannelResp() {}
  virtual ~ListLiveChannelResp() {}
  virtual bool ParseFromXmlString(const std::string& body);
  const ListLiveChannelResult& GetListResult() const { return result; }
  void ClearResult() { result.Clear(); }

 private:
  ListLiveChannelResult result;
};

/// \brief: 配置存储桶智能分层的响应
class PutBucketIntelligentTieringResp : public BaseResp {
 public:
  PutBucketIntelligentTieringResp() {}
  virtual ~PutBucketIntelligentTieringResp() {}
};

/// \brief: 获取配置存储桶智能分层配置的响应
class GetBucketIntelligentTieringResp : public BaseResp {
 public:
  GetBucketIntelligentTieringResp() {}
  virtual ~GetBucketIntelligentTieringResp() {}
  virtual bool ParseFromXmlString(const std::string& body);
  std::string GetStatus() const { return m_status; }
  uint32_t GetDays() const { return m_days; }
  uint32_t GetRequestFrequent() const { return m_freq; }

 private:
  std::string m_status;
  int m_days;
  int m_freq;
};

/// \brief: 配置存储桶Referer响应
class PutBucketRefererResp : public BaseResp {
 public:
  PutBucketRefererResp() {}
  virtual ~PutBucketRefererResp() {}
};

/// \brief: 获取存储桶Referer响应
class GetBucketRefererResp : public BaseResp {
 public:
  GetBucketRefererResp() {}
  virtual ~GetBucketRefererResp() {}
  virtual bool ParseFromXmlString(const std::string& body);
  std::string GetStatus() const { return m_status; }
  std::string GetRefererType() const { return m_referer_type; }
  std::vector<std::string> GetDomainList() const { return m_domain_list; }
  std::string GetEmptyReferConf() const { return m_empty_refer_conf; }
  std::string GetVerifySignatureUrl() const {return m_verify_signature_url; }

 private:
  std::string m_status;
  std::string m_referer_type;
  std::vector<std::string> m_domain_list;
  std::string m_empty_refer_conf;
  std::string m_verify_signature_url;
};

}  // namespace qcloud_cos
#endif  // BUCKET_RESP_H
