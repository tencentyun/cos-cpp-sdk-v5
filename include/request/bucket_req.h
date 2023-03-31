// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#ifndef BUCKET_REQ_H
#define BUCKET_REQ_H
#pragma once

#include "cos_defines.h"
#include "request/base_req.h"
#include "util/string_util.h"

namespace qcloud_cos {

class BucketReq : public BaseReq {
 public:
  BucketReq() : m_bucket_name("") {}
  BucketReq(const std::string& bucket_name) : m_bucket_name(bucket_name) {}
  virtual ~BucketReq() {}

  // getter and setter
  std::string GetBucketName() const { return m_bucket_name; }
  void SetBucketName(const std::string& bucket_name) {
    m_bucket_name = bucket_name;
  }
  virtual bool GenerateRequestBody(std::string* body) const { UNUSED_PARAM(body); return true; }

 private:
  std::string m_bucket_name;
};

class HeadBucketReq : public BucketReq {
 public:
  HeadBucketReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    m_method = "HEAD";
  }

  virtual ~HeadBucketReq() {}
};

class PutBucketReq : public BucketReq {
 public:
  PutBucketReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/");
  }

  virtual ~PutBucketReq() {}

  /// 定义Bucket的ACL属性,有效值：private,public-read-write,public-read
  /// 默认值：private
  void SetXCosAcl(const std::string& str) { AddHeader("x-cos-acl", str); }

  /// 赋予被授权者读的权限.格式：x-cos-grant-read: id=" ",id=" ".
  /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>"
  /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
  void SetXCosGrantRead(const std::string& str) {
    AddHeader("x-cos-grant-read", str);
  }

  /// 赋予被授权者写的权限,格式：x-cos-grant-write: id=" ",id=" "./
  /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
  /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
  void SetXCosGrantWrite(const std::string& str) {
    AddHeader("x-cos-grant-write", str);
  }

  /// 赋予被授权者读写权限.格式：x-cos-grant-full-control: id=" ",id=" ".
  /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
  /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
  void SetXCosGrantFullControl(const std::string& str) {
    AddHeader("x-cos-grant-full-control", str);
  }

  //设置桶为多az存储桶
  void SetMAZBucket() {
    std::string maz = "<CreateBucketConfiguration>";
    maz += "    <BucketAZConfig>MAZ</BucketAZConfig>";
    maz += "</CreateBucketConfiguration>";
    SetBody(maz);
  }
};

class GetBucketReq : public BucketReq {
 public:
  GetBucketReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
  }

  virtual ~GetBucketReq() {}

  /// \@brief 设置前缀，用来规定返回的文件前缀地址
  void SetPrefix(const std::string& prefix) { AddParam("prefix", prefix); }

  /// \brief 设置定界符，如果有 Prefix，则将 Prefix 到 delimiter
  /// 之间的相同路径归为一类，
  ///        定义为 Common Prefix，然后列出所有 Common Prefix。如果没有
  ///        Prefix，则从路径起点开始
  void SetDelimiter(const std::string& delimiter) {
    AddParam("delimiter", delimiter);
  }

  /// \brief 规定返回值的编码方式，可选值：url
  void SetEncodingType(const std::string& encoding_type) {
    AddParam("encoding-type", encoding_type);
  }

  /// \brief 默认以 UTF-8 二进制顺序列出条目，所有列出条目从marker开始
  void SetMarker(const std::string& marker) { AddParam("marker", marker); }

  /// \brief 单次返回最大的条目数量，默认1000
  void SetMaxKeys(uint64_t max_keys) {
    AddParam("max-keys", StringUtil::Uint64ToString(max_keys));
  }
};

