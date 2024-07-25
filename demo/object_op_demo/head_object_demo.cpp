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
 * 本样例演示了如何使用 COS C++ SDK 进行 Head Object 相关操作
 * 包括：Head Object、判断对象是否存在
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

void IsObjectExistDemo(qcloud_cos::CosAPI& cos) {
    bool is_exist = cos.IsObjectExist(bucket_name, "test.txt");

    std::cout << "=====================IsObjectExist=======================" << std::endl;
    std::cout << (is_exist ? "true" : "false") << std::endl;
    std::cout << "=========================================================" << std::endl;
}

void HeadObjectDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    qcloud_cos::HeadObjectReq req(bucket_name, object_name);
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = cos.HeadObject(req, &resp);

    std::cout << "===================HeadObjectResponse=====================" << std::endl;
    std::map<std::string, std::string> cos_metas = resp.GetXCosMetas();  // 获取自定义的元数据map
    std::string cos_meta = resp.GetXCosMeta("x-cos-meta-*");             // 获取指定自定义的元数据
    std::string restore = resp.GetXCosRestore();                         // 获得 archive 类型对象的当前恢复状态
    std::string sse = resp.GetXCosServerSideEncryption();                // Server端加密使用的算法
    PrintResult(result, resp);
    std::cout << "==========================================================" << std::endl;
}

int main() {
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    IsObjectExistDemo(cos);
    HeadObjectDemo(cos);
}