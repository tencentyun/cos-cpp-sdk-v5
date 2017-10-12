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

int main(int argc, char** argv) {
    qcloud_cos::CosConfig config("./config.json");
    qcloud_cos::CosAPI cos(config);

    std::string bucket_name = "sevenyounorthtest";
    qcloud_cos::CosSysConfig::SetDestDomain("180.163.3.250/seven_2_you");
    // 0. PutBucket
    {
        qcloud_cos::PutBucketReq req(bucket_name);
        qcloud_cos::PutBucketResp resp;
        qcloud_cos::CosResult result = cos.PutBucket(req, &resp);

        std::cout << "===================PutBucketResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "=========================================================" << std::endl;
    }

    // 1. GetBucket
    {
        qcloud_cos::GetBucketReq req(bucket_name);
        qcloud_cos::GetBucketResp resp;
        qcloud_cos::CosResult result = cos.GetBucket(req, &resp);

        std::cout << "===================GetBucketResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "=========================================================" << std::endl;
    }

    // 2. 设置跨园区复制配置
    {
        qcloud_cos::PutBucketReplicationReq req(bucket_name);
        req.SetRole("qcs::cam::uin/***:uin/****");
        qcloud_cos::ReplicationRule rule("sevenyou_10m",
                                         "qcs:id/0:cos:cn-south:appid/***:sevenyousouthtest",
                                         "", "RuleId_01", true);

        req.AddReplicationRule(rule);
        qcloud_cos::PutBucketReplicationResp resp;
        qcloud_cos::CosResult result = cos.PutBucketReplication(req, &resp);
        std::cout << "===================PutBucketReplicationResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 3. 获取跨园区复制配置
    {
        qcloud_cos::GetBucketReplicationReq req(bucket_name);
        qcloud_cos::GetBucketReplicationResp resp;
        qcloud_cos::CosResult result = cos.GetBucketReplication(req, &resp);

        std::cout << "===================GetBucketReplicationResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 4. 删除跨园区复制配置
    {
        qcloud_cos::DeleteBucketReplicationReq req(bucket_name);
        qcloud_cos::DeleteBucketReplicationResp resp;
        qcloud_cos::CosResult result = cos.DeleteBucketReplication(req, &resp);

        std::cout << "===================DeleteBucketReplicationResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 5. 设置生命周期配置
    {
        qcloud_cos::PutBucketLifecycleReq req(bucket_name);
        qcloud_cos::LifecycleRule rule;
        rule.SetIsEnable(true);
        rule.SetId("lifecycle_rule00");
        qcloud_cos::LifecycleFilter filter;
        filter.SetPrefix("prefix00");
        rule.SetFilter(filter);
        qcloud_cos::LifecycleTransition transition;
        transition.SetDays(123);
        transition.SetStorageClass("Nearline");
        rule.SetTransition(transition);
        req.AddRule(rule);

        qcloud_cos::PutBucketLifecycleResp resp;
        qcloud_cos::CosResult result = cos.PutBucketLifecycle(req, &resp);
        std::cout << "===================PutBucketLifecycleResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 6. 获取生命周期配置
    {
        qcloud_cos::GetBucketLifecycleReq req(bucket_name);
        qcloud_cos::GetBucketLifecycleResp resp;
        qcloud_cos::CosResult result = cos.GetBucketLifecycle(req, &resp);

        std::cout << "===================GetBucketLifecycleResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 7. 删除生命周期配置
    {
        qcloud_cos::DeleteBucketLifecycleReq req(bucket_name);
        qcloud_cos::DeleteBucketLifecycleResp resp;
        qcloud_cos::CosResult result = cos.DeleteBucketLifecycle(req, &resp);

        std::cout << "===================DeleteBucketLifecycleResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 8.1 设置ACL配置(通过Body, 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
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

    // 8.2 设置ACL配置(通过Header, 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
    {
        qcloud_cos::PutBucketACLReq req(bucket_name);
        req.SetXCosAcl("public-read-write");

        qcloud_cos::PutBucketACLResp resp;
        qcloud_cos::CosResult result = cos.PutBucketACL(req, &resp);
        std::cout << "===================PutBucketACLResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 9. 获取ACL配置
    {
        qcloud_cos::GetBucketACLReq req(bucket_name);
        qcloud_cos::GetBucketACLResp resp;
        qcloud_cos::CosResult result = cos.GetBucketACL(req, &resp);

        std::cout << "===================GetBucketACLResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 10. 设置CORS配置
    {
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

    // 11. 获取CORS配置
    {
        qcloud_cos::GetBucketCORSReq req(bucket_name);
        qcloud_cos::GetBucketCORSResp resp;
        qcloud_cos::CosResult result = cos.GetBucketCORS(req, &resp);

        std::cout << "===================GetBucketCORSResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 12. 删除CORS配置
    {
        qcloud_cos::DeleteBucketCORSReq req(bucket_name);
        qcloud_cos::DeleteBucketCORSResp resp;
        qcloud_cos::CosResult result = cos.DeleteBucketCORS(req, &resp);

        std::cout << "===================DeleteBucketCORSResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 13.1 简单上传(文件)
    {
        qcloud_cos::PutObjectByFileReq req(bucket_name, "/sevenyou_0831", "/data/sevenyou/temp/seven_0821_10M");
        req.SetXCosStorageClass("STANDARD_IA");
        qcloud_cos::PutObjectByFileResp resp;
        qcloud_cos::CosResult result = cos.PutObject(req, &resp);

        std::cout << "===================PutObjectResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "=========================================================" << std::endl;
    }

    // 13.2 简单上传(文件),特殊字符
    {
        qcloud_cos::PutObjectByFileReq req(bucket_name, "/→↓←→↖↗↙↘! \"#$%&'()*+,-./0123456789:;"
                           "<=>@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~",
                           "/data/sevenyou/temp/seven_0821_10M");
        req.SetXCosStorageClass("STANDARD_IA");
        qcloud_cos::PutObjectByFileResp resp;
        qcloud_cos::CosResult result = cos.PutObject(req, &resp);

        std::cout << "===================PutObjectResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "=========================================================" << std::endl;
    }

    // 13.3 简单上传(文件),中文字符
    {
        qcloud_cos::PutObjectByFileReq req(bucket_name, "/中文文件",
                           "/data/sevenyou/temp/seven_0821_10M");
        req.SetXCosStorageClass("STANDARD_IA");
        qcloud_cos::PutObjectByFileResp resp;
        qcloud_cos::CosResult result = cos.PutObject(req, &resp);

        std::cout << "===================PutObjectResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "=========================================================" << std::endl;
    }

    // 13.4 简单上传(流)
    {
        std::istringstream iss("put object");
        qcloud_cos::PutObjectByStreamReq req(bucket_name, "sevenyou_10m", iss);
        req.SetXCosStorageClass("STANDARD_IA");
        qcloud_cos::PutObjectByStreamResp resp;
        qcloud_cos::CosResult result = cos.PutObject(req, &resp);

        std::cout << "===================PutObjectResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "=========================================================" << std::endl;
    }

    // 13.5 简单上传(2g大文件)
    {
        qcloud_cos::PutObjectByFileReq req(bucket_name,
                                     "sevenyou_0803_2g", "/data/sevenyou/cos-cpp-sdk-26/testdata/seven_2g.tmp");
        qcloud_cos::PutObjectByFileResp resp;
        qcloud_cos::CosResult result = cos.PutObject(req, &resp);

        std::cout << "===================PutObjectResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "=========================================================" << std::endl;
    }

    // 14. Object属性查询
    {
        std::string object_name = "sevenyou_10m";
        qcloud_cos::HeadObjectReq req(bucket_name, object_name);
        qcloud_cos::HeadObjectResp resp;

        qcloud_cos::CosResult result = cos.HeadObject(req, &resp);

        std::cout << "===================HeadObjectResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "==========================================================" << std::endl;
    }

    // 15.1 下载文件
    {
        std::string object_name = "sevenyou_10m";
        qcloud_cos::GetObjectByFileReq req(bucket_name,
                                           object_name, "/data/sevenyou/temp/sevenyou_10m_download_03");
        qcloud_cos::GetObjectByFileResp resp;

        qcloud_cos::CosResult result = cos.GetObject(req, &resp);
        std::cout << "===================GetObjectResponse=====================";
        PrintResult(result, resp);
        std::cout << "=========================================================";
    }

    // 15.2 下载到输出流
    {
        std::string object_name = "sevenyou_10m";
        std::ostringstream os;
        qcloud_cos::GetObjectByStreamReq req(bucket_name,
                                             object_name, os);
        qcloud_cos::GetObjectByStreamResp resp;

        qcloud_cos::CosResult result = cos.GetObject(req, &resp);
        std::cout << "===================GetObjectResponse=====================";
        PrintResult(result, resp);
        std::cout << "=========================================================";
        std::cout << os.str() << std::endl;
    }

    // 15.3 下载文件(多线程分块上传, 可通过request设置线程池大小为1即可, 默认是按配置文件)
    {
        std::string object_name = "sevenyou_multipart_0831";
        qcloud_cos::MultiGetObjectReq req(bucket_name,
                                     object_name, "./sevenyou_10m");
        qcloud_cos::MultiGetObjectResp resp;

        qcloud_cos::CosResult result = cos.GetObject(req, &resp);
        std::cout << "===================GetObjectResponse=====================";
        PrintResult(result, resp);
        std::cout << "=========================================================";
    }

    std::string upload_id;
    // 16. Multipart
    {
        std::string object_name = "sevenyou_multipart_0830";
        std::vector<uint64_t> part_numbers;
        std::vector<std::string> etags;
        // 16.1 Initiate Multipart Upload
        {
            qcloud_cos::InitMultiUploadReq req(bucket_name, object_name);
            qcloud_cos::InitMultiUploadResp resp;
            qcloud_cos::CosResult result = cos.InitMultiUpload(req, &resp);

            std::cout << "=====================InitMultiUpload=====================";
            PrintResult(result, resp);
            std::cout << "=========================================================";

            upload_id = resp.GetUploadId();
        }

        // 16.2 Upload Part
        {
            // partNumber 1
            {
                std::fstream is("/data/sevenyou/temp/seven_0821_5M.part1");
                qcloud_cos::UploadPartDataReq req(bucket_name, object_name,
                                                  upload_id, is);
                req.SetPartNumber(1);
                qcloud_cos::UploadPartDataResp resp;
                qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);

                std::cout << "etag=" << resp.GetEtag() << std::endl;
                etags.push_back(resp.GetEtag());
                part_numbers.push_back(1);

                std::cout << "======================UploadPartData=====================";
                PrintResult(result, resp);
                std::cout << "=========================================================";
                is.close();
            }

#if 0
            {
                qcloud_cos::AbortMultiUploadReq req(bucket_name, object_name,
                                                    upload_id);
                qcloud_cos::AbortMultiUploadResp resp;
                qcloud_cos::CosResult result = cos.AbortMultiUpload(req, &resp);
                std::cout << "======================AbortUploadPart=====================";
                PrintResult(result, resp);
                std::cout << "=========================================================";
            }
#endif

            // partNumber 2
            {
                std::fstream is("/data/sevenyou/temp/seven_0821_5M.part2");
                qcloud_cos::UploadPartDataReq req(bucket_name, object_name,
                                                  upload_id, is);
                req.SetPartNumber(2);
                qcloud_cos::UploadPartDataResp resp;
                qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);

                etags.push_back(resp.GetEtag());
                part_numbers.push_back(2);

                std::cout << "======================UploadPartData=====================";
                PrintResult(result, resp);
                std::cout << "=========================================================";
                is.close();
            }
        }

        // 16.3 Complete
        {
            qcloud_cos::CompleteMultiUploadReq req(bucket_name, object_name, upload_id);
            qcloud_cos::CompleteMultiUploadResp resp;
            req.SetEtags(etags);
            req.SetPartNumbers(part_numbers);

            qcloud_cos::CosResult result = cos.CompleteMultiUpload(req, &resp);

            std::cout << "===================Complete=============================" << std::endl;
            PrintResult(result, resp);
            std::cout << "========================================================" << std::endl;
        }
    }

    // 17.
    {
        std::string object_name = "sevenyou_multipart_20170831_2G";
        qcloud_cos::MultiUploadObjectReq req(bucket_name,
                                             object_name, "/data/sevenyou/temp/seven_0821_2G");
        qcloud_cos::MultiUploadObjectResp resp;
        qcloud_cos::CosResult result = cos.MultiUploadObject(req, &resp);

        if (result.IsSucc()) {
            std::cout << resp.GetLocation() << std::endl;
            std::cout << resp.GetKey() << std::endl;
            std::cout << resp.GetBucket() << std::endl;
            std::cout << resp.GetEtag() << std::endl;
        } else {
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

    // 18. ListParts
    {
        qcloud_cos::ListPartsReq req(bucket_name, "sevenyou_multipart_0830", upload_id);
        req.SetMaxParts(1);
        req.SetPartNumberMarker("1");
        qcloud_cos::ListPartsResp resp;
        qcloud_cos::CosResult result = cos.ListParts(req, &resp);

        std::cout << "===================ListParts=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 19.1 设置ACL配置(通过Body, 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
    {
        qcloud_cos::PutObjectACLReq req(bucket_name, "sevenyou_10m");
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

    // 19.2 设置ACL配置(通过Header, 设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
    {
        qcloud_cos::PutObjectACLReq req(bucket_name, "sevenyou_10m");
        req.SetXCosAcl("public-read-write");

        qcloud_cos::PutObjectACLResp resp;
        qcloud_cos::CosResult result = cos.PutObjectACL(req, &resp);
        std::cout << "===================PutObjectACLResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 20. 获取ACL配置
    {
        qcloud_cos::GetObjectACLReq req(bucket_name, "sevenyou_10m");
        qcloud_cos::GetObjectACLResp resp;
        qcloud_cos::CosResult result = cos.GetObjectACL(req, &resp);

        std::cout << "===================GetObjectACLResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "====================================================================" << std::endl;
    }

    // 21. DeleteBucket
    {
        qcloud_cos::DeleteBucketReq req(bucket_name);
        qcloud_cos::DeleteBucketResp resp;
        qcloud_cos::CosResult result = cos.DeleteBucket(req, &resp);

        std::cout << "===================DeleteBucketResponse=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "=========================================================" << std::endl;
    }
}

