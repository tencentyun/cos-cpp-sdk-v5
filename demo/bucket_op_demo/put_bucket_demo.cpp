
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
 * 本样例演示了如何使用 COS C++ SDK 进行存储桶的创建
 */
using namespace qcloud_cos;

uint64_t appid = 12500000000;
std::string tmp_secret_id = "AKIDXXXXXXXX";
std::string tmp_secret_key = "1A2Z3YYYYYYYYYY";
std::string region = "ap-guangzhou@xxx";
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

void PutBucket(qcloud_cos::CosAPI& cos) {
  qcloud_cos::PutBucketReq req(bucket_name);
  //创建MAZ存储桶使用下方进行设置
  //req.SetMAZBucket();
  qcloud_cos::PutBucketResp resp;
  qcloud_cos::CosResult result = cos.PutBucket(req, &resp);

  std::cout << "===================PutBucketResponse====================="
            << std::endl;
  PrintResult(result, resp);
  std::cout << "========================================================="
            << std::endl;
}


int main() {
    try{
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    
      PutBucket(cos);
    }
    catch(const std::exception& e){
      std::cerr << e.what() << "]]" << '\n';
      
      std::cout << (strcmp(e.what(),"Invalid region configuration in CosConfig :ap-guangzhou@xxx") == 0) << std::endl;
    }
    std::cout << "Hello, World!" << std::endl;
    
}