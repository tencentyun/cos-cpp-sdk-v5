// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "op/bucket_op.h"
#include "util/codec_util.h"

namespace qcloud_cos {
    // TODO fix this with the HeadBucketReq
bool BucketOp::IsBucketExist(const std::string& bucket_name) {
    HeadBucketReq req(bucket_name);
    HeadBucketResp resp;
    CosResult result = HeadBucket(req, &resp);

    if (result.IsSucc()) {
        return true;
    }
    return false;
}

std::string BucketOp::GetBucketLocation(const std::string& bucket_name) {
    GetBucketLocationReq req(bucket_name);
    GetBucketLocationResp resp;
    CosResult result = GetBucketLocation(req, &resp);

    if (result.IsSucc()) {
        return resp.GetLocation();
    }

    return "";
}

CosResult BucketOp::HeadBucket(const HeadBucketReq& req, HeadBucketResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucket(const PutBucketReq& req, PutBucketResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucket(const GetBucketReq& req, GetBucketResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DeleteBucket(const DeleteBucketReq& req, DeleteBucketResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketVersioning(const GetBucketVersioningReq& req,
                                        GetBucketVersioningResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketVersioning(const PutBucketVersioningReq& req,
                                        PutBucketVersioningResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();

    std::string req_body;
    if (!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("Generate PutBucketVersioning Request Body fail.");
        return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

    return NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, false, resp);
}

CosResult BucketOp::GetBucketReplication(const GetBucketReplicationReq& req,
                                         GetBucketReplicationResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketReplication(const PutBucketReplicationReq& req,
                                         PutBucketReplicationResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
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
                        additional_params, req_body, false, resp);
}

CosResult BucketOp::DeleteBucketReplication(const DeleteBucketReplicationReq& req,
                                            DeleteBucketReplicationResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketLifecycle(const GetBucketLifecycleReq& req,
                                       GetBucketLifecycleResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketLifecycle(const PutBucketLifecycleReq& req,
                                       PutBucketLifecycleResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
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
                        additional_params, req_body, false, resp);
}

CosResult BucketOp::DeleteBucketLifecycle(const DeleteBucketLifecycleReq& req,
                                          DeleteBucketLifecycleResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketACL(const GetBucketACLReq& req,
                                 GetBucketACLResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketACL(const PutBucketACLReq& req,
                                 PutBucketACLResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
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
                        additional_params, req_body, false, resp);
}

CosResult BucketOp::GetBucketCORS(const GetBucketCORSReq& req,
                                  GetBucketCORSResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketCORS(const PutBucketCORSReq& req,
                                  PutBucketCORSResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
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
                        additional_params, req_body, false, resp);
}

CosResult BucketOp::DeleteBucketCORS(const DeleteBucketCORSReq& req,
                                     DeleteBucketCORSResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketLocation(const GetBucketLocationReq& req,
                                      GetBucketLocationResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketObjectVersions(const GetBucketObjectVersionsReq& req,
                                            GetBucketObjectVersionsResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config.GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

} // qcloud_cos