class ListMultipartUploadReq : public BucketReq {
 public:
  ListMultipartUploadReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("uploads", "");
  }

  virtual ~ListMultipartUploadReq() {}

  void SetPrefix(const std::string& prefix) { AddParam("prefix", prefix); }

  void SetDelimiter(const std::string& delimiter) {
    AddParam("delimiter", delimiter);
  }

  void SetEncodingType(const std::string& encoding_type) {
    AddParam("encoding-type", encoding_type);
  }

  void SetKeyMarker(const std::string& marker) {
    AddParam("key-marker", marker);
  }

  void SetMaxUploads(const std::string& max_uploads) {
    AddParam("max-uploads", max_uploads);
  }

  void SetUploadIdMarker(const std::string& upload_id_marker) {
    AddParam("upload-id-marker", upload_id_marker);
  }
};

class DeleteBucketReq : public BucketReq {
 public:
  DeleteBucketReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("DELETE");
  }

  ~DeleteBucketReq() {}
};

class GetBucketVersioningReq : public BucketReq {
 public:
  GetBucketVersioningReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("versioning", "");
  }

  virtual ~GetBucketVersioningReq() {}
};

class PutBucketVersioningReq : public BucketReq {
 public:
  PutBucketVersioningReq(const std::string& bucket_name)
      : BucketReq(bucket_name), m_status(true) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("versioning", "");
  }

  virtual ~PutBucketVersioningReq() {}

  /// \brief 版本是否开启
  void SetStatus(bool is_enable) { m_status = is_enable; }

  bool GetStatus() const { return m_status; }

  bool GenerateRequestBody(std::string* body) const;

 private:
  bool m_status;
};

class GetBucketReplicationReq : public BucketReq {
 public:
  GetBucketReplicationReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("replication", "");
  }

  virtual ~GetBucketReplicationReq() {}
};

class PutBucketReplicationReq : public BucketReq {
 public:
  PutBucketReplicationReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("replication", "");
  }

  virtual ~PutBucketReplicationReq() {}

  void SetRole(const std::string& role) { m_role = role; }

  void AddReplicationRule(const ReplicationRule& rule) {
    m_rules.push_back(rule);
  }

  void SetReplicationRule(const std::vector<ReplicationRule>& rules) {
    m_rules = rules;
  }

  bool GenerateRequestBody(std::string* body) const;

 private:
  std::string m_role;
  std::vector<ReplicationRule> m_rules;
};

class DeleteBucketReplicationReq : public BucketReq {
 public:
  DeleteBucketReplicationReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("Delete");
    SetPath("/");
    AddParam("replication", "");
  }

  virtual ~DeleteBucketReplicationReq() {}
};

class GetBucketLifecycleReq : public BucketReq {
 public:
  GetBucketLifecycleReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("lifecycle", "");
  }

  virtual ~GetBucketLifecycleReq() {}
};

class PutBucketLifecycleReq : public BucketReq {
 public:
  PutBucketLifecycleReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("lifecycle", "");
  }

  virtual ~PutBucketLifecycleReq() {}

  void AddRule(const LifecycleRule& rule) { m_rules.push_back(rule); }

  void SetRule(const std::vector<LifecycleRule>& rules) { m_rules = rules; }

  bool GenerateRequestBody(std::string* body) const;

 private:
  std::vector<LifecycleRule> m_rules;
};

class DeleteBucketLifecycleReq : public BucketReq {
 public:
  DeleteBucketLifecycleReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("DELETE");
    SetPath("/");
    AddParam("lifecycle", "");
  }

  virtual ~DeleteBucketLifecycleReq() {}
};

class GetBucketACLReq : public BucketReq {
 public:
  GetBucketACLReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("acl", "");
  }

  virtual ~GetBucketACLReq() {}
};

