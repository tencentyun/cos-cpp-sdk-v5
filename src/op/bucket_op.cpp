// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "op/bucket_op.h"
#include "cos_defines.h"
#include "util/codec_util.h"
#include "util/retry_util.h"

namespace qcloud_cos {

bool BucketOp::IsBucketExist(const std::string& bucket_name) {
  HeadBucketReq req(bucket_name);
  HeadBucketResp resp;
  CosResult result = HeadBucket(req, &resp);

  if (result.IsSucc()) {
    return true;
  }else if (UseDefaultDomain() && RetryUtil::ShouldRetryWithChangeDomain(result)){
    result = HeadBucket(req, &resp, COS_CHANGE_BACKUP_DOMAIN);
    if (result.IsSucc()) {
      return true;
    }
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
  result = GetBucketLocation(req, &resp, COS_CHANGE_BACKUP_DOMAIN);
  if (result.IsSucc()) {
    return resp.GetLocation();
  }
  return "";
}

CosResult BucketOp::HeadBucket(const HeadBucketReq& req, HeadBucketResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucket(const PutBucketReq& req, PutBucketResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  std::string req_body = req.GetBody();
  return NormalAction(host, path, req, req_body, false, resp);
}

CosResult BucketOp::GetBucket(const GetBucketReq& req, GetBucketResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::ListMultipartUpload(const ListMultipartUploadReq& req,
                                        ListMultipartUploadResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DeleteBucket(const DeleteBucketReq& req,
                                 DeleteBucketResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketVersioning(const GetBucketVersioningReq& req,
                                        GetBucketVersioningResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketVersioning(const PutBucketVersioningReq& req,
                                        PutBucketVersioningResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  std::string req_body;
  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate PutBucketVersioning Request Body fail.");
    return result;
  }
  std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::GetBucketReplication(const GetBucketReplicationReq& req,
                                         GetBucketReplicationResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketReplication(const PutBucketReplicationReq& req,
                                         PutBucketReplicationResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  std::string req_body;
  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate PutBucketReplication Request Body fail.");
    return result;
  }
  std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::DeleteBucketReplication(
    const DeleteBucketReplicationReq& req, DeleteBucketReplicationResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketLifecycle(const GetBucketLifecycleReq& req,
                                       GetBucketLifecycleResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketLifecycle(const PutBucketLifecycleReq& req,
                                       PutBucketLifecycleResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  std::string req_body;
  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate PutBucketLifecycle Request Body fail.");
    return result;
  }
  std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::DeleteBucketLifecycle(const DeleteBucketLifecycleReq& req,
                                          DeleteBucketLifecycleResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketACL(const GetBucketACLReq& req,
                                 GetBucketACLResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketACL(const PutBucketACLReq& req,
                                 PutBucketACLResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  CosResult result;
  std::string req_body;
  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  const std::map<std::string, std::string>& headers = req.GetHeaders();

  // 头部中不包含任何授权信息,则通过 Body 以 XML 格式传入 ACL 信息
  if (headers.find("x-cos-acl") == headers.end() &&
      headers.find("x-cos-grant-read") == headers.end() &&
      headers.find("x-cos-grant-write") == headers.end() &&
      headers.find("x-cos-grant-full-control") == headers.end()) {
    if (!req.GenerateRequestBody(&req_body)) {
      result.SetErrorMsg("Generate PutBucketACL Request Body fail.");
      return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
  }

  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::PutBucketPolicy(const PutBucketPolicyReq& req,
                                   PutBucketPolicyResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  std::string req_body = req.GetBody();
  std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::GetBucketPolicy(const GetBucketPolicyReq& req,
                                   GetBucketPolicyResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DeleteBucketPolicy(const DeleteBucketPolicyReq& req,
                                      DeleteBucketPolicyResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketCORS(const GetBucketCORSReq& req,
                                  GetBucketCORSResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketCORS(const PutBucketCORSReq& req,
                                  PutBucketCORSResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  std::string req_body;
  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate PutBucketCORS Request Body fail.");
    return result;
  }
  std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::DeleteBucketCORS(const DeleteBucketCORSReq& req,
                                     DeleteBucketCORSResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketLocation(const GetBucketLocationReq& req,
                                      GetBucketLocationResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketObjectVersions(
    const GetBucketObjectVersionsReq& req, GetBucketObjectVersionsResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketLogging(const PutBucketLoggingReq& req,
                                     PutBucketLoggingResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);

  std::string path = req.GetPath();
  std::string req_body;
  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate PutBucketLogging Request Body fail.");
    return result;
  }
  std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::GetBucketLogging(const GetBucketLoggingReq& req,
                                     GetBucketLoggingResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketDomain(const PutBucketDomainReq& req,
                                    PutBucketDomainResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  std::string req_body;

  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate PutBucketDomain Request Body fail.");
    return result;
  }

  std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::GetBucketDomain(const GetBucketDomainReq& req,
                                    GetBucketDomainResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketWebsite(const PutBucketWebsiteReq& req,
                                     PutBucketWebsiteResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  std::string req_body;
  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate PutBucketWebsite Request Body fail.");
    return result;
  }

  return NormalAction(host, path, req, req_body, false, resp);
}

CosResult BucketOp::GetBucketWebsite(const GetBucketWebsiteReq& req,
                                     GetBucketWebsiteResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DeleteBucketWebsite(const DeleteBucketWebsiteReq& req,
                                        DeleteBucketWebsiteResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketTagging(const PutBucketTaggingReq& req,
                                     PutBucketTaggingResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  std::string req_body;
  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate PutBucketWebsite Request Body fail.");
    return result;
  }

  std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::GetBucketTagging(const GetBucketTaggingReq& req,
                                     GetBucketTaggingResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DeleteBucketTagging(const DeleteBucketTaggingReq& req,
                                        DeleteBucketTaggingResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketInventory(const PutBucketInventoryReq& req,
                                       PutBucketInventoryResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  // id必须设置.
  if (!req.HasId()) {
    CosResult result;
    result.SetErrorMsg("PutBucketinventory need to set Id.");
    return result;
  }
  std::string req_body;
  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate PutBucketWebsite Request Body fail.");
    return result;
  }

  std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::GetBucketInventory(const GetBucketInventoryReq& req,
                                       GetBucketInventoryResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  // id必须设置.
  if (!req.HasId()) {
    CosResult result;
    result.SetErrorMsg("GetBucketinventory need to set Id.");
    return result;
  }

  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::ListBucketInventoryConfigurations(
    const ListBucketInventoryConfigurationsReq& req,
    ListBucketInventoryConfigurationsResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DeleteBucketInventory(const DeleteBucketInventoryReq& req,
                                          DeleteBucketInventoryResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  // id必须设置.
  if (!req.HasId()) {
    CosResult result;
    result.SetErrorMsg("DeleteBucketinventory need to set Id.");
    return result;
  }

  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketReferer(const PutBucketRefererReq& req,
                                     PutBucketRefererResp* resp, bool change_backup_domain) {
  return ProcessReq(req, resp);
}

CosResult BucketOp::GetBucketReferer(const GetBucketRefererReq& req,
                                     GetBucketRefererResp* resp, bool change_backup_domain) {
  return ProcessReq(req, resp);
}

CosResult BucketOp::ListLiveChannel(const ListLiveChannelReq& req,
                                    ListLiveChannelResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketIntelligentTiering(
    const PutBucketIntelligentTieringReq& req,
    PutBucketIntelligentTieringResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();

  std::string req_body;
  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate PutBucketWebsite Request Body fail.");
    return result;
  }

  std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, false, resp);
}

CosResult BucketOp::GetBucketIntelligentTiering(
    const GetBucketIntelligentTieringReq& req,
    GetBucketIntelligentTieringResp* resp, bool change_backup_domain) {
  std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName(), change_backup_domain);
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketToCI(const PutBucketToCIReq& req,
                             PutBucketToCIResp* resp) {
  std::string host = CosSysConfig::GetPICHost(GetAppId(), m_config->GetRegion(),
                                           req.GetBucketName());
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::ProcessReq(const BucketReq& req, BaseResp* resp,
                               bool is_ci_req) {
  std::string host =
      is_ci_req
          ? CosSysConfig::GetCIHost(req.GetBucketName(), m_config->GetRegion())
          : CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                  req.GetBucketName());
  std::string path = req.GetPath();

  std::string req_body;
  if (!req.GenerateRequestBody(&req_body)) {
    CosResult result;
    result.SetErrorMsg("Generate request body fail.");
    return result;
  }
  if (is_ci_req) {
    if (!req_body.empty()) {
      std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
      std::map<std::string, std::string> additional_headers;
      std::map<std::string, std::string> additional_params;
      additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
      return NormalAction(host, path, req, additional_headers, additional_params,
                          req_body, false, resp, is_ci_req);
    } else {
      return NormalAction(host, path, req, "", false, resp, is_ci_req);
    }
  } else {
    if (!req_body.empty()) {
      std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
      std::map<std::string, std::string> additional_headers;
      std::map<std::string, std::string> additional_params;
      additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
      CosResult result = NormalAction(host, path, req, additional_headers, additional_params,
                          req_body, false, resp, is_ci_req);
      if(UseDefaultDomain() && (RetryUtil::ShouldRetryWithChangeDomain(result))){
        host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                              req.GetBucketName(),COS_CHANGE_BACKUP_DOMAIN);
        return NormalAction(host, path, req, additional_headers, additional_params,
                          req_body, false, resp, is_ci_req);
      }
      return result;
    } else {
      CosResult result = NormalAction(host, path, req, "", false, resp, is_ci_req);
      if(UseDefaultDomain() && (RetryUtil::ShouldRetryWithChangeDomain(result))){
        host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                              req.GetBucketName(),COS_CHANGE_BACKUP_DOMAIN);
        return NormalAction(host, path, req, "", false, resp, is_ci_req);
      }
      return result;
    }
  }
}

CosResult BucketOp::CreateDocBucket(const CreateDocBucketReq& req,
                                    CreateDocBucketResp* resp) {
  std::string host = CosSysConfig::GetCIHost(req.GetBucketName(), m_config->GetRegion());
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::CreateDocProcessJobs(const CreateDocProcessJobsReq& req,
                                         CreateDocProcessJobsResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeDocProcessJob(const DescribeDocProcessJobReq& req,
                                          DescribeDocProcessJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeDocProcessJobs(const DescribeDocProcessJobsReq& req,
                                           DescribeDocProcessJobsResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeDocProcessQueues(const DescribeDocProcessQueuesReq& req, 
                                            DescribeDocProcessQueuesResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::UpdateDocProcessQueue(const UpdateDocProcessQueueReq& req,
                                          UpdateDocProcessQueueResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeMediaBuckets(const DescribeMediaBucketsReq& req,
                                         DescribeMediaBucketsResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::CreateMediaBucket(const CreateMediaBucketReq& req,
                                    CreateMediaBucketResp* resp) {
  std::string host = CosSysConfig::GetCIHost(req.GetBucketName(), m_config->GetRegion());
  std::string path = req.GetPath();
  return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DescribeMediaQueues(const DescribeMediaQueuesReq& req, 
                                        DescribeQueuesResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::UpdateMediaQueue(const UpdateMediaQueueReq& req,
                                          UpdateQueueResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::BatchImageAuditing(const BatchImageAuditingReq& req,
                                       BatchImageAuditingResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::CreateDataProcessJobs(const CreateDataProcessJobsReq& req,
                                 CreateDataProcessJobsResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::CancelDataProcessJob(const CancelDataProcessJobReq& req,
                                 CancelDataProcessJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeDataProcessJob(const DescribeDataProcessJobReq& req,
                                 DescribeDataProcessJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeImageAuditingJob(const DescribeImageAuditingJobReq& req,
                                             DescribeImageAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::CreateVideoAuditingJob(const CreateVideoAuditingJobReq& req,
                                             CreateVideoAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeVideoAuditingJob(const DescribeVideoAuditingJobReq& req,
                                   DescribeVideoAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::CreateAudioAuditingJob(const CreateAudioAuditingJobReq& req,
                                             CreateAudioAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeAudioAuditingJob(const DescribeAudioAuditingJobReq& req,
                                   DescribeAudioAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::CreateTextAuditingJob(const CreateTextAuditingJobReq& req,
                                             CreateTextAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeTextAuditingJob(const DescribeTextAuditingJobReq& req,
                                   DescribeTextAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::CreateDocumentAuditingJob(const CreateDocumentAuditingJobReq& req,
                                              CreateDocumentAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeDocumentAuditingJob(const DescribeDocumentAuditingJobReq& req,
                                      DescribeDocumentAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::CreateWebPageAuditingJob(const CreateWebPageAuditingJobReq& req,
                                              CreateWebPageAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

CosResult BucketOp::DescribeWebPageAuditingJob(const DescribeWebPageAuditingJobReq& req,
                                      DescribeWebPageAuditingJobResp* resp) {
  return ProcessReq(req, resp, true);
}

}  // namespace qcloud_cos
