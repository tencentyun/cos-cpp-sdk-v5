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
 * 本样例演示了如何使用 COS C++ SDK 进行简单上传和批量上传
 * 包括：上传本地文件、上传流类型、上传目录类型、上传本地文件夹下的对象
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

void PutObjectByFileDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "test.txt";
    std::string file_path = "./test_file/text.txt";
    qcloud_cos::PutObjectByFileReq req(bucket_name, object_name, file_path);
    // 限速上传对象，默认单位为 bit/s，限速值设置范围为 819200 - 838860800
    // uint64_t traffic_limit = 0;
    // req.SetTrafficLimit(traffic_limit);
    qcloud_cos::PutObjectByFileResp resp;
    qcloud_cos::CosResult result = cos.PutObject(req, &resp);

    std::cout << "====================PutObjectByFile======================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void PutObjectByStreamDemo(qcloud_cos::CosAPI& cos) {
    std::istringstream iss("put object");
    std::string object_name = "text.txt";
    qcloud_cos::PutObjectByStreamReq req(bucket_name, object_name, iss);
    // 限速上传对象，默认单位为 bit/s，限速值设置范围为 819200 - 838860800
    // uint64_t traffic_limit = 0;
    // req.SetTrafficLimit(traffic_limit);
    qcloud_cos::PutObjectByStreamResp resp;
    qcloud_cos::CosResult result = cos.PutObject(req, &resp);

    std::cout << "===================PutObjectByStream=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

void PutDirectoryDemo(qcloud_cos::CosAPI& cos) {
    std::string directory_name = "test_dir/"; // 目录名称，注意末尾需要有/
    PutDirectoryReq req(bucket_name, directory_name);
    PutDirectoryResp resp;
    CosResult result = cos.PutDirectory(req, &resp);

    std::cout << "=====================PutDirectory========================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

/*
 * 该 Demo 示范如何上传本地文件夹到 cos
 * 本示例会将本地 test_file 目录上传到 cos 的 test_dir下。
 * 例如，本地当前路径的 test_file 目录下有 text1.txt 和 text2.txt 文件，上传后 cos 的路径为 test_dir/text1.txt 和 test_dir/text2.txt
 */
void PutObjectsFromDirectoryDemo(qcloud_cos::CosAPI& cos) {
    std::string directory_name = "test_file"; // 目录名称，注意开头如果加./ 则上传后cos 的路径为 test_dir/test_file/text.txt
    std::string cos_path = "test_dir/"; // 目录名称，注意末尾需要有/
    PutObjectsByDirectoryReq req(bucket_name, directory_name);
    req.SetCosPath(cos_path);
    PutObjectsByDirectoryResp resp;
    CosResult result = cos.PutObjects(req, &resp);

    std::cout << "=====================PutDirectory========================" << std::endl;
    if (result.IsSucc()) {
        std::cout << "PutObjectsFromDirectory Succ." << std::endl;
        for (auto& r : resp.m_succ_put_objs) {
            std::cout << "file_name: " << r.m_file_name << "\nobject_name: " << r.m_object_name << "\nETag: " << r.m_cos_resp.GetEtag() << std::endl;
            std::cout << "====================================" << std::endl;
        }
    } else {
        std::cout << "PutObjectsFromDirectoryToCosPath Fail, ErrorMsg: "
                  << result.GetErrorMsg() << std::endl;
    }
    std::cout << "=========================================================" << std::endl;
}

int main() {
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    PutObjectByFileDemo(cos);
    PutObjectByStreamDemo(cos);
    PutDirectoryDemo(cos);
    PutObjectsFromDirectoryDemo(cos);
}
