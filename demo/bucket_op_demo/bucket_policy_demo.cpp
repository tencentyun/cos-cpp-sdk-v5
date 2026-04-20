#include <stdlib.h>

#include <iostream>
#include <string>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行存储桶访问策略(Policy)的设置、查询与删除
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

// 设置存储桶访问策略
void PutBucketPolicyDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutBucketPolicyReq req(bucket_name);
    qcloud_cos::PutBucketPolicyResp resp;
    std::string bucket_policy =
        "  {"
        "    \"Statement\": ["
        "      {"
        "        \"Principal\": {"
        "          \"qcs\": ["
        "            \"qcs::cam::uin/xxx:uin/xxxx\""  // 替换成您想授予权限的账户 uin / 子用户 uin
        "          ]\n"
        "        },\n"
        "        \"Effect\": \"allow\","
        "        \"Action\": ["
        "          \"cos:PutObject\""
        "        ],\n"
        "        \"Resource\": ["  // 这里改成允许的路径前缀
        "          \"qcs::cos:ap-guangzhou:uid/appid:backetname-appid/exampleobject\""
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

    std::cout << "===================PutBucketPolicy====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "======================================================" << std::endl;
}

// 查询存储桶访问策略
void GetBucketPolicyDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketPolicyReq req(bucket_name);
    qcloud_cos::GetBucketPolicyResp resp;
    qcloud_cos::CosResult result = cos.GetBucketPolicy(req, &resp);

    std::cout << resp.GetPolicy() << std::endl;
    std::cout << "===================GetBucketPolicy====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "======================================================" << std::endl;
}

// 删除存储桶访问策略
void DeleteBucketPolicyDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::DeleteBucketPolicyReq req(bucket_name);
    qcloud_cos::DeleteBucketPolicyResp resp;
    qcloud_cos::CosResult result = cos.DeleteBucketPolicy(req, &resp);

    std::cout << "===================DeleteBucketPolicy====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketPolicyDemo(cos);
        GetBucketPolicyDemo(cos);
        DeleteBucketPolicyDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
