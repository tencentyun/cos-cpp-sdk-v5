#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行手动的分块上传流程
 *   - InitMultiUpload    : 初始化分块上传，获取 upload_id
 *   - UploadPartData     : 上传单个分块
 *   - ListParts          : 列举已上传的分块
 *   - AbortMultiUpload   : 取消未完成的分块上传
 *   - CompleteMultiUpload: 完成分块上传，组合成完整对象
 *   - UploadPartCopy     : 分块拷贝
 *   - 以及一个封装好的完整流程 MultiUploadDemo 和 UploadCopyDemo
 */
using namespace qcloud_cos;

uint64_t appid = 12500000000;
std::string tmp_secret_id = "AKIDXXXXXXXX";
std::string tmp_secret_key = "1A2Z3YYYYYYYYYY";
std::string region = "ap-guangzhou";
std::string bucket_name = "examplebucket-12500000000";
std::string tmp_token = "token";

// 示例用 object key 与本地文件路径
std::string object_name = "test/sevenyou_e2_multi_upload";
std::string local_file = "./temp/testfilebig";
std::string copy_source = "xxxx-appid.cos.region.myqcloud.com/objectkey";

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

// 初始化分块上传，获取 upload_id
void InitMultiUploadDemo(qcloud_cos::CosAPI& cos, const std::string& obj, std::string* upload_id) {
    qcloud_cos::InitMultiUploadReq req(bucket_name, obj);
    qcloud_cos::InitMultiUploadResp resp;
    qcloud_cos::CosResult result = cos.InitMultiUpload(req, &resp);

    std::cout << "=====================InitMultiUpload=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;

    *upload_id = resp.GetUploadId();
}

// 上传单个分块
void UploadPartDataDemo(qcloud_cos::CosAPI& cos, const std::string& obj,
                        const std::string& upload_id, std::fstream& is,
                        uint64_t number, std::string* etag) {
    qcloud_cos::UploadPartDataReq req(bucket_name, obj, upload_id, is);
    req.SetPartNumber(number);
    qcloud_cos::UploadPartDataResp resp;
    qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);
    *etag = resp.GetEtag();

    std::cout << "======================UploadPartData=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

// 列举已上传的分块
void ListPartsDemo(qcloud_cos::CosAPI& cos, const std::string& obj, const std::string& upload_id) {
    qcloud_cos::ListPartsReq req(bucket_name, obj, upload_id);
    req.SetMaxParts(1);
    req.SetPartNumberMarker("1");
    qcloud_cos::ListPartsResp resp;
    qcloud_cos::CosResult result = cos.ListParts(req, &resp);

    std::cout << "===================ListParts=============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

// 取消未完成的分块上传
void AbortMultiUploadDemo(qcloud_cos::CosAPI& cos, const std::string& obj, const std::string& upload_id) {
    qcloud_cos::AbortMultiUploadReq req(bucket_name, obj, upload_id);
    qcloud_cos::AbortMultiUploadResp resp;
    qcloud_cos::CosResult result = cos.AbortMultiUpload(req, &resp);
    std::cout << "======================AbortUploadPart====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

// 完成分块上传
void CompleteMultiUploadDemo(qcloud_cos::CosAPI& cos, const std::string& obj,
                             const std::string& upload_id,
                             const std::vector<std::string>& etags,
                             const std::vector<uint64_t>& numbers) {
    qcloud_cos::CompleteMultiUploadReq req(bucket_name, obj, upload_id);
    qcloud_cos::CompleteMultiUploadResp resp;
    req.SetEtags(etags);
    req.SetPartNumbers(numbers);

    qcloud_cos::CosResult result = cos.CompleteMultiUpload(req, &resp);

    std::cout << "===================Complete==============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

// 分块拷贝
void UploadPartCopyDemo(qcloud_cos::CosAPI& cos, const std::string& obj,
                        const std::string& upload_id, const std::string& source,
                        const std::string& range, uint64_t number,
                        std::string* etag) {
    qcloud_cos::UploadPartCopyDataReq req(bucket_name, obj, upload_id, number);
    req.SetXCosCopySource(source);
    req.SetXCosCopySourceRange(range);
    qcloud_cos::UploadPartCopyDataResp resp;
    qcloud_cos::CosResult result = cos.UploadPartCopyData(req, &resp);

    std::cout << "===================UploadPartCopyData====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;

    if (result.IsSucc()) {
        *etag = resp.GetEtag();
    }
}

// 封装好的完整多分块上传流程
void MultiUploadDemo(qcloud_cos::CosAPI& cos) {
    std::string upload_id;
    std::vector<uint64_t> numbers;
    std::vector<std::string> etags;
    std::string etag1, etag2;

    InitMultiUploadDemo(cos, object_name, &upload_id);

    std::fstream is1(local_file);
    UploadPartDataDemo(cos, object_name, upload_id, is1, 1, &etag1);
    numbers.push_back(1);
    etags.push_back(etag1);
    is1.close();

    ListPartsDemo(cos, object_name, upload_id);
    // AbortMultiUploadDemo(cos, object_name, upload_id);

    std::fstream is2(local_file);
    UploadPartDataDemo(cos, object_name, upload_id, is2, 2, &etag2);
    numbers.push_back(2);
    etags.push_back(etag2);
    is2.close();

    CompleteMultiUploadDemo(cos, object_name, upload_id, etags, numbers);
}

// 用户自定义分块拷贝的完整流程
void UploadCopyDemo(qcloud_cos::CosAPI& cos) {
    std::string upload_id;
    std::string dst_object = "test/sevenyou_200MB_part_copy_from_north";
    std::vector<uint64_t> numbers;
    std::vector<std::string> etags;
    std::string etag1, etag2;

    InitMultiUploadDemo(cos, dst_object, &upload_id);

    UploadPartCopyDemo(cos, dst_object, upload_id, copy_source, "bytes=0-104857600", 1, &etag1);
    numbers.push_back(1);
    etags.push_back(etag1);

    UploadPartCopyDemo(cos, dst_object, upload_id, copy_source, "bytes=104857600-209715200", 2, &etag2);
    numbers.push_back(2);
    etags.push_back(etag2);

    CompleteMultiUploadDemo(cos, dst_object, upload_id, etags, numbers);
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        MultiUploadDemo(cos);
        // UploadCopyDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
