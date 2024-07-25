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
 * 本样例演示了如何使用 COS C++ SDK 进行对象的复制和移动
 * 包括：桶内复制、高级对象复制、桶内移动、用户自行分块复制
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

/*
 * 该 Demo 示范如何拷贝一个对象至当前 CosAPI 指定的存储桶中
 * 最大支持拷贝不超过5GB的对象，支持跨区域复制
 */
void PutObjectCopyDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test_dst.txt";                                        // 复制目标对象名称
    std::string source = bucket_name + ".cos." + region + ".myqcloud.com/test.txt";  // 复制源对象
    qcloud_cos::PutObjectCopyReq req(bucket_name, object_name);
    req.SetXCosCopySource(source);
    qcloud_cos::PutObjectCopyResp resp;
    qcloud_cos::CosResult result = cos.PutObjectCopy(req, &resp);

    std::cout << "===================PutObjectCopyResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "============================================================" << std::endl;
}

/*
 * 该 Demo 示范如何使用高级复制接口拷贝对象至当前 CosAPI 指定的存储桶中
 * 该接口封装了简单拷贝和分块拷贝，根据文件大小智能的选择拷贝对象的方式
 * 支持大对象，支持跨区域复制。推荐调用此接口
 * 通过全局可以设置分块拷贝分块大小
 */
void CopyDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test_dst.txt";                                        // 复制后对象名称
    std::string source = bucket_name + ".cos." + region + ".myqcloud.com/test_src.txt"; ;  // 复制源对象

    // 设置分块拷贝的分块大小，默认20MB，最大支持5GB，最小支持1MB
    // 此配置是全局性配置，主动设置后，后续初始化的所有CopyReq，都使用此配置
    CosSysConfig::SetUploadCopyPartSize(20 * 1024 * 1024); 
    
    qcloud_cos::CopyReq req(bucket_name, object_name);
    qcloud_cos::CopyResp resp;

    req.SetXCosCopySource(source);
    qcloud_cos::CosResult result = cos.Copy(req, &resp);

    std::cout << "===========================Copy=============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "============================================================" << std::endl;
}

/*
 * 该 Demo 示范如何在当前CosAPI的存储桶中移动一个对象
 * 支持大对象移动，但仅支持当前存储桶
 * 如需要移动跨桶对象，请使用 PutObjectCopy 或 Copy 接口复制后删除原文件
 */
void MoveObjectDemo(qcloud_cos::CosAPI& cos) {
    std::string src_object = "test_src.txt";
    std::string dst_object = "test_dst.txt";
    qcloud_cos::MoveObjectReq req(bucket_name, src_object, dst_object);

    qcloud_cos::CosResult result = cos.MoveObject(req);

    std::cout << "========================MoveObject==========================" << std::endl;
    if (result.IsSucc()) {
        std::cout << "MoveObject Succ." << std::endl;
    } else {
        std::cout << "MoveObject Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "============================================================" << std::endl;
}

/*
 * 该 Demo 示范用户如何自行组合分块操作拷贝对象到目标存储桶
 * 分块操作基于初始化、拷贝分块、完成分块三个接口实现将整个源对象切分为多个分块，然后再将这些分块拷贝到目标存储桶中
 * 支持大对象，支持跨区域，其中每个分块最大支持 5GB，最小支持 1MB，最后一个分块可以小于 1MB
 */
void CopyPartDemo(qcloud_cos::CosAPI& cos) {
    std::string dst_object = "test_dst.txt";
    std::string source = bucket_name + ".cos." + region + ".myqcloud.com/test_src.txt";  // 复制源对象

    // 1. InitMultiUpload
    qcloud_cos::InitMultiUploadReq init_req(bucket_name, dst_object);
    qcloud_cos::InitMultiUploadResp init_resp;
    qcloud_cos::CosResult init_result = cos.InitMultiUpload(init_req, &init_resp);

    std::cout << "=====================InitMultiUpload=====================" << std::endl;
    PrintResult(init_result, init_resp);
    std::cout << "=========================================================" << std::endl;

    // 2. UploadPartCopyData
    // UploadPartCopyData 部分，可以根据实际选择分块数量和每次分块的 range，这里以 2 个分块为例
    std::vector<std::string> etags;
    std::vector<uint64_t> part_numbers;
    std::string upload_id = init_resp.GetUploadId();
    {
        uint64_t part_number = 1;
        qcloud_cos::UploadPartCopyDataReq req(bucket_name, dst_object, upload_id, part_number);
        req.SetXCosCopySource(source);
        req.SetXCosCopySourceRange("bytes=0-104857599");  // 可根据实际选择 range
        qcloud_cos::UploadPartCopyDataResp resp;
        qcloud_cos::CosResult result = cos.UploadPartCopyData(req, &resp);
        std::cout << "==================UploadPartCopyDataResp1=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "==============================================================" << std::endl;
        if (result.IsSucc()) {
            part_numbers.push_back(1);
            etags.push_back(resp.GetEtag());
        }
    }
    {
        uint64_t part_number = 2;
        qcloud_cos::UploadPartCopyDataReq req(bucket_name, dst_object, upload_id, part_number);
        req.SetXCosCopySource(source);
        req.SetXCosCopySourceRange("bytes=104857600-209715199");  // 可根据实际选择 range
        qcloud_cos::UploadPartCopyDataResp resp;
        qcloud_cos::CosResult result = cos.UploadPartCopyData(req, &resp);

        std::cout << "==================UploadPartCopyDataResp2=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "==============================================================" << std::endl;
        if (result.IsSucc()) {
            part_numbers.push_back(2);
            etags.push_back(resp.GetEtag());
        }
    }

    // 3. Complete
    CompleteMultiUploadReq comp_req(bucket_name, dst_object, upload_id);
    CompleteMultiUploadResp comp_resp;

    comp_req.SetEtags(etags);
    comp_req.SetPartNumbers(part_numbers);

    qcloud_cos::CosResult result = cos.CompleteMultiUpload(comp_req, &comp_resp);

    std::cout << "===================Complete=============================" << std::endl;
    PrintResult(result, comp_resp);
    std::cout << "========================================================" << std::endl;

    return;
}
int main() {
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    PutObjectCopyDemo(cos);
    CopyDemo(cos);
    MoveObjectDemo(cos);
    CopyPartDemo(cos);
}