class PutBucketACLReq : public BucketReq {
 public:
  PutBucketACLReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("acl", "");
  }

  virtual ~PutBucketACLReq() {}

  /// 定义Bucket的ACL属性,有效值：private,public-read-write,public-read
  /// 默认值：private
  void SetXCosAcl(const std::string& str) { AddHeader("x-cos-acl", str); }

  /// 赋予被授权者读的权限.格式：x-cos-grant-read: id=" ",id=" ".
  /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>"
  /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
  void SetXCosGrantRead(const std::string& str) {
    AddHeader("x-cos-grant-read", str);
  }

  /// 赋予被授权者写的权限,格式：x-cos-grant-write: id=" ",id=" "./
  /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
  /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
  void SetXCosGrantWrite(const std::string& str) {
    AddHeader("x-cos-grant-write", str);
  }

  /// 赋予被授权者读写权限.格式：x-cos-grant-full-control: id=" ",id=" ".
  /// 当需要给子账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<SubUin>",
  /// 当需要给根账户授权时,id="qcs::cam::uin/<OwnerUin>:uin/<OwnerUin>"
  void SetXCosGrantFullControl(const std::string& str) {
    AddHeader("x-cos-grant-full-control", str);
  }

  /// Bucket 持有者 ID
  void SetOwner(const Owner& owner) { m_owner = owner; }

  /// 设置被授权者信息与权限信息
  void SetAccessControlList(const std::vector<Grant>& grants) {
    m_acl = grants;
  }

  /// 添加单个 Bucket 的授权信息
  void AddAccessControlList(const Grant& grant) { m_acl.push_back(grant); }

  /// 清空权限信息
  void ClearAccessControlList() {
    std::vector<Grant> tmp;
    m_acl.swap(tmp);
  }

  bool GenerateRequestBody(std::string* body) const;

 private:
  Owner m_owner;
  std::vector<Grant> m_acl;
};

class GetBucketPolicyReq : public BucketReq {
 public:
  GetBucketPolicyReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("policy", "");
  }

  virtual ~GetBucketPolicyReq() {}
};

class PutBucketPolicyReq : public BucketReq {
 public:
  PutBucketPolicyReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("policy", "");
  }

  virtual ~PutBucketPolicyReq() {}
};

class DeleteBucketPolicyReq : public BucketReq {
 public:
  DeleteBucketPolicyReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("DELETE");
    SetPath("/");
    AddParam("policy", "");
  }

  virtual ~DeleteBucketPolicyReq() {}
};

class DeleteBucketACLReq : public BucketReq {
 public:
  DeleteBucketACLReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("DELETE");
  }

  virtual ~DeleteBucketACLReq() {}
};

class GetBucketCORSReq : public BucketReq {
 public:
  GetBucketCORSReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("cors", "");
  }

  virtual ~GetBucketCORSReq() {}
};

class PutBucketCORSReq : public BucketReq {
 public:
  PutBucketCORSReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("cors", "");
    AddHeader("Content-Type", "application/xml");
  }

  virtual ~PutBucketCORSReq() {}

  bool GenerateRequestBody(std::string* body) const;

  void AddRule(const CORSRule& rule) { m_rules.push_back(rule); }

  void SetRules(const std::vector<CORSRule>& rules) { m_rules = rules; }

 private:
  std::vector<CORSRule> m_rules;
};

class DeleteBucketCORSReq : public BucketReq {
 public:
  DeleteBucketCORSReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("DELETE");
    SetPath("/");
    AddParam("cors", "");
  }

  virtual ~DeleteBucketCORSReq() {}
};

class GetBucketLocationReq : public BucketReq {
 public:
  GetBucketLocationReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("location", "");
  }

  virtual ~GetBucketLocationReq() {}
};

