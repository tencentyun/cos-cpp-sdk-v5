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
// #include <openssl/ssl.h>  // 双向认证引用头

/**
 * 本样例演示了如何使用 COS C++ SDK 进行对象的高级上传
 * 包括：高级上传接口（多线程、支持断点续传功能）、异步高级上传接口、异步简单上传接口（流&文件）、分块上传（用户自行调用初始化、上传、完成分块接口）
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
    // config.SetDestDomain("xxx.xxxx.com"); // 配置自定义域名
    // config.SetDomainSameToHost(true); // 配置自定义域名签名
    qcloud_cos::CosAPI cos_tmp(config);
    return cos_tmp;
}

/**
 * 本方法为 SSL_CTX 的回调方法，用户可以在此方法中配置 SSL_CTX 信息
 */
// int SslCtxCallback(void *ssl_ctx, void *data) {
//   std::cout << "ssl_ctx: " << ssl_ctx << " data: " << data << std::endl;

//   SSL_CTX *ctx = (SSL_CTX *)ssl_ctx;
//   std::cout << "ssl_ctx in" << std::endl;
//   SSL_CTX_use_PrivateKey_file(ctx, "/data/cert/client.key", SSL_FILETYPE_PEM);
//   SSL_CTX_use_certificate_chain_file(ctx, "/data/cert/client.crt");
//   std::cout << "ssl_ctx out" << std::endl;

//   return 0;
// }

/*
 * 该 Demo 示范如何使用高级上传接口进行对象上传
 * 仅支持文件上传，不支持流式上传，封装了分块上传的各接口
 * 可通过全局设置上传线程池大小、分块大小。该上传线程池是每次上传独立的。
 */
void MultiUploadObjectDemo(qcloud_cos::CosAPI& cos) {
    std::string local_file = "test_file/big_file.txt";
    std::string object_name = "big_file.txt";
    // 此配置是全局性配置，主动设置后，后续的高级上传，都使用此配置
    CosSysConfig::SetUploadThreadPoolSize(5);           // 上传线程池大小 默认5
    CosSysConfig::SetUploadPartSize(10 * 1024 * 1024);  // 上传分块大小 默认10M

    qcloud_cos::MultiPutObjectReq req(bucket_name, object_name, local_file);
    // req.SetHttps(); // 设置 https 请求
    // req.SetSSLCtxCallback(SslCtxCallback, nullptr); //双向认证回调
    qcloud_cos::MultiPutObjectResp resp;
    qcloud_cos::CosResult result = cos.MultiPutObject(req, &resp);

    std::cout << "===================MultiUploadObject=======================" << std::endl;
    if (result.IsSucc()) {
        std::cout << "MultiUpload Succ." << std::endl;
        std::cout << resp.GetLocation() << std::endl;
        std::cout << resp.GetKey() << std::endl;
        std::cout << resp.GetBucket() << std::endl;
        std::cout << resp.GetEtag() << std::endl;
    } else {
        std::cout << "MultiUpload Fail." << std::endl;
        // 获取具体失败在哪一步
        std::string resp_tag = resp.GetRespTag();
        if ("Init" == resp_tag) {
            // print result
        } else if ("Upload" == resp_tag) {
            // print result
        } else if ("Complete" == resp_tag) {
            // print result
        }
        PrintResult(result, resp);
    }
    std::cout << "===========================================================" << std::endl;
}

/*
 * 该方法是异步上传对象的进度回调示例
 */
void ProgressCallback(uint64_t transferred_size, uint64_t total_size, void* user_data) {
    qcloud_cos::ObjectReq* req = static_cast<qcloud_cos::ObjectReq*>(user_data);
    if (0 == transferred_size % 1048576) {
        std::cout << "ObjectName:" << req->GetObjectName() << ", TranferedSize:" << transferred_size << ",TotalSize:" << total_size << std::endl;
    }
}

/*
 * 该方法是异步上传对象的完成回调示例
 */
void PutObjectAsyncDoneCallback(const SharedAsyncContext& context, void* user_data) {
    UNUSED_PARAM(user_data)
    std::cout << "PutObjectAsyncDoneCallback, BucketName:" << context->GetBucketName() << ", ObjectName:" << context->GetObjectName() << ", LocalFile:" << context->GetLocalFilePath() << std::endl;
    if (context->GetResult().IsSucc()) {
        // 获取响应
        std::cout << "AsyncMultiPutObject succeed" << std::endl;
        std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
        AsyncResp resp = context->GetAsyncResp();
        // std::cout << "Location:" << resp.GetLocation() << std::endl;
        // std::cout << "Bucket:" << resp.GetBucket() << std::endl;
        // std::cout << "Key:" << resp.GetKey() << std::endl;
        std::cout << "ETag:" << resp.GetEtag() << std::endl;
        std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
    } else {
        std::cout << "AsyncMultiGetObject failed" << std::endl;
        std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
    }
}

