#include <stdlib.h>

#include <iostream>
#include <string>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 创建、查询、列举与删除存储桶清单任务
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

// 在存储桶中创建清单任务
void PutBucketInventoryDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutBucketInventoryReq req(bucket_name);
    req.SetId("list3");

    COSBucketDestination destination;
    destination.SetFormat("CSV");
    destination.SetAccountId("appid");
    std::string target_bucket = "qcs::cos:ap-guangzhou::xxxxxx-appid";
    destination.SetBucket(target_bucket);
    destination.SetPrefix("/");
    destination.SetEncryption(true);

    OptionalFields fields;
    fields.SetIsSize(true);
    fields.SetIsLastModified(true);
    fields.SetIsStorageClass(true);
    fields.SetIsMultipartUploaded(true);
    fields.SetIsReplicationStatus(true);
    fields.SetIsEtag(true);

    Inventory inventory;
    inventory.SetIsEnable(true);
    inventory.SetIncludedObjectVersions("All");
    inventory.SetFilter("/");
    inventory.SetId(req.GetId());
    inventory.SetFrequency("Daily");
    inventory.SetCOSBucketDestination(destination);
    inventory.SetOptionalFields(fields);

    req.SetInventory(inventory);

    qcloud_cos::PutBucketInventoryResp resp;
    qcloud_cos::CosResult result = cos.PutBucketInventory(req, &resp);
    std::cout << "===================PutBucketInventory===================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 查询存储桶中用户的清单任务信息
void GetBucketInventoryDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketInventoryReq req(bucket_name);
    qcloud_cos::GetBucketInventoryResp resp;
    req.SetId("list3");

    qcloud_cos::CosResult result = cos.GetBucketInventory(req, &resp);
    std::cout << "===================GetBucketInventory===================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 列举存储桶中所有清单任务
void ListBucketInventoryConfigurationsDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::ListBucketInventoryConfigurationsReq req(bucket_name);
    qcloud_cos::ListBucketInventoryConfigurationsResp resp;

    qcloud_cos::CosResult result = cos.ListBucketInventoryConfigurations(req, &resp);
    std::cout << "===================ListBucketInventoryConfigurations===========" << std::endl;
    PrintResult(result, resp);
    std::cout << "================================================================" << std::endl;
}

// 删除存储桶中指定的清单任务
void DeleteBucketInventoryDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::DeleteBucketInventoryReq req(bucket_name);
    qcloud_cos::DeleteBucketInventoryResp resp;
    req.SetId("list");
    qcloud_cos::CosResult result = cos.DeleteBucketInventory(req, &resp);
    std::cout << "===================DeleteBucketInventory===================" << std::endl;
    PrintResult(result, resp);
    std::cout << "===========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketInventoryDemo(cos);
        GetBucketInventoryDemo(cos);
        ListBucketInventoryConfigurationsDemo(cos);
        DeleteBucketInventoryDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