class GetBucketObjectVersionsReq : public BucketReq {
 public:
  GetBucketObjectVersionsReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("versions", "");
  }

  virtual ~GetBucketObjectVersionsReq() {}

  /// \@brief 设置前缀，用来规定返回的文件前缀地址
  void SetPrefix(const std::string& prefix) { AddParam("prefix", prefix); }

  /// \brief 设置定界符，如果有 Prefix，则将 Prefix 到 delimiter
  /// 之间的相同路径归为一类，
  ///        定义为 Common Prefix，然后列出所有 Common Prefix。如果没有
  ///        Prefix，则从路径起点开始
  void SetDelimiter(const std::string& delimiter) {
    AddParam("delimiter", delimiter);
  }

  /// \brief 规定返回值的编码方式，可选值：url
  void SetEncodingType(const std::string& encoding_type) {
    AddParam("encoding-type", encoding_type);
  }

  /// \brief 默认以 UTF-8 二进制顺序列出条目，所有列出条目从marker开始
  void SetKeyMarker(const std::string& marker) {
    AddParam("key-marker", marker);
  }

  /// \brief 单次返回最大的条目数量，默认1000
  void SetMaxKeys(uint64_t max_keys) {
    AddParam("max-keys", StringUtil::Uint64ToString(max_keys));
  }

  void SetVersionIdMarker(const std::string& version_id_marker) {
    AddParam("version-id-marker", version_id_marker);
  }
};

class PutBucketLoggingReq : public BucketReq {
 public:
  PutBucketLoggingReq(const std::string& bucket_name)
      : BucketReq(bucket_name), m_mask(0x00000000u) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("logging", "");
  }
  virtual ~PutBucketLoggingReq() {}

  void SetLoggingEnabled(const LoggingEnabled& rules) {
    m_mask = m_mask | 0x00000001u;
    m_rules = rules;
  }
  bool HasLoggingEnabled() const { return (m_mask & 0x00000001u) != 0; }

  bool GenerateRequestBody(std::string* body) const;

 private:
  uint64_t m_mask;
  LoggingEnabled m_rules;
};

class GetBucketLoggingReq : public BucketReq {
 public:
  GetBucketLoggingReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("logging", "");
  }
  virtual ~GetBucketLoggingReq() {}
};

class PutBucketDomainReq : public BucketReq {
 public:
  PutBucketDomainReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("domain", "");
  }
  virtual ~PutBucketDomainReq() {}

  void SetDomainRule(const DomainRule& rules) { m_rules = rules; }
  bool GenerateRequestBody(std::string* body) const;

 private:
  DomainRule m_rules;
};

class GetBucketDomainReq : public BucketReq {
 public:
  GetBucketDomainReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("domain", "");
  }
  virtual ~GetBucketDomainReq() {}
};

class PutBucketWebsiteReq : public BucketReq {
 public:
  PutBucketWebsiteReq(const std::string& bucket_name)
      : BucketReq(bucket_name),
        m_mask(0x00000000u),
        m_suffix(""),
        m_protocol(""),
        m_key("") {
    SetMethod("PUT");
    SetPath("/");
    AddParam("website", "");
  }

  virtual ~PutBucketWebsiteReq() {}

  void SetSuffix(const std::string& suffix) {
    m_mask = m_mask | 0x00000001u;
    m_suffix = suffix;
  }

  void SetProtocol(const std::string& protocol) {
    m_mask = m_mask | 0x00000002u;
    m_protocol = protocol;
  }

  void SetKey(const std::string& key) {
    m_mask = m_mask | 0x00000004u;
    m_key = key;
  }

  std::string GetSuffix() const { return m_suffix; }

  std::string GetProtocol() const { return m_protocol; }

  std::string GetKey() const { return m_key; }

  bool HasSuffix() const { return (m_mask & 0x00000001u) != 0; }

  bool HasProtocol() const { return (m_mask & 0x00000002u) != 0; }

  bool HasKey() const { return (m_mask & 0x00000004u) != 0; }

  bool HasRoutingRules() const { return !m_routingrules.empty(); }

  /// 设置重定向规则
  void SetRoutingRules(const std::vector<RoutingRule>& routingrules) {
    m_routingrules = routingrules;
  }

  /// 添加单个rule
  void AddRoutingRule(const RoutingRule& routingrule) {
    m_routingrules.push_back(routingrule);
  }

  /// 清空重定向规则
  void ClearRoutingRules() {
    std::vector<RoutingRule> tmp;
    m_routingrules.swap(tmp);
  }

  bool GenerateRequestBody(std::string* body) const;

