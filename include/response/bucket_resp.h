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
#include "response/base_resp.h"

namespace qcloud_cos {

class PutBucketResp : public BaseResp {
public:
    PutBucketResp() {}
    virtual ~PutBucketResp() {}
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
    std::vector<std::string> GetCommonPrefixes() const { return m_common_prefixes; }

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
} // namespace qcloud_cos
#endif // BUCKET_RESP_H
