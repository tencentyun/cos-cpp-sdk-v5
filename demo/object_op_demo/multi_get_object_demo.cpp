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
 * 本样例演示了如何使用 COS C++ SDK 进行对象的高级下载
 * 包括：多线程下载、断点下载（多线程、支持断点）、异步多线程下载、异步断点下载、异步简单下载
 * 均只支持下载到文件
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

/*
 * 该 Demo 示范如何使用多线程下载接口下载对象
 * 仅支持下载到文件，利用了分 range 多线程同时下载的方式下载对象
 * 可通过全局设置下载线程池大小、range 大小。该线程池是每次上传独立的
 */
void MultiGetObjectDemo(qcloud_cos::CosAPI& cos) {
    std::string file_path = "test_file/big_file.txt";
    std::string object_name = "big_file.txt";
    // 此配置是全局性配置，主动设置后，后续涉及多线程下载的接口，都使用此配置
    CosSysConfig::SetDownThreadPoolSize(10);          // 下载线程池大小 默认10
    CosSysConfig::SetDownSliceSize(4 * 1024 * 1024);  // 下载 range 大小 默认4M

    qcloud_cos::MultiGetObjectReq req(bucket_name, object_name, file_path);
    qcloud_cos::MultiGetObjectResp resp;

    qcloud_cos::CosResult result = cos.MultiGetObject(req, &resp);
    std::cout << "===================GetObjectResponse=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

/*
 * 该 Demo 示范如何使用断点下载接口下载对象
 * 仅支持下载到文件，利用了分 range 多线程同时下载的方式下载对象，支持断点下载
 * 可通过全局设置下载线程池大小、range 大小。该线程池是每次上传独立的。
 */
void ResumableGetObjectDemo(qcloud_cos::CosAPI& cos) {
    std::string file_path = "test_file/big_file.txt";
    std::string object_name = "big_file.txt";

    qcloud_cos::GetObjectByFileReq req(bucket_name, object_name, file_path);
    qcloud_cos::GetObjectByFileResp resp;

    CosResult result = cos.ResumableGetObject(req, &resp);
    std::cout << "===================ResumableGetObject====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "=========================================================" << std::endl;
}

/*
 * 该方法是异步下载对象的进度回调示例
 */
void ProgressCallback(uint64_t transferred_size, uint64_t total_size, void* user_data) {
    qcloud_cos::ObjectReq* req = static_cast<qcloud_cos::ObjectReq*>(user_data);
    if (0 == transferred_size % 1048576) {
        std::cout << "ObjectName:" << req->GetObjectName() << ", TranferedSize:" << transferred_size << ",TotalSize:" << total_size << std::endl;
    }
}

/*
 * 该方法是异步下载对象的完成回调示例
 */
void GetObjectAsyncDoneCallback(const SharedAsyncContext& context, void* user_data) {
    UNUSED_PARAM(user_data)
    std::cout << "MultiGetObjectAsyncDoneCallback, BucketName:"
              << context->GetBucketName()
              << ", ObjectName:" << context->GetObjectName()
              << ", LocalFile:" << context->GetLocalFilePath() << std::endl;
    // qcloud_cos::MultiGetObjectReq对应的响应为qcloud_cos::GetObjectByFileResp
    if (context->GetResult().IsSucc()) {
        // 获取响应
        std::cout << "AsyncMultiGetObject succeed" << std::endl;
        std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
        AsyncResp resp = context->GetAsyncResp();
        std::cout << "ETag:" << resp.GetEtag() << std::endl;
        std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
    } else {
        std::cout << "AsyncMultiGetObject failed" << std::endl;
        std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
    }
}

/*
 * 该 Demo 示范如何通过异步多线程接口下载对象
 * 仅支持下载到文件，利用了分 range 多线程同时下载的方式下载对象
 * 可通过全局设置异步线程池大小、下载线程池大小、range 大小
 * 注意：异步线程池是全局共用的，供所有的异步调用使用。下载线程池、range 大小与所有多线程类型下载接口共用配置
 * 例如调用两次异步接口，则两个异步操作会使用异步线程池进行调度。而每个异步操作中，可能会调用多线程相关接口创建出供本次使用的下载线程池，从而并发下载对象。
 * 支持下载进度回调和下载状态回调
 */
void AsyncMultiGetObjectDemo(qcloud_cos::CosAPI& cos) {
    CosSysConfig::SetAsynThreadPoolSize(2);  // 设置异步线程池大小 默认2

    std::string local_file = "test_file/big_file.txt";
    std::string object_name = "big_file.txt";
    qcloud_cos::AsyncMultiGetObjectReq req(bucket_name, object_name, local_file);
    // 设置下载进度回调
    req.SetTransferProgressCallback(&ProgressCallback);
    // 设置下载状态回调
    req.SetDoneCallback(&GetObjectAsyncDoneCallback);
    // 设置私有数据，对应回调中的 user_data
    req.SetUserData(&req);

    // 开始下载
    SharedAsyncContext context = cos.AsyncMultiGetObject(req);

    std::cout << "===================AsyncMultiGetObject======================" << std::endl;
    // 等待下载结束
    std::cout << "wait finish..." << std::endl;
    context->WaitUntilFinish();
    // context->WaitUntilFinish()阻塞完毕的逻辑是：异步线程在下载时文件落盘完毕。
    // 此时您的主线程如果立即结束，异步线程可能正在进行资源释放、结束线程等过程。此时可能会产生crash。
    // 此问题并不会对文件的下载造成影响，仅仅是体验上可能会出现意料之外的崩溃信息。

    // 检查结果
    if (context->GetResult().IsSucc()) {
        // 获取响应
        std::cout << "AsyncMultiGetObject succeed" << std::endl;
        std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
        AsyncResp resp = context->GetAsyncResp();
        std::cout << "ETag:" << resp.GetEtag() << std::endl;
        std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
    } else {
        std::cout << "AsyncMultiGetObject failed" << std::endl;
        std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
    }
    std::cout << "============================================================" << std::endl;
}

/*
 * 该 Demo 示范如何通过异步断点下载接口下载对象
 * 仅支持下载到文件，利用了分 range 多线程同时下载的方式下载对象，支持断点下载
 * 可通过全局设置异步线程池大小、下载线程池大小、range 大小。
 * 注意：异步线程池是全局共用的，供所有的异步调用使用。下载线程池、range 大小与所有多线程类型下载接口共用配置
 * 例如调用两次异步接口，则两个异步操作会使用异步线程池进行调度。而每个异步操作中，可能会调用多线程相关接口创建出供本次使用的下载线程池，从而并发下载对象。
 * 支持下载进度回调和下载状态回调
 */
void AsyncResumableGetObjectDemo(qcloud_cos::CosAPI& cos) {
    std::string local_file = "test_file/big_file.txt";
    std::string object_name = "big_file.txt";
    qcloud_cos::AsyncGetObjectReq req(bucket_name, object_name, local_file);
    // 设置下载进度回调
    req.SetTransferProgressCallback(&ProgressCallback);
    // 设置下载状态回调
    req.SetDoneCallback(&GetObjectAsyncDoneCallback);
    // 设置私有数据，对应回调中的 user_data
    req.SetUserData(&req);

    // 开始下载
    SharedAsyncContext context = cos.AsyncResumableGetObject(req);

    std::cout << "===================AsyncResumableGetObject======================" << std::endl;
    // 等待下载结束
    std::cout << "wait finish..." << std::endl;
    context->WaitUntilFinish();
    // context->WaitUntilFinish()阻塞完毕的逻辑是：异步线程在下载时文件落盘完毕。
    // 此时您的主线程如果立即结束，异步线程可能正在进行资源释放、结束线程等过程。此时可能会产生crash。
    // 此问题并不会对文件的下载造成影响，仅仅是体验上可能会出现意料之外的崩溃信息。

    // 检查结果
    if (context->GetResult().IsSucc()) {
        // 获取响应
        std::cout << "AsyncResumableGetObject succeed" << std::endl;
        std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
        AsyncResp resp = context->GetAsyncResp();
        std::cout << "ETag:" << resp.GetEtag() << std::endl;
        std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
    } else {
        std::cout << "AsyncResumableGetObject failed" << std::endl;
        std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
    }
    std::cout << "================================================================" << std::endl;
}

/*
 * 该 Demo 示范如何通过异步简单接口下载对象
 * 仅支持下载到文件
 * 可通过全局设置异步线程池大小
 * 注意：异步线程池是全局共用的，供异步调用使用
 * 例如调用两次异步接口，则两个异步操作会使用异步线程池进行调度
 * 支持下载进度回调和下载状态回调
 */
void AsyncGetObjectDemo(qcloud_cos::CosAPI& cos) {
    std::string local_file = "test_file/text.txt";
    std::string object_name = "text.txt";
    qcloud_cos::AsyncGetObjectReq req(bucket_name, object_name, local_file);
    req.SetRecvTimeoutInms(1000 * 60);
    // 设置下载进度回调
    req.SetTransferProgressCallback(&ProgressCallback);
    // 设置下载状态回调
    req.SetDoneCallback(&GetObjectAsyncDoneCallback);
    // 设置私有数据，对应回调中的 user_data
    req.SetUserData(&req);

    // 开始下载
    SharedAsyncContext context = cos.AsyncGetObject(req);
    std::cout << "===================AsyncGetObject======================" << std::endl;

    // 等待下载结束
    std::cout << "wait finish..." << std::endl;
    context->WaitUntilFinish();
    // context->WaitUntilFinish()阻塞完毕的逻辑是：异步线程在下载时文件落盘完毕。
    // 此时您的主线程如果立即结束，异步线程可能正在进行资源释放、结束线程等过程。此时可能会产生crash。
    // 此问题并不会对文件的下载造成影响，仅仅是体验上可能会出现意料之外的崩溃信息。

    // 检查结果
    if (context->GetResult().IsSucc()) {
        // 获取响应
        std::cout << "AsyncGetObject succeed" << std::endl;
        std::cout << "Result:" << context->GetResult().DebugString() << std::endl;
        AsyncResp resp = context->GetAsyncResp();
        std::cout << "ETag:" << resp.GetEtag() << std::endl;
        std::cout << "Crc64:" << resp.GetXCosHashCrc64Ecma() << std::endl;
    } else {
        std::cout << "AsyncGetObject failed" << std::endl;
        std::cout << "ErrorMsg:" << context->GetResult().GetErrorMsg() << std::endl;
    }
    std::cout << "=======================================================" << std::endl;
}

int main() {
    qcloud_cos::CosAPI cos = InitCosAPI();
    CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);
    MultiGetObjectDemo(cos);
    ResumableGetObjectDemo(cos);
    AsyncMultiGetObjectDemo(cos);
    AsyncResumableGetObjectDemo(cos);
    AsyncGetObjectDemo(cos);
}