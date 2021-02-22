// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/18/17
// Description:

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include "util/auth_tool.h"
#include "cos_api.h"
#include "cos_sys_config.h"
#include "cos_defines.h"

using namespace qcloud_cos;
void PrintResult(const qcloud_cos::CosResult& result, const qcloud_cos::BaseResp& resp) {
    if (result.IsSucc()) {
        std::cout << resp.DebugString() << std::endl;
    } else {
        std::cout << "ErrorInfo=" << result.GetErrorInfo() << std::endl;
        std::cout << "HttpStatus=" << result.GetHttpStatus() << std::endl;
        std::cout << "ErrorCode=" << result.GetErrorCode() << std::endl;
        std::cout << "ErrorMsg=" << result.GetErrorMsg() << std::endl;
        std::cout << "ResourceAddr=" << result.GetResourceAddr() << std::endl;
        std::cout << "XCosRequestId=" << result.GetXCosRequestId() << std::endl;
        std::cout << "XCosTraceId=" << result.GetXCosTraceId() << std::endl;
    }
}

void GetService(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetServiceReq req;
    qcloud_cos::GetServiceResp resp;
    qcloud_cos::CosResult result = cos.GetService(req, &resp);

    std::cout << "===================GetService=====================" << std::endl;
    PrintResult(result, resp);
    const qcloud_cos::Owner& owner = resp.GetOwner();
    const std::vector<qcloud_cos::Bucket>& buckets = resp.GetBuckets();
    std::cout << "owner.m_id=" << owner.m_id << ", owner.display_name=" << owner.m_display_name << std::endl;
    for (std::vector<qcloud_cos::Bucket>::const_iterator itr = buckets.begin(); itr != buckets.end(); ++itr) {
        const qcloud_cos::Bucket& bucket = *itr;
        std::cout << "Bucket name=" << bucket.m_name << ", location="
            << bucket.m_location << ", create_date=" << bucket.m_create_date << std::endl;
    }

    std::cout << "=========================================================" << std::endl;
}

void HeadBucket(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::HeadBucketReq req(bucket_name);
    qcloud_cos::HeadBucketResp resp;
    qcloud_cos::CosResult result = cos.HeadBucket(req, &resp);

    std::cout << "===================HeadBucketResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "==========================================================" << std::endl;
}

