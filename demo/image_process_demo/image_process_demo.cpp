#include <stdlib.h>

#include <iostream>
#include <string>

#include "cos_api.h"
#include "cos_sys_config.h"
#include "util/codec_util.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 进行数据万象图片处理，涵盖：
 *   - ImageThumbnail            : 图片缩放
 *   - PutImage                  : 上传时处理图片（锐化/旋转等）
 *   - AddWatermark              : 添加盲水印
 *   - GetImageWithWatermark     : 下载时添加盲水印
 *   - ExtractWatermark          : 提取盲水印
 *   - CloudImageProcess         : 云上处理图片（持久化）
 *   - PutQRcode                 : 上传图片时识别二维码
 *   - GetQRcode                 : 下载图片时识别二维码
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

// 图片缩放（下载时处理）
void ImageThumbnailDemo(qcloud_cos::CosAPI& cos, const std::string& object_name,
                        const std::string& local_file) {
    GetObjectByFileReq req(bucket_name, object_name, local_file);
    req.AddParam("imageMogr2/thumbnail/!50p", "");
    GetObjectByFileResp resp;
    CosResult result = cos.GetObject(req, &resp);
    if (result.IsSucc()) {
        std::cout << "ImageThumbnail Succ." << std::endl;
    } else {
        std::cout << "ImageThumbnail Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
}

// 上传时处理图片（锐化 + 旋转）
void PutImageDemo(qcloud_cos::CosAPI& cos, const std::string& object_name,
                  const std::string& local_file) {
    PutImageByFileReq req(bucket_name, object_name, local_file);
    PutImageByFileResp resp;

    PicOperation pic_operation;
    PicRules rule;
    rule.fileid = "/" + object_name + "_sharpen.jpg";
    rule.rule = "imageMogr2/sharpen/70";
    pic_operation.AddRule(rule);

    rule.fileid = "/" + object_name + "_rotate.jpg";
    rule.rule = "imageMogr2/rotate/90";
    pic_operation.AddRule(rule);

    req.SetPicOperation(pic_operation);

    CosResult result = cos.PutImage(req, &resp);
    if (result.IsSucc()) {
        std::cout << "PutImage Succ." << std::endl;
        std::cout << "ProcessResult: " << resp.GetUploadResult().to_string() << std::endl;
    } else {
        std::cout << "PutImage Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================PutImage============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 添加盲水印
void AddWatermarkDemo(qcloud_cos::CosAPI& cos, const std::string& object_name,
                      const std::string& local_file) {
    PutImageByFileReq req(bucket_name, object_name, local_file);
    PutImageByFileResp resp;

    PicOperation pic_operation;
    PicRules rule;
    rule.fileid = "/" + object_name + "_watermark.jpg";
    rule.rule = "watermark/3/type/3/text/" + CodecUtil::Base64Encode("test wartermart");
    pic_operation.AddRule(rule);
    req.SetPicOperation(pic_operation);

    CosResult result = cos.PutImage(req, &resp);
    if (result.IsSucc()) {
        std::cout << "AddWatermark Succ." << std::endl;
        std::cout << "ProcessResult: " << resp.GetUploadResult().to_string() << std::endl;
    } else {
        std::cout << "AddWatermark Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================AddWatermark=========================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 下载图片时添加盲水印
// https://cloud.tencent.com/document/product/436/46782
void GetImageWithWatermarkDemo(qcloud_cos::CosAPI& cos, const std::string& object_name,
                               const std::string& local_file) {
    GetObjectByFileReq req(bucket_name, object_name, local_file);
    GetObjectByFileResp resp;

    std::string watermark_text = "test";
    std::string watermark_param = "watermark/3/type/3/text/" + CodecUtil::Base64Encode(watermark_text);
    req.AddParam(watermark_param, "");

    CosResult result = cos.GetObject(req, &resp);
    if (result.IsSucc()) {
        std::cout << "GetImageWithWatermark Succ." << std::endl;
    } else {
        std::cout << "GetImageWithWatermark Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================GetImageWithWatermark================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 提取盲水印
void ExtractWatermarkDemo(qcloud_cos::CosAPI& cos, const std::string& object_name,
                          const std::string& local_file) {
    PutImageByFileReq req(bucket_name, object_name, local_file);
    PutImageByFileResp resp;

    PicOperation pic_operation;
    PicRules rule;
    rule.fileid = "/" + object_name + "_watermark_extract.jpg";
    rule.rule = "watermark/4/type/3";
    pic_operation.AddRule(rule);
    req.SetPicOperation(pic_operation);

    CosResult result = cos.PutImage(req, &resp);
    if (result.IsSucc()) {
        std::cout << "ExtractWatermark Succ." << std::endl;
        std::cout << "ProcessResult: " << resp.GetUploadResult().to_string() << std::endl;
    } else {
        std::cout << "ExtractWatermark Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================ExtractWatermark=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 云上处理图片（持久化处理已有对象）
void CloudImageProcessDemo(qcloud_cos::CosAPI& cos, const std::string& object_name) {
    CloudImageProcessReq req(bucket_name, object_name);
    CloudImageProcessResp resp;

    PicOperation pic_operation;
    PicRules rule;
    rule.fileid = "/" + object_name + "_thumbnail.jpg";
    rule.rule = "imageMogr2/thumbnail/!30p";
    pic_operation.AddRule(rule);

    rule.fileid = "/" + object_name + "_cut.jpg";
    rule.rule = "imageMogr2/cut/300x300";
    pic_operation.AddRule(rule);
    req.SetPicOperation(pic_operation);

    CosResult result = cos.CloudImageProcess(req, &resp);
    if (result.IsSucc()) {
        std::cout << "CloudImageProcess Succ." << std::endl;
        std::cout << "ProcessResult: " << resp.GetUploadResult().to_string() << std::endl;
    } else {
        std::cout << "CloudImageProcess Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================CloudImageProcess====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 上传图片识别二维码
void PutQRcodeDemo(qcloud_cos::CosAPI& cos, const std::string& object_name,
                   const std::string& local_file) {
    PutImageByFileReq req(bucket_name, object_name, local_file);
    PutImageByFileResp resp;

    PicOperation pic_operation;
    PicRules rule1;
    rule1.fileid = "/" + object_name + "_put_qrcode.jpg";
    rule1.rule = "QRcode/cover/1";
    pic_operation.AddRule(rule1);
    req.SetPicOperation(pic_operation);

    CosResult result = cos.PutImage(req, &resp);
    if (result.IsSucc()) {
        std::cout << "PutQRcode Succ." << std::endl;
        std::cout << "ProcessResult: " << resp.GetUploadResult().to_string() << std::endl;
    } else {
        std::cout << "PutQRcode Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================PutQRcode============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 下载图片时识别二维码
void GetQRcodeDemo(qcloud_cos::CosAPI& cos, const std::string& object_name) {
    GetQRcodeReq req(bucket_name, object_name);
    GetQRcodeResp resp;

    CosResult result = cos.GetQRcode(req, &resp);
    if (result.IsSucc()) {
        std::cout << "GetQRcode Succ." << std::endl;
        std::cout << "Result: " << resp.GetResult().to_string() << std::endl;
    } else {
        std::cout << "GetQRcode Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================GetQRcode============================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        // 请根据实际情况替换 object_name 和 local_file
        // ImageThumbnailDemo(cos, "test_ci/flower.jpg", "./flower_thumbnail.jpg");
        // PutImageDemo(cos, "test_ci/flower.jpg", "./flower.jpg");
        // AddWatermarkDemo(cos, "test_ci/flower.jpg", "./flower.jpg");
        // GetImageWithWatermarkDemo(cos, "test_ci/flower.jpg", "./flower_download_with_matermark.jpg");
        // ExtractWatermarkDemo(cos, "test_ci/flower_with_watermark.jpg", "./flower_with_watermark.jpg");
        // CloudImageProcessDemo(cos, "test_ci/flower.jpg");
        // PutQRcodeDemo(cos, "test_ci/qrcode.png", "./qrcode.png");
        // GetQRcodeDemo(cos, "test_ci/qrcode.png");
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
