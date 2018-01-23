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
    BucketReq(const std::string& bucket_name) : m_bucket_name(bucket_name) { }
    virtual ~BucketReq() {}

    // getter and setter
    std::string GetBucketName() const { return m_bucket_name; }
    void SetBucketName(const std::string& bucket_name) { m_bucket_name = bucket_name; }

private:
    std::string m_bucket_name;
};

class PutBucketReq : public BucketReq {
public:
    PutBucketReq(const std::string& bucket_name)
        : BucketReq(bucket_name) {
        SetMethod("PUT");
        SetPath("/");
    }

    virtual ~PutBucketReq() {}

    /// 定义Bucket的ACL属性,有效值：private,public-read-write,public-read
    /// 默认值：private
    void SetXCosAcl(const std::string& str) {
        AddHeader("x-cos-acl", str);
    }

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
};

class GetBucketReq : public BucketReq {
public:
    GetBucketReq(const std::string& bucket_name)
        : BucketReq(bucket_name) {
        SetMethod("GET");
        SetPath("/");
    }

    virtual ~GetBucketReq() {}

    /// \@brief 设置前缀，用来规定返回的文件前缀地址
    void SetPrefix(const std::string& prefix) {
        AddParam("prefix", prefix);
    }

    /// \brief 设置定界符，如果有 Prefix，则将 Prefix 到 delimiter 之间的相同路径归为一类，
    ///        定义为 Common Prefix，然后列出所有 Common Prefix。如果没有 Prefix，则从路径起点开始
    void SetDelimiter(const std::string& delimiter) {
        AddParam("delimiter", delimiter);
    }

    /// \brief 规定返回值的编码方式，可选值：url
    void SetEncodingType(const std::string& encoding_type) {
        AddParam("encoding-type", encoding_type);
    }

    /// \brief 默认以 UTF-8 二进制顺序列出条目，所有列出条目从marker开始
    void SetMarker(const std::string& marker) {
        AddParam("marker", marker);
    }

    /// \brief 单次返回最大的条目数量，默认1000
    void SetMaxKeys(uint64_t max_keys) {
        AddParam("max-keys", StringUtil::Uint64ToString(max_keys));
    }
};

class DeleteBucketReq : public BucketReq {
public:
    DeleteBucketReq(const std::string& bucket_name)
        : BucketReq(bucket_name) {
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

    void AddRule(const LifecycleRule& rule) {
        m_rules.push_back(rule);
    }

    void SetRule(const std::vector<LifecycleRule>& rules) {
        m_rules = rules;
    }

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
    GetBucketACLReq(const std::string& bucket_name)
        : BucketReq(bucket_name) {
        SetMethod("GET");
        SetPath("/");
        AddParam("acl", "");
    }

    virtual ~GetBucketACLReq() {}
};

class PutBucketACLReq : public BucketReq {
public:
    PutBucketACLReq(const std::string& bucket_name)
        : BucketReq(bucket_name) {
        SetMethod("PUT");
        SetPath("/");
        AddParam("acl", "");
    }

    virtual ~PutBucketACLReq() {}

    /// 定义Bucket的ACL属性,有效值：private,public-read-write,public-read
    /// 默认值：private
    void SetXCosAcl(const std::string& str) {
        AddHeader("x-cos-acl", str);
    }

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
    void SetOwner(const Owner& owner) {
        m_owner = owner;
    }

    /// 设置被授权者信息与权限信息
    void SetAccessControlList(const std::vector<Grant>& grants) {
        m_acl = grants;
    }

    /// 添加单个 Bucket 的授权信息
    void AddAccessControlList(const Grant& grant) {
        m_acl.push_back(grant);
    }

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

class DeleteBucketACLReq : public BucketReq {
public:
    DeleteBucketACLReq(const std::string& bucket_name)
        : BucketReq(bucket_name) {
        SetMethod("DELETE");
    }

    virtual ~DeleteBucketACLReq() {}
};

class GetBucketCORSReq : public BucketReq {
public:
    GetBucketCORSReq(const std::string& bucket_name)
        : BucketReq(bucket_name) {
        SetMethod("GET");
        SetPath("/");
        AddParam("cors", "");
    }

    virtual ~GetBucketCORSReq() {}
};

class PutBucketCORSReq : public BucketReq {
public:
    PutBucketCORSReq(const std::string& bucket_name)
        : BucketReq(bucket_name) {
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
    DeleteBucketCORSReq(const std::string& bucket_name)
        : BucketReq(bucket_name) {
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
    void SetPrefix(const std::string& prefix) {
        AddParam("prefix", prefix);
    }

    /// \brief 设置定界符，如果有 Prefix，则将 Prefix 到 delimiter 之间的相同路径归为一类，
    ///        定义为 Common Prefix，然后列出所有 Common Prefix。如果没有 Prefix，则从路径起点开始
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

} // namespace qcloud_cos

#endif // BUCKET_REQ_H
