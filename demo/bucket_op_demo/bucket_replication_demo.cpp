#include <stdlib.h>

#include <iostream>
#include <string>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行存储桶跨区域复制的设置、查询与删除
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

// 设置存储桶跨区域复制规则
void PutBucketReplicationDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutBucketReplicationReq req(bucket_name);
    req.SetRole("qcs::cam::uin/12345:uin/12345");  // 替换为用户 Role
    qcloud_cos::ReplicationRule rule(
        "", "qcs:id/0:cos:ap-xxx:appid/xxxxx:bucketname", "",
        "", true);  // 替换为用户 rule

    req.AddReplicationRule(rule);
    qcloud_cos::PutBucketReplicationResp resp;
    qcloud_cos::CosResult result = cos.PutBucketReplication(req, &resp);
    std::cout << "===================PutBucketReplicationResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "===================================================================" << std::endl;
}

// 查询存储桶跨区域复制规则
void GetBucketReplicationDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketReplicationReq req(bucket_name);
    qcloud_cos::GetBucketReplicationResp resp;
    qcloud_cos::CosResult result = cos.GetBucketReplication(req, &resp);

    std::cout << "===================GetBucketReplicationResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "===================================================================" << std::endl;
}

// 删除存储桶跨区域复制规则
void DeleteBucketReplicationDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::DeleteBucketReplicationReq req(bucket_name);
    qcloud_cos::DeleteBucketReplicationResp resp;
    qcloud_cos::CosResult result = cos.DeleteBucketReplication(req, &resp);

    std::cout << "===================DeleteBucketReplicationResponse=================" << std::endl;
    PrintResult(result, resp);
    std::cout << "===================================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketReplicationDemo(cos);
        GetBucketReplicationDemo(cos);
        DeleteBucketReplicationDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
