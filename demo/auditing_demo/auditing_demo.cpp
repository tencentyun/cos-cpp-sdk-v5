#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行内容审核，涵盖：
 *   图片审核：GetImageAuditing / BatchImageAuditing / DescribeImageAuditingJob
 *   视频审核：CreateVideoAuditingJob / DescribeVideoAuditingJob
 *   音频审核：CreateAudioAuditingJob / DescribeAudioAuditingJob
 *   文本审核：CreateTextAuditingJob / DescribeTextAuditingJob
 *   文档审核：CreateDocumentAuditingJob / DescribeDocumentAuditingJob
 *   网页审核：CreateWebPageAuditingJob / DescribeWebPageAuditingJob
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

// 图片同步审核
// https://cloud.tencent.com/document/product/436/45434
void GetImageAuditingDemo(qcloud_cos::CosAPI& cos, const std::string& object_name) {
    GetImageAuditingReq req(bucket_name);
    GetImageAuditingResp resp;
    req.SetObjectKey(object_name);
    // req.SetDetectUrl("https://exampleurl.com");
    // req.SetBizType("xxxxxxxxxxxxxxxxxxxxxxxxx");
    req.SetDataId("data_id_example");
    req.SetDetectType("ads,porn,politics,terrorism");
    req.SetInterval(1);
    req.SetMaxFrames(2);
    // req.SetLargeImageDetect(0);

    CosResult result = cos.GetImageAuditing(req, &resp);
    if (result.IsSucc()) {
        std::cout << "GetImageAuditing Succ." << std::endl;
        std::cout << "Result: \n" << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "GetImageAuditing Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================GetImageAuditing=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 图片批量审核
// https://cloud.tencent.com/document/product/436/63593
void BatchImageAuditingDemo(qcloud_cos::CosAPI& cos, const std::vector<std::string>& object_names) {
    BatchImageAuditingReq req(bucket_name);
    BatchImageAuditingResp resp;
    for (std::vector<std::string>::const_iterator it = object_names.begin();
         it != object_names.end(); ++it) {
        AuditingInput input = AuditingInput();
        input.SetObject(*it);
        input.SetDataId("data_id_example");
        req.AddInput(input);
    }
    req.SetDetectType("Ads,Porn,Politics,Terrorism");

    CosResult result = cos.BatchImageAuditing(req, &resp);
    if (result.IsSucc()) {
        std::cout << "BatchImageAuditing Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::vector<ImageAuditingJobsDetail> jobsDetails = resp.GetJobsDetails();
        for (std::vector<ImageAuditingJobsDetail>::const_iterator it = jobsDetails.begin();
             it != jobsDetails.end(); ++it) {
            std::cout << "JobsDetails: \n" << it->to_string() << std::endl;
        }
    } else {
        std::cout << "BatchImageAuditing Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================BatchImageAuditing===================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 图片审核任务结果查询
// https://cloud.tencent.com/document/product/436/68904
void DescribeImageAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& job_id) {
    DescribeImageAuditingJobReq req(bucket_name);
    DescribeImageAuditingJobResp resp;
    req.SetJobId(job_id);
    CosResult result = cos.DescribeImageAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeImageAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetail: \n" << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "DescribeImageAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeImageAuditingJob=============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 提交视频审核任务
// https://cloud.tencent.com/document/product/436/47316
void CreateVideoAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& object_name) {
    CreateVideoAuditingJobReq req(bucket_name);
    CreateVideoAuditingJobResp resp;
    req.SetObject(object_name);
    req.SetDataId("DataId");

    SnapShotConf snap_shot = SnapShotConf();
    snap_shot.SetCount(100);
    snap_shot.SetMode("Interval");
    req.SetSnapShot(snap_shot);
    req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
    req.SetCallBack("https://callback.com");
    req.SetCallBackVersion("Simple");
    req.SetDetectContent(1);

    CosResult result = cos.CreateVideoAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "CreateVideoAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetail: " << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "CreateVideoAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================CreateVideoAuditingJob===============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 查询视频审核任务结果
// https://cloud.tencent.com/document/product/436/47317
void DescribeVideoAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& job_id) {
    DescribeVideoAuditingJobReq req(bucket_name);
    DescribeVideoAuditingJobResp resp;
    req.SetJobId(job_id);
    CosResult result = cos.DescribeVideoAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeVideoAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetail: " << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "DescribeVideoAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeVideoAuditingJob=============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 提交音频审核任务
// https://cloud.tencent.com/document/product/436/54063
void CreateAudioAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& object_name) {
    CreateAudioAuditingJobReq req(bucket_name);
    CreateAudioAuditingJobResp resp;
    req.SetObject(object_name);
    req.SetDataId("DataId");
    req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
    req.SetCallBack("https://callback.com");
    req.SetCallBackVersion("Simple");

    CosResult result = cos.CreateAudioAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "CreateAudioAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetail: " << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "CreateAudioAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================CreateAudioAuditingJob===============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 查询音频审核任务结果
// https://cloud.tencent.com/document/product/436/54064
void DescribeAudioAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& job_id) {
    DescribeAudioAuditingJobReq req(bucket_name);
    DescribeAudioAuditingJobResp resp;
    req.SetJobId(job_id);
    CosResult result = cos.DescribeAudioAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeAudioAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetails: \n" << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "DescribeAudioAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeAudioAuditingJob=============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 提交文本审核任务
// https://cloud.tencent.com/document/product/436/56289
void CreateTextAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& object_name) {
    CreateTextAuditingJobReq req(bucket_name);
    CreateTextAuditingJobResp resp;
    req.SetObject(object_name);
    req.SetDataId("DataId");
    // req.SetContent("需要审核的文本");
    req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
    req.SetCallBack("https://callback.com");
    req.SetCallBackVersion("Simple");

    CosResult result = cos.CreateTextAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "CreateTextAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetail: " << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "CreateTextAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================CreateTextAuditingJob================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 查询文本审核任务结果
// https://cloud.tencent.com/document/product/436/56288
void DescribeTextAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& job_id) {
    DescribeTextAuditingJobReq req(bucket_name);
    DescribeTextAuditingJobResp resp;
    req.SetJobId(job_id);
    CosResult result = cos.DescribeTextAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeTextAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetails: " << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "DescribeTextAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeTextAuditingJob==============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 提交文档审核任务
// https://cloud.tencent.com/document/product/436/59381
void CreateDocumentAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& object_name) {
    CreateDocumentAuditingJobReq req(bucket_name);
    CreateDocumentAuditingJobResp resp;
    req.SetObject(object_name);
    req.SetDataId("DataId");
    req.SetType("docx");
    req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
    req.SetCallBack("https://callback.com");

    CosResult result = cos.CreateDocumentAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "CreateDocumentAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetail: " << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "CreateDocumentAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================CreateDocumentAuditingJob============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 查询文档审核任务结果
// https://cloud.tencent.com/document/product/436/59382
void DescribeDocumentAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& job_id) {
    DescribeDocumentAuditingJobReq req(bucket_name);
    DescribeDocumentAuditingJobResp resp;
    req.SetJobId(job_id);
    CosResult result = cos.DescribeDocumentAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeDocumentAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetails: \n" << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "DescribeDocumentAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeDocumentAuditingJob==========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 提交网页审核任务
// https://cloud.tencent.com/document/product/436/63958
void CreateWebPageAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& url) {
    CreateWebPageAuditingJobReq req(bucket_name);
    CreateWebPageAuditingJobResp resp;
    req.SetUrl(url);
    req.SetDataId("DataId");
    req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
    req.SetReturnHighligthHtml(true);
    req.SetCallBack("https://callback.com");

    CosResult result = cos.CreateWebPageAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "CreateWebPageAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetail: \n" << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "CreateWebPageAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================CreateWebPageAuditingJob=============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 查询网页审核任务结果
// https://cloud.tencent.com/document/product/436/63959
void DescribeWebPageAuditingJobDemo(qcloud_cos::CosAPI& cos, const std::string& job_id) {
    DescribeWebPageAuditingJobReq req(bucket_name);
    DescribeWebPageAuditingJobResp resp;
    req.SetJobId(job_id);
    CosResult result = cos.DescribeWebPageAuditingJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeWebPageAuditingJob Succ." << std::endl;
        std::cout << "RequestId: " << resp.GetRequestId() << std::endl;
        std::cout << "JobsDetails: " << resp.GetJobsDetail().to_string() << std::endl;
    } else {
        std::cout << "DescribeWebPageAuditingJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeWebPageAuditingJob===========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        // 下列调用均为演示用法，请根据实际情况取消对应注释并补充参数
        // GetImageAuditingDemo(cos, "data/audit/gif/test.gif");
        // std::vector<std::string> objects = {"data/audit/ads/clipboard.png", "data/audit/git/test.jpg"};
        // BatchImageAuditingDemo(cos, objects);
        // DescribeImageAuditingJobDemo(cos, "sifxxx");

        // CreateVideoAuditingJobDemo(cos, "data/audit/video/test.mp4");
        // DescribeVideoAuditingJobDemo(cos, "avxxx");

        // CreateAudioAuditingJobDemo(cos, "data/audit/audio/test.mp3");
        // DescribeAudioAuditingJobDemo(cos, "saxxx");

        // CreateTextAuditingJobDemo(cos, "data/audit/text/test.txt");
        // DescribeTextAuditingJobDemo(cos, "stxxx");

        // CreateDocumentAuditingJobDemo(cos, "data/audit/documents/test.docx");
        // DescribeDocumentAuditingJobDemo(cos, "sdxxx");

        // CreateWebPageAuditingJobDemo(cos, "https://www.test.com/");
        // DescribeWebPageAuditingJobDemo(cos, "shxxx");
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
