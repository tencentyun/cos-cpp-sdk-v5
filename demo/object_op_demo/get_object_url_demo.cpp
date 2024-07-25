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

/**
 * 本样例演示了如何使用 COS C++ SDK 获取对象链接
 * 包括：预签名链接、对象访问 URL
 */
using namespace qcloud_cos;

uint64_t appid = 12500000000;
std::string tmp_secret_id = "AKIDXXXXXXXX";
std::string tmp_secret_key = "1A2Z3YYYYYYYYYY";
std::string region = "ap-guangzhou";
std::string bucket_name = "examplebucket-12500000000";
std::string tmp_token = "token";

/*
 * 本方法包含调用是否正常的判断，和请求结果的输出
 * 可通过本方法判断是否请求成功，并输出结果信息
 */
void PrintResult(const qcloud_cos::CosResult& result, const qcloud_cos::BaseResp& resp) {
    if (result.IsSucc()) {
        std::cout << "Request Succ." << std::endl;
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

/*
 * 通过参数形式初始化 CosAPI 对象
 */
qcloud_cos::CosAPI InitCosAPI() {
    qcloud_cos::CosConfig config(appid, tmp_secret_id, tmp_secret_key, region);
    config.SetTmpToken(tmp_token);  // 推荐使用临时密钥初始化 CosAPI 对象, 如果您使用永久密钥初始化 CosAPI 对象，请注释
    qcloud_cos::CosAPI cos_tmp(config);
    return cos_tmp;
}

void GeneratePresignedUrlDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    qcloud_cos::GeneratePresignedUrlReq req(bucket_name, object_name, qcloud_cos::HTTP_GET);  // 可设置请求方法
    // 下方代码块可选则是否设置。默认签名生效的开始时间是本地当前时间，默认有效期 60s
    {
        req.SetHttps(); // 是否为 https
        uint64_t start_time_in_s = time(NULL);
        req.SetStartTimeInSec(start_time_in_s);  // 设置签名生效的开始时间
        req.SetExpiredTimeInSec(60);             // 设置签名的有效期
    }
    std::string presigned_url = cos.GeneratePresignedUrl(req);

    std::cout << "===================GeneratePresignedUrl=====================" << std::endl;
    std::cout << "Presigend Url=[" << presigned_url << "]" << std::endl;
    std::cout << "============================================================" << std::endl;
}

void GetObjectUrlDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    bool is_https = true; // 是否为 https
    std::string object_url = cos.GetObjectUrl(bucket_name, object_name, is_https);

    std::cout << "=======================GetObjectUrl=========================" << std::endl;
    std::cout << "object url=[" << object_url << "]" << std::endl;
    std::cout << "============================================================" << std::endl;
}

int main() {
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    GeneratePresignedUrlDemo(cos);
    GetObjectUrlDemo(cos);
}