/*
 * 该 Demo 示范如何通过异步高级上传对象进行对象上传
 * 仅支持文件上传，不支持流式上传，封装了分块上传的各接口、
 * 可通过全局设置异步线程池大小、上传线程池大小、分块大小
 * 注意：异步线程池是全局共用的，供异步调用使用。上传线程池、分块大小是和高级接口共用配置
 * 例如调用两次异步接口，则两个异步操作会使用异步线程池进行调度。而每个异步操作会调用高级接口创建出供本次使用的上传线程池，从而并发上传对象。
 * 支持上传进度回调和上传状态回调
 */
void AsyncMultiPutObjectDemo(qcloud_cos::CosAPI& cos) {
    std::string local_file = "test_file/big_file.txt";
    std::string object_name = "big_file.txt";
    qcloud_cos::AsyncMultiPutObjectReq req(bucket_name, object_name, local_file);
    req.SetRecvTimeoutInms(1000 * 60);
    // 设置上传进度回调
    req.SetTransferProgressCallback(&ProgressCallback);
    // 设置上传状态回调
    req.SetDoneCallback(&PutObjectAsyncDoneCallback);
    // 设置私有数据，对应回调中的 user_data
    req.SetUserData(&req);

    // 开始上传
    SharedAsyncContext context = cos.AsyncMultiPutObject(req);

    std::cout << "===================AsyncMultiPutObject======================" << std::endl;
    // 等待上传结束
    std::cout << "wait finish..." << std::endl;
    context->WaitUntilFinish();

    // 检查结果
    if (context->GetResult().IsSucc()) {
        // 获取响应
        std::cout << "AsyncMultiPutObject succeed" << std::endl;
        std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
        AsyncResp resp = context->GetAsyncResp();
        // std::cout << "Location:" << resp.GetLocation() << std::endl;
        // std::cout << "Bucket:" << resp.GetBucket() << std::endl;
        // std::cout << "Key:" << resp.GetKey() << std::endl;
        std::cout << "ETag:" << resp.GetEtag() << std::endl;
        std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
    } else {
        std::cout << "AsyncMultiPutObject failed" << std::endl;
        std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
    }
    std::cout << "============================================================" << std::endl;
}

/*
 * 该 Demo 示范如何通过异步普通接口上传对象
 * 包括流和文件两种方式上传
 * 可通过全局设置异步线程池大小
 * 注意：异步线程池是全局共用的，供异步调用使用
 * 例如调用两次异步接口，则两个异步操作会使用异步线程池进行调度
 * 支持上传进度回调和上传状态回调
 */
void AsyncPutObjectDemo(qcloud_cos::CosAPI& cos) {
    // 流上传
    {
        std::string object_name = "text.txt";
        std::istringstream iss("put object");
        qcloud_cos::AsyncPutObjectByStreamReq req(bucket_name, object_name, iss);
        req.SetRecvTimeoutInms(1000 * 60);
        // 设置上传进度回调
        req.SetTransferProgressCallback(&ProgressCallback);
        // 设置上传状态回调
        req.SetDoneCallback(&PutObjectAsyncDoneCallback);
        // 设置私有数据，对应回调中的 user_data
        req.SetUserData(&req);

        // 开始上传
        SharedAsyncContext context = cos.AsyncPutObject(req);

        std::cout << "===================AsyncPutObjectByStream======================" << std::endl;
        // 等待上传结束
        std::cout << "wait finish..." << std::endl;
        context->WaitUntilFinish();

        // 检查结果
        if (context->GetResult().IsSucc()) {
            // 获取响应
            std::cout << "AsyncPutObjectByStream succeed" << std::endl;
            std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
            AsyncResp resp = context->GetAsyncResp();
            // std::cout << "Location:" << resp.GetLocation() << std::endl;
            // std::cout << "Bucket:" << resp.GetBucket() << std::endl;
            // std::cout << "Key:" << resp.GetKey() << std::endl;
            std::cout << "ETag:" << resp.GetEtag() << std::endl;
            std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
        } else {
            std::cout << "AsyncPutObjectByStream failed" << std::endl;
            std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
        }
        std::cout << "===============================================================" << std::endl;
    }
    // 文件上传
    {
        std::string local_file = "test_file/text.txt";
        std::string object_name = "text.txt";
        qcloud_cos::AsyncPutObjectReq req(bucket_name, object_name, local_file);
        req.SetRecvTimeoutInms(1000 * 60);
        // 设置上传进度回调
        req.SetTransferProgressCallback(&ProgressCallback);
        // 设置上传状态回调
        req.SetDoneCallback(&PutObjectAsyncDoneCallback);
        // 设置私有数据，对应回调中的 user_data
        req.SetUserData(&req);

        // 开始上传
        SharedAsyncContext context = cos.AsyncPutObject(req);
        std::cout << "===================AsyncPutObjectByFile======================" << std::endl;

        // 等待上传结束
        std::cout << "wait finish..." << std::endl;
        context->WaitUntilFinish();

        // 检查结果
        if (context->GetResult().IsSucc()) {
            // 获取响应
            std::cout << "AsyncPutObjectByFile succeed" << std::endl;
            std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
            AsyncResp resp = context->GetAsyncResp();
            // std::cout << "Location:" << resp.GetLocation() << std::endl;
            // std::cout << "Bucket:" << resp.GetBucket() << std::endl;
            // std::cout << "Key:" << resp.GetKey() << std::endl;
            std::cout << "ETag:" << resp.GetEtag() << std::endl;
            std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
        } else {
            std::cout << "AsyncPutObjectByFile failed" << std::endl;
            std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
        }
        std::cout << "=============================================================" << std::endl;
    }
}

