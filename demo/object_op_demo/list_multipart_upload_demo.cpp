#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 列举存储桶中正在进行的分块上传任务
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

// 列举存储桶中正在进行的分块上传任务
void ListMultipartUploadDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::ListMultipartUploadReq req(bucket_name);
    qcloud_cos::ListMultipartUploadResp resp;
    qcloud_cos::CosResult result = cos.ListMultipartUpload(req, &resp);
    std::vector<Upload> rst = resp.GetUpload();

    for (std::vector<qcloud_cos::Upload>::const_iterator itr = rst.begin(); itr != rst.end(); ++itr) {
        const qcloud_cos::Upload& upload = *itr;
        std::cout << "key = " << upload.m_key << ", uploadid= " << upload.m_uploadid
                  << ", storage class = " << upload.m_storage_class
                  << ", m_initiated= " << upload.m_initiated << std::endl;
    }

    std::cout << "===================ListMultipartUpload=================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=======================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        ListMultipartUploadDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
