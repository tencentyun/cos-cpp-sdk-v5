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
 * 本样例演示了如何使用 COS C++ SDK 进行对象 ACL 的设置和获取
 * 包括：通过 body 和 head 两种方式设置对象的 ACL、获取对象的 ACL
 */
using namespace qcloud_cos;

uint64_t appid = 12500000000;
std::string tmp_secret_id = "AKIDXXXXXXXX";
std::string tmp_secret_key = "1A2Z3YYYYYYYYYY";
std::string region = "ap-guangzhou";
std::string bucket_name = "examplebucket-12500000000";
std::string tmp_token = "token";
std::string uin = "23300000000";
std::string sub_uin = "23300000000";

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
 * 该 Demo 示范如何上传对象的 ACL
 * 本样例先通过 body 的方式为主账号持有的对象设置子账号 FULL_CONTROL 访问权限。再通过 head 的方式设置子账号 READ 访问权限
 */
void PutObjectACLDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    // 1 通过Body设置ACL配置 (设置ACL可以通过Body、Header两种方式，但只能二选一，否则会有冲突)
    // ACL 请求构造参考 api 文档：https://cloud.tencent.com/document/product/436/7748
    {
        qcloud_cos::PutObjectACLReq req(bucket_name, object_name);
        qcloud_cos::Owner owner = {"qcs::cam::uin/" + uin + ":uin/" + uin,
                                   "testacl"};
        qcloud_cos::Grant grant;
        req.SetOwner(owner);
        grant.m_grantee.m_type = "CanonicalUser";
        grant.m_grantee.m_id = "qcs::cam::uin/" + uin + ":uin/" + sub_uin;
        grant.m_perm = "FULL_CONTROL";
        req.AddAccessControlList(grant);// 可以加入多条grant

        qcloud_cos::PutObjectACLResp resp;
        qcloud_cos::CosResult result = cos.PutObjectACL(req, &resp);
        std::cout << "===================PutObjectACLResponse======================" << std::endl;
        PrintResult(result, resp);
        std::cout << "=============================================================" << std::endl;
    }

    // 2 通过Header设置ACL配置
    {
        qcloud_cos::PutObjectACLReq req(bucket_name, object_name);
        req.SetXCosAcl("default");      // 对应 api 中的 x-cos-acl
        std::string grant_read = "id=\"" + uin + "/"+ sub_uin + "\"";
        req.SetXCosGrantRead(grant_read);         // 对应 api 中的 x-cos-grant-read
        // std::string grant_full_control = "id=\"" + uin + "/"+ sub_uin + "/\"";
        // req.SetXCosGrantFullControl(grant_full_control);  // 对应 api 中的 x-cos-grant-full-control

        qcloud_cos::PutObjectACLResp resp;
        qcloud_cos::CosResult result = cos.PutObjectACL(req, &resp);
        std::cout << "===================PutObjectACLResponse======================" << std::endl;
        PrintResult(result, resp);
        std::cout << "=============================================================" << std::endl;
    }
}

void GetObjectACLDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    qcloud_cos::GetObjectACLReq req(bucket_name, object_name);
    qcloud_cos::GetObjectACLResp resp;
    qcloud_cos::CosResult result = cos.GetObjectACL(req, &resp);

    std::cout << "======================GetObjectACLResponse========================" << std::endl;
    PrintResult(result, resp);
    std::cout << "==================================================================" << std::endl;
}

int main() {
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    PutObjectACLDemo(cos);
    GetObjectACLDemo(cos);
}