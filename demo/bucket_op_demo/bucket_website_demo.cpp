#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 配置、查询与删除存储桶静态网站功能
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

// 为存储桶配置静态网站
void PutBucketWebsiteDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::PutBucketWebsiteReq req(bucket_name);
    qcloud_cos::PutBucketWebsiteResp resp;

    req.SetSuffix("index.xml");
    req.SetProtocol("https");
    req.SetKey("Error.html");

    // 设置重定向规则1: 404 -> 404.htmp
    qcloud_cos::RoutingRule routerule1;
    qcloud_cos::Condition temp_condtion1;
    temp_condtion1.SetHttpErrorCodeReturnedEquals(404);
    routerule1.SetCondition(temp_condtion1);
    qcloud_cos::Redirect temp_redirect1;
    temp_redirect1.SetProtocol("https");
    temp_redirect1.SetReplaceKeyWith("404.htmp");
    routerule1.SetRedirect(temp_redirect1);

    // 设置重定向规则2: 403 -> 403.htmp
    qcloud_cos::RoutingRule routerule2;
    qcloud_cos::Condition temp_condtion2;
    temp_condtion2.SetHttpErrorCodeReturnedEquals(403);
    routerule2.SetCondition(temp_condtion2);
    qcloud_cos::Redirect temp_redirect2;
    temp_redirect2.SetProtocol("https");
    temp_redirect2.SetReplaceKeyWith("403.htmp");
    routerule2.SetRedirect(temp_redirect2);

    // 设置重定向规则3: img/ 前缀 + 402 -> 401.htmp
    qcloud_cos::RoutingRule routerule3;
    qcloud_cos::Condition temp_condtion3;
    temp_condtion3.SetKeyPrefixEquals("img/");
    temp_condtion3.SetHttpErrorCodeReturnedEquals(402);
    routerule3.SetCondition(temp_condtion3);
    qcloud_cos::Redirect temp_redirect3;
    temp_redirect3.SetProtocol("https");
    temp_redirect3.SetReplaceKeyWith("401.htmp");
    routerule3.SetRedirect(temp_redirect3);

    // 设置重定向规则4: img1/ 前缀 -> 替换前缀
    qcloud_cos::RoutingRule routerule4;
    qcloud_cos::Condition temp_condtion4;
    temp_condtion4.SetKeyPrefixEquals("img1/");
    routerule4.SetCondition(temp_condtion4);
    qcloud_cos::Redirect temp_redirect4;
    temp_redirect4.SetProtocol("https");
    temp_redirect4.SetReplaceKeyPrefixWith("402.htmp");
    routerule4.SetRedirect(temp_redirect4);

    req.AddRoutingRule(routerule1);
    req.AddRoutingRule(routerule2);
    req.AddRoutingRule(routerule3);
    req.AddRoutingRule(routerule4);

    qcloud_cos::CosResult result = cos.PutBucketWebsite(req, &resp);
    std::cout << "===================PutBucketWebsite==================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=====================================================" << std::endl;
}

// 获取与存储桶关联的静态网站配置信息
void GetBucketWebsiteDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::GetBucketWebsiteReq req(bucket_name);
    qcloud_cos::GetBucketWebsiteResp resp;
    qcloud_cos::CosResult result = cos.GetBucketWebsite(req, &resp);

    std::cout << "===================GetBucketWebsite==================" << std::endl;
    std::cout << "Suffix:" << resp.GetSuffix() << std::endl;
    std::cout << "Protocol:" << resp.GetProtocol() << std::endl;
    std::cout << "ErrorDocument:" << resp.GetKey() << std::endl;

    std::vector<RoutingRule> routingrules = resp.GetRoutingRules();
    for (std::vector<RoutingRule>::iterator it = routingrules.begin(); it != routingrules.end(); ++it) {
        const Condition& condition = it->GetCondition();
        std::cout << "condition httpcode " << condition.GetHttpErrorCodeReturnedEquals() << std::endl;
        std::cout << "condition KeyPrefixEquals: " << condition.GetKeyPrefixEquals() << std::endl;

        const Redirect& redirect = it->GetRedirect();
        std::cout << "redirect Protocol: " << redirect.GetProtocol() << std::endl;
        std::cout << "redirect ReplaceKeyWith: " << redirect.GetReplaceKeyWith() << std::endl;
        std::cout << "redirect ReplaceKeyPrefixWith: " << redirect.GetReplaceKeyPrefixWith() << std::endl;
        std::cout << std::endl;
    }

    PrintResult(result, resp);
    std::cout << "=====================================================" << std::endl;
}

// 删除存储桶中的静态网站配置
void DeleteBucketWebsiteDemo(qcloud_cos::CosAPI& cos) {
    qcloud_cos::DeleteBucketWebsiteReq req(bucket_name);
    qcloud_cos::DeleteBucketWebsiteResp resp;
    qcloud_cos::CosResult result = cos.DeleteBucketWebsite(req, &resp);

    std::cout << "===================DeleteBucketWebsite==================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        PutBucketWebsiteDemo(cos);
        GetBucketWebsiteDemo(cos);
        DeleteBucketWebsiteDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
