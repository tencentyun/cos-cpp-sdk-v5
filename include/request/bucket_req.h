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

private:
    std::string m_bucket_name;
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

class PutBucketLifecycleReq : public BucketReq {
public:

};
} // namespace qcloud_cos

#endif // BUCKET_REQ_H
