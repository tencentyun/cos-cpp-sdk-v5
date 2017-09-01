// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/13/17
// Description:

#ifndef BUCKETOP_H
#define BUCKETOP_H
#pragma once

#include "cos_sys_config.h"
#include "op/base_op.h"
#include "op/cos_result.h"
#include "request/bucket_req.h"
#include "response/bucket_resp.h"

namespace qcloud_cos {

/// \brief 封装了Bucket相关的操作
class BucketOp : public BaseOp {
public:
    /// \brief BucketOp构造函数
    ///
    /// \param cos_conf Cos配置
    explicit BucketOp(CosConfig& config) : BaseOp(config) {}

    /// \brief BucketOp析构函数
    virtual ~BucketOp() {}

    /// \brief 列出Bucket下的部分或者全部Object
    ///        (详见:https://www.qcloud.com/document/product/436/7734)
    ///
    /// \param req  GetBucket请求
    /// \param resp GetBucket返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucket(const GetBucketReq& req, GetBucketResp* resp);

    /// \brief 列出Bucket下的跨域复制配置
    ///
    /// \param req  GetBucketReplication请求
    /// \param resp GetBucketReplication返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketReplication(const GetBucketReplicationReq& req,
                                   GetBucketReplicationResp* resp);

    /// \brief 增加/替换Bucket下的跨域复制配置
    ///
    /// \param req  PutBucketReplication请求
    /// \param resp PutBucketReplication返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketReplication(const PutBucketReplicationReq& req,
                                   PutBucketReplicationResp* resp);

    /// \brief 删除Bucket下的跨域复制配置
    ///
    /// \param req  DeleteBucketReplication请求
    /// \param resp DeleteBucketReplication返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketReplication(const DeleteBucketReplicationReq& req,
                                      DeleteBucketReplicationResp* resp);

    // TODO(sevenyou)
    // std::string GetBucketCORS();
    // std::string GetBucketACL()
    // std::string GetBucketLocation();
    // std::string GetBucketLifecycle();
    // std::string GetBucketTagging();
    // std::string PutBucket()
    // std::string PutBucketACL();
    // std::string PutBucketCORS();
    // std::string PutBucketLifecycle();
    // std::string PutBucketTagging();
    // std::string DeleteBucket();
    // std::string DeleteBucketCORS();
    // std::string DeleteBucketLifecycle();
    // std::string DeleteBucketTagging();
    // std::string HeadBucket();
    // std::string ListMultipartUploads();
};

} // namespace qcloud_cos
#endif // BUCKETOP_H
