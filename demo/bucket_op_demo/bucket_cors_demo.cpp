#include <stdlib.h>

#include <iostream>
#include <string>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行存储桶跨域访问(CORS)的设置、查询与删除
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

// 设置存储桶 CORS 规则
void PutBucketCORSDemo(qcloud_cos::CosAPI& cos) {
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
    std::cout << "===================PutBucketCORSResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "============================================================" << std::endl;
}

// 查询存储桶 CORS 规则
void GetBucketCORSDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketCORSReq req(bucket_name);
    qcloud_cos::GetBucketCORSResp resp;
    qcloud_cos::CosResult result = cos.GetBucketCORS(req, &resp);

    std::cout << "===================GetBucketCORSResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "============================================================" << std::endl;
}

// 删除存储桶 CORS 规则
void DeleteBucketCORSDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::DeleteBucketCORSReq req(bucket_name);
    qcloud_cos::DeleteBucketCORSResp resp;
    qcloud_cos::CosResult result = cos.DeleteBucketCORS(req, &resp);

    std::cout << "===================DeleteBucketCORSResponse=================" << std::endl;
    PrintResult(result, resp);
    std::cout << "============================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketCORSDemo(cos);
        GetBucketCORSDemo(cos);
        DeleteBucketCORSDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
