#include <stdlib.h>

#include <iostream>
#include <string>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 配置与查询存储桶日志投递
 */
using namespace qcloud_cos;

uint64_t appid = 12500000000;
std::string tmp_secret_id = "AKIDXXXXXXXX";
std::string tmp_secret_key = "1A2Z3YYYYYYYYYY";
std::string region = "ap-guangzhou";
std::string bucket_name = "examplebucket-12500000000";
std::string tmp_token = "token";
// 替换为您的日志投递目标存储桶名和前缀
std::string target_bucket_name = "targetlog-12500000000";
std::string target_prefix = "log/";

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

// 开启存储桶日志投递
void PutBucketLoggingDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutBucketLoggingReq req(bucket_name);
    qcloud_cos::LoggingEnabled rules;
    rules.SetTargetBucket(target_bucket_name);
    rules.SetTargetPrefix(target_prefix);

    req.SetLoggingEnabled(rules);
    qcloud_cos::PutBucketLoggingResp resp;
    qcloud_cos::CosResult result = cos.PutBucketLogging(req, &resp);

    std::cout << "===================PutBucketLoggingResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "===============================================================" << std::endl;
}

// 查询存储桶日志投递配置
void GetBucketLoggingDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketLoggingReq req(bucket_name);
    qcloud_cos::GetBucketLoggingResp resp;
    qcloud_cos::CosResult result = cos.GetBucketLogging(req, &resp);

    std::cout << "===================GetBucketLoggingResponse====================" << std::endl;

    const qcloud_cos::LoggingEnabled loggingenabled = resp.GetLoggingEnabled();
    std::cout << "TargetBucket = " << loggingenabled.GetTargetBucket() << std::endl;
    std::cout << "TargetPrefix = " << loggingenabled.GetTargetPrefix() << std::endl;
    PrintResult(result, resp);
    std::cout << "===============================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketLoggingDemo(cos);
        GetBucketLoggingDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
