#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 的限速能力，涵盖：
 *   - 简单上传限速      : PutObjectByFileLimitTraffic
 *   - 简单下载限速      : GetObjectByFileLimitTraffic
 *   - 多线程下载限速    : MultiGetObjectLimitTraffic
 *   - 分块上传限速      : UploadPartDataLimitTraffic
 *   - 多线程上传限速    : MultiUploadObjectLimitTraffic
 */
using namespace qcloud_cos;

uint64_t appid = 12500000000;
std::string tmp_secret_id = "AKIDXXXXXXXX";
std::string tmp_secret_key = "1A2Z3YYYYYYYYYY";
std::string region = "ap-guangzhou";
std::string bucket_name = "examplebucket-12500000000";
std::string tmp_token = "token";

// 限速参数，单位 bit/s。范围: 819200 (100KB/s) ~ 838860800 (100MB/s)
uint64_t traffic_limit = 1024 * 1024 * 8;  // 1MB/s

std::string object_name = "test/limit_traffic_object";
std::string local_upload_file = "./temp/testfile";
std::string local_download_file = "./temp/testfile.down";

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

// 简单上传限速
void PutObjectByFileLimitTrafficDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutObjectByFileReq req(bucket_name, object_name, local_upload_file);
    req.SetTrafficLimit(traffic_limit);
    qcloud_cos::PutObjectByFileResp resp;
    qcloud_cos::CosResult result = cos.PutObject(req, &resp);
    std::cout << "===================PutObjectByFileLimitTraffic==========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 简单下载限速
void GetObjectByFileLimitTrafficDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetObjectByFileReq req(bucket_name, object_name, local_download_file);
    req.SetTrafficLimit(traffic_limit);
    qcloud_cos::GetObjectByFileResp resp;
    qcloud_cos::CosResult result = cos.GetObject(req, &resp);
    std::cout << "===================GetObjectByFileLimitTraffic==========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 多线程下载限速
void MultiGetObjectLimitTrafficDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::MultiGetObjectReq req(bucket_name, object_name, local_download_file);
    req.SetTrafficLimit(traffic_limit);
    qcloud_cos::MultiGetObjectResp resp;
    qcloud_cos::CosResult result = cos.MultiGetObject(req, &resp);
    std::cout << "===================MultiGetObjectLimitTraffic===========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 分块上传限速（需要先 InitMultiUpload 获取 upload_id, 此处演示函数用法，需根据实际流程组合调用）
void UploadPartDataLimitTrafficDemo(qcloud_cos::CosAPI& cos,
                                    const std::string& upload_id, std::fstream& is,
                                    uint64_t number, std::string* etag) {
    qcloud_cos::UploadPartDataReq req(bucket_name, object_name, upload_id, is);
    req.SetPartNumber(number);
    req.SetTrafficLimit(traffic_limit);
    qcloud_cos::UploadPartDataResp resp;
    qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);
    *etag = resp.GetEtag();

    std::cout << "======================UploadPartDataLimitTraffic========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 多线程上传限速
void MultiUploadObjectLimitTrafficDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::MultiPutObjectReq req(bucket_name, object_name, local_upload_file);
    req.SetTrafficLimit(traffic_limit);
    req.SetRecvTimeoutInms(1000 * 60);
    qcloud_cos::MultiPutObjectResp resp;
    qcloud_cos::CosResult result = cos.MultiPutObject(req, &resp);

    if (result.IsSucc()) {
        std::cout << "MultiUpload Succ." << std::endl;
        std::cout << resp.GetLocation() << std::endl;
        std::cout << resp.GetKey() << std::endl;
        std::cout << resp.GetBucket() << std::endl;
        std::cout << resp.GetEtag() << std::endl;
    } else {
        std::string resp_tag = resp.GetRespTag();
        std::cout << "MultiUpload Fail, respond tag " << resp_tag << std::endl;
    }
    std::cout << "===================MultiUploadObjectLimitTraffic========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutObjectByFileLimitTrafficDemo(cos);
        GetObjectByFileLimitTrafficDemo(cos);
        MultiGetObjectLimitTrafficDemo(cos);
        MultiUploadObjectLimitTrafficDemo(cos);
        // UploadPartDataLimitTrafficDemo 需要 upload_id，请与 multi_upload_object_demo 组合使用
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