/*
 * 该 Demo 示范用户如何自行组合分块上传各接口进行对象上传
 * 分块操作基于初始化、上传分块、完成分块三个接口可以实现将对象切分为多个分块，然后再将这些分块上传到 cos，最后发起 Complete 完成分块上传
 * 本 Demo 中的上传分块接口 UploadPartData 仅支持传入流，最多支持10000分块，每个分块大小为1MB - 5GB，最后一个分块可以小于1MB
 */

void PutPartDemo(qcloud_cos::CosAPI& cos) {
    std::string object_name = "big_file.txt";

    // 1. InitMultiUpload
    qcloud_cos::InitMultiUploadReq init_req(bucket_name, object_name);
    qcloud_cos::InitMultiUploadResp init_resp;
    qcloud_cos::CosResult init_result = cos.InitMultiUpload(init_req, &init_resp);

    std::cout << "=====================InitMultiUpload=====================" << std::endl;
    PrintResult(init_result, init_resp);
    std::cout << "=========================================================" << std::endl;

    // 2. UploadPartData
    // UploadPartData 部分，可以根据实际选择分块数量和分块大小，这里以 2 个分块为例

        // Complete 需要的两个列表：
    std::vector<std::string> etags;
    std::vector<uint64_t> part_numbers;

    std::string upload_id = init_resp.GetUploadId();
    {
        uint64_t part_number = 1;
        // 模拟上传分块数据，这里以 100M 为例
        uint64_t copy_size = 1024 * 1024 * 100;
        std::vector<char> data(copy_size, 'A');
        std::string content(data.begin(), data.end());
        std::istringstream iss(content);
        qcloud_cos::UploadPartDataReq req(bucket_name, object_name, upload_id, iss);
        req.SetPartNumber(part_number);
            // 限速上传对象，默认单位为 bit/s，限速值设置范围为 819200 - 838860800, 即800Kb/s-800Mb/s
            uint64_t traffic_limit = 8192*1024*10; // 100MB 文件 5M
            req.SetTrafficLimit(traffic_limit);
        qcloud_cos::UploadPartDataResp resp;
        std::chrono::time_point<std::chrono::steady_clock> start_ts, end_ts;
        start_ts = std::chrono::steady_clock::now();
        qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);
        end_ts = std::chrono::steady_clock::now();
        auto time_consumed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - start_ts)
            .count();
        float rate =
          ((float)copy_size / 1024 / 1024) / ((float)time_consumed_ms / 1000);
        SDK_LOG_ERR("send part_number: %d, send_size: %" PRIu64 " MB, time_consumed: %" PRIu64
                    " ms, rate: %.2f MB/s , traffic_limit : %.2f MB", part_number, copy_size/ 1024 / 1024, time_consumed_ms, rate, traffic_limit/1024/1024/8.0);
        std::cout << "==================UploadPartDataResp1=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "==========================================================" << std::endl;
        if (result.IsSucc()) {
            part_numbers.push_back(part_number);
            etags.push_back(resp.GetEtag());
        }
    }
    {
        uint64_t part_number = 2;
        uint64_t copy_size = 1024 * 1024 * 100;
        std::vector<char> data(copy_size, 'A');
        std::string content(data.begin(), data.end());
        std::istringstream iss(content);
        qcloud_cos::UploadPartDataReq req(bucket_name, object_name, upload_id, iss);
        req.SetPartNumber(part_number);
            // 限速上传对象，默认单位为 bit/s，限速值设置范围为 819200 - 838860800, 即800Kb/s-800Mb/s
        uint64_t traffic_limit = 8192 * 1024 * 5 ;
        req.SetTrafficLimit(traffic_limit);
        qcloud_cos::UploadPartDataResp resp;
        qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);
        std::cout << "==================UploadPartDataResp2=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "==========================================================" << std::endl;
        if (result.IsSucc()) {
            part_numbers.push_back(part_number);
            etags.push_back(resp.GetEtag());
        }
    }
    {
        uint64_t part_number = 3;
        uint64_t copy_size = 1024 * 1024 * 10;
        std::vector<char> data(copy_size, 'A');
        std::string content(data.begin(), data.end());
        std::istringstream iss(content);
        qcloud_cos::UploadPartDataReq req(bucket_name, object_name, upload_id, iss);
        req.SetPartNumber(part_number);
            // 限速上传对象，默认单位为 bit/s，限速值设置范围为 819200 - 838860800, 即800Kb/s-800Mb/s
            uint64_t traffic_limit = 8192 * 1024;
            req.SetTrafficLimit(traffic_limit);
        qcloud_cos::UploadPartDataResp resp;
        qcloud_cos::CosResult result = cos.UploadPartData(req, &resp);
        std::cout << "==================UploadPartDataResp2=====================" << std::endl;
        PrintResult(result, resp);
        std::cout << "==========================================================" << std::endl;
        if (result.IsSucc()) {
            part_numbers.push_back(part_number);
            etags.push_back(resp.GetEtag());
        }
    }

    // 3. Complete
    CompleteMultiUploadReq comp_req(bucket_name, object_name, upload_id);
    CompleteMultiUploadResp comp_resp;

    comp_req.SetEtags(etags);
    comp_req.SetPartNumbers(part_numbers);

    qcloud_cos::CosResult result = cos.CompleteMultiUpload(comp_req, &comp_resp);

    std::cout << "===================Complete=============================" << std::endl;
    PrintResult(result, comp_resp);
    std::cout << "========================================================" << std::endl;

    return;
}
void PutObjectResumableSingleThreadSyncDemo(qcloud_cos::CosAPI& cos) {
    std::string local_file = "SingleThreadSync.txt";
    std::string object_name = "SingleThreadSync.txt";

    qcloud_cos::PutObjectResumableSingleSyncReq req(bucket_name, object_name, local_file);
    req.AddHeader("x-cos-meta-ssss1","1xxxxxxx");
    req.AddHeader("x-cos-meta-ssss2","2xxxxxxx");
    req.AddHeader("x-cos-meta-ssss3","3xxxxxxx");
    req.AddHeader("x-cos-meta-ssss4","4xxxxxxx");
    uint64_t traffic_limit = 8192 * 1024;//1MB
    req.SetTrafficLimit(traffic_limit);
    //req.SetHttps();
    //req.SetSSLCtxCallback(SslCtxCallback, nullptr);
    qcloud_cos::PutObjectResumableSingleSyncResp resp;
    qcloud_cos::CosResult result = cos.PutObjectResumableSingleThreadSync(req, &resp);
    if (result.IsSucc()) {
        std::cout << "MultiUpload Succ." << std::endl;
        std::cout << resp.GetLocation() << std::endl;
        std::cout << resp.GetKey() << std::endl;
        std::cout << resp.GetBucket() << std::endl;
        std::cout << resp.GetEtag() << std::endl;
    } else {
        std::cout << "MultiUpload Fail." << std::endl;
        // 获取具体失败在哪一步
        std::string resp_tag = resp.GetRespTag();
        if ("Init" == resp_tag) {
            // print result
        } else if ("Upload" == resp_tag) {
            // print result
        } else if ("Complete" == resp_tag) {
            // print result
        }
        PrintResult(result, resp);
    }
    std::cout << "===========================================================" << std::endl;
}
int main() {
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    MultiUploadObjectDemo(cos);
    AsyncMultiPutObjectDemo(cos);
    AsyncPutObjectDemo(cos);
    PutPartDemo(cos);
    PutObjectResumableSingleThreadSyncDemo(cos);
}