void DeleteBucket(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::DeleteBucketReq req(bucket_name);
    qcloud_cos::DeleteBucketResp resp;
    qcloud_cos::CosResult result = cos.DeleteBucket(req, &resp);

    std::cout << "===================DeleteBucketResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void PutBucket(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::PutBucketReq req(bucket_name);
    qcloud_cos::PutBucketResp resp;
    qcloud_cos::CosResult result = cos.PutBucket(req, &resp);

    std::cout << "===================PutBucketResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void GetBucket(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketReq req(bucket_name);
    qcloud_cos::GetBucketResp resp;
    qcloud_cos::CosResult result = cos.GetBucket(req, &resp);

    std::cout << "===================GetBucketResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void PutBucketVersioning(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::PutBucketVersioningReq req(bucket_name);
    req.SetStatus(true);
    qcloud_cos::PutBucketVersioningResp resp;

    qcloud_cos::CosResult result = cos.PutBucketVersioning(req, &resp);
    std::cout << "===================PutBucketVersioningResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void GetBucketVersioning(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketVersioningReq req(bucket_name);
    qcloud_cos::GetBucketVersioningResp resp;
    qcloud_cos::CosResult result = cos.GetBucketVersioning(req, &resp);

    std::cout << "Bucket status= " << resp.GetStatus() << std::endl;
    std::cout << "===================GetBucketVersioningResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void PutBucketReplication(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::PutBucketReplicationReq req(bucket_name);
    req.SetRole("qcs::cam::uin/2779643970:uin/2779643970");
    qcloud_cos::ReplicationRule rule("",
                                     "qcs:id/0:cos:cn-south:appid/1251668577:sevenyoutest1251668577",
                                     "", "", true);

    req.AddReplicationRule(rule);
    qcloud_cos::PutBucketReplicationResp resp;
    qcloud_cos::CosResult result = cos.PutBucketReplication(req, &resp);
    std::cout << "===================PutBucketReplicationResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void GetBucketReplication(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketReplicationReq req(bucket_name);
    qcloud_cos::GetBucketReplicationResp resp;
    qcloud_cos::CosResult result = cos.GetBucketReplication(req, &resp);

    std::cout << "===================GetBucketReplicationResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void DeleteBucketReplication(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::DeleteBucketReplicationReq req(bucket_name);
    qcloud_cos::DeleteBucketReplicationResp resp;
    qcloud_cos::CosResult result = cos.DeleteBucketReplication(req, &resp);

    std::cout << "===================DeleteBucketReplicationResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void PutBucketLifecycle(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::PutBucketLifecycleReq req(bucket_name);
    {
        qcloud_cos::LifecycleRule rule;
        rule.SetIsEnable(true);
        rule.SetId("lifecycle_rule00");
        qcloud_cos::LifecycleFilter filter;
        filter.SetPrefix("sevenyou_e1");
        rule.SetFilter(filter);
        qcloud_cos::LifecycleTransition transition;
        transition.SetDays(30);
        transition.SetStorageClass("Standard_IA");
        rule.AddTransition(transition);
        req.AddRule(rule);
    }

    {
        qcloud_cos::LifecycleRule rule;
        rule.SetIsEnable(true);
        rule.SetId("lifecycle_rule01");
        qcloud_cos::LifecycleFilter filter;
        filter.SetPrefix("sevenyou_e1");
        rule.SetFilter(filter);
        qcloud_cos::LifecycleTransition transition;
        transition.SetDays(1);
        transition.SetStorageClass("Standard");
        rule.AddTransition(transition);
        qcloud_cos::LifecycleTransition transition2;
        transition2.SetDays(2);
        transition2.SetStorageClass("Standard_IA");
        rule.AddTransition(transition2);
        req.AddRule(rule);
    }

    qcloud_cos::PutBucketLifecycleResp resp;
    qcloud_cos::CosResult result = cos.PutBucketLifecycle(req, &resp);
    std::cout << "===================PutBucketLifecycleResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void GetBucketLifecycle(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketLifecycleReq req(bucket_name);
    qcloud_cos::GetBucketLifecycleResp resp;
    qcloud_cos::CosResult result = cos.GetBucketLifecycle(req, &resp);

    const std::vector<qcloud_cos::LifecycleRule>& rules = resp.GetRules();
    for (int idx = 0; idx != rules.size(); ++idx) {
        std::cout << "id = " << rules[idx].GetId() << std::endl;
    }

    std::cout << "===================GetBucketLifecycleResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void DeleteBucketLifecycle(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::DeleteBucketLifecycleReq req(bucket_name);
    qcloud_cos::DeleteBucketLifecycleResp resp;
    qcloud_cos::CosResult result = cos.DeleteBucketLifecycle(req, &resp);

    std::cout << "===================DeleteBucketLifecycleResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void PutBucketACL(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    // 1. 设置ACL配置(通过Body, 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
    {
        qcloud_cos::PutBucketACLReq req(bucket_name);
        qcloud_cos::Owner owner = {"qcs::cam::uin/491107627:uin/491107627", "qcs::cam::uin/491107627:uin/491107627" };
        qcloud_cos::Grant grant;
        req.SetOwner(owner);
        grant.m_grantee.m_type = "Group";
        grant.m_grantee.m_uri = "http://cam.qcloud.com/groups/global/AllUsers";
        grant.m_perm = "READ";
        req.AddAccessControlList(grant);

        qcloud_cos::PutBucketACLResp resp;
        qcloud_cos::CosResult result = cos.PutBucketACL(req, &resp);
        std::cout << "===================PutBucketACLResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

#if 0
    // 2. 设置ACL配置(通过Header, 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
    {
        qcloud_cos::PutBucketACLReq req(bucket_name);
        req.SetXCosAcl("public-read-write");

        qcloud_cos::PutBucketACLResp resp;
        qcloud_cos::CosResult result = cos.PutBucketACL(req, &resp);
        std::cout << "===================PutBucketACLResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }
#endif
}

void GetBucketACL(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketACLReq req(bucket_name);
    qcloud_cos::GetBucketACLResp resp;
    qcloud_cos::CosResult result = cos.GetBucketACL(req, &resp);

    std::vector<qcloud_cos::Grant> grants = resp.GetAccessControlList();
    for (size_t idx = 0; idx < grants.size(); ++idx) {
        std::cout << "Type is " << grants[idx].m_grantee.m_type << std::endl;
    }
    std::cout << "===================GetBucketACLResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void PutBucketCORS(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::PutBucketCORSReq req(bucket_name);
    qcloud_cos::CORSRule rule;
    rule.m_id = "123";
    rule.m_allowed_headers.push_back("x-cos-meta-test");
    rule.m_allowed_origins.push_back("http://www.qq.com");
    rule.m_allowed_origins.push_back("http://www.qcloud.com");
    rule.m_allowed_methods.push_back("PUT");
    rule.m_allowed_methods.push_back("GET");
    rule.m_max_age_secs = "600";
    rule.m_expose_headers.push_back("x-cos-expose");
    req.AddRule(rule);

    qcloud_cos::PutBucketCORSResp resp;
    qcloud_cos::CosResult result = cos.PutBucketCORS(req, &resp);
    std::cout << "===================PutBucketCORSResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void GetBucketCORS(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketCORSReq req(bucket_name);
    qcloud_cos::GetBucketCORSResp resp;
    qcloud_cos::CosResult result = cos.GetBucketCORS(req, &resp);

    std::cout << "===================GetBucketCORSResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void DeleteBucketCORS(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::DeleteBucketCORSReq req(bucket_name);
    qcloud_cos::DeleteBucketCORSResp resp;
    qcloud_cos::CosResult result = cos.DeleteBucketCORS(req, &resp);

    std::cout << "===================DeleteBucketCORSResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void PutObjectByFile(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                     const std::string& object_name, const std::string& file_path) {
    qcloud_cos::PutObjectByFileReq req(bucket_name, object_name, file_path);
    //req.AddParam("versionId", "MTg0NDY3NDI1NjQ4NjUyMjQ1MTA");
    qcloud_cos::PutObjectByFileResp resp;
    qcloud_cos::CosResult result = cos.PutObject(req, &resp);

    std::cout << "===================PutObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

// 限速上传对象
void PutObjectByFileLimitTraffic(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                     const std::string& object_name, const std::string& file_path,
                     const uint64_t traffic_limit, bool set_header = true) {
    qcloud_cos::PutObjectByFileReq req(bucket_name, object_name, file_path);
    if (set_header) {
        req.SetTrafficLimitByHeader(StringUtil::Uint64ToString(traffic_limit));
    } else {
        req.SetTrafficLimitByParam(StringUtil::Uint64ToString(traffic_limit));
    }
    qcloud_cos::PutObjectByFileResp resp;
    qcloud_cos::CosResult result = cos.PutObject(req, &resp);
    std::cout << "===================PutObjectFileLimitTrafficResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void PutObjectByStream(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    std::istringstream iss("put object");
    qcloud_cos::PutObjectByStreamReq req(bucket_name, "sevenyou_10m", iss);
    qcloud_cos::PutObjectByStreamResp resp;
    qcloud_cos::CosResult result = cos.PutObject(req, &resp);

    std::cout << "===================PutObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void HeadObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                const std::string& object_name) {
    qcloud_cos::HeadObjectReq req(bucket_name, object_name);
    //req.AddParam("versionId", "MTg0NDY3NDI1NjQ4NjUyMjQ1MTA");
    qcloud_cos::HeadObjectResp resp;

    qcloud_cos::CosResult result = cos.HeadObject(req, &resp);

    std::cout << "===================HeadObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "==========================================================" << std::endl;
}

void GetObjectByFile(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                     const std::string& object_name, const std::string& file_path) {
    qcloud_cos::GetObjectByFileReq req(bucket_name, object_name, file_path);
    //req.AddParam("versionId", "MTg0NDY3NDI1NjQwODU3NzU2MTA");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = cos.GetObject(req, &resp);
    std::cout << "===================GetObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

// 限速下载对象
void GetObjectByFileLimitTraffic(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                     const std::string& object_name, const std::string& file_path,
                     const uint64_t traffic_limit, bool set_header = true) {
    qcloud_cos::GetObjectByFileReq req(bucket_name, object_name, file_path);
    if (set_header) {
        req.SetTrafficLimitByHeader(StringUtil::Uint64ToString(traffic_limit));
    } else {
        req.SetTrafficLimitByParam(StringUtil::Uint64ToString(traffic_limit));
    }
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = cos.GetObject(req, &resp);
    std::cout << "===================GetObjectByFileLimitTrafficResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void GetObjectByStream(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                       const std::string& object_name) {
    std::ostringstream os;
    qcloud_cos::GetObjectByStreamReq req(bucket_name, object_name, os);
    qcloud_cos::GetObjectByStreamResp resp;

    qcloud_cos::CosResult result = cos.GetObject(req, &resp);
    std::cout << "===================GetObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
    std::cout << os.str() << std::endl;
}

void MultiGetObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name, const std::string& file_path) {
    qcloud_cos::MultiGetObjectReq req(bucket_name, object_name, file_path);
    qcloud_cos::MultiGetObjectResp resp;

    qcloud_cos::CosResult result = cos.GetObject(req, &resp);
    std::cout << "===================GetObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

// 多线程限速下载对象
void MultiGetObjectLimitTraffic(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name, const std::string& file_path,
                    const uint64_t traffic_limit, bool set_header = true) {
    qcloud_cos::MultiGetObjectReq req(bucket_name, object_name, file_path);
    if (set_header) {
        req.SetTrafficLimitByHeader(StringUtil::Uint64ToString(traffic_limit));
    } else {
        req.SetTrafficLimitByParam(StringUtil::Uint64ToString(traffic_limit));
    }
    qcloud_cos::MultiGetObjectResp resp;
    qcloud_cos::CosResult result = cos.GetObject(req, &resp);
    std::cout << "===================MultiGetObjectLimitTrafficResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void InitMultiUpload(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                     const std::string& object_name, std::string* upload_id) {
    qcloud_cos::InitMultiUploadReq req(bucket_name, object_name);
    qcloud_cos::InitMultiUploadResp resp;
    qcloud_cos::CosResult result = cos.InitMultiUpload(req, &resp);

    std::cout << "=====================InitMultiUpload=====================";
    PrintResult(result, resp);
    std::cout << "=========================================================";

    *upload_id = resp.GetUploadId();
}

void UploadPartData(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name, const std::string& upload_id,
                    std::fstream& is, uint64_t number,
                    std::string* etag) {
    qcloud_cos::UploadPartDataReq req(bucket_name, object_name,
                                      upload_id, is);
    req.SetPartNumber(number);
    qcloud_cos::UploadPartDataResp resp;
    qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);
    *etag = resp.GetEtag();

    std::cout << "======================UploadPartData=====================";
    PrintResult(result, resp);
    std::cout << "=========================================================";
}

// 限速上传分块
void UploadPartDataLimitTraffic(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name, const std::string& upload_id,
                    std::fstream& is, uint64_t number, std::string* etag, 
                    const uint64_t traffic_limit, bool set_header = true) {
    qcloud_cos::UploadPartDataReq req(bucket_name, object_name, upload_id, is);
    req.SetPartNumber(number);
    if (set_header) {
        req.SetTrafficLimitByHeader(StringUtil::Uint64ToString(traffic_limit));
    } else {
        req.SetTrafficLimitByParam(StringUtil::Uint64ToString(traffic_limit));
    }
    qcloud_cos::UploadPartDataResp resp;
    qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);
    *etag = resp.GetEtag();

    std::cout << "======================UploadPartDataLimitTrafficResponse=====================";
    PrintResult(result, resp);
    std::cout << "=========================================================";
}

void AbortMultiUpload(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                      const std::string& object_name, const std::string& upload_id) {
    qcloud_cos::AbortMultiUploadReq req(bucket_name, object_name,
                                        upload_id);
    qcloud_cos::AbortMultiUploadResp resp;
    qcloud_cos::CosResult result = cos.AbortMultiUpload(req, &resp);
    std::cout << "======================AbortUploadPart=====================";
    PrintResult(result, resp);
    std::cout << "=========================================================";
}

void CompleteMultiUpload(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                         const std::string& object_name, const std::string& upload_id,
                         const std::vector<std::string>& etags,
                         const std::vector<uint64_t>& numbers) {
    qcloud_cos::CompleteMultiUploadReq req(bucket_name, object_name, upload_id);
    qcloud_cos::CompleteMultiUploadResp resp;
    req.AddParam("versionId", "MTg0NDY3NDI1NjQ4NjUyMjQ1MTA");
    req.SetEtags(etags);
    req.SetPartNumbers(numbers);

    qcloud_cos::CosResult result = cos.CompleteMultiUpload(req, &resp);

    std::cout << "===================Complete=============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

void MultiUploadObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                       const std::string& object_name, const std::string& local_file) {
    qcloud_cos::MultiUploadObjectReq req(bucket_name,
                                         object_name, local_file);
    req.SetRecvTimeoutInms(1000 * 60);
    qcloud_cos::MultiUploadObjectResp resp;
    qcloud_cos::CosResult result = cos.MultiUploadObject(req, &resp);

    if (result.IsSucc()) {
        std::cout << "MultiUpload Succ." << std::endl;
        std::cout << resp.GetLocation() << std::endl;
        std::cout << resp.GetKey() << std::endl;
        std::cout << resp.GetBucket() << std::endl;
        std::cout << resp.GetEtag() << std::endl;
    } else {
        std::cout << "MultiUpload Fail." << std::endl;
        // 获取具体失败在哪一步
        std::string resp_tag = resp.GetRespTag();
        if ("Init" == resp_tag) {
            // print result
        } else if ("Upload" == resp_tag) {
            // print result
        } else if ("Complete" == resp_tag) {
            // print result
        }
    }
    std::cout << "===================MultiUpload=============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

//限速多线程上传
void MultiUploadObjectLimitTraffic(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                       const std::string& object_name, const std::string& local_file, 
                       const uint64_t traffic_limit, bool set_header = true) {
    qcloud_cos::MultiUploadObjectReq req(bucket_name, object_name, local_file);
    if (set_header) {
        req.SetTrafficLimitByHeader(StringUtil::Uint64ToString(traffic_limit));
    } else {
        req.SetTrafficLimitByParam(StringUtil::Uint64ToString(traffic_limit));
    }
    req.SetRecvTimeoutInms(1000 * 60);
    qcloud_cos::MultiUploadObjectResp resp;
    qcloud_cos::CosResult result = cos.MultiUploadObject(req, &resp);

    if (result.IsSucc()) {
        std::cout << "MultiUpload Succ." << std::endl;
        std::cout << resp.GetLocation() << std::endl;
        std::cout << resp.GetKey() << std::endl;
        std::cout << resp.GetBucket() << std::endl;
        std::cout << resp.GetEtag() << std::endl;
    } else {
        std::string resp_tag = resp.GetRespTag();
        std::cout << "MultiUpload Fail, respond tag " << resp_tag << std::endl;
    }
    std::cout << "===================MultiUpload=============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

void ListParts(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
               const std::string& object_name, const std::string& upload_id) {
    qcloud_cos::ListPartsReq req(bucket_name, object_name, upload_id);
    req.SetMaxParts(1);
    req.SetPartNumberMarker("1");
    qcloud_cos::ListPartsResp resp;
    qcloud_cos::CosResult result = cos.ListParts(req, &resp);

    std::cout << "===================ListParts=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void PutObjectACL(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                  const std::string& object_name) {
    // 1 设置ACL配置(通过Body, 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
    {
        qcloud_cos::PutObjectACLReq req(bucket_name, object_name);
        qcloud_cos::Owner owner = {"qcs::cam::uin/491107627:uin/491107627", "qcs::cam::uin/491107627:uin/491107627" };
        qcloud_cos::Grant grant;
        req.SetOwner(owner);
        grant.m_grantee.m_type = "Group";
        grant.m_grantee.m_uri = "http://cam.qcloud.com/groups/global/AllUsers";
        grant.m_perm = "READ";
        req.AddAccessControlList(grant);

        qcloud_cos::PutObjectACLResp resp;
        qcloud_cos::CosResult result = cos.PutObjectACL(req, &resp);
        std::cout << "===================PutObjectACLResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 2 设置ACL配置(通过Header, 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
    {
        qcloud_cos::PutObjectACLReq req(bucket_name, object_name);
        req.SetXCosAcl("public-read-write");

        qcloud_cos::PutObjectACLResp resp;
        qcloud_cos::CosResult result = cos.PutObjectACL(req, &resp);
        std::cout << "===================PutObjectACLResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }
}

void GetObjectACL(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                  const std::string& object_name) {
    qcloud_cos::GetObjectACLReq req(bucket_name, object_name);
    qcloud_cos::GetObjectACLResp resp;
    qcloud_cos::CosResult result = cos.GetObjectACL(req, &resp);

    std::cout << "===================GetObjectACLResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void PutObjectCopy(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                   const std::string& object_name, const std::string& source) {
    qcloud_cos::PutObjectCopyReq req(bucket_name, object_name);
    req.SetXCosCopySource(source);
    qcloud_cos::PutObjectCopyResp resp;
    qcloud_cos::CosResult result = cos.PutObjectCopy(req, &resp);

    std::cout << "===================PutObjectCopyResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void DeleteObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& object_name) {
    qcloud_cos::DeleteObjectReq req(bucket_name, object_name);
    qcloud_cos::DeleteObjectResp resp;
    qcloud_cos::CosResult result = cos.DeleteObject(req, &resp);

    std::cout << "===================DeleteObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void DeleteObjects(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                   const std::vector<ObjectVersionPair>& objects) {
    qcloud_cos::DeleteObjectsReq req(bucket_name, objects);
    qcloud_cos::DeleteObjectsResp resp;
    qcloud_cos::CosResult result = cos.DeleteObjects(req, &resp);

    std::cout << "===================DeleteObjectsResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
    std::cout << "Resp body=[" << resp.DebugString() << "]" << std::endl;
}

void UploadPartCopy(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name, const std::string& upload_id,
                    const std::string& source, const std::string& range,
                    uint64_t number, std::string* etag) {
    qcloud_cos::UploadPartCopyDataReq req(bucket_name, object_name, upload_id, number);
    req.SetXCosCopySource(source);
    req.SetXCosCopySourceRange(range);
    qcloud_cos::UploadPartCopyDataResp resp;
    qcloud_cos::CosResult result = cos.UploadPartCopyData(req, &resp);

    std::cout << "===================UploadPartCopyDataResp=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;

    if (result.IsSucc()) {
        *etag = resp.GetEtag();
    }
}

void Copy(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
          const std::string& object_name, const std::string& source) {
    qcloud_cos::CopyReq req(bucket_name, object_name);
    qcloud_cos::CopyResp resp;

    req.SetXCosCopySource(source);

    qcloud_cos::CosResult result = cos.Copy(req, &resp);
    std::cout << "===================Copy=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void ListMultipartUpload(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::ListMultipartUploadReq req(bucket_name);
    qcloud_cos::ListMultipartUploadResp resp;
    qcloud_cos::CosResult result = cos.ListMultipartUpload(req, &resp);
    std::vector<Upload> rst = resp.GetUpload();

    for (std::vector<qcloud_cos::Upload>::const_iterator itr = rst.begin(); itr != rst.end(); ++itr) {
        const qcloud_cos::Upload& upload = *itr;
        std::cout << "key = " << upload.m_key << ", uploadid= " << upload.m_uploadid << ", storagen class = "
                  << upload.m_storage_class << ", m_initiated= " << upload.m_initiated << std::endl;
    }

    std::cout << "===================ListMultipartUpload=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;

}

// 开启bucket日志配置
void PutBucketLogging(qcloud_cos::CosAPI& cos, const std::string& bucket_name, 
                      const std::string& TargetBucketname, 
                      const std::string& TargetPrefix) {
    qcloud_cos::PutBucketLoggingReq req(bucket_name);
    qcloud_cos::LoggingEnabled rules;
    rules.SetTargetBucket(TargetBucketname);
    rules.SetTargetPrefix(TargetPrefix);
    
    req.SetLoggingEnabled(rules);
    qcloud_cos::PutBucketLoggingResp resp;
   
    qcloud_cos::CosResult result = cos.PutBucketLogging(req, &resp);
   
    std::cout << "===================PutBucketLoggingResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

// 获取bucket日志配置
void GetBucketLogging(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketLoggingReq req(bucket_name);
    qcloud_cos::GetBucketLoggingResp resp;
    qcloud_cos::CosResult result = cos.GetBucketLogging(req, &resp);

    std::cout << "===================GetBucketLoggingResponse=====================" << std::endl;

    const qcloud_cos::LoggingEnabled loggingenabled = resp.GetLoggingEnabled();
    std::cout << "TargetBucket = " << loggingenabled.GetTargetBucket() << std::endl;
    std::cout << "TargetPrefix = " << loggingenabled.GetTargetPrefix() << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

// 设置存储桶绑定自定义域名 
void PutBucketDomain(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::PutBucketDomainReq req(bucket_name);
	DomainRule rules;
    rules.SetStatus("ENABLED");
    rules.SetName("www.abc.com");
    rules.SetType("REST");
    //rules.SetForcedReplacement();
    req.SetDomainRule(rules);
    qcloud_cos::PutBucketDomainResp resp;
    qcloud_cos::CosResult result = cos.PutBucketDomain(req, &resp);

    std::cout << "===================PutBucketDomainResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

// 获取存储桶绑定自定义域名
void GetBucketDomain(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketDomainReq req(bucket_name);
    qcloud_cos::GetBucketDomainResp resp;
    qcloud_cos::CosResult result = cos.GetBucketDomain(req, &resp);

    std::cout << "===================GetBucketDomainResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

// 为存储桶配置静态网站
void PutBucketWebsite(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    
    qcloud_cos::PutBucketWebsiteReq req(bucket_name);
    qcloud_cos::PutBucketWebsiteResp resp;
    
    req.SetSuffix("index.xml"); //必选项
    req.SetProtocol("https");
    req.SetKey("Error.html");
	
    //设置重定向规则，最多设置100条 
	
    //设置第一条规则
    qcloud_cos::RoutingRule routerule1;
    qcloud_cos::Condition temp_condtion1;
    temp_condtion1.SetHttpErrorCodeReturnedEquals(404);//需要设置，默认404
    routerule1.SetCondition(temp_condtion1);
    qcloud_cos::Redirect temp_redirect1;
    temp_redirect1.SetProtocol("https");
    temp_redirect1.SetReplaceKeyWith("404.htmp");
    routerule1.SetRedirect(temp_redirect1);
	
    //设置第二条规则
    qcloud_cos::RoutingRule routerule2;
    qcloud_cos::Condition temp_condtion2;
    temp_condtion2.SetHttpErrorCodeReturnedEquals(403);//需要设置，默认404
    routerule2.SetCondition(temp_condtion2);
    qcloud_cos::Redirect temp_redirect2;
    temp_redirect2.SetProtocol("https");
    temp_redirect2.SetReplaceKeyWith("403.htmp");
    routerule2.SetRedirect(temp_redirect2);
	
	
    //设置第三条规则
    qcloud_cos::RoutingRule routerule3;
    qcloud_cos::Condition temp_condtion3;
    temp_condtion3.SetKeyPrefixEquals("img/");
    temp_condtion3.SetHttpErrorCodeReturnedEquals(402);
    routerule3.SetCondition(temp_condtion3);
    qcloud_cos::Redirect temp_redirect3;
    temp_redirect3.SetProtocol("https");
    temp_redirect3.SetReplaceKeyWith("401.htmp");
    routerule3.SetRedirect(temp_redirect3);
	
	
    qcloud_cos::RoutingRule routerule4;
    qcloud_cos::Condition temp_condtion4;
    temp_condtion4.SetKeyPrefixEquals("img1/");
    routerule4.SetCondition(temp_condtion4);
	
    qcloud_cos::Redirect temp_redirect4;
    temp_redirect4.SetProtocol("https");
    temp_redirect4.SetReplaceKeyPrefixWith("402.htmp");
    routerule4.SetRedirect(temp_redirect4);

    req.AddRoutingRule(routerule1);
    req.AddRoutingRule(routerule2);
    req.AddRoutingRule(routerule3);
    req.AddRoutingRule(routerule4);
    
	
    qcloud_cos::CosResult result = cos.PutBucketWebsite(req, &resp);
    std::cout << "===================PutBucketWebsite=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;

}

// 获取与存储桶关联的静态网站配置信息
void GetBucketWebsite(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketWebsiteReq req(bucket_name);
    qcloud_cos::GetBucketWebsiteResp resp;
	
    qcloud_cos::CosResult result = cos.GetBucketWebsite(req, &resp);
    std::cout << "===================GetBucketWebsite=====================" << std::endl;
	
    std::cout <<  "Suffix:" << resp.GetSuffix() << std::endl;
    std::cout <<  "Protocol:" << resp.GetProtocol() << std::endl;
    std::cout <<  "ErrorDocument:" << resp.GetKey() << std::endl;
	
    std::vector<RoutingRule> routingrules = resp.GetRoutingRules();
    std::vector<RoutingRule>::iterator it = routingrules.begin();
    for(; it != routingrules.end(); ++it) {
        const Condition& condition = it->GetCondition();
	    std::cout << "condition httpcode" << condition.GetHttpErrorCodeReturnedEquals() << std::endl;
	    std::cout << "condition KeyPrefixEquals:" << condition.GetKeyPrefixEquals() << std::endl;

	    const Redirect& redirect = it->GetRedirect();
	    std::cout << "redirect Protocol:" <<redirect.GetProtocol() << std::endl;
	    std::cout << "redirect ReplaceKeyWith" << redirect.GetReplaceKeyWith() << std::endl;
	    std::cout << "redirect ReplaceKeyPrefixWith" <<redirect.GetReplaceKeyPrefixWith() << std::endl;
		
	    std::cout << std::endl;
	}
	
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

// 删除存储桶中的静态网站配置
void DeleteBucketWebsite(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::DeleteBucketWebsiteReq req(bucket_name);
    qcloud_cos::DeleteBucketWebsiteResp resp;
	
    qcloud_cos::CosResult result = cos.DeleteBucketWebsite(req, &resp);
    std::cout << "===================DeleteBucketWebsite=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

// 为已存在的 Bucket 设置标签(Tag)
void PutBucketTagging(qcloud_cos::CosAPI& cos, const std::string& bucket_name, std::vector<Tag>& tagset) {
    qcloud_cos::PutBucketTaggingReq req(bucket_name);
    qcloud_cos::PutBucketTaggingResp resp;
    req.SetTagSet(tagset);
	
    qcloud_cos::CosResult result = cos.PutBucketTagging(req, &resp);
    std::cout << "===================PutBucketTagging=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

//用于查询指定存储桶下已有的存储桶标签
void GetBucketTagging(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketTaggingReq req(bucket_name);
    qcloud_cos::GetBucketTaggingResp resp;
	
    qcloud_cos::CosResult result = cos.GetBucketTagging(req, &resp);
    std::cout << "===================GetBucketTagging=====================" << std::endl;
    std::vector<Tag> tagset = resp.GetTagSet();
    for(std::vector<Tag>::iterator it = tagset.begin(); it != tagset.end(); ++it) {
        std::cout << it->GetKey() << ":" << it->GetValue() << std::endl;
    }
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}
//删除指定存储桶下已有的存储桶标签
void DeleteBucketTagging(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::DeleteBucketTaggingReq req(bucket_name);
    qcloud_cos::DeleteBucketTaggingResp resp;

    qcloud_cos::CosResult result = cos.DeleteBucketTagging(req, &resp);
    std::cout << "===================DeleteBucketTagging=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;	
}
// 在存储桶中创建清单任务
void PutBucketInventory(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::PutBucketInventoryReq req(bucket_name);
    //req.SetId("list3");
	
    //COSBucketDestination destination;
    //destination.SetFormat("CSV");
    //destination.SetAccountId("100010974959");
	
    //destination.SetBucket("qcs::cos:ap-guangzhou::loggtest-1234567890");
    //destination.SetPrefix("/");
    //destination.SetEncryption(true);
	
    //OptionalFields fields;
   // fields.SetIsSize(true);
   // fields.SetIsLastModified(true);
   // fields.SetIsStorageClass(true);
   // fields.SetIsMultipartUploaded(true);
   // fields.SetIsReplicationStatus(true);
   // fields.SetIsEtag(true);
   //
   // Inventory inventory;
   // inventory.SetIsEnable(true);
   // inventory.SetIncludedObjectVersions("All");
   // inventory.SetFilter("/");
   // inventory.SetId(req.GetId());
   // inventory.SetFrequency("Daily");
   // inventory.SetCOSBucketDestination(destination);
   // inventory.SetOptionalFields(fields);
   //
   // req.SetInventory(inventory);
   
    qcloud_cos::PutBucketInventoryResp resp;
   
    qcloud_cos::CosResult result = cos.PutBucketInventory(req, &resp);
    std::cout << "===================PutBucketinventory=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;	
}

// 用于查询存储桶中用户的清单任务信息
void GetBucketInventory(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::GetBucketInventoryReq req(bucket_name);
    qcloud_cos::GetBucketInventoryResp resp;
    req.SetId("list2");
        
    qcloud_cos::CosResult result = cos.GetBucketInventory(req, &resp);
    //std::cout << "===================GetBucketinventory=====================" << std::endl;
    //    
    //const Inventory inventory = resp.GetInventory();
    //std::cout << "inventory isenabled:" << inventory.GetIsEnable() << std::endl;
    //std::cout << "inventory IncludedObjectVersions:" << inventory.GetIncludedObjectVersions() << std::endl;
    //std::cout << "inventory filter:" << inventory.GetFilter() << std::endl;
    //std::cout << "inventory id:" << inventory.GetId() << std::endl;
    //std::cout << "inventory Frequency:" << inventory.GetFrequency() << std::endl;

    //std:: cout << "===================================" << std::endl;
    //COSBucketDestination destination = inventory.GetCOSBucketDestination();
    //std::cout << "destination Format:" << destination.GetFormat() << std::endl;
    //std::cout << "destination AccountID:" << destination.GetAccountId() << std::endl;
    //std::cout << "destination Bucket:" << destination.GetBucket() << std::endl;
    //std::cout << "destination Encryption:" << destination.GetEncryption() << std::endl;

    //std:: cout << "===================================" << std::endl;
    //    
    //OptionalFields fields = inventory.GetOptionalFields();

    //std::cout << "fields Size:" << fields.GetIsSize() << std::endl;
    //std::cout << "fields LastModified:" << fields.GetIsLastModified() << std::endl;
    //std::cout << "fields StorageClass:" << fields.GetIsStorageClass() << std::endl;
    //std::cout << "fields Region:" << fields.GetIsMultipartUploaded() << std::endl;
    //std::cout << "fields ReplicationStatus:" << fields.GetIsReplicationStatus() << std::endl;
    //std::cout << "fields Tag:" << 	fields.GetIsETag() << std::endl;

    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;	
}

// 请求返回一个存储桶中的所有清单任务
void ListBucketInventoryConfigurations(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::ListBucketInventoryConfigurationsReq req(bucket_name);
    qcloud_cos::ListBucketInventoryConfigurationsResp resp;
	
    qcloud_cos::CosResult result = cos.ListBucketInventoryConfigurations(req, &resp);
   // std::cout << "===================ListBucketInventoryConfigurations=====================" << std::endl;
   //     
   // std::vector<Inventory> inventory_vec = resp.GetInventory();
   //     
   // std::cout << resp.GetIsTruncated() << std::endl;
   // std::cout << resp.GetContinuationToken() << std::endl;
   // std::cout << resp.GetNextContinuationToken() << std::endl;
   //     
   // std::vector<Inventory>::iterator itr = inventory_vec.begin();
   // for(; itr != inventory_vec.end(); ++itr) {
   //     	
   //    std:: cout << "==============Inventory=============================" << std::endl;
   //    std::cout << "inventory id:" << itr->GetId() << std::endl;
   //    std::cout << "inventory isenabled:" <<  itr->GetIsEnable() << std::endl;
   //    std::cout << "inventory IncludedObjectVersions:" << itr->GetIncludedObjectVersions() << std::endl;
   //    std::cout << "inventory filter:" << itr->GetFilter() << std::endl;
   //    std::cout << "inventory Frequency:" << 	itr->GetFrequency() << std::endl;

   //    std:: cout << "==============GetCOSBucketDestination==================" << std::endl;
   //    COSBucketDestination destination = itr->GetCOSBucketDestination();
   //    std::cout << "destination Format:" << destination.GetFormat() << std::endl;
   //    std::cout << "destination AccountID:" << destination.GetAccountId() << std::endl;
   //    std::cout << "destination Bucket:" << destination.GetBucket() << std::endl;
   //    std::cout << "destination Encryption:" << destination.GetEncryption() << std::endl;
   //      
   //     	
   //    std:: cout << "===================OptionalFields======================" << std::endl;
   //    OptionalFields fields = itr->GetOptionalFields();
   //    std::cout << "fields Size:" << fields.GetIsSize() << std::endl;
   //    std::cout << "fields LastModified:" << fields.GetIsLastModified() << std::endl;
   //    std::cout << "fields StorageClass:" << fields.GetIsStorageClass() << std::endl;
   //    std::cout << "fields Region:" << fields.GetIsMultipartUploaded() << std::endl;
   //    std::cout << "fields ReplicationStatus:" << fields.GetIsReplicationStatus() << std::endl;
   //    std::cout << "fields Tag:" << 	fields.GetIsETag() << std::endl;

   // }	
   // std::cout << "===============ListBucketInventoryConfigurations end================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;	
}

// 用于删除存储桶中指定的清单任务.
void DeleteBucketInventory(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::DeleteBucketInventoryReq req(bucket_name);
    qcloud_cos::DeleteBucketInventoryResp resp;
    req.SetId("list");
    qcloud_cos::CosResult result = cos.DeleteBucketInventory(req, &resp);
    std::cout << "===================DeleteBucketinventory=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;	
}

// 创建直播通道
void CreateLiveChannel(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& channel_name) {
    qcloud_cos::PutLiveChannelReq req(bucket_name, channel_name);
    qcloud_cos::PutLiveChannelResp resp;
    std::map<std::string, std::string> url_params;
    url_params.insert(std::make_pair("a", "b"));
    LiveChannelConfiguration config("test", "Enabled", "HLS", 5, 10, "playlist.m3u8");
    req.SetLiveChannelConfig(config);
    req.SetUrlParams(url_params);
    req.SetExpire(1000);
    qcloud_cos::CosResult result = cos.PutLiveChannel(req, &resp);
    std::cout << "===================CreateLiveChannel=====================" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        std::cout << "PlayUrl:" << resp.GetPlayUrl() << std::endl;
        std::cout << "PublishUrl:" << resp.GetPublishUrl() << std::endl;
    }
    std::cout << "====================================================================" << std::endl;	
}

void GetLiveChannel(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& channel_name) {
    qcloud_cos::GetLiveChannelReq req(bucket_name, channel_name);
    qcloud_cos::GetLiveChannelResp resp;
    qcloud_cos::CosResult result = cos.GetLiveChannel(req, &resp);
    std::cout << "===================GetLiveChannel=====================" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        LiveChannelConfiguration chan_conf = resp.GetLiveChannelConf();
        std::stringstream oss;
        oss << "Description:" << chan_conf.GetDescription() << ",";
        oss << "Switch:" << chan_conf.GetSwitch() << ",";
        oss << "Type:" << chan_conf.GetType() << ",";
        oss << "FragDuration:" << chan_conf.GetFragDuration() << ",";
        oss << "FragCount:" << chan_conf.GetFragCount() << ",";
        oss << "PlaylistName:" << chan_conf.GetPlaylistName() << ",";
        oss << "PublishUrls:" << chan_conf.GetPublishUrl() << ",";
        oss << "PlayUrls:" << chan_conf.GetPlayUrl();
        std::cout << "LiveChannelConfiguration:" << oss.str() << std::endl;
    }
    std::cout << "====================================================================" << std::endl;
}

void GetRtmpSignedPublishUrl(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& channel_name) {
    std::cout << "===================GetRtmpSignedPublishUrl=====================" << std::endl;
    std::cout << "PublishUrl:" << 
        cos.GetRtmpSignedPublishUrl(bucket_name, channel_name, 3600, std::map<std::string, std::string>()) << std::endl;
    std::cout << "====================================================================" << std::endl;
}

void PutLiveChannelSwitch(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& channel_name) {
    qcloud_cos::PutLiveChannelSwitchReq req(bucket_name, channel_name);
    qcloud_cos::PutLiveChannelSwitchResp resp;
    qcloud_cos::CosResult result;

    std::cout << "===================PutLiveChannelSwitch disabled=====================" << std::endl;
    req.SetDisabled();
    result = cos.PutLiveChannelSwitch(req, &resp);
    PrintResult(result, resp);

    std::cout << "===================PutLiveChannelSwitch enabled=====================" << std::endl;
    req.SetEnabled();
    result = cos.PutLiveChannelSwitch(req, &resp);
    PrintResult(result, resp);

    std::cout << "====================================================================" << std::endl;
}

void GetLiveChannelHistory(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& channel_name) {
    qcloud_cos::GetLiveChannelHistoryReq req(bucket_name, channel_name);
    qcloud_cos::GetLiveChannelHistoryResp resp;
    qcloud_cos::CosResult result = cos.GetLiveChannelHistory(req, &resp);
    std::cout << "===================GetLiveChannelHistory=====================" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        const std::vector<LiveRecord> & chan_history = resp.GetChanHistory();
        std::vector<LiveRecord>::const_iterator it = chan_history.begin();
        for (; it != chan_history.end(); ++it) {
            std::stringstream oss;
            oss << "StartTime:" << it->m_start_time << ", ";
            oss << "EndTime:" << it->m_end_time << ", ";
            oss << "RemoteAddr:" << it->m_remote_addr << ", ";
            oss << "RequestId:" << it->m_request_id;
            std::cout << oss.str() << std::endl;
        }
    }
    std::cout << "====================================================================" << std::endl;
}

void GetLiveChannelStatus(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& channel_name) {
    qcloud_cos::GetLiveChannelStatusReq req(bucket_name, channel_name);
    qcloud_cos::GetLiveChannelStatusResp resp;
    qcloud_cos::CosResult result = cos.GetLiveChannelStatus(req, &resp);
    std::cout << "===================GetLiveChannelStatus=====================" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        const LiveChannelStatus& chan_status = resp.GetLiveChannelStatus();
        if (chan_status.m_status == "Idle") {
            std::cout << "Status:" << chan_status.m_status << std::endl;
        } else {
            std::stringstream oss;
            oss << "Status:" << chan_status.m_status << ", ";
            oss << "ConnectedTime:" << chan_status.m_connected_time << ", ";
            oss << "RemoteAddr:" << chan_status.m_remote_addr << ", ";
            oss << "RequestId:" << chan_status.m_request_id;
            if (chan_status.m_has_video) {
                oss << "Width:" << chan_status.m_video.m_width << ", ";
                oss << "Heigh:" << chan_status.m_video.m_heigh << ", ";
                oss << "FrameRate:" << chan_status.m_video.m_framerate << ", ";
                oss << "Bindwidth:" << chan_status.m_video.m_bandwidth << ", ";
                oss << "Codec:" << chan_status.m_video.m_codec << ", ";
            }  
            if (chan_status.m_has_audio) {
                oss << "Bindwidth:" << chan_status.m_audio.m_bandwidth << ", ";
                oss << "SampleRate:" << chan_status.m_audio.m_samplerate << ", ";
                oss << "Codec:" << chan_status.m_audio.m_codec << ", ";
            }
            std::cout << oss.str() << std::endl;
        }
    }
    std::cout << "====================================================================" << std::endl;
}

void GetLiveChannelVodPlaylist(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& channel_name) {
    qcloud_cos::GetLiveChannelVodPlaylistReq req(bucket_name, channel_name);
    qcloud_cos::GetLiveChannelVodPlaylistResp resp;
    req.SetTime(time(NULL) - 10000, time(NULL));
    qcloud_cos::CosResult result = cos.GetLiveChannelVodPlaylist(req, &resp);
    std::cout << "===================GetLiveChannelVodPlaylist=====================" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        resp.WriteResultToFile("./playlist.m3u8");
    }
    std::cout << "====================================================================" << std::endl;
}

void PostLiveChannelVodPlaylist(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& channel_name) {
    qcloud_cos::PostLiveChannelVodPlaylistReq req(bucket_name, channel_name);
    qcloud_cos::PostLiveChannelVodPlaylistResp resp;
    req.SetTime(time(NULL) - 10000, time(NULL));
    req.SetPlaylistName("newplaylist.m3u8");
    qcloud_cos::CosResult result = cos.PostLiveChannelVodPlaylist(req, &resp);
    std::cout << "===================PostLiveChannelVodPlaylist=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void DeleteLiveChannel(qcloud_cos::CosAPI& cos, const std::string& bucket_name, const std::string& channel_name) {
    qcloud_cos::DeleteLiveChannelReq req(bucket_name, channel_name);
    qcloud_cos::DeleteLiveChannelResp resp;
    qcloud_cos::CosResult result = cos.DeleteLiveChannel(req, &resp);
    std::cout << "===================DeleteLiveChannel=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void ListLiveChannel(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
    qcloud_cos::ListLiveChannelReq req(bucket_name);
    qcloud_cos::ListLiveChannelResp resp;
    ListLiveChannelResult list_result;
    qcloud_cos::CosResult result;
    std::cout << "===================ListLiveChannel=====================" << std::endl;
    do {
        req.SetMaxKeys(10);
        req.SetMarker(list_result.m_next_marker);
        result = cos.ListLiveChannel(req, &resp);
        PrintResult(result, resp);
        if (result.IsSucc()) {
            list_result = resp.GetListResult();
            std::stringstream oss;
            oss << "MaxKeys:" << list_result.m_max_keys << ", ";
            oss << "Marker:" << list_result.m_marker << ", ";
            oss << "Prefix:" << list_result.m_prefix << ", ";
            oss << "IsTruncated:" << list_result.m_is_truncated << ", ";
            oss << "NextMarker:" << list_result.m_next_marker << ", ";
            std::vector<LiveChannel>::const_iterator it = list_result.m_channels.begin();
            for (; it != list_result.m_channels.end(); ++it) {
                oss << "Name:" << it->m_name << ", LastModified:" << it->m_last_modified << std::endl;
            }
            std::cout << oss.str() << std::endl;
            resp.ClearResult();
       }
    } while(list_result.m_is_truncated == "true" && result.IsSucc());
    std::cout << "====================================================================" << std::endl;
}

int main(int argc, char** argv) {
    qcloud_cos::CosConfig config("./config.json");
    qcloud_cos::CosAPI cos(config);

    std::string bucket_name = "test1-1234567890";
    //PutBucketInventory(cos, bucket_name);
    //GetBucketInventory(cos,bucket_name);
    //PutBucketDomain(cos, bucket_name);
    //GetBucketDomain(cos, bucket_name);
    //ListBucketInventoryConfigurations(cos, bucket_name); 
    //DeleteBucketInventory(cos, bucket_name);
    //ListBucketInventoryConfigurations(cos, bucket_name); 
    //PutBucketLogging(cos, bucket_name, bucket_name1, "log"); 
    //GetBucketLogging(cos, bucket_name);
    //std::vector<Tag> tagset;
    //Tag tag1;
    //tag1.SetKey("age");
    //tag1.SetValue("19");
	 
    //Tag tag2;
    //tag2.SetKey("name");
    //tag2.SetValue("xiaoming");
	 
    //Tag tag3;
    //tag3.SetKey("sex");
    //tag3.SetValue("male");
	
    //tagset.push_back(tag1);
    //tagset.push_back(tag2);
    //tagset.push_back(tag3);
	 
    //PutBucketTagging(cos, bucket_name, tagset);
    //GetBucketTagging(cos, bucket_name);
    //DeleteBucketTagging(cos, bucket_name);
	
    //PutBucketWebsite(cos, bucket_name);
    //GetBucketWebsite(cos, bucket_name);
    //DeleteBucketWebsite(cos, bucket_name);	
	
    //GetService(cos);
    //PutBucket(cos, bucket_name);
    //GetBucket(cos, bucket_name);
    // PutBucketVersioning(cos, bucket_name);
    // GetBucketVersioning(cos, bucket_name);
    //PutBucketLogging(cos, bucket_name, targetbucket_name, prefix);
    //GetBucketLogging(cos, bucket_name);
    // PutBucketReplication(cos, bucket_name);
    // GetBucketReplication(cos, bucket_name);
    //DeleteBucketReplication(cos, bucket_name);
    //PutBucketLifecycle(cos, bucket_name);
    //GetBucketLifecycle(cos, bucket_name);
    //DeleteBucketLifecycle(cos, bucket_name);
    //PutBucketACL(cos, bucket_name);
    //GetBucketACL(cos, bucket_name);
    //PutBucketCORS(cos, bucket_name);
    //GetBucketCORS(cos, bucket_name);
    //DeleteBucketCORS(cos, bucket_name);

    //// 简单上传(文件)
    //PutObjectByFile(cos, bucket_name, "sevenyou_1102_south", "/data/sevenyou/temp/seven_0821_10M");
    // PutObjectByFile(cos, bucket_name, "sevenyou_e2_1102_north", "/data/sevenyou/temp/seven_0821_10M");
    // PutObjectByFile(cos, bucket_name, "sevenyounorthtest2_normal", "/data/sevenyou/temp/seven_0821_10M");
    //PutObjectByFile(cos, bucket_name, "sevenyou_e2_north_put", "/data/sevenyou/temp/seven_0821_10M");
    // HeadObject(cos, bucket_name, "sevenyou_1102_north.jpg");
    //GetObjectByFile(cos, bucket_name, "costest.php", "/data/sevenyou/temp/costest.php");
    //PutObjectByFile(cos, bucket_name, "sevenyou_e2_north_put", "/data/sevenyou/temp/seven_0821_10M");
    //// 简单上传(文件),特殊字符
    //PutObjectByFile(cos, bucket_name, "/→↓←→↖↗↙↘! \"#$%&'()*+,-./0123456789:;"
    //                "<=>@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~",
    //                "/data/sevenyou/temp/seven_0821_10M");
    //// 简单上传(文件),中文字符
    //PutObjectByFile(cos, bucket_name, "/中文文件", "/data/sevenyou/temp/seven_0821_10M");
    //// 简单上传(文件), 大文件
    //PutObjectByFile(cos, bucket_name, "sevenyou_0803_2g",
    //                "/data/sevenyou/cos-cpp-sdk-26/testdata/seven_2g.tmp");


    //PutObjectByStream(cos, bucket_name);

    //HeadObject(cos, bucket_name, "sevenyou_e1_south_put_copy");
    //HeadObject(cos, bucket_name, "sevenyou_e2_abc.jar");
    //HeadObject(cos, bucket_name, "sevenyou_6G");
    // GetObjectByFile(cos, bucket_name, "sevenyou_e1_abc", "/data/sevenyou/temp/sevenyou_10m_download_03");
    // GetObjectByFile(cos, bucket_name, "sevenyou_e2_abc", "/data/sevenyou/temp/sevenyou_10m_download_03");
    //GetObjectByStream(cos, bucket_name, "sevenyou_e2_abc");
    // MultiGetObject(cos, bucket_name, "sevenyou_1102_south_multi", "/data/sevenyou/temp/sevenyou_10m_download_03");

    // {
    //     std::string upload_id;
    //     std::string object_name = "sevenyou_e2_1102_north_multi";
    //     std::vector<uint64_t> numbers;
    //     std::vector<std::string> etags;

    //     std::string etag1 = "", etag2 = "";
    //     InitMultiUpload(cos, bucket_name, object_name, &upload_id);

    //     std::fstream is1("/data/sevenyou/temp/seven_0821_5M.part1");
    //     UploadPartData(cos, bucket_name, object_name, upload_id, is1, 1, &etag1);
    //     numbers.push_back(1);
    //     etags.push_back(etag1);
    //     is1.close();

    //     ListParts(cos, bucket_name, object_name, upload_id);
    //     // AbortMultiUpload(cos, bucket_name, object_name, upload_id);

    //     std::fstream is2("/data/sevenyou/temp/seven_0821_5M.part2");
    //     UploadPartData(cos, bucket_name, object_name, upload_id, is2, 2, &etag2);
    //     numbers.push_back(2);
    //     etags.push_back(etag2);

    //     is2.close();

    //     CompleteMultiUpload(cos, bucket_name, object_name, upload_id, etags, numbers);
    // }

    // {
    //     std::string upload_id;
    //     std::string object_name = "sevenyou_2G_part_copy_from_north";
    //     std::vector<uint64_t> numbers;
    //     std::vector<std::string> etags;

    //     std::string etag1 = "", etag2 = "";
    //     InitMultiUpload(cos, bucket_name, object_name, &upload_id);

    //     UploadPartCopy(cos, bucket_name, object_name, upload_id, "sevenyousouth-1251668577.cos.ap-guangzhou.myqcloud.com/seven_10G.tmp", "bytes=0-1048576000", 1, &etag1);
    //     numbers.push_back(1);
    //     etags.push_back(etag1);

    //     UploadPartCopy(cos, bucket_name, object_name, upload_id, "sevenyoutest-7319456.cos.cn-north.myqcloud.com/sevenyou_2G_part", "bytes=1048576000-2097152000", 2, &etag2);
    //     numbers.push_back(2);
    //     etags.push_back(etag2);

    //     CompleteMultiUpload(cos, bucket_name, object_name, upload_id, etags, numbers);
    // }


    // // Copy(cos, bucket_name, "sevenyou_6G_diff_region_copy_part", "sevenyoutest-123456.cos.ap-beijing-1.myqcloud.com/sevenyou_6G");

    // //MultiUploadObject(cos, bucket_name, "sevenyou_e2_multi", "/data/sevenyou/temp/seven_50M.tmp.0925");
    // //MultiUploadObject(cos, bucket_name, "sevenyou_1102_north_multi", "/data/sevenyou/temp/seven_50M.tmp.0925");


    // //PutObjectACL(cos, bucket_name, "sevenyou_10m");

    // //PutObjectCopy(cos, bucket_name, "sevenyou_e3_put_obj_copy_from_north_normal",
    // //              "sevenyounorthtestbak-7319456.cn-north.myqcloud.com/sevenyou_1102_north");
    // //PutObjectCopy(cos, bucket_name, "sevenyou_e1_put_obj_copy_from_north_multi",
    // //              "sevenyounorthtestbak-7319456.cn-north.myqcloud.com/sevenyou_1102_north_multi");
    // //PutObjectCopy(cos, bucket_name, "sevenyou_e2_put_obj_copy_from_south_multi",
    // //              "sevenyousouthtest-7319456.cn-north.myqcloud.com/sevenyou_1102_south_multi");
    // //PutObjectCopy(cos, bucket_name, "sevenyou_e2_north_put_copy_from_south_16",
    // //              "sevenyousouthtest-7319456.cn-south.myqcloud.com/sevenyou_e1_south_put");

    // //DeleteObject(cos, bucket_name, "sevenyou_e1_multi");
    // //DeleteObject(cos, bucket_name, "sevenyou_e2_put");

    // //DeleteBucket(cos, bucket_name);

    // {
    //     qcloud_cos::GeneratePresignedUrlReq req(bucket_name, "seven_50M.tmp", qcloud_cos::HTTP_GET);
    //     std::string presigned_url = cos.GeneratePresignedUrl(req);
    //     std::cout << "Presigend Uril=[" << presigned_url << "]" << std::endl;
    // }

    // {
    //     std::string presigned_url = cos.GeneratePresignedUrl(bucket_name, "seven_50M.tmp", 1514799284, 1514899284);
    //     std::cout << "Presigend Uril=[" << presigned_url << "]" << std::endl;
    // }

    // {
    //     std::cout << "Bucket=" << bucket_name << ", Location=" << cos.GetBucketLocation(bucket_name) << std::endl;
    // }

    // std::cout << "IsBucketExist=";
    // std::cout << (cos.IsBucketExist("abcdefg") ? "true" : "false") << std::endl;
    // std::cout << (cos.IsBucketExist(bucket_name) ? "true" : "false") << std::endl;

    // std::cout << "IsObjectExist=" << std::endl;
    // std::cout << (cos.IsObjectExist(bucket_name, "abcdefg") ? "true" : "false") << std::endl;
    // std::cout << (cos.IsObjectExist(bucket_name, "seven_50M.tmp") ? "true" : "false") << std::endl;

    // // Batch Delete
    // {
    //     std::vector<std::string> objects;
    //     std::vector<ObjectVersionPair> to_be_deleted;
    //     objects.push_back("batch_delete_test_00");
    //     objects.push_back("batch_delete_test_01");
    //     objects.push_back("batch_delete_test_02");
    //     objects.push_back("batch_delete_test_03");
    //     for (size_t idx = 0; idx < objects.size(); ++idx) {
    //         ObjectVersionPair pair;
    //         pair.m_object_name = objects[idx];
    //         if (idx == 2) {
    //             pair.m_version_id = "MTg0NDY3NDI1NTg4Mjc3NzExNjI";
    //         } else if (idx == 3) {
    //             pair.m_version_id = "MTg0NDY3NDI1NTg4Mjc3NzA3Nzc";
    //         }


    //         to_be_deleted.push_back(pair);
    //     }


    //     DeleteObjects(cos, bucket_name, to_be_deleted);
    // }

    // {
    //     GetBucketObjectVersionsReq req(bucket_name);
    //     req.SetPrefix("batch_delete_test_");
    //     req.SetEncodingType("url");
    //     GetBucketObjectVersionsResp resp;
    //     CosResult result = cos.GetBucketObjectVersions(req, &resp);
    //     std::cout << "===================DeleteBucketResponse=====================" << std::endl;
    //     PrintResult(result, resp);
    //     std::cout << "=========================================================" << std::endl;
    // }

    // // Restore
    // {
    //     PostObjectRestoreReq req(bucket_name, "restore_test_obj");
    //     req.SetExiryDays(30);
    //     req.SetTier("Standard");
    //     PostObjectRestoreResp resp;

    //     CosResult result = cos.PostObjectRestore(req, &resp);
    //     std::cout << "======================PostObjectRestore=-=========" << std::endl;
    //     PrintResult(result, resp);
    //     std::cout << "=========================================================" << std::endl;
    // }

    //限速上传下载
    //{
    //    std::string bucket_name = "testbucket";
    //    std::string object_name_prefix = "test_traffic_limit_";
    //    std::string upload_file_path = "/data/testtrafficlimit/testfile_100M";
    //    std::string download_file_path_prefix = "/data/testtrafficlimit/download_test_traffic_limit_";
    //    std::string object_name, download_file_path;
    //    if (access(upload_file_path.c_str(), F_OK)) {
    //        std::cerr << upload_file_path << " not exists" << std::endl;
    //        return -1;
    //    }        
    //    //限速上传/下载文件,by header
    //    object_name = object_name_prefix + "1";
    //    download_file_path = download_file_path_prefix + "1";
    //    std::cout << "===================PutObjectByFileLimitTraffic, by header, traffic limit = 1MB/s = 8388608 byte/sec =====================" << std::endl;
    //    PutObjectByFileLimitTraffic(cos, bucket_name, object_name, upload_file_path, 8388608);
    //    std::cout << "===================GetObjectByFileLimitTraffic, by header, Traffic limit = 4MB/s = 33554432 byte/sec =====================" << std::endl;
    //    GetObjectByFileLimitTraffic(cos, bucket_name, object_name, download_file_path, 33554432);
    //    DeleteObject(cos, bucket_name, object_name);
    //
    //    //限速上传/下载文件,by param
    //    object_name = object_name_prefix + "2";
    //    download_file_path = download_file_path_prefix + "2";
    //    std::cout << "===================PutObjectByFileLimitTraffic, by param, traffic limit = = 1MB/s = 8388608 byte/sec =====================" << std::endl;
    //    PutObjectByFileLimitTraffic(cos, bucket_name, object_name, upload_file_path, 8388608, false);
    //    std::cout << "===================GetObjectByFileLimitTraffic, by param, Traffic limit = 4MB/s = 33554432 byte/sec=====================" << std::endl;
    //    GetObjectByFileLimitTraffic(cos, bucket_name, object_name, download_file_path, 33554432, false);
    //    DeleteObject(cos, bucket_name, download_file_path);
    //    
    //    //限速分块上传文件
    //    object_name = object_name_prefix + "3";
    //    download_file_path = download_file_path_prefix + "3";
    //    std::string upload_id;
    //    std::vector<uint64_t> numbers;
    //    std::vector<std::string> etags;
    //    std::string etag1 = "", etag2 = "";
    //    InitMultiUpload(cos, bucket_name, object_name, &upload_id);
    //    std::cout << "upload_id:" << upload_id << std::endl;
    //    std::cout << "===================UploadPartDataLimitTraffic, part 1, Traffic limit = 4MB/s = 33554432 byte/sec =====================" << std::endl;
    //    std::fstream fs1(upload_file_path);
    //    UploadPartDataLimitTraffic(cos, bucket_name, object_name, upload_id, fs1, 1, &etag1, 33554432);
    //    numbers.push_back(1);
    //    etags.push_back(etag1);        
    //    std::cout << "===================UploadPartDataLimitTraffic, part 2, Traffic limit = 4MB/s = 33554432 byte/sec =====================" << std::endl;        
    //    std::fstream fs2(upload_file_path);
    //    UploadPartDataLimitTraffic(cos, bucket_name, object_name, upload_id, fs2, 2, &etag2, 33554432);
    //    numbers.push_back(2);
    //    etags.push_back(etag2);
    //    CompleteMultiUpload(cos, bucket_name, object_name, upload_id, etags, numbers);
    //
    //    //多线程限速下载文件
    //    std::cout << "===================MultiThradDownObjectLimitTraffic, Traffic limit = 1MB/s = 8388608 byte/sec =====================" << std::endl;
    //    MultiGetObjectLimitTraffic(cos, bucket_name, object_name, download_file_path, 8388608);
    //
    //    //多线程限速上传文件
    //    object_name = object_name_prefix + "4";
    //    std::cout << "===================MultiThradUploadObjectLimitTraffic, Traffic limit = 1MB/s = 8388608 byte/sec =====================" << std::endl;
    //    MultiUploadObjectLimitTraffic(cos, bucket_name, object_name, upload_file_path, 8388608);
    //
    //    system("rm -f /data/testtrafficlimit/download_test_traffic_limit*");
    //}

    //{
    //    CreateLiveChannel(cos, bucket_name, "test-ch-1");
    //    GetLiveChannel(cos, bucket_name, "test-ch-1");
    //    GetRtmpSignedPublishUrl(cos, bucket_name, "test-ch-1");
    //    PutLiveChannelSwitch(cos, bucket_name, "test-ch-1");
    //    GetLiveChannelHistory(cos, bucket_name, "test-ch-1");
    //    GetLiveChannelStatus(cos, bucket_name, "test-ch-1");
    //    GetLiveChannelVodPlaylist(cos, bucket_name, "test-ch-1");
    //    PostLiveChannelVodPlaylist(cos, bucket_name, "test-ch-1");
    //    ListLiveChannel(cos, bucket_name);
    //    DeleteLiveChannel(cos, bucket_name, "test-ch-1");
    //}

    return 0;
}
