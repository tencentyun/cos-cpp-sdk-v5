#include <stdlib.h>

#include <iostream>
#include <string>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 设置与查询存储桶 Referer 防盗链
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

// 配置存储桶 Referer
void PutBucketRefererDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutBucketRefererReq req(bucket_name);
    qcloud_cos::PutBucketRefererResp resp;
    req.SetStatus("Enabled");
    req.SetRefererType("White-List");
    req.AddDomain("test1.com");
    req.AddDomain("test2.com");
    qcloud_cos::CosResult result = cos.PutBucketReferer(req, &resp);
    std::cout << "===================PutBucketReferer====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=======================================================" << std::endl;
}

// 获取存储桶 Referer
void GetBucketRefererDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketRefererReq req(bucket_name);
    qcloud_cos::GetBucketRefererResp resp;
    qcloud_cos::CosResult result = cos.GetBucketReferer(req, &resp);
    std::cout << "===================GetBucketReferer====================" << std::endl;
    PrintResult(result, resp);
    if (result.IsSucc()) {
        std::cout << "Status: " << resp.GetStatus() << std::endl;
        std::cout << "RefererType: " << resp.GetRefererType() << std::endl;
        for (const auto& domain : resp.GetDomainList()) {
            std::cout << "Domain: " << domain << std::endl;
        }
        std::cout << "EmptyReferConfiguration: " << resp.GetEmptyReferConf() << std::endl;
    }
    std::cout << "=======================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketRefererDemo(cos);
        GetBucketRefererDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