 private:
  uint64_t m_mask;
  std::string m_suffix;
  std::string m_protocol;
  std::string m_key;
  std::vector<RoutingRule> m_routingrules;
};

class GetBucketWebsiteReq : public BucketReq {
 public:
  GetBucketWebsiteReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("Get");
    SetPath("/");
    AddParam("website", "");
  }

  virtual ~GetBucketWebsiteReq() {}
};

class DeleteBucketWebsiteReq : public BucketReq {
 public:
  DeleteBucketWebsiteReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("DELETE");
    SetPath("/");
    AddParam("website", "");
  }

  virtual ~DeleteBucketWebsiteReq() {}
};

class PutBucketTaggingReq : public BucketReq {
 public:
  PutBucketTaggingReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("tagging", "");
  }

  void SetTagSet(std::vector<Tag>& tagset) { m_tagset = tagset; }

  std::vector<Tag> GetTagSet() { return m_tagset; }

  //清除tag规则.
  void ClearTagSet() {
    std::vector<Tag> temp;
    m_tagset.swap(temp);
  }

  /// 添加单个tag.
  void AddTag(const Tag& tag) { m_tagset.push_back(tag); }

  bool GenerateRequestBody(std::string* body) const;
  virtual ~PutBucketTaggingReq() {}

 private:
  std::vector<Tag> m_tagset;
};

class GetBucketTaggingReq : public BucketReq {
 public:
  GetBucketTaggingReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("tagging", "");
  }
  virtual ~GetBucketTaggingReq() {}
};

class DeleteBucketTaggingReq : public BucketReq {
 public:
  DeleteBucketTaggingReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("DELETE");
    SetPath("/");
    AddParam("tagging", "");
  }

  virtual ~DeleteBucketTaggingReq() {}
};

class PutBucketInventoryReq : public BucketReq {
 public:
  PutBucketInventoryReq(const std::string& bucket_name)
      : BucketReq(bucket_name), m_mask(0x00000000u) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("inventory", "");
  }

  void SetInventory(Inventory& inventory) {
    m_mask = m_mask | 0x00000001u;
    m_inventory = inventory;
  }

  bool HasInventory() const { return (m_mask & 0x00000001u) != 0; }

  const Inventory& GetInventory() const { return m_inventory; }

  void SetId(const std::string id) {
    m_mask = m_mask | 0x00000001u;
    m_id = id;
    AddParam("id", m_id);
  }

  std::string GetId() const { return m_id; }

  bool HasId() const { return (m_mask & 0x00000001u) != 0; }

  bool GenerateRequestBody(std::string* body) const;
  virtual ~PutBucketInventoryReq() {}

 private:
  uint64_t m_mask;
  std::string m_id;
  Inventory m_inventory;
};

class GetBucketInventoryReq : public BucketReq {
 public:
  GetBucketInventoryReq(const std::string& bucket_name)
      : BucketReq(bucket_name), m_mask(0x00000000u) {
    SetMethod("GET");
    SetPath("/");
    AddParam("inventory", "");
  }

  void SetId(const std::string id) {
    m_mask = m_mask | 0x00000001u;
    m_id = id;
    AddParam("id", m_id);
  }

  std::string GetId() const { return m_id; }

  bool HasId() const { return (m_mask & 0x00000001u) != 0; }

  virtual ~GetBucketInventoryReq() {}

 private:
  uint64_t m_mask;
  std::string m_id;
};

class ListBucketInventoryConfigurationsReq : public BucketReq {
 public:
  ListBucketInventoryConfigurationsReq(const std::string& bucket_name)
      : BucketReq(bucket_name), m_mask(0x00000000u) {
    SetMethod("GET");
    SetPath("/");
    AddParam("inventory", "");
  }

  void SetContinuationToken(const std::string continuation_token) {
    m_mask = m_mask | 0x00000001u;
    m_continuation_token = continuation_token;
    AddParam("continuation-token", m_continuation_token);
  }

  std::string GetContinuationToken() const { return m_continuation_token; }

