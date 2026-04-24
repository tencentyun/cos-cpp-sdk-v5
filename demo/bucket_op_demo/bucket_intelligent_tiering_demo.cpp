#include <stdlib.h>

#include <iostream>
#include <string>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 开启/查询存储桶智能分层配置
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

// 开启存储桶智能分层
void PutBucketIntelligentTieringDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutBucketIntelligentTieringReq req(bucket_name);
    qcloud_cos::PutBucketIntelligentTieringResp resp;
    std::cout << "===================PutIntelligentTiering===================" << std::endl;
    req.SetStatus(true);
    req.SetDays(60);
    qcloud_cos::CosResult result = cos.PutBucketIntelligentTiering(req, &resp);
    PrintResult(result, resp);
    std::cout << "===========================================================" << std::endl;
}

// 查询存储桶智能分层配置
void GetBucketIntelligentTieringDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketIntelligentTieringReq req(bucket_name);
    qcloud_cos::GetBucketIntelligentTieringResp resp;
    std::cout << "===================GetIntelligentTiering===================" << std::endl;
    qcloud_cos::CosResult result = cos.GetBucketIntelligentTiering(req, &resp);
    PrintResult(result, resp);
    if (result.IsSucc()) {
        std::cout << "Status: " << resp.GetStatus() << std::endl;
        std::cout << "Days: " << resp.GetDays() << std::endl;
        std::cout << "RequestFrequent: " << resp.GetRequestFrequent() << std::endl;
    }
    std::cout << "===========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketIntelligentTieringDemo(cos);
        GetBucketIntelligentTieringDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
