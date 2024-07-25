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
 * 本样例演示了如何使用 COS C++ SDK 进行删除对象操作
 * 包括：删除单个对象、删除指定多个对象、删除指定目录的所有对象、删除指定前缀的对象
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

void DeleteObjectDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    qcloud_cos::DeleteObjectReq req(bucket_name, object_name);
    // req.SetXCosVersionId("xxxxx");// 可以指定删除的版本
    qcloud_cos::DeleteObjectResp resp;
    qcloud_cos::CosResult result = cos.DeleteObject(req, &resp);

    std::cout << "===================DeleteObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "============================================================" << std::endl;
}

/*
 * 该 Demo 示范如何批量删除对象
 * 该方法是否成功，需要判断resp.GetErrorMsgs()是否为空
 */
void DeleteObjectsDemo(qcloud_cos::CosAPI& cos) {
    std::vector<ObjectVersionPair> to_be_deleted;
    {
        ObjectVersionPair pair;
        std::string object_name = "test_dir/audio.mp3";
        std::string version_id = "";  // 有需要可以指定删除的版本，如果为空则不指定
        to_be_deleted.push_back(ObjectVersionPair(object_name, version_id));
    }
    {
        std::string object_name = "test_dir/video.mp4";
        std::string version_id = "";  // 有需要可以指定删除的版本，如果为空则不指定
        to_be_deleted.push_back(ObjectVersionPair(object_name, version_id));
    }

    qcloud_cos::DeleteObjectsReq req(bucket_name, to_be_deleted);
    // req.SetQuiet();  // 设置为 Quiet模式， 则不返回删除成功的对象信息，默认为 Verbose 模式
    qcloud_cos::DeleteObjectsResp resp;
    qcloud_cos::CosResult result = cos.DeleteObjects(req, &resp);

    std::cout << "===================DeleteObjectsResponse=====================" << std::endl;
    std::vector<DeletedInfo> deleted_infos = resp.GetDeletedInfos();  // 单个删除成功的对象条目，仅当使用 Verbose 模式才会返回该元素
    std::vector<ErrorInfo> error_infos = resp.GetErrorMsgs();         // 单个删除失败的对象条目
    if (!error_infos.empty()) {
        std::cout << "==================Failed part message==================" << std::endl;
        for (ErrorInfo& error_info : error_infos) {
            std::cout << "key: " << error_info.m_key << "\ncode: " << error_info.m_code << "\nmessage: " << error_info.m_message << std::endl;
            std::cout << "====================================" << std::endl;
        }
    } else {
        std::cout << "DeleteObjects All Succ." << std::endl;
    }
    std::cout << "=============================================================" << std::endl;
}

void DeleteDirectoryDemo(qcloud_cos::CosAPI& cos) {
    std::string directory_name = "test_dir/";  // 目录名称，注意末尾需要有/
    DeleteObjectsByPrefixReq req(bucket_name, directory_name);
    DeleteObjectsByPrefixResp resp;
    CosResult result = cos.DeleteObjects(req, &resp);

    std::cout << "===================DeleteDirectory=====================" << std::endl;
    if (result.IsSucc()) {
        std::cout << "DeleteDirectory Succ." << std::endl;
    } else {
        std::cout << "DeleteDirectory Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }

    std::cout << "Succ del objs:" << std::endl;
    for (auto& obj : resp.m_succ_del_objs) {
        std::cout << obj << std::endl;
    }
    std::cout << "=======================================================" << std::endl;
}

void DeleteObjectsByPrefixDemo(qcloud_cos::CosAPI& cos) {
    std::string prefix = "test_dir";  // 指定前缀，test_dir目录下的所有对象均会被删除，且以test_dir开头的对象也会被删除
    DeleteObjectsByPrefixReq req(bucket_name, prefix);
    DeleteObjectsByPrefixResp resp;
    CosResult result = cos.DeleteObjects(req, &resp);

    std::cout << "===================DeleteObjectsByPrefix=====================" << std::endl;
    if (result.IsSucc()) {
        std::cout << "DeleteObjectsByPrefix Succ." << std::endl;
    } else {
        std::cout << "DeleteObjectsByPrefix Fail, ErrorMsg: "
                  << result.GetErrorMsg() << std::endl;
    }

    std::cout << "Succ del objs:" << std::endl;
    for (auto& obj : resp.m_succ_del_objs) {
        std::cout << obj << std::endl;
    }
    std::cout << "=============================================================" << std::endl;
}

int main() {
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    DeleteObjectDemo(cos);
    DeleteObjectsDemo(cos);
    DeleteDirectoryDemo(cos);
    DeleteObjectsByPrefixDemo(cos);
}