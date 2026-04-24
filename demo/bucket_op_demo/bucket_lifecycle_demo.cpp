#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行存储桶生命周期的设置、查询与删除
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

// 设置存储桶生命周期
void PutBucketLifecycleDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutBucketLifecycleReq req(bucket_name);
    {
        qcloud_cos::LifecycleRule rule;
        rule.SetIsEnable(true);
        rule.SetId("lifecycle_rule00");
        qcloud_cos::LifecycleFilter filter;
        filter.SetPrefix("sevenyou_e1");
        rule.SetFilter(filter);
        qcloud_cos::LifecycleTransition transition;
        transition.SetDays(30);
        transition.SetStorageClass("Standard_IA");
        rule.AddTransition(transition);
        req.AddRule(rule);
    }

    qcloud_cos::PutBucketLifecycleResp resp;
    qcloud_cos::CosResult result = cos.PutBucketLifecycle(req, &resp);
    std::cout << "===================PutBucketLifecycleResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=================================================================" << std::endl;
}

// 查询存储桶生命周期
void GetBucketLifecycleDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketLifecycleReq req(bucket_name);
    qcloud_cos::GetBucketLifecycleResp resp;
    qcloud_cos::CosResult result = cos.GetBucketLifecycle(req, &resp);

    const std::vector<qcloud_cos::LifecycleRule>& rules = resp.GetRules();
    for (size_t idx = 0; idx != rules.size(); ++idx) {
        std::cout << "id = " << rules[idx].GetId() << std::endl;
    }

    std::cout << "===================GetBucketLifecycleResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=================================================================" << std::endl;
}

// 删除存储桶生命周期
void DeleteBucketLifecycleDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::DeleteBucketLifecycleReq req(bucket_name);
    qcloud_cos::DeleteBucketLifecycleResp resp;
    qcloud_cos::CosResult result = cos.DeleteBucketLifecycle(req, &resp);

    std::cout << "===================DeleteBucketLifecycleResponse==================" << std::endl;
    PrintResult(result, resp);
    std::cout << "==================================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketLifecycleDemo(cos);
        GetBucketLifecycleDemo(cos);
        DeleteBucketLifecycleDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
