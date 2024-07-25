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
 * 本样例演示了如何使用 COS C++ SDK 进行对象标签的设置、获取和删除
 * 包括：设置标签、获取标签、删除标签
 */
using namespace qcloud_cos;

uint64_t appid = 12500000000;
std::string tmp_secret_id = "AKIDXXXXXXXX";
std::string tmp_secret_key = "1A2Z3YYYYYYYYYY";
std::string region = "ap-guangzhou";
std::string bucket_name = "examplebucket-12500000000";
std::string tmp_token = "token";
std::string uin = "23300000000";

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

void PutObjectTaggingDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    qcloud_cos::PutObjectTaggingReq req(bucket_name, object_name);
    qcloud_cos::PutObjectTaggingResp resp;
    std::vector<Tag> tagset;
    Tag tag1;
    tag1.SetKey("age");
    tag1.SetValue("19");

    Tag tag2;
    tag2.SetKey("name");
    tag2.SetValue("xiaoming");

    Tag tag3;
    tag3.SetKey("sex");
    tag3.SetValue("male");

    tagset.push_back(tag1);
    tagset.push_back(tag2);
    tagset.push_back(tag3);
    req.SetTagSet(tagset);

    qcloud_cos::CosResult result = cos.PutObjectTagging(req, &resp);
    std::cout << "=========================PutBucketTagging===========================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void GetObjectTaggingDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    qcloud_cos::GetObjectTaggingReq req(bucket_name, object_name);
    qcloud_cos::GetObjectTaggingResp resp;

    qcloud_cos::CosResult result = cos.GetObjectTagging(req, &resp);
    std::cout << "=========================GetObjectTagging===========================" << std::endl;
    std::vector<Tag> tagset = resp.GetTagSet();
    for (std::vector<Tag>::iterator it = tagset.begin(); it != tagset.end(); ++it) {
        std::cout << it->GetKey() << ":" << it->GetValue() << std::endl;
    }
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

void DeleteObjectTaggingDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    qcloud_cos::DeleteObjectTaggingReq req(bucket_name, object_name);
    qcloud_cos::DeleteObjectTaggingResp resp;

    qcloud_cos::CosResult result = cos.DeleteObjectTagging(req, &resp);
    std::cout << "=======================DeleteObjectTagging==========================" << std::endl;
    PrintResult(result, resp);
    std::cout << "====================================================================" << std::endl;
}

int main() {
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    PutObjectTaggingDemo(cos);
    GetObjectTaggingDemo(cos);
    DeleteObjectTaggingDemo(cos);
}