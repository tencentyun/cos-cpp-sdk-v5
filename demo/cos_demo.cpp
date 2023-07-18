// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/18/17
// Description:

#include <stdlib.h>
#include <sys/stat.h>

#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"
#include "util/auth_tool.h"

#include "Poco/TaskManager.h"

using namespace qcloud_cos;
void PrintResult(const qcloud_cos::CosResult& result,
                 const qcloud_cos::BaseResp& resp) {
  if (result.IsSucc()) {
    std::cout << resp.DebugString() << std::endl;
  } else {
    std::cout << "ErrorMsg=" << result.GetErrorMsg() << std::endl;
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

  std::cout << "===================GetService====================="
            << std::endl;
  PrintResult(result, resp);
  const qcloud_cos::Owner& owner = resp.GetOwner();
  const std::vector<qcloud_cos::Bucket>& buckets = resp.GetBuckets();
  std::cout << "owner.m_id=" << owner.m_id
            << ", owner.display_name=" << owner.m_display_name << std::endl;
  for (std::vector<qcloud_cos::Bucket>::const_iterator itr = buckets.begin();
       itr != buckets.end(); ++itr) {
    const qcloud_cos::Bucket& bucket = *itr;
    std::cout << "Bucket name=" << bucket.m_name
              << ", location=" << bucket.m_location
              << ", create_date=" << bucket.m_create_date << std::endl;
  }

  std::cout << "========================================================="
            << std::endl;
}

void HeadBucket(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::HeadBucketReq req(bucket_name);
  qcloud_cos::HeadBucketResp resp;
  qcloud_cos::CosResult result = cos.HeadBucket(req, &resp);

  std::cout << "===================HeadBucketResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "=========================================================="
            << std::endl;
}

void DeleteBucket(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::DeleteBucketReq req(bucket_name);
  qcloud_cos::DeleteBucketResp resp;
  qcloud_cos::CosResult result = cos.DeleteBucket(req, &resp);

  std::cout << "===================DeleteBucketResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

void PutBucket(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::PutBucketReq req(bucket_name);
  //创建MAZ存储桶使用下方进行设置
  //req.SetMAZBucket();
  qcloud_cos::PutBucketResp resp;
  qcloud_cos::CosResult result = cos.PutBucket(req, &resp);

  std::cout << "===================PutBucketResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

void GetBucket(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::GetBucketReq req(bucket_name);
  qcloud_cos::GetBucketResp resp;
  qcloud_cos::CosResult result = cos.GetBucket(req, &resp);

  std::cout << "===================GetBucketResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

void PutBucketVersioning(qcloud_cos::CosAPI& cos,
                         const std::string& bucket_name) {
  qcloud_cos::PutBucketVersioningReq req(bucket_name);
  req.SetStatus(true);
  qcloud_cos::PutBucketVersioningResp resp;

  qcloud_cos::CosResult result = cos.PutBucketVersioning(req, &resp);
  std::cout
      << "===================PutBucketVersioningResponse====================="
      << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void GetBucketVersioning(qcloud_cos::CosAPI& cos,
                         const std::string& bucket_name) {
  qcloud_cos::GetBucketVersioningReq req(bucket_name);
  qcloud_cos::GetBucketVersioningResp resp;
  qcloud_cos::CosResult result = cos.GetBucketVersioning(req, &resp);

  std::cout << "Bucket status= " << resp.GetStatus() << std::endl;
  std::cout
      << "===================GetBucketVersioningResponse====================="
      << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void PutBucketReplication(qcloud_cos::CosAPI& cos,
                          const std::string& bucket_name) {
  qcloud_cos::PutBucketReplicationReq req(bucket_name);
  req.SetRole("qcs::cam::uin/2779643970:uin/2779643970");
  qcloud_cos::ReplicationRule rule(
      "", "qcs:id/0:cos:cn-south:appid/1251668577:sevenyoutest1251668577", "",
      "", true);

  req.AddReplicationRule(rule);
  qcloud_cos::PutBucketReplicationResp resp;
  qcloud_cos::CosResult result = cos.PutBucketReplication(req, &resp);
  std::cout
      << "===================PutBucketReplicationResponse====================="
      << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void GetBucketReplication(qcloud_cos::CosAPI& cos,
                          const std::string& bucket_name) {
  qcloud_cos::GetBucketReplicationReq req(bucket_name);
  qcloud_cos::GetBucketReplicationResp resp;
  qcloud_cos::CosResult result = cos.GetBucketReplication(req, &resp);

  std::cout
      << "===================GetBucketReplicationResponse====================="
      << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void DeleteBucketReplication(qcloud_cos::CosAPI& cos,
                             const std::string& bucket_name) {
  qcloud_cos::DeleteBucketReplicationReq req(bucket_name);
  qcloud_cos::DeleteBucketReplicationResp resp;
  qcloud_cos::CosResult result = cos.DeleteBucketReplication(req, &resp);

  std::cout << "===================DeleteBucketReplicationResponse============="
               "========"
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void PutBucketLifecycle(qcloud_cos::CosAPI& cos,
                        const std::string& bucket_name) {
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
  std::cout
      << "===================PutBucketLifecycleResponse====================="
      << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void GetBucketLifecycle(qcloud_cos::CosAPI& cos,
                        const std::string& bucket_name) {
  qcloud_cos::GetBucketLifecycleReq req(bucket_name);
  qcloud_cos::GetBucketLifecycleResp resp;
  qcloud_cos::CosResult result = cos.GetBucketLifecycle(req, &resp);

  const std::vector<qcloud_cos::LifecycleRule>& rules = resp.GetRules();
  for (size_t idx = 0; idx != rules.size(); ++idx) {
    std::cout << "id = " << rules[idx].GetId() << std::endl;
  }

  std::cout
      << "===================GetBucketLifecycleResponse====================="
      << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void DeleteBucketLifecycle(qcloud_cos::CosAPI& cos,
                           const std::string& bucket_name) {
  qcloud_cos::DeleteBucketLifecycleReq req(bucket_name);
  qcloud_cos::DeleteBucketLifecycleResp resp;
  qcloud_cos::CosResult result = cos.DeleteBucketLifecycle(req, &resp);

  std::cout
      << "===================DeleteBucketLifecycleResponse====================="
      << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void PutBucketACL(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  // 1. 设置ACL配置(通过Body,
  // 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
  {
    qcloud_cos::PutBucketACLReq req(bucket_name);
    qcloud_cos::Owner owner = {"qcs::cam::uin/491107627:uin/491107627",
                               "qcs::cam::uin/491107627:uin/491107627"};
    qcloud_cos::Grant grant;
    req.SetOwner(owner);
    grant.m_grantee.m_type = "Group";
    grant.m_grantee.m_uri = "http://cam.qcloud.com/groups/global/AllUsers";
    grant.m_perm = "READ";
    req.AddAccessControlList(grant);

    qcloud_cos::PutBucketACLResp resp;
    qcloud_cos::CosResult result = cos.PutBucketACL(req, &resp);
    std::cout << "===================PutBucketACLResponse====================="
              << std::endl;
    PrintResult(result, resp);
    std::cout << "============================================================="
                 "======="
              << std::endl;
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
  std::cout << "===================GetBucketACLResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void GetBucketPolicy(qcloud_cos::CosAPI& cos,const std::string& bucket_name) {
  qcloud_cos::GetBucketPolicyReq req(bucket_name);
  qcloud_cos::GetBucketPolicyResp resp;
  qcloud_cos::CosResult result = cos.GetBucketPolicy(req, &resp);

  std::cout << resp.GetPolicy() << std::endl; 
  std::cout << "===================GetBucketPolicy====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void PutBucketPolicy(qcloud_cos::CosAPI& cos,const std::string& bucket_name) {
  qcloud_cos::PutBucketPolicyReq req(bucket_name);
  qcloud_cos::PutBucketPolicyResp resp;
  std::string bucket_policy = 
                    "  {"
                    "    \"Statement\": ["
                    "      {"
                    "        \"Principal\": {"
                    "          \"qcs\": ["
                    "            \"qcs::cam::uin/100000000001:uin/100000000011\"" //替换成您想授予权限的账户 uin
                    "          ]\n"
                    "        },\n"
                    "        \"Effect\": \"allow\","
                    "        \"Action\": ["
                    "          \"cos:PutObject\""
                    "        ],\n"
                    "        \"Resource\": [" //这里改成允许的路径前缀，可以根据自己网站的用户登录态判断允许上传的具体路径，例子： a.jpg 或者 a/* 或者 * (使用通配符*存在重大安全风险, 请谨慎评估使用)
                    "          \"qcs::cos:ap-guangzhou:uid/1250000000:examplebucket-1250000000/exampleobject\""
                    "        ],\n"
                    "        \"Condition\": {"
                    "          \"string_equal\": {"
                    "            \"cos:x-cos-mime-limit\": \"image/jpeg\""
                    "          }"
                    "        }"
                    "      }"
                    "    ],"
                    "    \"Version\": \"2.0\""
                    "  }";
  std::cout << bucket_policy << std::endl;
  req.SetBody(bucket_policy);
  qcloud_cos::CosResult result = cos.PutBucketPolicy(req, &resp);

  std::cout << "===================PutBucketPolicy====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void DeleteBucketPolicy(qcloud_cos::CosAPI& cos,const std::string& bucket_name) {
  qcloud_cos::DeleteBucketPolicyReq req(bucket_name);
  qcloud_cos::DeleteBucketPolicyResp resp;
  qcloud_cos::CosResult result = cos.DeleteBucketPolicy(req, &resp);

  std::cout << "===================DeleteBucketPolicy====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
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
  std::cout << "===================PutBucketCORSResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void GetBucketCORS(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::GetBucketCORSReq req(bucket_name);
  qcloud_cos::GetBucketCORSResp resp;
  qcloud_cos::CosResult result = cos.GetBucketCORS(req, &resp);

  std::cout << "===================GetBucketCORSResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void DeleteBucketCORS(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::DeleteBucketCORSReq req(bucket_name);
  qcloud_cos::DeleteBucketCORSResp resp;
  qcloud_cos::CosResult result = cos.DeleteBucketCORS(req, &resp);

  std::cout
      << "===================DeleteBucketCORSResponse====================="
      << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void PutObjectByFile(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                     const std::string& object_name,
                     const std::string& file_path) {
  qcloud_cos::PutObjectByFileReq req(bucket_name, object_name, file_path);
  // req.AddParam("versionId", "MTg0NDY3NDI1NjQ4NjUyMjQ1MTA");
  qcloud_cos::PutObjectByFileResp resp;
  qcloud_cos::CosResult result = cos.PutObject(req, &resp);

  std::cout << "===================PutObjectResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

// 限速上传对象
void PutObjectByFileLimitTraffic(qcloud_cos::CosAPI& cos,
                                 const std::string& bucket_name,
                                 const std::string& object_name,
                                 const std::string& file_path,
                                 const uint64_t traffic_limit) {
  qcloud_cos::PutObjectByFileReq req(bucket_name, object_name, file_path);
  req.SetTrafficLimit(traffic_limit);
  qcloud_cos::PutObjectByFileResp resp;
  qcloud_cos::CosResult result = cos.PutObject(req, &resp);
  std::cout << "===================PutObjectFileLimitTrafficResponse==========="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

void PutObjectByStream(qcloud_cos::CosAPI& cos,
                       const std::string& bucket_name) {
  std::istringstream iss("put object");
  qcloud_cos::PutObjectByStreamReq req(bucket_name, "sevenyou_10m", iss);
  qcloud_cos::PutObjectByStreamResp resp;
  qcloud_cos::CosResult result = cos.PutObject(req, &resp);

  std::cout << "===================PutObjectResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

void HeadObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                const std::string& object_name) {
  qcloud_cos::HeadObjectReq req(bucket_name, object_name);
  // req.AddParam("versionId", "MTg0NDY3NDI1NjQ4NjUyMjQ1MTA");
  qcloud_cos::HeadObjectResp resp;

  qcloud_cos::CosResult result = cos.HeadObject(req, &resp);

  std::cout << "===================HeadObjectResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "=========================================================="
            << std::endl;
}

// 下载对象到指定文件
void GetObjectByFile(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                     const std::string& object_name,
                     const std::string& file_path) {
  qcloud_cos::GetObjectByFileReq req(bucket_name, object_name, file_path);
  // req.AddParam("versionId", "MTg0NDY3NDI1NjQwODU3NzU2MTA");
  qcloud_cos::GetObjectByFileResp resp;

  qcloud_cos::CosResult result = cos.GetObject(req, &resp);
  std::cout << "===================GetObjectResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

// 限速下载对象
void GetObjectByFileLimitTraffic(qcloud_cos::CosAPI& cos,
                                 const std::string& bucket_name,
                                 const std::string& object_name,
                                 const std::string& file_path,
                                 const uint64_t traffic_limit) {
  qcloud_cos::GetObjectByFileReq req(bucket_name, object_name, file_path);
  req.SetTrafficLimit(traffic_limit);
  qcloud_cos::GetObjectByFileResp resp;

  qcloud_cos::CosResult result = cos.GetObject(req, &resp);
  std::cout << "===================GetObjectByFileLimitTrafficResponse========="
               "============"
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

void GetObjectByStream(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                       const std::string& object_name) {
  std::ostringstream os;
  qcloud_cos::GetObjectByStreamReq req(bucket_name, object_name, os);
  qcloud_cos::GetObjectByStreamResp resp;

  qcloud_cos::CosResult result = cos.GetObject(req, &resp);
  std::cout << "===================GetObjectResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
  std::cout << os.str() << std::endl;
}

// 多线程下载对象
void MultiGetObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name,
                    const std::string& file_path) {
  qcloud_cos::MultiGetObjectReq req(bucket_name, object_name, file_path);
  qcloud_cos::MultiGetObjectResp resp;

  qcloud_cos::CosResult result = cos.MultiGetObject(req, &resp);
  std::cout << "===================GetObjectResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

// 多线程限速下载对象
void MultiGetObjectLimitTraffic(qcloud_cos::CosAPI& cos,
                                const std::string& bucket_name,
                                const std::string& object_name,
                                const std::string& file_path,
                                const uint64_t traffic_limit) {
  qcloud_cos::MultiGetObjectReq req(bucket_name, object_name, file_path);
  req.SetTrafficLimit(traffic_limit);
  qcloud_cos::MultiGetObjectResp resp;
  qcloud_cos::CosResult result = cos.MultiGetObject(req, &resp);
  std::cout << "===================MultiGetObjectLimitTrafficResponse=========="
               "==========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

// 初始化分块上传
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

// 上传分块
void UploadPartData(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name,
                    const std::string& upload_id, std::fstream& is,
                    uint64_t number, std::string* etag) {
  qcloud_cos::UploadPartDataReq req(bucket_name, object_name, upload_id, is);
  req.SetPartNumber(number);
  qcloud_cos::UploadPartDataResp resp;
  qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);
  *etag = resp.GetEtag();

  std::cout << "======================UploadPartData=====================";
  PrintResult(result, resp);
  std::cout << "=========================================================";
}

// 限速上传分块
void UploadPartDataLimitTraffic(qcloud_cos::CosAPI& cos,
                                const std::string& bucket_name,
                                const std::string& object_name,
                                const std::string& upload_id, std::fstream& is,
                                uint64_t number, std::string* etag,
                                const uint64_t traffic_limit) {
  qcloud_cos::UploadPartDataReq req(bucket_name, object_name, upload_id, is);
  req.SetPartNumber(number);
  req.SetTrafficLimit(traffic_limit);
  qcloud_cos::UploadPartDataResp resp;
  qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);
  *etag = resp.GetEtag();

  std::cout << "======================UploadPartDataLimitTrafficResponse======="
               "==============";
  PrintResult(result, resp);
  std::cout << "=========================================================";
}

void AbortMultiUpload(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                      const std::string& object_name,
                      const std::string& upload_id) {
  qcloud_cos::AbortMultiUploadReq req(bucket_name, object_name, upload_id);
  qcloud_cos::AbortMultiUploadResp resp;
  qcloud_cos::CosResult result = cos.AbortMultiUpload(req, &resp);
  std::cout << "======================AbortUploadPart=====================";
  PrintResult(result, resp);
  std::cout << "=========================================================";
}

void CompleteMultiUpload(qcloud_cos::CosAPI& cos,
                         const std::string& bucket_name,
                         const std::string& object_name,
                         const std::string& upload_id,
                         const std::vector<std::string>& etags,
                         const std::vector<uint64_t>& numbers) {
  qcloud_cos::CompleteMultiUploadReq req(bucket_name, object_name, upload_id);
  qcloud_cos::CompleteMultiUploadResp resp;
  req.AddParam("versionId", "MTg0NDY3NDI1NjQ4NjUyMjQ1MTA");
  req.SetEtags(etags);
  req.SetPartNumbers(numbers);

  qcloud_cos::CosResult result = cos.CompleteMultiUpload(req, &resp);

  std::cout << "===================Complete============================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 多线程上传对象
void MultiUploadObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                       const std::string& object_name,
                       const std::string& local_file) {
  qcloud_cos::MultiPutObjectReq req(bucket_name, object_name, local_file);
  req.SetRecvTimeoutInms(1000 * 60);
  qcloud_cos::MultiPutObjectResp resp;
  qcloud_cos::CosResult result = cos.MultiPutObject(req, &resp);

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
  std::cout
      << "===================MultiUploadObject============================="
      << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

#ifdef _WIN32
// 上传宽字符文件
void MultiUploadObjectWideChar(qcloud_cos::CosAPI& cos,
                               const std::string& bucket_name) {
  std::string local_file = "./안녕하십니까.mp4";
  std::string object_name = "안녕하십니까.mp4";
  qcloud_cos::MultiPutObjectReq req(bucket_name, object_name, local_file);
  req.SetWideCharPath();  // 需要调用该函数
  req.SetRecvTimeoutInms(1000 * 60);
  qcloud_cos::MultiPutObjectResp resp;
  qcloud_cos::CosResult result = cos.MultiPutObject(req, &resp);

  if (result.IsSucc()) {
    std::cout << "MultiUploadObjectWideChar Succ." << std::endl;
    std::cout << resp.GetLocation() << std::endl;
    std::cout << resp.GetKey() << std::endl;
    std::cout << resp.GetBucket() << std::endl;
    std::cout << resp.GetEtag() << std::endl;
  } else {
    std::cout << "MultiUploadObjectWideChar Fail." << std::endl;
  }
  std::cout << "===================MultiUploadObjectWideChar==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}
#endif

//限速多线程上传
void MultiUploadObjectLimitTraffic(qcloud_cos::CosAPI& cos,
                                   const std::string& bucket_name,
                                   const std::string& object_name,
                                   const std::string& local_file,
                                   const uint64_t traffic_limit) {
  qcloud_cos::MultiPutObjectReq req(bucket_name, object_name, local_file);
  req.SetTrafficLimit(traffic_limit);
  req.SetRecvTimeoutInms(1000 * 60);
  qcloud_cos::MultiPutObjectResp resp;
  qcloud_cos::CosResult result = cos.MultiPutObject(req, &resp);

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
  std::cout << "===================MultiUpload============================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
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
  std::cout
      << "===================================================================="
      << std::endl;
}

void PutObjectACL(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                  const std::string& object_name) {
  // 1 设置ACL配置(通过Body,
  // 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
  {
    qcloud_cos::PutObjectACLReq req(bucket_name, object_name);
    qcloud_cos::Owner owner = {"qcs::cam::uin/491107627:uin/491107627",
                               "qcs::cam::uin/491107627:uin/491107627"};
    qcloud_cos::Grant grant;
    req.SetOwner(owner);
    grant.m_grantee.m_type = "Group";
    grant.m_grantee.m_uri = "http://cam.qcloud.com/groups/global/AllUsers";
    grant.m_perm = "READ";
    req.AddAccessControlList(grant);

    qcloud_cos::PutObjectACLResp resp;
    qcloud_cos::CosResult result = cos.PutObjectACL(req, &resp);
    std::cout << "===================PutObjectACLResponse====================="
              << std::endl;
    PrintResult(result, resp);
    std::cout << "============================================================="
                 "======="
              << std::endl;
  }

  // 2 设置ACL配置(通过Header,
  // 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
  {
    qcloud_cos::PutObjectACLReq req(bucket_name, object_name);
    req.SetXCosAcl("public-read-write");

    qcloud_cos::PutObjectACLResp resp;
    qcloud_cos::CosResult result = cos.PutObjectACL(req, &resp);
    std::cout << "===================PutObjectACLResponse====================="
              << std::endl;
    PrintResult(result, resp);
    std::cout << "============================================================="
                 "======="
              << std::endl;
  }
}

void GetObjectACL(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                  const std::string& object_name) {
  qcloud_cos::GetObjectACLReq req(bucket_name, object_name);
  qcloud_cos::GetObjectACLResp resp;
  qcloud_cos::CosResult result = cos.GetObjectACL(req, &resp);

  std::cout << "===================GetObjectACLResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void PutObjectCopy(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                   const std::string& object_name, const std::string& source) {
  qcloud_cos::PutObjectCopyReq req(bucket_name, object_name);
  req.SetXCosCopySource(source);
  qcloud_cos::PutObjectCopyResp resp;
  qcloud_cos::CosResult result = cos.PutObjectCopy(req, &resp);

  std::cout << "===================PutObjectCopyResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

void DeleteObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                  const std::string& object_name) {
  qcloud_cos::DeleteObjectReq req(bucket_name, object_name);
  qcloud_cos::DeleteObjectResp resp;
  qcloud_cos::CosResult result = cos.DeleteObject(req, &resp);

  std::cout << "===================DeleteObjectResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}

void DeleteObjects(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                   const std::vector<ObjectVersionPair>& objects) {
  qcloud_cos::DeleteObjectsReq req(bucket_name, objects);
  qcloud_cos::DeleteObjectsResp resp;
  qcloud_cos::CosResult result = cos.DeleteObjects(req, &resp);

  std::cout << "===================DeleteObjectsResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
  std::cout << "Resp body=[" << resp.DebugString() << "]" << std::endl;
}

void UploadPartCopy(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name,
                    const std::string& upload_id, const std::string& source,
                    const std::string& range, uint64_t number,
                    std::string* etag) {
  qcloud_cos::UploadPartCopyDataReq req(bucket_name, object_name, upload_id,
                                        number);
  req.SetXCosCopySource(source);
  req.SetXCosCopySourceRange(range);
  qcloud_cos::UploadPartCopyDataResp resp;
  qcloud_cos::CosResult result = cos.UploadPartCopyData(req, &resp);

  std::cout << "===================UploadPartCopyDataResp====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;

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
  std::cout
      << "===================================================================="
      << std::endl;
}

void ListMultipartUpload(qcloud_cos::CosAPI& cos,
                         const std::string& bucket_name) {
  qcloud_cos::ListMultipartUploadReq req(bucket_name);
  qcloud_cos::ListMultipartUploadResp resp;
  qcloud_cos::CosResult result = cos.ListMultipartUpload(req, &resp);
  std::vector<Upload> rst = resp.GetUpload();

  for (std::vector<qcloud_cos::Upload>::const_iterator itr = rst.begin();
       itr != rst.end(); ++itr) {
    const qcloud_cos::Upload& upload = *itr;
    std::cout << "key = " << upload.m_key << ", uploadid= " << upload.m_uploadid
              << ", storagen class = " << upload.m_storage_class
              << ", m_initiated= " << upload.m_initiated << std::endl;
  }

  std::cout << "===================ListMultipartUpload====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
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

  std::cout
      << "===================PutBucketLoggingResponse====================="
      << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 获取bucket日志配置
void GetBucketLogging(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::GetBucketLoggingReq req(bucket_name);
  qcloud_cos::GetBucketLoggingResp resp;
  qcloud_cos::CosResult result = cos.GetBucketLogging(req, &resp);

  std::cout
      << "===================GetBucketLoggingResponse====================="
      << std::endl;

  const qcloud_cos::LoggingEnabled loggingenabled = resp.GetLoggingEnabled();
  std::cout << "TargetBucket = " << loggingenabled.GetTargetBucket()
            << std::endl;
  std::cout << "TargetPrefix = " << loggingenabled.GetTargetPrefix()
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 设置存储桶绑定自定义域名
void PutBucketDomain(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::PutBucketDomainReq req(bucket_name);
  DomainRule rules;
  rules.SetStatus("ENABLED");
  rules.SetName("www.abc.com");
  rules.SetType("REST");
  // rules.SetForcedReplacement();
  req.SetDomainRule(rules);
  qcloud_cos::PutBucketDomainResp resp;
  qcloud_cos::CosResult result = cos.PutBucketDomain(req, &resp);

  std::cout << "===================PutBucketDomainResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 获取存储桶绑定自定义域名
void GetBucketDomain(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::GetBucketDomainReq req(bucket_name);
  qcloud_cos::GetBucketDomainResp resp;
  qcloud_cos::CosResult result = cos.GetBucketDomain(req, &resp);

  std::cout << "===================GetBucketDomainResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 为存储桶配置静态网站
void PutBucketWebsite(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::PutBucketWebsiteReq req(bucket_name);
  qcloud_cos::PutBucketWebsiteResp resp;

  req.SetSuffix("index.xml");  //必选项
  req.SetProtocol("https");
  req.SetKey("Error.html");

  //设置重定向规则，最多设置100条

  //设置第一条规则
  qcloud_cos::RoutingRule routerule1;
  qcloud_cos::Condition temp_condtion1;
  temp_condtion1.SetHttpErrorCodeReturnedEquals(404);  //需要设置，默认404
  routerule1.SetCondition(temp_condtion1);
  qcloud_cos::Redirect temp_redirect1;
  temp_redirect1.SetProtocol("https");
  temp_redirect1.SetReplaceKeyWith("404.htmp");
  routerule1.SetRedirect(temp_redirect1);

  //设置第二条规则
  qcloud_cos::RoutingRule routerule2;
  qcloud_cos::Condition temp_condtion2;
  temp_condtion2.SetHttpErrorCodeReturnedEquals(403);  //需要设置，默认404
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
  std::cout << "===================PutBucketWebsite====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 获取与存储桶关联的静态网站配置信息
void GetBucketWebsite(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::GetBucketWebsiteReq req(bucket_name);
  qcloud_cos::GetBucketWebsiteResp resp;

  qcloud_cos::CosResult result = cos.GetBucketWebsite(req, &resp);
  std::cout << "===================GetBucketWebsite====================="
            << std::endl;

  std::cout << "Suffix:" << resp.GetSuffix() << std::endl;
  std::cout << "Protocol:" << resp.GetProtocol() << std::endl;
  std::cout << "ErrorDocument:" << resp.GetKey() << std::endl;

  std::vector<RoutingRule> routingrules = resp.GetRoutingRules();
  std::vector<RoutingRule>::iterator it = routingrules.begin();
  for (; it != routingrules.end(); ++it) {
    const Condition& condition = it->GetCondition();
    std::cout << "condition httpcode"
              << condition.GetHttpErrorCodeReturnedEquals() << std::endl;
    std::cout << "condition KeyPrefixEquals:" << condition.GetKeyPrefixEquals()
              << std::endl;

    const Redirect& redirect = it->GetRedirect();
    std::cout << "redirect Protocol:" << redirect.GetProtocol() << std::endl;
    std::cout << "redirect ReplaceKeyWith" << redirect.GetReplaceKeyWith()
              << std::endl;
    std::cout << "redirect ReplaceKeyPrefixWith"
              << redirect.GetReplaceKeyPrefixWith() << std::endl;

    std::cout << std::endl;
  }

  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 删除存储桶中的静态网站配置
void DeleteBucketWebsite(qcloud_cos::CosAPI& cos,
                         const std::string& bucket_name) {
  qcloud_cos::DeleteBucketWebsiteReq req(bucket_name);
  qcloud_cos::DeleteBucketWebsiteResp resp;

  qcloud_cos::CosResult result = cos.DeleteBucketWebsite(req, &resp);
  std::cout << "===================DeleteBucketWebsite====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 为已存在的 Bucket 设置标签(Tag)
void PutBucketTagging(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                      std::vector<Tag>& tagset) {
  qcloud_cos::PutBucketTaggingReq req(bucket_name);
  qcloud_cos::PutBucketTaggingResp resp;
  req.SetTagSet(tagset);

  qcloud_cos::CosResult result = cos.PutBucketTagging(req, &resp);
  std::cout << "===================PutBucketTagging====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

//用于查询指定存储桶下已有的存储桶标签
void GetBucketTagging(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::GetBucketTaggingReq req(bucket_name);
  qcloud_cos::GetBucketTaggingResp resp;

  qcloud_cos::CosResult result = cos.GetBucketTagging(req, &resp);
  std::cout << "===================GetBucketTagging====================="
            << std::endl;
  std::vector<Tag> tagset = resp.GetTagSet();
  for (std::vector<Tag>::iterator it = tagset.begin(); it != tagset.end();
       ++it) {
    std::cout << it->GetKey() << ":" << it->GetValue() << std::endl;
  }
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}
//删除指定存储桶下已有的存储桶标签
void DeleteBucketTagging(qcloud_cos::CosAPI& cos,
                         const std::string& bucket_name) {
  qcloud_cos::DeleteBucketTaggingReq req(bucket_name);
  qcloud_cos::DeleteBucketTaggingResp resp;

  qcloud_cos::CosResult result = cos.DeleteBucketTagging(req, &resp);
  std::cout << "===================DeleteBucketTagging====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}
// 在存储桶中创建清单任务
void PutBucketInventory(qcloud_cos::CosAPI& cos,
                        const std::string& bucket_name) {
  qcloud_cos::PutBucketInventoryReq req(bucket_name);
  // req.SetId("list3");

  // COSBucketDestination destination;
  // destination.SetFormat("CSV");
  // destination.SetAccountId("100010974959");

  // destination.SetBucket("qcs::cos:ap-guangzhou::loggtest-1234567890");
  // destination.SetPrefix("/");
  // destination.SetEncryption(true);

  // OptionalFields fields;
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
  std::cout << "===================PutBucketinventory====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 用于查询存储桶中用户的清单任务信息
void GetBucketInventory(qcloud_cos::CosAPI& cos,
                        const std::string& bucket_name) {
  qcloud_cos::GetBucketInventoryReq req(bucket_name);
  qcloud_cos::GetBucketInventoryResp resp;
  req.SetId("list2");

  qcloud_cos::CosResult result = cos.GetBucketInventory(req, &resp);
  // std::cout << "===================GetBucketinventory====================="
  // << std::endl;
  //
  // const Inventory inventory = resp.GetInventory();
  // std::cout << "inventory isenabled:" << inventory.GetIsEnable() <<
  // std::endl; std::cout << "inventory IncludedObjectVersions:" <<
  // inventory.GetIncludedObjectVersions() << std::endl; std::cout << "inventory
  // filter:" << inventory.GetFilter() << std::endl; std::cout << "inventory
  // id:"
  // << inventory.GetId() << std::endl; std::cout << "inventory Frequency:" <<
  // inventory.GetFrequency() << std::endl;

  // std:: cout << "===================================" << std::endl;
  // COSBucketDestination destination = inventory.GetCOSBucketDestination();
  // std::cout << "destination Format:" << destination.GetFormat() << std::endl;
  // std::cout << "destination AccountID:" << destination.GetAccountId() <<
  // std::endl; std::cout << "destination Bucket:" << destination.GetBucket() <<
  // std::endl; std::cout << "destination Encryption:" <<
  // destination.GetEncryption() << std::endl;

  // std:: cout << "===================================" << std::endl;
  //
  // OptionalFields fields = inventory.GetOptionalFields();

  // std::cout << "fields Size:" << fields.GetIsSize() << std::endl;
  // std::cout << "fields LastModified:" << fields.GetIsLastModified() <<
  // std::endl; std::cout << "fields StorageClass:" <<
  // fields.GetIsStorageClass()
  // << std::endl; std::cout << "fields Region:" <<
  // fields.GetIsMultipartUploaded() << std::endl; std::cout << "fields
  // ReplicationStatus:" << fields.GetIsReplicationStatus() << std::endl;
  // std::cout << "fields Tag:" << 	fields.GetIsETag() << std::endl;

  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 请求返回一个存储桶中的所有清单任务
void ListBucketInventoryConfigurations(qcloud_cos::CosAPI& cos,
                                       const std::string& bucket_name) {
  qcloud_cos::ListBucketInventoryConfigurationsReq req(bucket_name);
  qcloud_cos::ListBucketInventoryConfigurationsResp resp;

  qcloud_cos::CosResult result =
      cos.ListBucketInventoryConfigurations(req, &resp);
  // std::cout <<
  // "===================ListBucketInventoryConfigurations====================="
  // << std::endl;
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
  //    std:: cout << "==============Inventory=============================" <<
  //    std::endl; std::cout << "inventory id:" << itr->GetId() << std::endl;
  //    std::cout << "inventory isenabled:" <<  itr->GetIsEnable() << std::endl;
  //    std::cout << "inventory IncludedObjectVersions:" <<
  //    itr->GetIncludedObjectVersions() << std::endl; std::cout << "inventory
  //    filter:" << itr->GetFilter() << std::endl; std::cout << "inventory
  //    Frequency:" << 	itr->GetFrequency() << std::endl;

  //    std:: cout << "==============GetCOSBucketDestination=================="
  //    << std::endl; COSBucketDestination destination =
  //    itr->GetCOSBucketDestination(); std::cout << "destination Format:" <<
  //    destination.GetFormat() << std::endl; std::cout << "destination
  //    AccountID:" << destination.GetAccountId() << std::endl; std::cout <<
  //    "destination Bucket:" << destination.GetBucket() << std::endl; std::cout
  //    << "destination Encryption:" << destination.GetEncryption() <<
  //    std::endl;
  //
  //
  //    std:: cout << "===================OptionalFields======================"
  //    << std::endl; OptionalFields fields = itr->GetOptionalFields();
  //    std::cout << "fields Size:" << fields.GetIsSize() << std::endl;
  //    std::cout << "fields LastModified:" << fields.GetIsLastModified() <<
  //    std::endl; std::cout << "fields StorageClass:" <<
  //    fields.GetIsStorageClass() << std::endl; std::cout << "fields Region:"
  //    << fields.GetIsMultipartUploaded() << std::endl; std::cout << "fields
  //    ReplicationStatus:" << fields.GetIsReplicationStatus() << std::endl;
  //    std::cout << "fields Tag:" << 	fields.GetIsETag() << std::endl;

  // }
  // std::cout << "===============ListBucketInventoryConfigurations
  // end================" << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 用于删除存储桶中指定的清单任务.
void DeleteBucketInventory(qcloud_cos::CosAPI& cos,
                           const std::string& bucket_name) {
  qcloud_cos::DeleteBucketInventoryReq req(bucket_name);
  qcloud_cos::DeleteBucketInventoryResp resp;
  req.SetId("list");
  qcloud_cos::CosResult result = cos.DeleteBucketInventory(req, &resp);
  std::cout << "===================DeleteBucketinventory====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 配置存储桶referer
void PutBucketReferer(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::PutBucketRefererReq req(bucket_name);
  qcloud_cos::PutBucketRefererResp resp;
  req.SetStatus("Enabled");
  req.SetRefererType("White-List");
  req.AddDomain("test1.com");
  req.AddDomain("test2.com");
  qcloud_cos::CosResult result = cos.PutBucketReferer(req, &resp);
  std::cout << "===================PutBucketReferer====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 获取存储桶referer
void GetBucketReferer(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::GetBucketRefererReq req(bucket_name);
  qcloud_cos::GetBucketRefererResp resp;
  qcloud_cos::CosResult result = cos.GetBucketReferer(req, &resp);
  std::cout << "===================GetBucketReferer====================="
            << std::endl;
  PrintResult(result, resp);
  if (result.IsSucc()) {
    std::cout << "Status:" << resp.GetStatus() << std::endl;
    std::cout << "RefererType:" << resp.GetRefererType() << std::endl;
    for (auto& domain : resp.GetDomainList()) {
      std::cout << "Domain:" << domain << std::endl;
    }
    std::cout << "EmptyReferConfiguration:" << resp.GetEmptyReferConf()
              << std::endl;
  }
  std::cout
      << "===================================================================="
      << std::endl;
}

// 创建直播通道
void CreateLiveChannel(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                       const std::string& channel_name) {
  qcloud_cos::PutLiveChannelReq req(bucket_name, channel_name);
  qcloud_cos::PutLiveChannelResp resp;
  std::map<std::string, std::string> url_params;
  url_params.insert(std::make_pair("a", "b"));
  LiveChannelConfiguration config("test", "Enabled", "HLS", 5, 10,
                                  "playlist.m3u8");
  req.SetLiveChannelConfig(config);
  req.SetUrlParams(url_params);
  req.SetExpire(1000);
  qcloud_cos::CosResult result = cos.PutLiveChannel(req, &resp);
  std::cout << "===================CreateLiveChannel====================="
            << std::endl;
  PrintResult(result, resp);
  if (result.IsSucc()) {
    std::cout << "PlayUrl:" << resp.GetPlayUrl() << std::endl;
    std::cout << "PublishUrl:" << resp.GetPublishUrl() << std::endl;
  }
  std::cout
      << "===================================================================="
      << std::endl;
}

// 获取直播通道配置
void GetLiveChannel(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& channel_name) {
  qcloud_cos::GetLiveChannelReq req(bucket_name, channel_name);
  qcloud_cos::GetLiveChannelResp resp;
  qcloud_cos::CosResult result = cos.GetLiveChannel(req, &resp);
  std::cout << "===================GetLiveChannel====================="
            << std::endl;
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
  std::cout
      << "===================================================================="
      << std::endl;
}

// 获取通道推流URL
void GetRtmpSignedPublishUrl(qcloud_cos::CosAPI& cos,
                             const std::string& bucket_name,
                             const std::string& channel_name) {
  std::cout << "===================GetRtmpSignedPublishUrl====================="
            << std::endl;
  std::cout << "PublishUrl:"
            << cos.GetRtmpSignedPublishUrl(bucket_name, channel_name, 3600,
                                           std::map<std::string, std::string>())
            << std::endl;
  std::cout
      << "===================================================================="
      << std::endl;
}

// 修改直播通道开关
void PutLiveChannelSwitch(qcloud_cos::CosAPI& cos,
                          const std::string& bucket_name,
                          const std::string& channel_name) {
  qcloud_cos::PutLiveChannelSwitchReq req(bucket_name, channel_name);
  qcloud_cos::PutLiveChannelSwitchResp resp;
  qcloud_cos::CosResult result;

  std::cout
      << "===================PutLiveChannelSwitch disabled====================="
      << std::endl;
  req.SetDisabled();
  result = cos.PutLiveChannelSwitch(req, &resp);
  PrintResult(result, resp);

  std::cout
      << "===================PutLiveChannelSwitch enabled====================="
      << std::endl;
  req.SetEnabled();
  result = cos.PutLiveChannelSwitch(req, &resp);
  PrintResult(result, resp);

  std::cout
      << "===================================================================="
      << std::endl;
}

// 获取直播通道推流历史
void GetLiveChannelHistory(qcloud_cos::CosAPI& cos,
                           const std::string& bucket_name,
                           const std::string& channel_name) {
  qcloud_cos::GetLiveChannelHistoryReq req(bucket_name, channel_name);
  qcloud_cos::GetLiveChannelHistoryResp resp;
  qcloud_cos::CosResult result = cos.GetLiveChannelHistory(req, &resp);
  std::cout << "===================GetLiveChannelHistory====================="
            << std::endl;
  PrintResult(result, resp);
  if (result.IsSucc()) {
    const std::vector<LiveRecord>& chan_history = resp.GetChanHistory();
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
  std::cout
      << "===================================================================="
      << std::endl;
}

// 获取直播通道推流状态
void GetLiveChannelStatus(qcloud_cos::CosAPI& cos,
                          const std::string& bucket_name,
                          const std::string& channel_name) {
  qcloud_cos::GetLiveChannelStatusReq req(bucket_name, channel_name);
  qcloud_cos::GetLiveChannelStatusResp resp;
  qcloud_cos::CosResult result = cos.GetLiveChannelStatus(req, &resp);
  std::cout << "===================GetLiveChannelStatus====================="
            << std::endl;
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
  std::cout
      << "===================================================================="
      << std::endl;
}

// 下载直播通道点播播放列表
void GetLiveChannelVodPlaylist(qcloud_cos::CosAPI& cos,
                               const std::string& bucket_name,
                               const std::string& channel_name) {
  qcloud_cos::GetLiveChannelVodPlaylistReq req(bucket_name, channel_name);
  qcloud_cos::GetLiveChannelVodPlaylistResp resp;
  req.SetTime(time(NULL) - 10000, time(NULL));
  qcloud_cos::CosResult result = cos.GetLiveChannelVodPlaylist(req, &resp);
  std::cout
      << "===================GetLiveChannelVodPlaylist====================="
      << std::endl;
  PrintResult(result, resp);
  if (result.IsSucc()) {
    resp.WriteResultToFile("./playlist.m3u8");
  }
  std::cout
      << "===================================================================="
      << std::endl;
}

// 为直播通道产生点播播放列表
void PostLiveChannelVodPlaylist(qcloud_cos::CosAPI& cos,
                                const std::string& bucket_name,
                                const std::string& channel_name) {
  qcloud_cos::PostLiveChannelVodPlaylistReq req(bucket_name, channel_name);
  qcloud_cos::PostLiveChannelVodPlaylistResp resp;
  req.SetTime(time(NULL) - 10000, time(NULL));
  req.SetPlaylistName("newplaylist.m3u8");
  qcloud_cos::CosResult result = cos.PostLiveChannelVodPlaylist(req, &resp);
  std::cout
      << "===================PostLiveChannelVodPlaylist====================="
      << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 删除直播通道
void DeleteLiveChannel(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                       const std::string& channel_name) {
  qcloud_cos::DeleteLiveChannelReq req(bucket_name, channel_name);
  qcloud_cos::DeleteLiveChannelResp resp;
  qcloud_cos::CosResult result = cos.DeleteLiveChannel(req, &resp);
  std::cout << "===================DeleteLiveChannel====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout
      << "===================================================================="
      << std::endl;
}

// 获取直播通道列表
void ListLiveChannel(qcloud_cos::CosAPI& cos, const std::string& bucket_name) {
  qcloud_cos::ListLiveChannelReq req(bucket_name);
  qcloud_cos::ListLiveChannelResp resp;
  ListLiveChannelResult list_result;
  qcloud_cos::CosResult result;
  std::cout << "===================ListLiveChannel====================="
            << std::endl;
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
      std::vector<LiveChannel>::const_iterator it =
          list_result.m_channels.begin();
      for (; it != list_result.m_channels.end(); ++it) {
        oss << "Name:" << it->m_name << ", LastModified:" << it->m_last_modified
            << std::endl;
      }
      std::cout << oss.str() << std::endl;
      resp.ClearResult();
    }
  } while (list_result.m_is_truncated == "true" && result.IsSucc());
  std::cout
      << "===================================================================="
      << std::endl;
}

void TestIntelligentTiering(qcloud_cos::CosAPI& cos,
                            const std::string& bucket_name) {
  {
    qcloud_cos::PutBucketIntelligentTieringReq req(bucket_name);
    qcloud_cos::PutBucketIntelligentTieringResp resp;
    qcloud_cos::CosResult result;
    std::cout << "===================PutIntelligentTiering====================="
              << std::endl;
    req.SetStatus(true);
    req.SetDays(60);
    result = cos.PutBucketIntelligentTiering(req, &resp);
    PrintResult(result, resp);
    std::cout << "============================================================="
                 "======="
              << std::endl;
  }

  {
    qcloud_cos::GetBucketIntelligentTieringReq req(bucket_name);
    qcloud_cos::GetBucketIntelligentTieringResp resp;
    qcloud_cos::CosResult result;
    std::cout << "===================GetIntelligentTiering====================="
              << std::endl;
    result = cos.GetBucketIntelligentTiering(req, &resp);
    PrintResult(result, resp);
    if (result.IsSucc()) {
      std::cout << "Status:" << resp.GetStatus() << std::endl;
      std::cout << "Days:" << resp.GetDays() << std::endl;
      std::cout << "RequestFrequent:" << resp.GetRequestFrequent() << std::endl;
    }
    std::cout << "============================================================="
                 "======="
              << std::endl;
  }
}

static void ResumableGetObject(qcloud_cos::CosAPI& cos,
                               const std::string& bucket_name,
                               const std::string& object_name,
                               const std::string& file_path) {
  qcloud_cos::GetObjectByFileReq req(bucket_name, object_name, file_path);
  qcloud_cos::GetObjectByFileResp resp;

  CosResult resule = cos.ResumableGetObject(req, &resp);
  if (resule.IsSucc()) {
    std::cout << "ResumableGetObject Succ." << std::endl;
  } else {
    std::cout << "ResumableGetObject Fail." << std::endl;
  }
  std::cout
      << "===================ResumableGetObject============================="
      << std::endl;
  PrintResult(resule, resp);
  std::cout << "========================================================"
            << std::endl;
}

static void ProgressCallback(uint64_t transferred_size, uint64_t total_size,
                             void* user_data) {
  qcloud_cos::ObjectReq* req = static_cast<qcloud_cos::ObjectReq*>(user_data);
  if (0 == transferred_size % 1048576) {
    std::cout << "ObjectName:" << req->GetObjectName()
              << ", TranferedSize:" << transferred_size
              << ",TotalSize:" << total_size << std::endl;
  }
}

static void MultiGetObjectAsyncDoneCallback(const SharedAsyncContext& context,
                                            void* user_data) {
  UNUSED_PARAM(user_data)
  std::cout << "MultiGetObjectAsyncDoneCallback, BucketName:"
            << context->GetBucketName()
            << ", ObjectName:" << context->GetObjectName()
            << ", LocalFile:" << context->GetLocalFilePath() << std::endl;
  // qcloud_cos::MultiGetObjectReq对应的响应为qcloud_cos::GetObjectByFileResp
  if (context->GetResult().IsSucc()) {
    // 获取响应
    std::cout << "AsyncMultiGetObject succeed" << std::endl;
    std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
    AsyncResp resp = context->GetAsyncResp();
    std::cout << "ETag:" << resp.GetEtag() << std::endl;
    std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
  } else {
    std::cout << "AsyncMultiGetObject failed" << std::endl;
    std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
  }
}
//  异步下载对象,支持更新下载进度
void AsyncMultiGetObject(qcloud_cos::CosAPI& cos,
                         const std::string& bucket_name,
                         const std::string& object_name,
                         const std::string& file_path) {
  qcloud_cos::AsyncMultiGetObjectReq req(bucket_name, object_name, file_path);
  // 设置进度回调
  req.SetTransferProgressCallback(&ProgressCallback);
  // 设置状态回调
  req.SetDoneCallback(&MultiGetObjectAsyncDoneCallback);
  // 设置私有数据
  req.SetUserData(&req);

  // 开始下载
  SharedAsyncContext context = cos.AsyncMultiGetObject(req);

  // 等待下载结束
  context->WaitUntilFinish();

  // 检查结果
  if (context->GetResult().IsSucc()) {
    // 获取响应
    std::cout << "AsyncMultiGetObject succeed" << std::endl;
    std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
    AsyncResp resp = context->GetAsyncResp();
    std::cout << "ETag:" << resp.GetEtag() << std::endl;
    std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
  } else {
    std::cout << "AsyncMultiGetObject failed" << std::endl;
    std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
  }
  //context->WaitUntilFinish()阻塞完毕的逻辑是：异步线程在下载时文件落盘完毕。
  //此时您的主线程如果立即结束，异步线程可能正在进行资源释放、结束线程等过程。此时可能会产生crash。
  //此问题并不会对文件的下载造成影响，仅仅是体验上可能会出现意料之外的崩溃信息。
  //可以采用此策略避免crash。
  //或者可以采用下方AsyncMultiGetObjectWithTaskManager函数中的方式，使用能透传TaskManager的接口，使用join的方式避免crash。
  std::this_thread::sleep_for(std::chrono::seconds(1));
}

//  异步下载对象,支持更新下载进度
//  回传taskManager对象,可用使用它等待异步线程彻底结束
void AsyncMultiGetObjectWithTaskManager(qcloud_cos::CosAPI& cos,
                         const std::string& bucket_name,
                         const std::string& object_name,
                         const std::string& file_path) {
  qcloud_cos::AsyncMultiGetObjectReq req(bucket_name, object_name, file_path);
  // 设置进度回调
  req.SetTransferProgressCallback(&ProgressCallback);
  // 设置状态回调
  req.SetDoneCallback(&MultiGetObjectAsyncDoneCallback);
  // 设置私有数据
  req.SetUserData(&req);

  // 开始下载
  Poco::TaskManager* taskManager;
  qcloud_cos::SharedAsyncContext context = cos.AsyncMultiGetObject(req,taskManager);

  // 等待下载结束
  context->WaitUntilFinish();

  // 检查结果
  if (context->GetResult().IsSucc()) {
    // 获取响应
    std::cout << "AsyncMultiGetObject succeed" << std::endl;
    std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
    AsyncResp resp = context->GetAsyncResp();
    std::cout << "ETag:" << resp.GetEtag() << std::endl;
    std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
  } else {
    std::cout << "AsyncMultiGetObject failed" << std::endl;
    std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
  }

  //  使用taskManager等待异步线程彻底结束
  (*taskManager).joinAll();
}

static void MultiPutObjectAsyncDoneCallback(const SharedAsyncContext& context,
                                            void* user_data) {
  UNUSED_PARAM(user_data)
  std::cout << "MultiPutObjectAsyncDoneCallback, BucketName:"
            << context->GetBucketName()
            << ", ObjectName:" << context->GetObjectName()
            << ", LocalFile:" << context->GetLocalFilePath() << std::endl;
  if (context->GetResult().IsSucc()) {
    // 获取响应
    std::cout << "AsyncMultiPutObject succeed" << std::endl;
    std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
    AsyncResp resp = context->GetAsyncResp();
    //std::cout << "Location:" << resp.GetLocation() << std::endl;
    //std::cout << "Bucket:" << resp.GetBucket() << std::endl;
    //std::cout << "Key:" << resp.GetKey() << std::endl;
    std::cout << "ETag:" << resp.GetEtag() << std::endl;
    std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
  } else {
    std::cout << "AsyncMultiGetObject failed" << std::endl;
    std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
  }
}

// 异步多上传对象,支持更新上传进度
void AsyncMultiPutObject(qcloud_cos::CosAPI& cos,
                         const std::string& bucket_name,
                         const std::string& object_name,
                         const std::string& local_file) {
  qcloud_cos::AsyncMultiPutObjectReq req(bucket_name, object_name, local_file);
  req.SetRecvTimeoutInms(1000 * 60);
  // 设置上传进度回调
  req.SetTransferProgressCallback(&ProgressCallback);
  // 设置上传状态回调
  req.SetDoneCallback(&MultiPutObjectAsyncDoneCallback);
  // 设置私有数据
  req.SetUserData(&req);

  // 开始上传
  SharedAsyncContext context = cos.AsyncMultiPutObject(req);

  // 等待上传结束
  std::cout << "wait finish..." << std::endl;
  context->WaitUntilFinish();

  // 检查结果
  if (context->GetResult().IsSucc()) {
    // 获取响应
    std::cout << "AsyncMultiPutObject succeed" << std::endl;
    std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
    AsyncResp resp = context->GetAsyncResp();
    //std::cout << "Location:" << resp.GetLocation() << std::endl;
    //std::cout << "Bucket:" << resp.GetBucket() << std::endl;
    //std::cout << "Key:" << resp.GetKey() << std::endl;
    std::cout << "ETag:" << resp.GetEtag() << std::endl;
    std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
  } else {
    std::cout << "AsyncMultiPutObject failed" << std::endl;
    std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
  }
}

// 从目录上传文件
void PutObjectsFromDirectory(qcloud_cos::CosAPI& cos,
                             const std::string& bucket_name,
                             const std::string& directory_name) {
  PutObjectsByDirectoryReq req(bucket_name, directory_name);
  PutObjectsByDirectoryResp resp;
  CosResult result = cos.PutObjects(req, &resp);
  if (result.IsSucc()) {
    std::cout << "PutObjectsFromDirectory Succ." << std::endl;
    for (auto& r : resp.m_succ_put_objs) {
      std::cout << "file_name: " << r.m_file_name
                << ", object_name: " << r.m_object_name
                << ", ETag: " << r.m_cos_resp.GetEtag() << std::endl;
    }
  } else {
    std::cout << "PutObjectsFromDirectory Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
}

// 从目录上传文件到cos指定目录
void PutObjectsFromDirectoryToCosPath(qcloud_cos::CosAPI& cos,
                                      const std::string& bucket_name,
                                      const std::string& directory_name,
                                      const std::string& cos_path) {
  PutObjectsByDirectoryReq req(bucket_name, directory_name);
  req.SetCosPath(cos_path);
  PutObjectsByDirectoryResp resp;
  CosResult result = cos.PutObjects(req, &resp);
  if (result.IsSucc()) {
    std::cout << "PutObjectsFromDirectoryToCosPath Succ." << std::endl;
    for (auto& r : resp.m_succ_put_objs) {
      std::cout << "file_name: " << r.m_file_name
                << ", object_name: " << r.m_object_name
                << ", ETag: " << r.m_cos_resp.GetEtag() << std::endl;
    }
  } else {
    std::cout << "PutObjectsFromDirectoryToCosPath Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
}

// 创建目录
void PutDirectory(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                  const std::string& directory_name) {
  PutDirectoryReq req(bucket_name, directory_name);
  PutDirectoryResp resp;
  CosResult result = cos.PutDirectory(req, &resp);
  if (result.IsSucc()) {
    std::cout << "PutDirectory Succ." << std::endl;
  } else {
    std::cout << "PutDirectory Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
}

// 删除目录
void DeleteDirectory(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                     const std::string& directory_name) {
  DeleteObjectsByPrefixReq req(bucket_name, directory_name);
  DeleteObjectsByPrefixResp resp;
  CosResult result = cos.DeleteObjects(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DeleteDirectory Succ." << std::endl;
  } else {
    std::cout << "DeleteDirectory Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }

  std::cout << "Succ del objs:" << std::endl;
  for (auto& obj : resp.m_succ_del_objs) {
    std::cout << obj << std::endl;
  }
}

// 下载目录到本地
void DownloadFromDirectory(qcloud_cos::CosAPI& cos,
                           const std::string& bucket_name,
                           const std::string& server_dir_name,
                           const std::string& local_dir_name) {
  qcloud_cos::GetBucketReq get_bucket_req(bucket_name);
  get_bucket_req.SetPrefix(server_dir_name);
  qcloud_cos::CosResult get_bucket_result;
  bool is_truncated = false;

  do {
    qcloud_cos::GetBucketResp get_bucket_resp;
    get_bucket_result = cos.GetBucket(get_bucket_req, &get_bucket_resp);
    if (get_bucket_result.IsSucc()) {
      std::vector<Content> contents = get_bucket_resp.GetContents();
      for (auto& content : contents) {
        std::cout << "====process " << content.m_key << std::endl;
        std::string local_file_name = local_dir_name + content.m_key;

        if (StringUtil::StringEndsWith(local_file_name, "/")) {
          continue;
        }

        size_t found_dir = local_file_name.find_last_of("/");
        if (found_dir) {
          std::string dirname = local_file_name.substr(0, found_dir);
          char* p_dirname = const_cast<char*>(dirname.c_str());
          struct stat buffer;
          if (stat(p_dirname, &buffer) != 0) {
            std::cout << "====mkdir " << dirname << std::endl;
            std::string mkdir_cmd = "mkdir -p " + dirname;
            system(mkdir_cmd.c_str());
          }
        }

        GetObjectByFileReq get_req(bucket_name, content.m_key, local_file_name);
        GetObjectByFileResp get_resp;
        CosResult get_result = cos.GetObject(get_req, &get_resp);
        if (get_result.IsSucc()) {
          std::cout << "====download " << content.m_key << " to "
                    << local_file_name << " succeed" << std::endl;
        } else {
          std::cout << "====download " << content.m_key << " to "
                    << local_file_name << " failed" << std::endl;
        }
      }
      get_bucket_req.SetMarker(
          get_bucket_resp.GetNextMarker());  // 设置下次列举的起始key
      is_truncated = get_bucket_resp.IsTruncated();
    }
  } while (get_bucket_result.IsSucc() && is_truncated);
}

// 指定前缀删除
void DeleteObjectsByPrefix(qcloud_cos::CosAPI& cos,
                           const std::string& bucket_name,
                           const std::string& prefix) {
  DeleteObjectsByPrefixReq req(bucket_name, prefix);
  DeleteObjectsByPrefixResp resp;
  CosResult result = cos.DeleteObjects(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DeleteObjectsByPrefix Succ." << std::endl;
  } else {
    std::cout << "DeleteObjectsByPrefix Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }

  std::cout << "Succ del objs:" << std::endl;
  for (auto& obj : resp.m_succ_del_objs) {
    std::cout << obj << std::endl;
  }
}

// 移动桶内对象
void MoveObject(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                const std::string& src_object, const std::string& dst_object) {
  MoveObjectReq req(bucket_name, src_object, dst_object);
  CosResult result = cos.MoveObject(req);
  if (result.IsSucc()) {
    std::cout << "MoveObject Succ." << std::endl;
  } else {
    std::cout << "MoveObject Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
}

/*数据处理*/

// 图片缩放
void ImageThumbnail(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                    const std::string& object_name,
                    const std::string& local_file) {
  GetObjectByFileReq req(bucket_name, object_name, local_file);
  req.AddParam("imageMogr2/thumbnail/!50p", "");
  GetObjectByFileResp resp;
  CosResult result = cos.GetObject(req, &resp);
  if (result.IsSucc()) {
    std::cout << "ImageThumbnail Succ." << std::endl;
  } else {
    std::cout << "ImageThumbnail Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
}

// 上传时处理图片
void PutImage(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
              const std::string& object_name, const std::string& local_file) {
  PutImageByFileReq req(bucket_name, object_name, local_file);
  PutImageByFileResp resp;

  PicOperation pic_operation;
  PicRules rule;

  // 锐化
  rule.fileid = "/" + object_name + "_sharpen.jpg";
  rule.rule = "imageMogr2/sharpen/70";
  pic_operation.AddRule(rule);

  // 旋转
  rule.fileid = "/" + object_name + "_rotate.jpg";
  rule.rule = "imageMogr2/rotate/90";
  pic_operation.AddRule(rule);

  req.SetPicOperation(pic_operation);

  CosResult result = cos.PutImage(req, &resp);
  if (result.IsSucc()) {
    std::cout << "PutImage Succ." << std::endl;
    std::cout << "ProcessResult: " << resp.GetUploadResult().to_string()
              << std::endl;
  } else {
    std::cout << "PutImage Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================PutImage============================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 添加盲水印
void AddWatermark(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                  const std::string& object_name,
                  const std::string& local_file) {
  PutImageByFileReq req(bucket_name, object_name, local_file);
  PutImageByFileResp resp;

  PicOperation pic_operation;
  PicRules rule;

  // 添加盲水印
  rule.fileid = "/" + object_name + "_watermark.jpg";
  // std::string image = "http://" + bucket_name +
  // ".cos.ap-guangzhou.myqcloud.com/" + object_name + "_watermark.jpg";
  rule.rule =
      "watermark/3/type/3/text/" + CodecUtil::Base64Encode("test wartermart");
  pic_operation.AddRule(rule);
  req.SetPicOperation(pic_operation);

  CosResult result = cos.PutImage(req, &resp);
  if (result.IsSucc()) {
    std::cout << "PutImage Succ." << std::endl;
    std::cout << "ProcessResult: " << resp.GetUploadResult().to_string()
              << std::endl;
  } else {
    std::cout << "PutImage Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================PutImage============================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 下载图片时添加盲水印
// https://cloud.tencent.com/document/product/436/46782
void GetImageWithWatermark(qcloud_cos::CosAPI& cos,
                           const std::string& bucket_name,
                           const std::string& object_name,
                           const std::string& local_file) {
  GetObjectByFileReq req(bucket_name, object_name, local_file);
  GetObjectByFileResp resp;

  std::string watermark_text = "test";  // 盲水印文本
  std::string watermark_param =
      "watermark/3/type/3/text/" + CodecUtil::Base64Encode(watermark_text);
  req.AddParam(watermark_param, "");

  CosResult result = cos.GetObject(req, &resp);
  if (result.IsSucc()) {
    std::cout << "GetImageWithWatermark Succ." << std::endl;
  } else {
    std::cout << "GetImageWithWatermark Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
  std::cout
      << "===================GetImageWithWatermark============================="
      << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 提取盲水印
void ExtractWatermark(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                      const std::string& object_name,
                      const std::string& local_file) {
  PutImageByFileReq req(bucket_name, object_name, local_file);
  PutImageByFileResp resp;

  PicOperation pic_operation;
  PicRules rule;

  // 提取盲水印
  rule.fileid = "/" + object_name + "_watermark_extract.jpg";
  rule.rule = "watermark/4/type/3";
  pic_operation.AddRule(rule);
  req.SetPicOperation(pic_operation);

  CosResult result = cos.PutImage(req, &resp);
  if (result.IsSucc()) {
    std::cout << "PutImage Succ." << std::endl;
    std::cout << "ProcessResult: " << resp.GetUploadResult().to_string()
              << std::endl;
  } else {
    std::cout << "PutImage Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================PutImage============================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 云上处理图片
void CloudImageProcess(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                       const std::string& object_name) {
  CloudImageProcessReq req(bucket_name, object_name);
  CloudImageProcessResp resp;

  PicOperation pic_operation;
  PicRules rule;
  rule.fileid = "/" + object_name + "_thumbnail.jpg";
  rule.rule = "imageMogr2/thumbnail/!30p";
  pic_operation.AddRule(rule);

  rule.fileid = "/" + object_name + "_cut.jpg";
  rule.rule = "imageMogr2/cut/300x300";
  pic_operation.AddRule(rule);
  req.SetPicOperation(pic_operation);

  CosResult result = cos.CloudImageProcess(req, &resp);
  if (result.IsSucc()) {
    std::cout << "CloudImageProcess Succ." << std::endl;
    std::cout << "ProcessResult: " << resp.GetUploadResult().to_string()
              << std::endl;
  } else {
    std::cout << "CloudImageProcess Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout
      << "===================CloudImageProcess============================="
      << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 上传图片识别二维码
void PutQRcode(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
               const std::string& object_name, const std::string& local_file) {
  PutImageByFileReq req(bucket_name, object_name, local_file);
  PutImageByFileResp resp;

  PicOperation pic_operation;
  PicRules rule1;
  rule1.fileid = "/" + object_name + "_put_qrcode.jpg";
  rule1.rule = "QRcode/cover/1";
  pic_operation.AddRule(rule1);
  req.SetPicOperation(pic_operation);

  CosResult result = cos.PutImage(req, &resp);
  if (result.IsSucc()) {
    std::cout << "PutQRcode Succ." << std::endl;
    std::cout << "ProcessResult: " << resp.GetUploadResult().to_string()
              << std::endl;
  } else {
    std::cout << "PutQRcode Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================PutQRcode============================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 下载图片时识别二维码
void GetQRcode(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
               const std::string& object_name) {
  GetQRcodeReq req(bucket_name, object_name);
  GetQRcodeResp resp;

  CosResult result = cos.GetQRcode(req, &resp);
  if (result.IsSucc()) {
    std::cout << "GetQRcode Succ." << std::endl;
    std::cout << "Result: " << resp.GetResult().to_string() << std::endl;
  } else {
    std::cout << "GetQRcode Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================GetQRcode============================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 查询文档预览开通状态
// https://cloud.tencent.com/document/product/436/54057
void DescribeDocProcessBuckets(qcloud_cos::CosAPI& cos) {
  DescribeDocProcessBucketsReq req;
  DescribeDocProcessBucketsResp resp;

  req.SetRegions("ap-guangzhou");
  // 设置存储桶名称，以“,”分隔，支持多个存储桶，精确搜索
  // req.SetBucketNames("xxx");
  // 设置存储桶名称前缀，前缀搜索
  // req.SetBucketName("xxx");
  // 设置第几页
  // req.SetPageNumber();
  // 设置每页个数
  // req.SetPageSize();

  CosResult result = cos.DescribeDocProcessBuckets(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeDocProcessBuckets Succ." << std::endl;
    std::cout << "Result: " << resp.GetResult().to_string() << std::endl;
  } else {
    std::cout << "DescribeDocProcessBuckets Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
  std::cout << "===================DescribeDocProcessBuckets==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 文档预览
// https://cloud.tencent.com/document/product/436/54058
void DocPreview(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                const std::string& object_name, const std::string& local_file) {
  DocPreviewReq req(bucket_name, object_name, local_file);
  req.SetSrcType("docx");
  DocPreviewResp resp;
  CosResult result = cos.DocPreview(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DocPreview Succ." << std::endl;
  } else {
    std::cout << "DocPreview Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================DocPreview============================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 提交一个文档预览任务
void CreateDocProcessJobs(qcloud_cos::CosAPI& cos,
                          const std::string& bucket_name) {
  CreateDocProcessJobsReq req(bucket_name);
  CreateDocProcessJobsResp resp;
  req.SetVerifyCert(false);
  Input input;
  input.object = "test.docx";
  req.SetInput(input);

  Operation operation;
  Output output;
  output.bucket = "wqingzhang";
  output.region = "ap-chongqing";
  output.object = "/test-ci/test-create-doc-process-${Number}";

  operation.output = output;
  req.SetOperation(operation);
  // req.SetQueueId("xxx");

  CosResult result = cos.CreateDocProcessJobs(req, &resp);
  if (result.IsSucc()) {
    std::cout << "CreateDocProcessJobs Succ." << std::endl;
    std::cout << "jobs_detail: " << resp.GetJobsDetail().to_string()
              << std::endl;
  } else {
    std::cout << "CreateDocProcessJobs Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout
      << "===================CreateDocProcessJobs============================="
      << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 查询文档预览任务
void DescribeDocProcessJob(qcloud_cos::CosAPI& cos,
                           const std::string& bucket_name) {
  DescribeDocProcessJobReq req(bucket_name);
  DescribeDocProcessJobResp resp;

  req.SetJobId("xxx");

  CosResult result = cos.DescribeDocProcessJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeDocProcessJobs Succ." << std::endl;
    std::cout << "jobs_detail: " << resp.GetJobsDetail().to_string()
              << std::endl;
  } else {
    std::cout << "DescribeDocProcessJobs Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
  std::cout << "===================DescribeDocProcessJobs======================"
               "======="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 拉取符合条件的文档预览任务
void DescribeDocProcessJobs(qcloud_cos::CosAPI& cos,
                            const std::string& bucket_name) {
  DescribeDocProcessJobsReq req(bucket_name);
  DescribeDocProcessJobsResp resp;

  req.SetQueueId("xxx");

  CosResult result = cos.DescribeDocProcessJobs(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeDocProcessJobs Succ." << std::endl;
    std::vector<JobsDetail> jobs_detail = resp.GetJobsDetails();
    for (auto& job : jobs_detail) {
      std::cout << "job: " << job.to_string() << std::endl;
    }
    std::cout << "NextToken: " << resp.GetNextToken() << std::endl;
  } else {
    std::cout << "DescribeDocProcessJobs Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
  std::cout << "===================DescribeDocProcessJobs======================"
               "======="
            << std::endl;
  // PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 查询文档预览队列
void DescribeDocProcessQueues(qcloud_cos::CosAPI& cos,
                              const std::string& bucket_name) {
  DescribeDocProcessQueuesReq req(bucket_name);
  DescribeDocProcessQueuesResp resp;

  CosResult result = cos.DescribeDocProcessQueues(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeDocProcessQueues Succ." << std::endl;
    std::cout << "ReqeustId: " << resp.GetRequestId()
              << ", PageNumber: " << resp.GetPageNumber()
              << ", PageSize: " << resp.GetPageSize()
              << ", QueueList: " << resp.GetQueueList().to_string()
              << ", NonExistPIDs: " << resp.GetNonExistPIDs().to_string()
              << std::endl;
  } else {
    std::cout << "DescribeDocProcessQueues Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
  std::cout << "===================DescribeDocProcessQueues===================="
               "========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 更新文档预览队列
void UpdateDocProcessQueue(qcloud_cos::CosAPI& cos,
                           const std::string& bucket_name) {
  UpdateDocProcessQueueReq req(bucket_name);
  UpdateDocProcessQueueResp resp;

  req.SetName("queue-doc-process-1");
  req.SetQueueId("xxx");
  req.SetState("Active");

  NotifyConfig notify_config;
  notify_config.url = "http://example.com";
  notify_config.state = "On";
  notify_config.type = "Url";
  notify_config.event = "TransCodingFinish";
  req.SetNotifyConfig(notify_config);

  CosResult result = cos.UpdateDocProcessQueue(req, &resp);
  if (result.IsSucc()) {
    std::cout << "UpdateDocProcessQueue Succ." << std::endl;
    std::cout << "ReqeustId: " << resp.GetRequestId()
              << ", QueueList: " << resp.GetQueueList().to_string()
              << std::endl;
  } else {
    std::cout << "UpdateDocProcessQueue Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
  std::cout
      << "===================UpdateDocProcessQueue============================="
      << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 获取对象不带签名的URL
void GetObjectUrl(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                  const std::string& object_name) {
  std::cout << "https url: " << cos.GetObjectUrl(bucket_name, object_name)
            << std::endl;
  std::cout << "http url: " << cos.GetObjectUrl(bucket_name, object_name, false)
            << std::endl;
  std::cout << "ap-beijing https url: "
            << cos.GetObjectUrl(bucket_name, object_name, true, "ap-beijing")
            << std::endl;
  std::cout << "ap-beijing http url: "
            << cos.GetObjectUrl(bucket_name, object_name, false, "ap-beijing")
            << std::endl;
}

// 查询已经开通媒体处理功能的存储桶
// https://cloud.tencent.com/document/product/436/48988
void DescribeMediaBuckets(qcloud_cos::CosAPI& cos) {
  DescribeMediaBucketsReq req;
  DescribeMediaBucketsResp resp;

  req.SetRegions("ap-guangzhou");
  // 设置存储桶名称，以“,”分隔，支持多个存储桶，精确搜索
  // req.SetBucketNames("xxx");
  // 设置存储桶名称前缀，前缀搜索
  // req.SetBucketName("xxx");
  // 设置第几页
  // req.SetPageNumber();
  // 设置每页个数
  // req.SetPageSize();

  CosResult result = cos.DescribeMediaBuckets(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeMediaBuckets Succ." << std::endl;
    std::cout << "Result: " << resp.GetResult().to_string() << std::endl;
  } else {
    std::cout << "DescribeMediaBuckets Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================DescribeMediaBuckets==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 获取媒体文件某个时间的截图
// https://cloud.tencent.com/document/product/436/55671
void GetSnapshot(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                 const std::string& object_name,
                 const std::string& local_file) {
  GetSnapshotReq req(bucket_name, object_name, local_file);
  GetSnapshotResp resp;
  req.SetTime(100);
  // req.SetWitdh();
  // req.SetHeight();
  // req.SetFormat();
  CosResult result = cos.GetSnapshot(req, &resp);
  if (result.IsSucc()) {
    std::cout << "GetSnapshot Succ." << std::endl;
  } else {
    std::cout << "GetSnapshot Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================GetSnapshot==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 获取媒体信息
// https://cloud.tencent.com/document/product/436/55672
void GetMediaInfo(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                  const std::string& object_name) {
  GetMediaInfoReq req(bucket_name, object_name);
  GetMediaInfoResp resp;
  CosResult result = cos.GetMediaInfo(req, &resp);
  if (result.IsSucc()) {
    std::cout << "GetMediaInfo Succ." << std::endl;
    std::cout << "Result: " << resp.GetResult().to_string() << std::endl;
  } else {
    std::cout << "GetMediaInfo Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================GetMediaInfo==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 获取私有 M3U8 ts 资源的下载授权
// https://cloud.tencent.com/document/product/436/63740
void GetPm3u8(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                 const std::string& object_name,
                 const std::string& local_file) {
  GetPm3u8Req req(bucket_name, object_name, local_file);
  GetPm3u8Resp resp;
  req.SetExpires(3600);
  CosResult result = cos.GetPm3u8(req, &resp);
  if (result.IsSucc()) {
    std::cout << "GetPm3u8 Succ." << std::endl;
  } else {
    std::cout << "GetPm3u8 Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================GetPm3u8==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 查询媒体处理队列
void DescribeMediaQueues(qcloud_cos::CosAPI& cos,
                              const std::string& bucket_name) {
  DescribeMediaQueuesReq req(bucket_name);
  DescribeQueuesResp resp;

  CosResult result = cos.DescribeMediaQueues(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeMediaQueues Succ." << std::endl;
    std::cout << "ReqeustId: " << resp.GetRequestId()
              << ", PageNumber: " << resp.GetPageNumber()
              << ", PageSize: " << resp.GetPageSize()
              << ", QueueList: " << resp.GetQueueList().to_string()
              << ", NonExistPIDs: " << resp.GetNonExistPIDs().to_string()
              << std::endl;
  } else {
    std::cout << "DescribeMediaQueues Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
  std::cout << "===================DescribeMediaQueues===================="
               "========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 更新媒体处理队列
void UpdateMediaQueue(qcloud_cos::CosAPI& cos,
                           const std::string& bucket_name) {
  UpdateMediaQueueReq req(bucket_name);
  UpdateQueueResp resp;

  req.SetName("queue-media-process-1");
  req.SetQueueId("xxx");
  req.SetState("Active");

  NotifyConfig notify_config;
  notify_config.url = "http://example.com";
  notify_config.state = "On";
  notify_config.type = "Url";
  notify_config.event = "TransCodingFinish";
  req.SetNotifyConfig(notify_config);

  CosResult result = cos.UpdateMediaQueue(req, &resp);
  if (result.IsSucc()) {
    std::cout << "UpdateMediaQueue Succ." << std::endl;
    std::cout << "ReqeustId: " << resp.GetRequestId()
              << ", QueueList: " << resp.GetQueueList().to_string()
              << std::endl;
  } else {
    std::cout << "UpdateMediaQueue Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
  std::cout
      << "===================UpdateMediaQueue============================="
      << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 创建视频截图任务
void CreateSnapshotJobs(qcloud_cos::CosAPI& cos,
                           const std::string& bucket_name) { 
  CreateMediaProcessJobsReq req(bucket_name);
  CreateMediaProcessJobsResp resp;

  MediaProcessJobsOptions opt;
  opt.input.bucket = bucket_name;
  opt.input.region = "ap-chongqing";
  opt.input.object = "test.mp4";
  opt.tag = "Snapshot";
  // opt.operation.template_id = "XXXXXXXXXXXXXXXXXXX";
  opt.operation.snapshot.mode = "Interval";
  opt.operation.snapshot.start = "0";
  opt.operation.snapshot.time_interval = "5";
  opt.operation.snapshot.count = "3";
  
  opt.operation.output.bucket = bucket_name;
  opt.operation.output.region = "ap-chongqing";
  opt.operation.output.object = "snapshot/test-${number}.jpg";
  req.setMediaProcessOperation(opt);

  CosResult result = cos.CreateMediaProcessJobs(req, &resp);
  if (result.IsSucc()) {
    std::cout << "CreateMediaProcessJobs Succ." << std::endl;
  } else {
    std::cout << "CreateMediaProcessJobs Fail, ErrorMsg: "
              << result.GetErrorMsg() << std::endl;
  }
  std::cout
      << "===================UpdateMediaQueue============================="
      << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 图片同步审核
// https://cloud.tencent.com/document/product/436/45434
void GetImageAuditing(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                  const std::string& object_name) {
  GetImageAuditingReq req(bucket_name);
  GetImageAuditingResp resp;
  // 审核对象
  req.SetObjectKey(object_name);
  // 审核图片链接，Object和URL只用设置一个，设置URL时默认审核URL
  // req.SetDetectUrl("https://exampleurl.com");
  // 审核策略，不填为默认审核策略
  // req.SetBizType("xxxxxxxxxxxxxxxxxxxxxxxxx");
  // 该字段在结果中返回原始内容
  req.SetDataId("data_id_example");
  // 审核场景类型
  req.SetDetectType("ads,porn,politics,terrorism");
  // GIF截帧间隔，单位秒
  req.SetInterval(1);
  // GIF最大截帧数量
  req.SetMaxFrames(2);
  // 超过大小限制的图片是否压缩，0 不压缩， 1 压缩
  // req.SetLargeImageDetect(0);

  CosResult result = cos.GetImageAuditing(req, &resp);
  if (result.IsSucc()) {
    std::cout << "GetImageAuditing Succ." << std::endl;
    std::cout << "Result: \n" << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "GetImageAuditing Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================GetImageAuditing==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 图片批量审核
// https://cloud.tencent.com/document/product/436/63593
void BatchImageAuditing(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                      const std::vector<std::string>& object_names) {
  BatchImageAuditingReq req(bucket_name);
  BatchImageAuditingResp resp;
  for(std::vector<std::string>::const_iterator it = object_names.begin();
       it != object_names.end(); ++it) {
    AuditingInput input = AuditingInput();
    input.SetObject(*it);
    // 审核图片链接，Object和URL只用设置一个，设置URL时默认审核URL
    // input.SetUrl("https://exampleurl.com");
    // 图片标识，该字段在结果中返回原始内容
    input.SetDataId("data_id_example");
    // GIF截帧间隔，单位秒
    // input.SetInterval(5);
    // GIF最大截帧数量
    // input.SetMaxFrames(20);
    // 超过大小限制的图片是否压缩，0 不压缩， 1 压缩
    // input.SetLargeImageDetect(0);
    req.AddInput(input);
  }
  // 审核策略，不填为默认审核策略
  // req.SetBizType("biz_type_example");
  // 审核场景类型
  req.SetDetectType("Ads,Porn,Politics,Terrorism");
  CosResult result = cos.BatchImageAuditing(req, &resp);
  if (result.IsSucc()) {
    std::cout << "BatchImageAuditing Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::vector<ImageAuditingJobsDetail> jobsDetails = resp.GetJobsDetails();
    for (std::vector<ImageAuditingJobsDetail>::const_iterator it = jobsDetails.begin();
         it != jobsDetails.end(); ++it) {
      std::cout << "JobsDetails: \n" << it->to_string() << std::endl;
    }
  } else {
    std::cout << "BatchImageAuditing Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================BatchImageAuditing==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 图片审核任务结果查询
// https://cloud.tencent.com/document/product/436/68904
void DescribeImageAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                        const std::string& job_id) {
  DescribeImageAuditingJobReq req(bucket_name);
  DescribeImageAuditingJobResp resp;
  req.SetJobId(job_id);
  CosResult result = cos.DescribeImageAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeImageAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetail: \n" << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "DescribeImageAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================DescribeImageAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 提交视频审核任务
// https://cloud.tencent.com/document/product/436/47316
void CreateVideoAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                              const std::string& object_name) {
  CreateVideoAuditingJobReq req(bucket_name);
  CreateVideoAuditingJobResp resp;
  // input
  // 需要审核的视频object
  req.SetObject(object_name);
  // 审核视频链接，Object和URL只用设置一个，设置URL时默认审核URL
  //req.SetUrl("https://exampleurl.com");
  // 视频标识，该字段在结果中返回原始内容
  req.SetDataId("DataId");
  // 用户业务字段
  // UserInfo user_info = UserInfo();
  // user_info.SetIp("ip_example");
  // req.SetUserInfo(user_info);

  // conf
  SnapShotConf snap_shot = SnapShotConf();
  // 截图数量
  snap_shot.SetCount(100);
  // 截图类型，Interval表示间隔模式、Average平均模式、Fps固定帧率模式，默认为Interval，
  snap_shot.SetMode("Interval");
  req.SetSnapShot(snap_shot);
  // 审核场景
  req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
  // 审核策略
  // req.SetBizType("xxxxxxxxxxxxxxxxxxxxxxxxx");
  // 回调地址
  req.SetCallBack("https://callback.com");
  // 回调信息内容 默认为Simple
  req.SetCallBackVersion("Simple");
  // 是否审核视频声音，1 审核视频画面截图和声音，0 审核视频画面，默认0
  req.SetDetectContent(1);

  CosResult result = cos.CreateVideoAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "CreateVideoAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetail: " << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "CreateVideoAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================CreateVideoAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}


// 查询视频审核任务结果
// https://cloud.tencent.com/document/product/436/47317
void DescribeVideoAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                            const std::string& job_id) {
  DescribeVideoAuditingJobReq req(bucket_name);
  DescribeVideoAuditingJobResp resp;
  req.SetJobId(job_id);
  CosResult result = cos.DescribeVideoAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeVideoAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetail: " << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "DescribeVideoAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================DescribeVideoAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 提交音频审核任务
// https://cloud.tencent.com/document/product/436/54063
void CreateAudioAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                            const std::string& object_name) {
  CreateAudioAuditingJobReq req(bucket_name);
  CreateAudioAuditingJobResp resp;
  // input
  // 需要审核的音频object
  req.SetObject(object_name);
  // 审核音频链接，Object和URL只用设置一个，设置URL时默认审核URL
  //req.SetUrl("https://exampleurl.com");
  // 音频标识，该字段在结果中返回原始内容
  req.SetDataId("DataId");
  // 用户业务字段
  // UserInfo user_info = UserInfo();
  // user_info.SetType("type");
  // req.SetUserInfo(user_info);

  // conf
  // 审核场景
  req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
  // 审核策略
  // req.SetBizType("xxxxxxxxxxxxxxxxxxxxxxxxx");
  // 回调地址
  req.SetCallBack("https://callback.com");
  // 回调信息内容 默认为Simple
  req.SetCallBackVersion("Simple");

  CosResult result = cos.CreateAudioAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "CreateAudioAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetail: " << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "CreateAudioAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================CreateAudioAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}


// 查询音频审核任务结果
// https://cloud.tencent.com/document/product/436/54064
void DescribeAudioAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                              const std::string& job_id) {
  DescribeAudioAuditingJobReq req(bucket_name);
  DescribeAudioAuditingJobResp resp;
  req.SetJobId(job_id);
  CosResult result = cos.DescribeAudioAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeAudioAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetails: \n" << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "DescribeAudioAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================DescribeAudioAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 提交文本审核任务
// https://cloud.tencent.com/document/product/436/56289
void CreateTextAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                            const std::string& object_name) {
  CreateTextAuditingJobReq req(bucket_name);
  CreateTextAuditingJobResp resp;
  // input
  // 需要审核的文本object
  req.SetObject(object_name);
  // 审核文本链接，Object和URL只用设置一个，设置URL时默认审核URL
  //req.SetUrl("https://exampleurl.com");
  // 音频标识，该字段在结果中返回原始内容
  req.SetDataId("DataId");
  // 需要审核的文本
  // req.SetContent("需要审核的文本");
  // 用户业务字段
  // UserInfo user_info = UserInfo();
  // user_info.SetType("type");
  // req.SetUserInfo(user_info);

  // conf
  // 审核场景
  req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
  // 审核策略
  // req.SetBizType("xxxxxxxxxxxxxxxxxxxxxxxxx");
  // 回调地址
  req.SetCallBack("https://callback.com");
  // 回调信息内容 默认为Simple
  req.SetCallBackVersion("Simple");

  CosResult result = cos.CreateTextAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "CreateTextAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetail: " << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "CreateTextAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================CreateTextAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}


// 查询文本审核任务结果
// https://cloud.tencent.com/document/product/436/56288
void DescribeTextAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                              const std::string& job_id) {
  DescribeTextAuditingJobReq req(bucket_name);
  DescribeTextAuditingJobResp resp;
  req.SetJobId(job_id);
  CosResult result = cos.DescribeTextAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeTextAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetails: " << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "DescribeTextAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================DescribeTextAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 提交文档审核任务
// https://cloud.tencent.com/document/product/436/59381
void CreateDocumentAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                           const std::string& object_name) {
  CreateDocumentAuditingJobReq req(bucket_name);
  CreateDocumentAuditingJobResp resp;
  // input
  // 需要审核的音频object
  req.SetObject(object_name);
  // 审核音频链接，Object和URL只用设置一个，设置URL时默认审核URL
  //req.SetUrl("https://exampleurl.com");
  // 文档标识，该字段在结果中返回原始内容
  req.SetDataId("DataId");
  // 指定文档后缀
  req.SetType("docx");
  // 用户业务字段
  // UserInfo user_info = UserInfo();
  // user_info.SetType("type");
  // req.SetUserInfo(user_info);

  // conf
  // 审核场景
  req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
  // 审核策略
  // req.SetBizType("biz_type");
  // 回调地址
  req.SetCallBack("https://callback.com");

  CosResult result = cos.CreateDocumentAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "CreateDocumentAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetail: " << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "CreateDocumentAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================CreateTextAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}


// 查询文档审核任务结果
// https://cloud.tencent.com/document/product/436/59382
void DescribeDocumentAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                             const std::string& job_id) {
  DescribeDocumentAuditingJobReq req(bucket_name);
  DescribeDocumentAuditingJobResp resp;
  req.SetJobId(job_id);
  CosResult result = cos.DescribeDocumentAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeDocumentAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetails: \n" << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "DescribeDocumentAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================DescribeDocumentAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}

// 提交网页审核任务
// https://cloud.tencent.com/document/product/436/63958
void CreateWebPageAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                               const std::string& url) {
  CreateWebPageAuditingJobReq req(bucket_name);
  CreateWebPageAuditingJobResp resp;
  // input
  // 审核网页链接
  req.SetUrl(url);
  // 音频标识，该字段在结果中返回原始内容
  req.SetDataId("DataId");
  // 用户业务字段
  // UserInfo user_info = UserInfo();
  // user_info.SetType("type");
  // req.SetUserInfo(user_info);

  // conf
  // 审核场景
  req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
  // 是否需要高亮展示网页内的违规文本
  req.SetReturnHighligthHtml(true);
  // 回调地址
  req.SetCallBack("https://callback.com");

  CosResult result = cos.CreateWebPageAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "CreateWebPageAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetail: \n" << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "CreateWebPageAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================CreateWebPageAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}


// 查询网页审核任务结果
// https://cloud.tencent.com/document/product/436/63959
void DescribeWebPageAuditingJob(qcloud_cos::CosAPI& cos, const std::string& bucket_name,
                                 const std::string& job_id) {
  DescribeWebPageAuditingJobReq req(bucket_name);
  DescribeWebPageAuditingJobResp resp;
  req.SetJobId(job_id);
  CosResult result = cos.DescribeWebPageAuditingJob(req, &resp);
  if (result.IsSucc()) {
    std::cout << "DescribeWebPageAuditingJob Succ." << std::endl;
    std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
    std::cout << "JobsDetails: " << resp.GetJobsDetail().to_string() << std::endl;
  } else {
    std::cout << "DescribeWebPageAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg()
              << std::endl;
  }
  std::cout << "===================DescribeWebPageAuditingJob==================="
               "=========="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================"
            << std::endl;
}


void TestLogCallback(const std::string& log) {
  std::ofstream ofs;
  ofs.open("test.log", std::ios_base::app);
  ofs << log;
  ofs.close();
}

int main(int argc, char** argv) {
  UNUSED_PARAM(argc)
  UNUSED_PARAM(argv)
  // config.json中字段的说明，可以参考https://cloud.tencent.com/document/product/436/12301
  qcloud_cos::CosConfig config("./config.json");
  uint64_t appId = 1253960454;
  // qcloud_cos::CosConfig config(appId, "AKIDfT9kBBRy9U6F0EMIvuaeccxqip0nb9HeQbWbFs5lnYW0yoEcgM_haOcv720H4IVV", "GLjqLTwThXqUiQ5WIEaHrVIyLYnPrZ+3H85woyhldgw=", "ap-chongqing");

  config.SetLogCallback(&TestLogCallback);
  // config.SetTmpToken("1TzrvTlSKxEkIdauOIDvPv6syF9xEP9a16bc3f53ab6336d49462944cfb6559ab7gLxfJt17XvYEs58-qIMu1HikuBqCNE5c6kvUOegTe7wc1DtslKyDGiPm-jR6wGZT5Ifo3AWPKzgrUPhgfildF1R-aoJ4WUP0YNF-YR9kY4k24dCgANe-5B_MNcDGTL2h0WjqhmSeB55woZqX0XKlR46veF8AbBfuEuOnDalFQg3_gFdwBxPh2WNp-AfG7j6GBt4NW1_Sv780nb_SPCMH7x2Eank-qeYe1nwMwYwpsFxLvFlgLXfdXZ_VrzlszjNNu25nXTYUD4bnNvWV32dYqDQK6gUWX0lb8rl9o5gahGhS1Ka_DjicALLQCK84_Vkq5cPKM3J7ngTT-wH_mo4NTYyG3fgZZ6nyvEYhf4uNIM");
  qcloud_cos::CosAPI cos(config);
  std::string bucket_name =
      "test-12345";  //替换为用户的存储桶名，由bucketname-appid
  ///组成，appid必须填入，可以在COS控制台查看存储桶名称。
  /// https://console.cloud.tencent.com/cos5/bucket

  // PutBucketInventory(cos, bucket_name);
  // GetBucketInventory(cos,bucket_name);
  // PutBucketDomain(cos, bucket_name);
  // GetBucketDomain(cos, bucket_name);
  // ListBucketInventoryConfigurations(cos, bucket_name);
  // DeleteBucketInventory(cos, bucket_name);
  // ListBucketInventoryConfigurations(cos, bucket_name);
  // PutBucketLogging(cos, bucket_name, bucket_name1, "log");
  // GetBucketLogging(cos, bucket_name);
  // std::vector<Tag> tagset;
  // Tag tag1;
  // tag1.SetKey("age");
  // tag1.SetValue("19");

  // Tag tag2;
  // tag2.SetKey("name");
  // tag2.SetValue("xiaoming");

  // Tag tag3;
  // tag3.SetKey("sex");
  // tag3.SetValue("male");

  // tagset.push_back(tag1);
  // tagset.push_back(tag2);
  // tagset.push_back(tag3);

  // PutBucketTagging(cos, bucket_name, tagset);
  // GetBucketTagging(cos, bucket_name);
  // DeleteBucketTagging(cos, bucket_name);

  // PutBucketWebsite(cos, bucket_name);
  // GetBucketWebsite(cos, bucket_name);
  // DeleteBucketWebsite(cos, bucket_name);

  // GetService(cos);
  // PutBucket(cos, bucket_name);
  // GetBucket(cos, bucket_name);
  // PutBucketVersioning(cos, bucket_name);
  // GetBucketVersioning(cos, bucket_name);
  // PutBucketLogging(cos, bucket_name, targetbucket_name, prefix);
  // GetBucketLogging(cos, bucket_name);
  // PutBucketReplication(cos, bucket_name);
  // GetBucketReplication(cos, bucket_name);
  // DeleteBucketReplication(cos, bucket_name);
  // PutBucketLifecycle(cos, bucket_name);
  // GetBucketLifecycle(cos, bucket_name);
  // DeleteBucketLifecycle(cos, bucket_name);
  // PutBucketACL(cos, bucket_name);
  // GetBucketACL(cos, bucket_name);
  // PutBucketCORS(cos, bucket_name);
  // GetBucketCORS(cos, bucket_name);
  // DeleteBucketCORS(cos, bucket_name);
  // PutBucketReferer(cos, bucket_name);
  // GetBucketReferer(cos, bucket_name);

  //// 简单上传(文件)
  // PutObjectByFile(cos, bucket_name, "sevenyou_1102_south",
  // "/data/sevenyou/temp/seven_0821_10M");
  // PutObjectByFile(cos, bucket_name, "sevenyou_e2_1102_north",
  // "/data/sevenyou/temp/seven_0821_10M"); PutObjectByFile(cos, bucket_name,
  // "sevenyounorthtest2_normal", "/data/sevenyou/temp/seven_0821_10M");
  // PutObjectByFile(cos, bucket_name, "sevenyou_e2_north_put",
  // "/data/sevenyou/temp/seven_0821_10M");
  // HeadObject(cos, bucket_name, "sevenyou_1102_north.jpg");
  // GetObjectByFile(cos, bucket_name, "costest.php",
  // "/data/sevenyou/temp/costest.php"); PutObjectByFile(cos, bucket_name,
  // "sevenyou_e2_north_put", "/data/sevenyou/temp/seven_0821_10M");
  //// 简单上传(文件),特殊字符
  // PutObjectByFile(cos, bucket_name, "/→↓←→↖↗↙↘! \"#$%&'()*+,-./0123456789:;"
  //                "<=>@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~",
  //                "/data/sevenyou/temp/seven_0821_10M");
  //// 简单上传(文件),中文字符
  // PutObjectByFile(cos, bucket_name, "/中文文件",
  // "/data/sevenyou/temp/seven_0821_10M");
  //// 简单上传(文件), 大文件
  // PutObjectByFile(cos, bucket_name, "test", "./config.json");

  // PutObjectByStream(cos, bucket_name);

  // HeadObject(cos, bucket_name, "sevenyou_e1_south_put_copy");
  // HeadObject(cos, bucket_name, "sevenyou_e2_abc.jar");
  // HeadObject(cos, bucket_name, "sevenyou_6G");
  // GetObjectByFile(cos, bucket_name, "sevenyou_e1_abc",
  // "/data/sevenyou/temp/sevenyou_10m_download_03"); GetObjectByFile(cos,
  // bucket_name, "sevenyou_e2_abc",
  // "/data/sevenyou/temp/sevenyou_10m_download_03");
  // GetObjectByStream(cos, bucket_name, "sevenyou_e2_abc");
  // MultiGetObject(cos, bucket_name, "test-mp", "./bigfile_get");

  // {
  //     std::string upload_id;
  //     std::string object_name = "sevenyou_e2_1102_north_multi";
  //     std::vector<uint64_t> numbers;
  //     std::vector<std::string> etags;

  //     std::string etag1 = "", etag2 = "";
  //     InitMultiUpload(cos, bucket_name, object_name, &upload_id);

  //     std::fstream is1("/data/sevenyou/temp/seven_0821_5M.part1");
  //     UploadPartData(cos, bucket_name, object_name, upload_id, is1, 1,
  //     &etag1); numbers.push_back(1); etags.push_back(etag1); is1.close();

  //     ListParts(cos, bucket_name, object_name, upload_id);
  //     // AbortMultiUpload(cos, bucket_name, object_name, upload_id);

  //     std::fstream is2("/data/sevenyou/temp/seven_0821_5M.part2");
  //     UploadPartData(cos, bucket_name, object_name, upload_id, is2, 2,
  //     &etag2); numbers.push_back(2); etags.push_back(etag2);

  //     is2.close();

  //     CompleteMultiUpload(cos, bucket_name, object_name, upload_id, etags,
  //     numbers);
  // }

  // {
  //     std::string upload_id;
  //     std::string object_name = "sevenyou_2G_part_copy_from_north";
  //     std::vector<uint64_t> numbers;
  //     std::vector<std::string> etags;

  //     std::string etag1 = "", etag2 = "";
  //     InitMultiUpload(cos, bucket_name, object_name, &upload_id);

  //     UploadPartCopy(cos, bucket_name, object_name, upload_id,
  //     "sevenyousouth-1251668577.cos.ap-guangzhou.myqcloud.com/seven_10G.tmp",
  //     "bytes=0-1048576000", 1, &etag1); numbers.push_back(1);
  //     etags.push_back(etag1);

  //     UploadPartCopy(cos, bucket_name, object_name, upload_id,
  //     "sevenyoutest-7319456.cos.cn-north.myqcloud.com/sevenyou_2G_part",
  //     "bytes=1048576000-2097152000", 2, &etag2); numbers.push_back(2);
  //     etags.push_back(etag2);

  //     CompleteMultiUpload(cos, bucket_name, object_name, upload_id, etags,
  //     numbers);
  // }

  // // Copy(cos, bucket_name, "sevenyou_6G_diff_region_copy_part",
  // "sevenyoutest-123456.cos.ap-beijing-1.myqcloud.com/sevenyou_6G");

  // MultiUploadObject(cos, bucket_name, "test", "/tmp/testfile10GB");
  // "/data/sevenyou/temp/seven_50M.tmp.0925");
  // //MultiUploadObject(cos, bucket_name, "sevenyou_1102_north_multi",
  // "/data/sevenyou/temp/seven_50M.tmp.0925");

  // //PutObjectACL(cos, bucket_name, "sevenyou_10m");

  // //PutObjectCopy(cos, bucket_name,
  // "sevenyou_e3_put_obj_copy_from_north_normal",
  // //
  // "sevenyounorthtestbak-7319456.cn-north.myqcloud.com/sevenyou_1102_north");
  // //PutObjectCopy(cos, bucket_name,
  // "sevenyou_e1_put_obj_copy_from_north_multi",
  // //
  // "sevenyounorthtestbak-7319456.cn-north.myqcloud.com/sevenyou_1102_north_multi");
  // //PutObjectCopy(cos, bucket_name,
  // "sevenyou_e2_put_obj_copy_from_south_multi",
  // //
  // "sevenyousouthtest-7319456.cn-north.myqcloud.com/sevenyou_1102_south_multi");
  // //PutObjectCopy(cos, bucket_name,
  // "sevenyou_e2_north_put_copy_from_south_16",
  // //
  // "sevenyousouthtest-7319456.cn-south.myqcloud.com/sevenyou_e1_south_put");

  // //DeleteObject(cos, bucket_name, "sevenyou_e1_multi");
  // //DeleteObject(cos, bucket_name, "sevenyou_e2_put");

  // //DeleteBucket(cos, bucket_name);

  // {
  //     qcloud_cos::GeneratePresignedUrlReq req(bucket_name, "seven_50M.tmp",
  //     qcloud_cos::HTTP_GET); std::string presigned_url =
  //     cos.GeneratePresignedUrl(req); std::cout << "Presigend Uril=[" <<
  //     presigned_url << "]" << std::endl;
  // }

  // {
  //     std::string presigned_url = cos.GeneratePresignedUrl(bucket_name,
  //     "seven_50M.tmp", 1514799284, 1514899284); std::cout << "Presigend
  //     Uril=[" << presigned_url << "]" << std::endl;
  // }

  // {
  //     std::cout << "Bucket=" << bucket_name << ", Location=" <<
  //     cos.GetBucketLocation(bucket_name) << std::endl;
  // }

  // std::cout << "IsBucketExist=";
  // std::cout << (cos.IsBucketExist("abcdefg") ? "true" : "false") <<
  // std::endl; std::cout << (cos.IsBucketExist(bucket_name) ? "true" : "false")
  // << std::endl;

  // std::cout << "IsObjectExist=" << std::endl;
  // std::cout << (cos.IsObjectExist(bucket_name, "abcdefg") ? "true" : "false")
  // << std::endl; std::cout << (cos.IsObjectExist(bucket_name, "seven_50M.tmp")
  // ? "true" : "false") << std::endl;

  // Batch Delete
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
  //     std::cout <<
  //     "===================DeleteBucketResponse=====================" <<
  //     std::endl; PrintResult(result, resp); std::cout <<
  //     "=========================================================" <<
  //     std::endl;
  // }

  // Restore
  // {
  //     PostObjectRestoreReq req(bucket_name, "restore_test_obj");
  //     req.SetExiryDays(30);
  //     req.SetTier("Standard");
  //     PostObjectRestoreResp resp;

  //     CosResult result = cos.PostObjectRestore(req, &resp);
  //     std::cout << "======================PostObjectRestore=-=========" <<
  //     std::endl; PrintResult(result, resp); std::cout <<
  //     "=========================================================" <<
  //     std::endl;
  // }

  // 限速上传下载
  //{
  //    std::string bucket_name = "testbucket";
  //    std::string object_name_prefix = "test_traffic_limit_";
  //    std::string upload_file_path = "/data/testtrafficlimit/testfile_100M";
  //    std::string download_file_path_prefix =
  //    "/data/testtrafficlimit/download_test_traffic_limit_"; std::string
  //    object_name, download_file_path; if (access(upload_file_path.c_str(),
  //    F_OK)) {
  //        std::cerr << upload_file_path << " not exists" << std::endl;
  //        return -1;
  //    }
  //    //限速上传/下载文件,by header
  //    object_name = object_name_prefix + "1";
  //    download_file_path = download_file_path_prefix + "1";
  //    std::cout << "===================PutObjectByFileLimitTraffic, by header,
  //    traffic limit = 1MB/s = 8388608 byte/sec =====================" <<
  //    std::endl; PutObjectByFileLimitTraffic(cos, bucket_name, object_name,
  //    upload_file_path, 8388608); std::cout <<
  //    "===================GetObjectByFileLimitTraffic, by header, Traffic
  //    limit = 4MB/s = 33554432 byte/sec =====================" << std::endl;
  //    GetObjectByFileLimitTraffic(cos, bucket_name, object_name,
  //    download_file_path, 33554432); DeleteObject(cos, bucket_name,
  //    object_name);
  //
  //    //限速上传/下载文件,by param
  //    object_name = object_name_prefix + "2";
  //    download_file_path = download_file_path_prefix + "2";
  //    std::cout << "===================PutObjectByFileLimitTraffic, by param,
  //    traffic limit = = 1MB/s = 8388608 byte/sec =====================" <<
  //    std::endl; PutObjectByFileLimitTraffic(cos, bucket_name, object_name,
  //    upload_file_path, 8388608, false); std::cout <<
  //    "===================GetObjectByFileLimitTraffic, by param, Traffic limit
  //    = 4MB/s = 33554432 byte/sec=====================" << std::endl;
  //    GetObjectByFileLimitTraffic(cos, bucket_name, object_name,
  //    download_file_path, 33554432, false); DeleteObject(cos, bucket_name,
  //    download_file_path);
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
  //    std::cout << "===================UploadPartDataLimitTraffic, part 1,
  //    Traffic limit = 4MB/s = 33554432 byte/sec =====================" <<
  //    std::endl; std::fstream fs1(upload_file_path);
  //    UploadPartDataLimitTraffic(cos, bucket_name, object_name, upload_id,
  //    fs1, 1, &etag1, 33554432); numbers.push_back(1); etags.push_back(etag1);
  //    std::cout << "===================UploadPartDataLimitTraffic, part 2,
  //    Traffic limit = 4MB/s = 33554432 byte/sec =====================" <<
  //    std::endl; std::fstream fs2(upload_file_path);
  //    UploadPartDataLimitTraffic(cos, bucket_name, object_name, upload_id,
  //    fs2, 2, &etag2, 33554432); numbers.push_back(2); etags.push_back(etag2);
  //    CompleteMultiUpload(cos, bucket_name, object_name, upload_id, etags,
  //    numbers);
  //
  //    //多线程限速下载文件
  //    std::cout << "===================MultiThradDownObjectLimitTraffic,
  //    Traffic limit = 1MB/s = 8388608 byte/sec =====================" <<
  //    std::endl; MultiGetObjectLimitTraffic(cos, bucket_name, object_name,
  //    download_file_path, 8388608);
  //
  //    //多线程限速上传文件
  //    object_name = object_name_prefix + "4";
  //    std::cout << "===================MultiThradUploadObjectLimitTraffic,
  //    Traffic limit = 1MB/s = 8388608 byte/sec =====================" <<
  //    std::endl; MultiUploadObjectLimitTraffic(cos, bucket_name, object_name,
  //    upload_file_path, 8388608);
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

  // TestIntelligentTiering(cos, bucket_name);

  // 断点下载
  // ResumableGetObject(cos, bucket_name, "bigfile", "./bigfile_resume");

  // async
  //{
  //    AsyncMultiPutObject(cos, bucket_name, "bigfile", "./bigfile");
  //    AsyncMultiGetObject(cos, bucket_name, "bigfile", "./bigfile_download");
  //}

  //{
  // 上传目录下的文件
  //    PutObjectsFromDirectory(cos, bucket_name, "/tmp/cos-cpp-sdk-v5/");
  // 上传目录下的文件到cos指定路径
  //    PutObjectsFromDirectoryToCosPath(cos, bucket_name,
  //    "/tmp/cos-cpp-sdk-v5/", "my_test_path/");
  // 删除目录
  //    //DeleteDirectory(cos, bucket_name, "my_test_path/");
  // 按前缀删除
  //    DeleteObjectsByPrefix(cos, bucket_name, "sub1");
  // 移动对象
  //    MoveObject(cos, bucket_name, "hello3.txt", "hello4.txt");
  //    MoveObject(cos, bucket_name, "hello2.txt", "test_dir/hello4.txt");
  // 下载目录
  //    DownloadFromDirectory(cos, bucket_name, "stream657619/", "/tmp/");
  //}

  // 图片处理
//  {
//      ImageThumbnail(cos, bucket_name, "flower.jpg",  "flower_thumbnail.jpg");
//      PutImage(cos, bucket_name, "test_ci/flower.jpg",  "./flower.jpg");
//      AddWatermark(cos, bucket_name, "test_ci/flower.jpg",  "./flower.jpg");
//      GetImageWithWatermark(cos, bucket_name, "test_ci/flower.jpg",
//      "./flower_download_with_matermark.jpg"); ExtractWatermark(cos,
//      bucket_name, "test_ci/flower_download_with_matermark.jpg",
//      "./flower_download_with_matermark.jpg"); CloudImageProcess(cos,
//      bucket_name, "test_ci/flower.jpg"); PutQRcode(cos, bucket_name,
//      "test_ci/qrcode.png", "./qrcode.png"); GetQRcode(cos, bucket_name,
//      "test_ci/qrcode.png");
//  }

  // 文档接口
  //{
  //    DescribeDocProcessBuckets(cos);
  //    DocPreview(cos, bucket_name, "test.docx", "test_preview.jpg");
     CreateDocProcessJobs(cos, bucket_name);
  //    DescribeDocProcessJob(cos, bucket_name);
  //    DescribeDocProcessJobs(cos, bucket_name);
    //  DescribeDocProcessQueues(cos, bucket_name);
  //    UpdateDocProcessQueue(cos, bucket_name);
  //}
  // GetObjectUrl(cos, bucket_name, "test_object");

  // 媒体接口
  //{
  //  DescribeMediaBuckets(cos);
  //  GetSnapshot(cos, bucket_name, "1920_1080.mp4", "snapshot.jpg");
  //  GetMediaInfo(cos, bucket_name, "1920_1080.mp4");
  //  GetPm3u8(cos, bucket_name, "video.m3u8");
  //  DescribeMediaQueues(cos, bucket_name);
  //  UpdateMediaQueue(cos, bucket_name);
      // CreateSnapshotJobs(cos, bucket_name);
  //}

  // 图片审核
  //{
  //  GetImageAuditing(cos, bucket_name, "data/audit/gif/test.gif");
  //  std::vector<std::string> objects = {"data/audit/ads/clipboard.png", "data/audit/git/test.jpg"};
  //  BatchImageAuditing(cos, bucket_name, objects);
  //  DescribeImageAuditingJob(cos, bucket_name, "sife0bdb79d72211ecb174525400bc4ce1");
  //}

  // 视频审核
  //{
  //  CreateVideoAuditingJob(cos, bucket_name, "data/audit/video/test.mp4");
  //  DescribeVideoAuditingJob(cos, bucket_name, "avda91c516d81a11ec933c525400863904");
  //}

  // 音频审核
  //{
  //  CreateAudioAuditingJob(cos, bucket_name, "data/audit/audio/test.mp3");
  //  DescribeAudioAuditingJob(cos, bucket_name, "sa3be08072d81f11ecb378525400bc4ce1");
  //}

  // 文本审核
  //{
  //  CreateTextAuditingJob(cos, bucket_name, "data/audit/text/test.txt");
  //  DescribeTextAuditingJob(cos, bucket_name, "stf8d06a90d82211ecb378525400bc4ce1");
  //}

  // 文档审核
  //{
  //  CreateDocumentAuditingJob(cos, bucket_name, "data/audit/documents/test.docx");
  //  DescribeDocumentAuditingJob(cos, bucket_name, "sd5609cbe0db1211ec9bdf525400863904");
  //}

  // 网页审核
  //{
  //  CreateWebPageAuditingJob(cos, bucket_name, "https://www.test.com/");
  //  DescribeWebPageAuditingJob(cos, bucket_name, "shea80c920db2a11ec954f525400bc4ce1");
  //}

#if defined(_WIN32)
  // MultiUploadObjectWideChar(cos, bucket_name);
  system("pause");
#endif
  return 0;
}