  bool HasContinuationToken() const { return (m_mask & 0x00000001u) != 0; }

  virtual ~ListBucketInventoryConfigurationsReq() {}

 private:
  uint64_t m_mask;
  std::string m_continuation_token;
};

class DeleteBucketInventoryReq : public BucketReq {
 public:
  DeleteBucketInventoryReq(const std::string& bucket_name)
      : BucketReq(bucket_name), m_mask(0x00000000u), m_id("") {
    SetMethod("DELETE");
    SetPath("/");
    AddParam("inventory", "");
  }

  void SetId(const std::string id) {
    m_mask = m_mask | 0x00000001u;
    m_id = id;
    AddParam("id", m_id);
  }

  std::string GetId() const { return m_id; }

  bool HasId() const { return (m_mask & 0x00000001u) != 0; }

  virtual ~DeleteBucketInventoryReq() {}

 private:
  uint64_t m_mask;
  std::string m_id;
};

/// \brief: 列举直播通道请求
class ListLiveChannelReq : public BucketReq {
 public:
  ListLiveChannelReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("live", "");
  }
  void SetMaxKeys(uint32_t max_keys) {
    AddParam("max-keys", StringUtil::IntToString(max_keys));
  }
  void SetMarker(const std::string& marker) { AddParam("marker", marker); }
  void SetPrefix(const std::string& prefix) { AddParam("prefix", prefix); }
  virtual ~ListLiveChannelReq() {}
};

/// \brief: 配置存储桶智能分层特性
class PutBucketIntelligentTieringReq : public BucketReq {
 public:
  PutBucketIntelligentTieringReq(const std::string& bucket_name)
      : BucketReq(bucket_name), m_status(false), m_days(30) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("intelligenttiering", "");
  }
  virtual ~PutBucketIntelligentTieringReq() {}

  /// \brief 设置智能分层的状态，true表示Enabled，false表示Suspended
  void SetStatus(bool is_enable) { m_status = is_enable; }

  /// \brief
  /// 指定智能分层存储配置中标准层数据转换为低频层数据的天数限制，默认值为30天
  void SetDays(uint32_t days) { m_days = days; }

  bool GenerateRequestBody(std::string* body) const;

 private:
  bool m_status;
  uint32_t m_days;
};

/// \brief: 获取存储桶智能分层配置
class GetBucketIntelligentTieringReq : public BucketReq {
 public:
  GetBucketIntelligentTieringReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("intelligenttiering", "");
  }
  virtual ~GetBucketIntelligentTieringReq() {}
};

class PutBucketRefererReq : public BucketReq {
 public:
  PutBucketRefererReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/");
    AddParam("referer", "");
  }

  /// \brief 是否开启防盗链，枚举值：Enabled、Disabled
  void SetStatus(const std::string& status) { m_status = status; }

  /// \brief 防盗链类型，枚举值：Black-List、White-List
  void SetRefererType(const std::string& referer_type) {
    m_referer_type = referer_type;
  }

  /// \brief 生效域名列表， 支持多个域名且为前缀匹配， 支持带端口的域名和 IP，
  /// 支持通配符*，做二级域名或多级域名的通配
  void AddDomain(const std::string& domain) { m_domain_list.push_back(domain); }

  /// \brief 是否允许空 Referer 访问，枚举值：Allow、Deny，默认值为 Deny
  void SetEmptyReferConfig(const std::string& empty_refer_conf) {
    m_empty_refer_conf = empty_refer_conf;
  }

  bool GenerateRequestBody(std::string* body) const;
  virtual ~PutBucketRefererReq() {}

 private:
  std::string m_status;
  std::string m_referer_type;
  std::vector<std::string> m_domain_list;
  std::string m_empty_refer_conf;
};

class GetBucketRefererReq : public BucketReq {
 public:
  GetBucketRefererReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/");
    AddParam("referer", "");
  }
  virtual ~GetBucketRefererReq() {}
};

}  // namespace qcloud_cos

#endif  // BUCKET_REQ_H
