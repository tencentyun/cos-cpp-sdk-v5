// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:


#include "op/bucket_op.h"
#include "util/codec_util.h"


namespace qcloud_cos {

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
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucket(const PutBucketReq& req, PutBucketResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucket(const GetBucketReq& req, GetBucketResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::ListMultipartUpload(const ListMultipartUploadReq& req, ListMultipartUploadResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DeleteBucket(const DeleteBucketReq& req, DeleteBucketResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketVersioning(const GetBucketVersioningReq& req,
                                        GetBucketVersioningResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketVersioning(const PutBucketVersioningReq& req,
                                        PutBucketVersioningResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
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
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketReplication(const PutBucketReplicationReq& req,
                                         PutBucketReplicationResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
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
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketLifecycle(const GetBucketLifecycleReq& req,
                                       GetBucketLifecycleResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketLifecycle(const PutBucketLifecycleReq& req,
                                       PutBucketLifecycleResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
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
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketACL(const GetBucketACLReq& req,
                                 GetBucketACLResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketACL(const PutBucketACLReq& req,
                                 PutBucketACLResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
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
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketCORS(const PutBucketCORSReq& req,
                                  PutBucketCORSResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
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
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketLocation(const GetBucketLocationReq& req,
                                      GetBucketLocationResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::GetBucketObjectVersions(const GetBucketObjectVersionsReq& req,
                                            GetBucketObjectVersionsResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketLogging(const PutBucketLoggingReq& req, 
                                     PutBucketLoggingResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
   
    std::string path = req.GetPath();
    std::string req_body;
    if(!req.GenerateRequestBody(&req_body)) {

        CosResult result;
        result.SetErrorInfo("Generate PutBucketLogging Request Body fail.");
        return result;   
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
    return NormalAction(host, path, req, additional_headers, 
                        additional_params, req_body, false, resp);
}

CosResult BucketOp::GetBucketLogging(const GetBucketLoggingReq& req, 
                                     GetBucketLoggingResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();

    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketDomain(const PutBucketDomainReq& req, 
                                    PutBucketDomainResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
    std::string req_body;

    if(!req.GenerateRequestBody(&req_body)) {

        CosResult result;
        result.SetErrorInfo("Generate PutBucketDomain Request Body fail.");
        return result;   
    }
	
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
	
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

    return NormalAction(host, path, req, additional_headers, 
                        additional_params, req_body, false, resp);
}


CosResult BucketOp::GetBucketDomain(const GetBucketDomainReq& req, 
                                    GetBucketDomainResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();

    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketWebsite(const PutBucketWebsiteReq& req, 
                                    PutBucketWebsiteResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
    std::string req_body;
    if(!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("Generate PutBucketWebsite Request Body fail.");
        return result;   
    }
	
    return NormalAction(host, path, req, req_body, false, resp);
}


CosResult BucketOp::GetBucketWebsite(const GetBucketWebsiteReq& req, 
                                    GetBucketWebsiteResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DeleteBucketWebsite(const DeleteBucketWebsiteReq& req, 
                                    DeleteBucketWebsiteResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}
                    
CosResult BucketOp::PutBucketTagging(const PutBucketTaggingReq& req, 
                                    PutBucketTaggingResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
	
    std::string req_body;
    if(!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("Generate PutBucketWebsite Request Body fail.");
        return result;   
    }
	
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
	
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
    return NormalAction(host, path, req, additional_headers, 
                        additional_params, req_body, false, resp);
}


CosResult BucketOp::GetBucketTagging(const GetBucketTaggingReq& req, 
                                     GetBucketTaggingResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
	
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DeleteBucketTagging(const DeleteBucketTaggingReq& req, 
					DeleteBucketTaggingResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
	
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketInventory(const PutBucketInventoryReq& req, 
					PutBucketInventoryResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
	
    // id必须设置.
    if(!req.HasId()) {
	CosResult result;
        result.SetErrorInfo("PutBucketinventory need to set Id.");
        return result;
    }
    std::string req_body;
    if(!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("Generate PutBucketWebsite Request Body fail.");
        return result;   
    }

    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
	
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

    return NormalAction(host, path, req, additional_headers, 
                        additional_params, req_body, false, resp);
}

CosResult BucketOp::GetBucketInventory(const GetBucketInventoryReq& req, 
					GetBucketInventoryResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
	
    // id必须设置.
    if(!req.HasId()) {
	CosResult result;
        result.SetErrorInfo("GetBucketinventory need to set Id.");
        return result;
    }
	
    return NormalAction(host, path, req, "", false, resp);
}


CosResult BucketOp::ListBucketInventoryConfigurations(const ListBucketInventoryConfigurationsReq& req,
                                                      ListBucketInventoryConfigurationsResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
	
    return NormalAction(host, path, req, "", false, resp);																			
}

CosResult BucketOp::DeleteBucketInventory(const DeleteBucketInventoryReq& req, 
                                          DeleteBucketInventoryResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                            req.GetBucketName());
    std::string path = req.GetPath();
	
    // id必须设置.
    if(!req.HasId()) {
	CosResult result;
        result.SetErrorInfo("DeleteBucketinventory need to set Id.");
        return result;
    }
    		
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::ListLiveChannel(const ListLiveChannelReq& req, ListLiveChannelResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::PutBucketIntelligentTiering(const PutBucketIntelligentTieringReq& req,
                                                PutBucketIntelligentTieringResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    
    std::string req_body;
    if(!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("Generate PutBucketWebsite Request Body fail.");
        return result;   
    }

    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
    return NormalAction(host, path, req, additional_headers, additional_params, req_body, false, resp);
}

CosResult BucketOp::GetBucketIntelligentTiering(const GetBucketIntelligentTieringReq& req,
                                                GetBucketIntelligentTieringResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::CreateDocProcessJobs(const CreateDocProcessJobsReq& req, CreateDocProcessJobsResp *resp) {
    std::string host =  req.GetBucketName() + ".ci." + m_config->GetRegion() + ".myqcloud.com";
    std::string path = req.GetPath();

    std::string req_body;
    if(!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("Generate CreateDocProcessJobs Request Body fail.");
        return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
    return NormalAction(host, path, req, additional_headers, additional_params, req_body, false, resp);
}

CosResult BucketOp::DescribeDocProcessJob(const DescribeDocProcessJobReq& req, DescribeDocProcessJobResp *resp) {
    std::string host =  req.GetBucketName() + ".ci." + m_config->GetRegion() + ".myqcloud.com";
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DescribeDocProcessJobs(const DescribeDocProcessJobsReq& req, DescribeDocProcessJobsResp *resp) {
    std::string host =  req.GetBucketName() + ".ci." + m_config->GetRegion() + ".myqcloud.com";
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::DescribeDocProcessQueues(const DescribeDocProcessQueuesReq& req, DescribeDocProcessQueuesResp *resp) {
    std::string host =  req.GetBucketName() + ".ci." + m_config->GetRegion() + ".myqcloud.com";
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult BucketOp::UpdateDocProcessQueue(const UpdateDocProcessQueueReq& req, UpdateDocProcessQueueResp *resp) {
    std::string host =  req.GetBucketName() + ".ci." + m_config->GetRegion() + ".myqcloud.com";
    std::string path = req.GetPath();
    std::string req_body;
    if(!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("Generate UpdateDocProcessQueue Request Body fail.");
        return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
    return NormalAction(host, path, req, additional_headers, additional_params, req_body, false, resp);
}

} // qcloud_cos
