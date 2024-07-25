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
 * 本样例演示了如何使用 COS C++ SDK 进行简单下载和列出
 * 包括：下载到本地文件、下载到流、列出桶下的文件
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

void GetObjectByFileDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test_src.txt";
    std::string file_path = "./test_file/text2.txt";
    qcloud_cos::GetObjectByFileReq req(bucket_name, object_name, file_path);
    // 限速上传对象，默认单位为 bit/s，限速值设置范围为 819200 - 838860800
    // uint64_t traffic_limit = 0;
    // req.SetTrafficLimit(traffic_limit);
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = cos.GetObject(req, &resp);
    std::cout << "===================GetObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void GetObjectByStreamDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    std::ostringstream os;
    qcloud_cos::GetObjectByStreamReq req(bucket_name, object_name, os);
    // 限速上传对象，默认单位为 bit/s，限速值设置范围为 819200 - 838860800
    // uint64_t traffic_limit = 0;
    // req.SetTrafficLimit(traffic_limit);
    qcloud_cos::GetObjectByStreamResp resp;

    qcloud_cos::CosResult result = cos.GetObject(req, &resp);
    std::cout << "===================GetObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
    std::cout << os.str() << std::endl;
}

void GetBucketDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketReq req(bucket_name);
    // 设置列出的对象名以 prefix 为前缀
    req.SetPrefix("test");
    // 设置最大列出多少个对象, 一次 listobject 最大支持1000
    req.SetMaxKeys(10);
    qcloud_cos::GetBucketResp resp;
    qcloud_cos::CosResult result = cos.GetBucket(req, &resp);

    std::cout << "===================GetBucketResponse=====================" << std::endl;
    if (result.IsSucc()) {
        // object contents 表示此次列出的对象列表
        std::vector<Content> contents = resp.GetContents();
        for (const Content& content : contents) {
            // 对象的 key
            std::string key = content.m_key;
            // 对象的 etag
            std::string etag = content.m_etag;
            // 对象的长度
            std::string file_size = content.m_size;
            // 对象的存储类型
            std::string storage_classes = content.m_storage_class;
            std::cout << "key:" << key << "\netag:" << etag << "\nfile_size:" << file_size << "\nstorage_classes" << storage_classes << std::endl;
            std::cout << "==================================" << std::endl;
        }

        if (resp.IsTruncated()) {
            // 表示还没有列完，被截断了

            // 下一次开始的位置
            std::string next_marker = resp.GetNextMarker();
            std::cout << "next_marker:" << next_marker << std::endl;
        }
    } else {
        std::cout << "GetBucket Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }

    std::cout << "=========================================================" << std::endl;
}

int main() {
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    GetObjectByFileDemo(cos);
    GetObjectByStreamDemo(cos);
    GetBucketDemo(cos);
}