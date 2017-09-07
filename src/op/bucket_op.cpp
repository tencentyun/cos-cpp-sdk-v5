// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "op/bucket_op.h"
#include "util/codec_util.h"

namespace qcloud_cos {

CosResult BucketOp::PutBucket(const PutBucketReq& req, PutBucketResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", resp);
}

CosResult BucketOp::GetBucket(const GetBucketReq& req, GetBucketResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", resp);
}

CosResult BucketOp::GetBucketReplication(const GetBucketReplicationReq& req,
                                         GetBucketReplicationResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", resp);
}

CosResult BucketOp::PutBucketReplication(const PutBucketReplicationReq& req,
                                         PutBucketReplicationResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();

    std::string req_body;
    if (!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("Generate PutBucketReplication Request Body fail.");
        return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

    return NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, resp);
}

CosResult BucketOp::DeleteBucketReplication(const DeleteBucketReplicationReq& req,
                                            DeleteBucketReplicationResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", resp);
}

} // qcloud_cos
