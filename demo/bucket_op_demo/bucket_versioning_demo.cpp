#include <stdlib.h>
#include <sys/stat.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"
#include "util/auth_tool.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行存储桶版本控制的设置与查询
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

// 开启/关闭存储桶版本控制
void PutBucketVersioningDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutBucketVersioningReq req(bucket_name);
    req.SetStatus(true);
    qcloud_cos::PutBucketVersioningResp resp;

    qcloud_cos::CosResult result = cos.PutBucketVersioning(req, &resp);
    std::cout << "===================PutBucketVersioningResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "==================================================================" << std::endl;
}

// 查询存储桶版本控制状态
void GetBucketVersioningDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketVersioningReq req(bucket_name);
    qcloud_cos::GetBucketVersioningResp resp;
    qcloud_cos::CosResult result = cos.GetBucketVersioning(req, &resp);

    std::cout << "Bucket status= " << resp.GetStatus() << std::endl;
    std::cout << "===================GetBucketVersioningResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "==================================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketVersioningDemo(cos);
        GetBucketVersioningDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
