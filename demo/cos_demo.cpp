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

#include "util/auth_tool.h"
#include "cos_api.h"
#include "cos_sys_config.h"
#include "cos_defines.h"

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
        transition.SetStorageClass("NEARLINE");
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

void GetObjectByStream(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                       const std::string& object_name) {
    std::ostringstream os;
    qcloud_cos::GetObjectByStreamReq req(bucket_name,
                                         object_name, os);
    qcloud_cos::GetObjectByStreamResp resp;

    qcloud_cos::CosResult result = cos.GetObject(req, &resp);
    std::cout << "===================GetObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
    std::cout << os.str() << std::endl;
}

void MultiGetObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name, const std::string& file_path) {
    qcloud_cos::MultiGetObjectReq req(bucket_name,
                                      object_name, file_path);
    qcloud_cos::MultiGetObjectResp resp;

    qcloud_cos::CosResult result = cos.GetObject(req, &resp);
    std::cout << "===================GetObjectResponse=====================" << std::endl;
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


int main(int argc, char** argv) {
    qcloud_cos::CosConfig config("./config.json");
    qcloud_cos::CosAPI cos(config);

    std::string bucket_name = "sevenyousouth-1251668577";

    //GetService(cos);
    //PutBucket(cos, bucket_name);
    //GetBucket(cos, bucket_name);
    // PutBucketVersioning(cos, bucket_name);
    // GetBucketVersioning(cos, bucket_name);
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
// PutObjectByFile(cos, bucket_name, "sevenyou_1102_north.jpg", "/data/sevenyou/temp/seven_0821_10M");
    //PutObjectByFile(cos, bucket_name, "sevenyou_1102_south", "/data/sevenyou/temp/seven_0821_10M");
    // PutObjectByFile(cos, bucket_name, "sevenyou_e2_1102_north", "/data/sevenyou/temp/seven_0821_10M");
    // PutObjectByFile(cos, bucket_name, "sevenyounorthtest2_normal", "/data/sevenyou/temp/seven_0821_10M");
    //PutObjectByFile(cos, bucket_name, "sevenyou_e2_north_put", "/data/sevenyou/temp/seven_0821_10M");
    // HeadObject(cos, bucket_name, "sevenyou_1102_north.jpg");
    // GetObjectByFile(cos, bucket_name, "sevenyou_e2_1102_north", "/data/sevenyou/temp/sevenyou_10m_download_04");
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

    {
        std::string upload_id;
        std::string object_name = "sevenyou_e2_1102_north_multi";
        std::vector<uint64_t> numbers;
        std::vector<std::string> etags;

        std::string etag1 = "", etag2 = "";
        InitMultiUpload(cos, bucket_name, object_name, &upload_id);

        std::fstream is1("/data/sevenyou/temp/seven_0821_5M.part1");
        UploadPartData(cos, bucket_name, object_name, upload_id, is1, 1, &etag1);
        numbers.push_back(1);
        etags.push_back(etag1);
        is1.close();

        ListParts(cos, bucket_name, object_name, upload_id);
        // AbortMultiUpload(cos, bucket_name, object_name, upload_id);

        std::fstream is2("/data/sevenyou/temp/seven_0821_5M.part2");
        UploadPartData(cos, bucket_name, object_name, upload_id, is2, 2, &etag2);
        numbers.push_back(2);
        etags.push_back(etag2);

        is2.close();

        CompleteMultiUpload(cos, bucket_name, object_name, upload_id, etags, numbers);
    }

    {
        std::string upload_id;
        std::string object_name = "sevenyou_2G_part_copy_from_north";
        std::vector<uint64_t> numbers;
        std::vector<std::string> etags;

        std::string etag1 = "", etag2 = "";
        InitMultiUpload(cos, bucket_name, object_name, &upload_id);

        UploadPartCopy(cos, bucket_name, object_name, upload_id, "sevenyousouth-1251668577.cos.ap-guangzhou.myqcloud.com/seven_10G.tmp", "bytes=0-1048576000", 1, &etag1);
        numbers.push_back(1);
        etags.push_back(etag1);

        UploadPartCopy(cos, bucket_name, object_name, upload_id, "sevenyoutest-7319456.cos.cn-north.myqcloud.com/sevenyou_2G_part", "bytes=1048576000-2097152000", 2, &etag2);
        numbers.push_back(2);
        etags.push_back(etag2);

        CompleteMultiUpload(cos, bucket_name, object_name, upload_id, etags, numbers);
    }


    // Copy(cos, bucket_name, "sevenyou_6G_diff_region_copy_part", "sevenyoutest-123456.cos.ap-beijing-1.myqcloud.com/sevenyou_6G");

    //MultiUploadObject(cos, bucket_name, "sevenyou_e2_multi", "/data/sevenyou/temp/seven_50M.tmp.0925");
    //MultiUploadObject(cos, bucket_name, "sevenyou_1102_north_multi", "/data/sevenyou/temp/seven_50M.tmp.0925");


    //PutObjectACL(cos, bucket_name, "sevenyou_10m");
    //GetObjectACL(cos, bucket_name, "sevenyou_10m");

    //PutObjectCopy(cos, bucket_name, "sevenyou_e3_put_obj_copy_from_north_normal",
    //              "sevenyounorthtestbak-7319456.cn-north.myqcloud.com/sevenyou_1102_north");
    //PutObjectCopy(cos, bucket_name, "sevenyou_e1_put_obj_copy_from_north_multi",
    //              "sevenyounorthtestbak-7319456.cn-north.myqcloud.com/sevenyou_1102_north_multi");
    //PutObjectCopy(cos, bucket_name, "sevenyou_e2_put_obj_copy_from_south_multi",
    //              "sevenyousouthtest-7319456.cn-north.myqcloud.com/sevenyou_1102_south_multi");
    //PutObjectCopy(cos, bucket_name, "sevenyou_e2_north_put_copy_from_south_16",
    //              "sevenyousouthtest-7319456.cn-south.myqcloud.com/sevenyou_e1_south_put");

    //DeleteObject(cos, bucket_name, "sevenyou_e1_multi");
    //DeleteObject(cos, bucket_name, "sevenyou_e2_put");

    //DeleteBucket(cos, bucket_name);
}

