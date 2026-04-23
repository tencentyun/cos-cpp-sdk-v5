#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行文档预览与文档处理任务管理：
 *   - DescribeDocProcessBuckets   : 查询开通文档预览的存储桶
 *   - DocPreview                  : 文档预览
 *   - CreateDocProcessJobs        : 提交文档预览任务
 *   - DescribeDocProcessJob       : 查询单个文档预览任务
 *   - DescribeDocProcessJobs      : 拉取符合条件的文档预览任务列表
 *   - DescribeDocProcessQueues    : 查询文档预览队列
 *   - UpdateDocProcessQueue       : 更新文档预览队列
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

// 查询文档预览开通状态
// https://cloud.tencent.com/document/product/436/54057
void DescribeDocProcessBucketsDemo(qcloud_cos::CosAPI& cos) {
    DescribeDocProcessBucketsReq req;
    DescribeDocProcessBucketsResp resp;

    req.SetRegions("ap-guangzhou");
    // req.SetBucketNames("xxx");
    // req.SetBucketName("xxx");
    // req.SetPageNumber();
    // req.SetPageSize();

    CosResult result = cos.DescribeDocProcessBuckets(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeDocProcessBuckets Succ." << std::endl;
        std::cout << "Result: " << resp.GetResult().to_string() << std::endl;
    } else {
        std::cout << "DescribeDocProcessBuckets Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeDocProcessBuckets============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 文档预览
// https://cloud.tencent.com/document/product/436/54058
void DocPreviewDemo(qcloud_cos::CosAPI& cos, const std::string& object_name,
                    const std::string& local_file) {
    DocPreviewReq req(bucket_name, object_name, local_file);
    req.SetSrcType("docx");
    DocPreviewResp resp;
    CosResult result = cos.DocPreview(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DocPreview Succ." << std::endl;
    } else {
        std::cout << "DocPreview Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DocPreview===========================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 提交文档预览任务
void CreateDocProcessJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDocProcessJobsReq req(bucket_name);
    CreateDocProcessJobsResp resp;
    req.SetVerifyCert(false);

    Input input;
    input.object = "test.docx";
    req.SetInput(input);

    Operation operation;
    Output output;
    output.bucket = bucket_name;
    output.region = "ap-chongqing";
    output.object = "/test-ci/test-create-doc-process-${Number}";
    operation.output = output;
    req.SetOperation(operation);
    // req.SetQueueId("xxx");

    CosResult result = cos.CreateDocProcessJobs(req, &resp);
    if (result.IsSucc()) {
        std::cout << "CreateDocProcessJobs Succ." << std::endl;
        std::cout << "jobs_detail: " << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "CreateDocProcessJobs Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================CreateDocProcessJobs=================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 查询单个文档预览任务
void DescribeDocProcessJobDemo(qcloud_cos::CosAPI& cos, const std::string& job_id) {
    DescribeDocProcessJobReq req(bucket_name);
    DescribeDocProcessJobResp resp;
    req.SetJobId(job_id);

    CosResult result = cos.DescribeDocProcessJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeDocProcessJob Succ." << std::endl;
        std::cout << "jobs_detail: " << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "DescribeDocProcessJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeDocProcessJob================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 拉取符合条件的文档预览任务列表
void DescribeDocProcessJobsDemo(qcloud_cos::CosAPI& cos, const std::string& queue_id) {
    DescribeDocProcessJobsReq req(bucket_name);
    DescribeDocProcessJobsResp resp;
    req.SetQueueId(queue_id);

    CosResult result = cos.DescribeDocProcessJobs(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeDocProcessJobs Succ." << std::endl;
        std::vector<JobsDetail> jobs_detail = resp.GetJobsDetails();
        for (std::vector<JobsDetail>::iterator it = jobs_detail.begin(); it != jobs_detail.end(); ++it) {
            std::cout << "job: " << it->to_string() << std::endl;
        }
        std::cout << "NextToken: " << resp.GetNextToken() << std::endl;
    } else {
        std::cout << "DescribeDocProcessJobs Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeDocProcessJobs===============" << std::endl;
    std::cout << "========================================================" << std::endl;
}

// 查询文档预览队列
void DescribeDocProcessQueuesDemo(qcloud_cos::CosAPI& cos) {
    DescribeDocProcessQueuesReq req(bucket_name);
    DescribeDocProcessQueuesResp resp;

    CosResult result = cos.DescribeDocProcessQueues(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeDocProcessQueues Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId()
                  << ", PageNumber: " << resp.GetPageNumber()
                  << ", PageSize: " << resp.GetPageSize()
                  << ", QueueList: " << resp.GetQueueList().to_string()
                  << ", NonExistPIDs: " << resp.GetNonExistPIDs().to_string() << std::endl;
    } else {
        std::cout << "DescribeDocProcessQueues Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeDocProcessQueues=============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 更新文档预览队列
void UpdateDocProcessQueueDemo(qcloud_cos::CosAPI& cos, const std::string& queue_id) {
    UpdateDocProcessQueueReq req(bucket_name);
    UpdateDocProcessQueueResp resp;

    req.SetName("queue-doc-process-1");
    req.SetQueueId(queue_id);
    req.SetState("Active");

    NotifyConfig notify_config;
    notify_config.url = "http://example.com";
    notify_config.state = "On";
    notify_config.type = "Url";
    notify_config.event = "TransCodingFinish";
    req.SetNotifyConfig(notify_config);

    CosResult result = cos.UpdateDocProcessQueue(req, &resp);
    if (result.IsSucc()) {
        std::cout << "UpdateDocProcessQueue Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId()
                  << ", QueueList: " << resp.GetQueueList().to_string() << std::endl;
    } else {
        std::cout << "UpdateDocProcessQueue Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================UpdateDocProcessQueue================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        // DescribeDocProcessBucketsDemo(cos);
        // DocPreviewDemo(cos, "test.docx", "test_preview.jpg");
        // CreateDocProcessJobsDemo(cos);
        // DescribeDocProcessJobDemo(cos, "xxx");
        // DescribeDocProcessJobsDemo(cos, "xxx");
        // DescribeDocProcessQueuesDemo(cos);
        // UpdateDocProcessQueueDemo(cos, "xxx");
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
