#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 设置、查询与删除存储桶标签
 */
using namespace qcloud_cos;

uint64_t appid = 12500000000;
std::string tmp_secret_id = "AKIDXXXXXXXX";
std::string tmp_secret_key = "1A2Z3YYYYYYYYYY";
std::string region = "ap-guangzhou";
std::string bucket_name = "examplebucket-12500000000";
std::string tmp_token = "token";

void PrintResult(const qcloud_cos::CosResult& result, const qcloud_cos::BaseResp& resp) {
    if (result.IsSucc()) {
        std::cout << "Request Succ." << std::endl;
        std::cout << resp.DebugString() << std::endl;
    } else {
        std::cout << "ErrorMsg=" << result.GetErrorMsg() << std::endl;
        std::cout << "HttpStatus=" << result.GetHttpStatus() << std::endl;
        std::cout << "ErrorCode=" << result.GetErrorCode() << std::endl;
        std::cout << "ResourceAddr=" << result.GetResourceAddr() << std::endl;
        std::cout << "XCosRequestId=" << result.GetXCosRequestId() << std::endl;
        std::cout << "XCosTraceId=" << result.GetXCosTraceId() << std::endl;
    }
}

qcloud_cos::CosAPI InitCosAPI() {
    qcloud_cos::CosConfig config(appid, tmp_secret_id, tmp_secret_key, region);
    config.SetTmpToken(tmp_token);
    qcloud_cos::CosAPI cos_tmp(config);
    return cos_tmp;
}

// 为已存在的 Bucket 设置标签(Tag)
void PutBucketTaggingDemo(qcloud_cos::CosAPI& cos) {
    std::vector<Tag> tagset;
    Tag tag1;
    tag1.SetKey("age");
    tag1.SetValue("19");

    Tag tag2;
    tag2.SetKey("name");
    tag2.SetValue("xiaoming");
    tagset.push_back(tag1);
    tagset.push_back(tag2);

    qcloud_cos::PutBucketTaggingReq req(bucket_name);
    qcloud_cos::PutBucketTaggingResp resp;
    req.SetTagSet(tagset);

    qcloud_cos::CosResult result = cos.PutBucketTagging(req, &resp);
    std::cout << "===================PutBucketTagging====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=======================================================" << std::endl;
}

// 查询指定存储桶下已有的存储桶标签
void GetBucketTaggingDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketTaggingReq req(bucket_name);
    qcloud_cos::GetBucketTaggingResp resp;

    qcloud_cos::CosResult result = cos.GetBucketTagging(req, &resp);
    std::cout << "===================GetBucketTagging====================" << std::endl;
    std::vector<Tag> tagset = resp.GetTagSet();
    for (std::vector<Tag>::iterator it = tagset.begin(); it != tagset.end(); ++it) {
        std::cout << it->GetKey() << ":" << it->GetValue() << std::endl;
    }
    PrintResult(result, resp);
    std::cout << "=======================================================" << std::endl;
}

// 删除指定存储桶下已有的存储桶标签
void DeleteBucketTaggingDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::DeleteBucketTaggingReq req(bucket_name);
    qcloud_cos::DeleteBucketTaggingResp resp;

    qcloud_cos::CosResult result = cos.DeleteBucketTagging(req, &resp);
    std::cout << "===================DeleteBucketTagging=================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=======================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketTaggingDemo(cos);
        GetBucketTaggingDemo(cos);
        DeleteBucketTaggingDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
