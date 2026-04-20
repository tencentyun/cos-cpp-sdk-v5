#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行存储桶 ACL 的设置与查询
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

// 设置存储桶 ACL
void PutBucketACLDemo(qcloud_cos::CosAPI& cos) {
    // 通过 Body 方式设置 ACL (与 Header 方式只能二选一)
    qcloud_cos::PutBucketACLReq req(bucket_name);
    qcloud_cos::Owner owner = {"qcs::cam::uin/xxx:uin/xxx",
                               "qcs::cam::uin/xxx:uin/xxx"};
    qcloud_cos::Grant grant;
    req.SetOwner(owner);
    grant.m_grantee.m_type = "Group";
    grant.m_grantee.m_uri = "http://cam.qcloud.com/groups/global/AllUsers";
    grant.m_perm = "READ";
    req.AddAccessControlList(grant);

    qcloud_cos::PutBucketACLResp resp;
    qcloud_cos::CosResult result = cos.PutBucketACL(req, &resp);
    std::cout << "===================PutBucketACLResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "===========================================================" << std::endl;

    // 通过 Header 方式设置 ACL 示例:
    // qcloud_cos::PutBucketACLReq req2(bucket_name);
    // req2.SetXCosAcl("public-read");
    // cos.PutBucketACL(req2, &resp);
}

// 查询存储桶 ACL
void GetBucketACLDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketACLReq req(bucket_name);
    qcloud_cos::GetBucketACLResp resp;
    qcloud_cos::CosResult result = cos.GetBucketACL(req, &resp);

    std::vector<qcloud_cos::Grant> grants = resp.GetAccessControlList();
    for (size_t idx = 0; idx < grants.size(); ++idx) {
        std::cout << "Type is " << grants[idx].m_grantee.m_type << std::endl;
    }
    std::cout << "===================GetBucketACLResponse====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "===========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketACLDemo(cos);
        GetBucketACLDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
