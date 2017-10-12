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

CosResult BucketOp::DeleteBucket(const DeleteBucketReq& req, DeleteBucketResp* resp) {
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

CosResult BucketOp::GetBucketLifecycle(const GetBucketLifecycleReq& req,
                                       GetBucketLifecycleResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", resp);
}

CosResult BucketOp::PutBucketLifecycle(const PutBucketLifecycleReq& req,
                                       PutBucketLifecycleResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();

    std::string req_body;
    if (!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("Generate PutBucketLifecycle Request Body fail.");
        return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

    return NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, resp);
}

CosResult BucketOp::DeleteBucketLifecycle(const DeleteBucketLifecycleReq& req,
                                          DeleteBucketLifecycleResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", resp);
}

CosResult BucketOp::GetBucketACL(const GetBucketACLReq& req,
                                 GetBucketACLResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", resp);
}

CosResult BucketOp::PutBucketACL(const PutBucketACLReq& req,
                                 PutBucketACLResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();

    CosResult result;
    std::string req_body;
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    const std::map<std::string, std::string>& headers = req.GetHeaders();

    // 头部中不包含任何授权信息,则通过 Body 以 XML 格式传入 ACL 信息
    if (headers.find("x-cos-acl") == headers.end()
        && headers.find("x-cos-grant-read") == headers.end()
        && headers.find("x-cos-grant-write") == headers.end()
        && headers.find("x-cos-grant-full-control") == headers.end()) {
        if (!req.GenerateRequestBody(&req_body)) {
            result.SetErrorInfo("Generate PutBucketACL Request Body fail.");
            return result;
        }
        std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
        additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
    }

    return NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, resp);
}

CosResult BucketOp::GetBucketCORS(const GetBucketCORSReq& req,
                                  GetBucketCORSResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", resp);
}

CosResult BucketOp::PutBucketCORS(const PutBucketCORSReq& req,
                                  PutBucketCORSResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();

    std::string req_body;
    if (!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("Generate PutBucketCORS Request Body fail.");
        return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

    return NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, resp);
}

CosResult BucketOp::DeleteBucketCORS(const DeleteBucketCORSReq& req,
                                     DeleteBucketCORSResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", resp);
}

} // qcloud_cos
