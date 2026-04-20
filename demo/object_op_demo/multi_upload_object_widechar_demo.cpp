#include <stdlib.h>

#include <iostream>
#include <string>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 在 Windows 平台下使用宽字符(中文/韩文等)本地路径
 * 进行多线程分块上传。
 *
 * 核心用法：
 *   - 构造 `MultiPutObjectReq` 时 `local_file_path` 传入 UTF-8 编码的字符串；
 *   - 调用 `req.SetWideCharPath()` 通知 SDK 该路径为宽字符路径（仅 _WIN32 有效）；
 *   - 使用 `cos.MultiPutObject(req, &resp)` 发起上传。
 *
 * 注意：该示例仅在 Windows 平台下有效，其他平台直接跳过。
 */
using namespace qcloud_cos;

uint64_t appid = 12500000000;
std::string tmp_secret_id = "AKIDXXXXXXXX";
std::string tmp_secret_key = "1A2Z3YYYYYYYYYY";
std::string region = "ap-guangzhou";
std::string bucket_name = "examplebucket-12500000000";
std::string tmp_token = "token";

// 宽字符本地文件路径（示例为包含中文的路径，实际请替换为您的文件路径）
// 字符串需以 UTF-8 编码保存
std::string wide_char_local_file = "./测试目录/测试文件.bin";
std::string object_name = "test/multi_upload_wide_char_object";

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

// 使用宽字符本地路径进行多线程分块上传（仅 Windows 平台有效）
void MultiUploadObjectWideCharDemo(qcloud_cos::CosAPI& cos) {
#if defined(_WIN32)
    qcloud_cos::MultiPutObjectReq req(bucket_name, object_name, wide_char_local_file);
    // 标记本地路径为宽字符路径
    req.SetWideCharPath();
    req.SetRecvTimeoutInms(1000 * 60);
    qcloud_cos::MultiPutObjectResp resp;
    qcloud_cos::CosResult result = cos.MultiPutObject(req, &resp);

    if (result.IsSucc()) {
        std::cout << "MultiUploadObjectWideChar Succ." << std::endl;
        std::cout << "Location: " << resp.GetLocation() << std::endl;
        std::cout << "Key: " << resp.GetKey() << std::endl;
        std::cout << "Bucket: " << resp.GetBucket() << std::endl;
        std::cout << "ETag: " << resp.GetEtag() << std::endl;
    } else {
        std::string resp_tag = resp.GetRespTag();
        std::cout << "MultiUploadObjectWideChar Fail, respond tag: " << resp_tag << std::endl;
    }
    std::cout << "===================MultiUploadObjectWideChar===========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
#else
    (void)cos;  // suppress unused-parameter warning
    std::cout << "MultiUploadObjectWideCharDemo: 仅支持 Windows 平台, 当前平台已跳过."
              << std::endl;
#endif
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        MultiUploadObjectWideCharDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
