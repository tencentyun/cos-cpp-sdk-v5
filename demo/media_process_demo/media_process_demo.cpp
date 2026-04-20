#include <stdlib.h>

#include <iostream>
#include <string>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行媒体处理相关操作：
 *   - DescribeMediaBuckets   : 查询已开通媒体处理的存储桶
 *   - GetSnapshot            : 获取媒体文件指定时间点截图
 *   - GetMediaInfo           : 获取媒体文件信息
 *   - GetPm3u8               : 获取私有 M3U8 ts 资源的下载授权
 *   - DescribeMediaQueues    : 查询媒体处理队列
 *   - UpdateMediaQueue       : 更新媒体处理队列
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

// 查询开通媒体处理的存储桶
// https://cloud.tencent.com/document/product/436/48988
void DescribeMediaBucketsDemo(qcloud_cos::CosAPI& cos) {
    DescribeMediaBucketsReq req;
    DescribeMediaBucketsResp resp;

    req.SetRegions("ap-guangzhou");
    // req.SetBucketNames("xxx");
    // req.SetBucketName("xxx");

    CosResult result = cos.DescribeMediaBuckets(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeMediaBuckets Succ." << std::endl;
        std::cout << "Result: " << resp.GetResult().to_string() << std::endl;
    } else {
        std::cout << "DescribeMediaBuckets Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeMediaBuckets=================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 获取媒体文件某个时间的截图
// https://cloud.tencent.com/document/product/436/55671
void GetSnapshotDemo(qcloud_cos::CosAPI& cos, const std::string& object_name,
                     const std::string& local_file) {
    GetSnapshotReq req(bucket_name, object_name, local_file);
    GetSnapshotResp resp;
    req.SetTime(100);
    // req.SetWitdh();
    // req.SetHeight();
    // req.SetFormat();
    CosResult result = cos.GetSnapshot(req, &resp);
    if (result.IsSucc()) {
        std::cout << "GetSnapshot Succ." << std::endl;
    } else {
        std::cout << "GetSnapshot Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================GetSnapshot==========================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 获取媒体信息
// https://cloud.tencent.com/document/product/436/55672
void GetMediaInfoDemo(qcloud_cos::CosAPI& cos, const std::string& object_name) {
    GetMediaInfoReq req(bucket_name, object_name);
    GetMediaInfoResp resp;
    CosResult result = cos.GetMediaInfo(req, &resp);
    if (result.IsSucc()) {
        std::cout << "GetMediaInfo Succ." << std::endl;
        std::cout << "Result: " << resp.GetResult().to_string() << std::endl;
    } else {
        std::cout << "GetMediaInfo Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================GetMediaInfo=========================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 获取私有 M3U8 ts 资源的下载授权
// https://cloud.tencent.com/document/product/436/63740
void GetPm3u8Demo(qcloud_cos::CosAPI& cos, const std::string& object_name,
                  const std::string& local_file) {
    GetPm3u8Req req(bucket_name, object_name, local_file);
    GetPm3u8Resp resp;
    req.SetExpires(3600);
    CosResult result = cos.GetPm3u8(req, &resp);
    if (result.IsSucc()) {
        std::cout << "GetPm3u8 Succ." << std::endl;
    } else {
        std::cout << "GetPm3u8 Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================GetPm3u8=============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 查询媒体处理队列
void DescribeMediaQueuesDemo(qcloud_cos::CosAPI& cos) {
    DescribeMediaQueuesReq req(bucket_name);
    DescribeQueuesResp resp;

    CosResult result = cos.DescribeMediaQueues(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeMediaQueues Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId()
                  << ", PageNumber: " << resp.GetPageNumber()
                  << ", PageSize: " << resp.GetPageSize()
                  << ", QueueList: " << resp.GetQueueList().to_string()
                  << ", NonExistPIDs: " << resp.GetNonExistPIDs().to_string() << std::endl;
    } else {
        std::cout << "DescribeMediaQueues Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeMediaQueues==================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 更新媒体处理队列
void UpdateMediaQueueDemo(qcloud_cos::CosAPI& cos, const std::string& queue_id) {
    UpdateMediaQueueReq req(bucket_name);
    UpdateQueueResp resp;

    req.SetName("queue-media-process-1");
    req.SetQueueId(queue_id);
    req.SetState("Active");

    NotifyConfig notify_config;
    notify_config.url = "http://example.com";
    notify_config.state = "On";
    notify_config.type = "Url";
    notify_config.event = "TransCodingFinish";
    req.SetNotifyConfig(notify_config);

    CosResult result = cos.UpdateMediaQueue(req, &resp);
    if (result.IsSucc()) {
        std::cout << "UpdateMediaQueue Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId()
                  << ", QueueList: " << resp.GetQueueList().to_string() << std::endl;
    } else {
        std::cout << "UpdateMediaQueue Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================UpdateMediaQueue=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        // DescribeMediaBucketsDemo(cos);
        // GetSnapshotDemo(cos, "1920_1080.mp4", "snapshot.jpg");
        // GetMediaInfoDemo(cos, "1920_1080.mp4");
        // GetPm3u8Demo(cos, "video.m3u8", "video_pm3u8.m3u8");
        // DescribeMediaQueuesDemo(cos);
        // UpdateMediaQueueDemo(cos, "xxx");
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
