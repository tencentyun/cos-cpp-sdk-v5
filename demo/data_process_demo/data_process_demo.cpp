#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"

/**
 * 本样例演示了如何使用 COS C++ SDK 提交与查询各类异步数据处理任务，涵盖：
 *   - CreateFileUncompressJobs       : 文件解压缩任务
 *   - DescribeFileUncompressJobs     : 查询文件解压缩任务
 *   - DescribeDataProcessJobs        : 查询异步任务
 *   - CancelDataProcessJob           : 取消异步任务
 *   - CreateSnapshotJobs             : 视频截图任务
 *   - CreateTranscodeJobs            : 视频转码任务
 *   - CreateAnimationJobs            : 视频转动图任务
 *   - CreateConcatJobs               : 视频拼接任务
 *   - CreateSmartCoverJobs           : 智能封面任务
 *   - CreateDigitalWatermarkJobs     : 数字水印任务
 *   - CreateExtractDigitalWatermarkJobs : 提取数字水印任务
 *   - CreateVideoMontageJobs         : 精彩集锦任务
 *   - CreateVoiceSeparateJobs        : 人声分离任务
 *   - CreateSegmentJobs              : 转封装(分段)任务
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

// 创建文件解压缩任务
void CreateFileUncompressJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.zip";
    opt.tag = "FileUncompress";

    opt.operation.file_uncompress_config.prefix = "output/";
    opt.operation.file_uncompress_config.prefix_replaced = "1";
    // opt.operation.file_uncompress_config.un_compress_key = "MTIzNDU2Nzg5MA==";

    opt.operation.output.bucket = bucket_name;
    opt.operation.output.region = "ap-chongqing";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    if (result.IsSucc()) {
        std::cout << "CreateFileUncompressJobs Succ." << std::endl;
    } else {
        std::cout << "CreateFileUncompressJobs Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================CreateFileUncompressJobs=============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 查询文件解压缩任务
void DescribeFileUncompressJobsDemo(qcloud_cos::CosAPI& cos, const std::string& job_id) {
    DescribeDataProcessJobReq req(bucket_name);
    DescribeDataProcessJobResp resp;
    req.SetJobId(job_id);

    CosResult result = cos.DescribeDataProcessJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeFileUncompressJobs Succ." << std::endl;
    } else {
        std::cout << "DescribeFileUncompressJobs Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeFileUncompressJobs===========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 查询异步任务
void DescribeDataProcessJobsDemo(qcloud_cos::CosAPI& cos, const std::string& job_id) {
    DescribeDataProcessJobReq req(bucket_name);
    DescribeDataProcessJobResp resp;
    req.SetJobId(job_id);

    CosResult result = cos.DescribeDataProcessJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "DescribeDataProcessJob Succ." << std::endl;
    } else {
        std::cout << "DescribeDataProcessJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================DescribeDataProcessJob===============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 取消异步任务
void CancelDataProcessJobDemo(qcloud_cos::CosAPI& cos, const std::string& job_id) {
    CancelDataProcessJobReq req(bucket_name);
    CancelDataProcessJobResp resp;
    req.SetJobId(job_id);

    CosResult result = cos.CancelDataProcessJob(req, &resp);
    if (result.IsSucc()) {
        std::cout << "CancelDataProcessJob Succ." << std::endl;
    } else {
        std::cout << "CancelDataProcessJob Fail, ErrorMsg: " << result.GetErrorMsg() << std::endl;
    }
    std::cout << "===================CancelDataProcessJob=================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 创建视频截图任务
void CreateSnapshotJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.mp4";
    opt.tag = "Snapshot";
    opt.operation.snapshot.mode = "Interval";
    opt.operation.snapshot.start = "0";
    opt.operation.snapshot.time_interval = "5";
    opt.operation.snapshot.count = "3";

    opt.operation.output.bucket = bucket_name;
    opt.operation.output.region = "ap-chongqing";
    opt.operation.output.object = "snapshot/test-${number}.jpg";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    std::cout << "===================CreateSnapshotJobs===================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 创建视频转码任务（简化版，仅核心参数；完整参数请参考官方文档）
void CreateTranscodeJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.mp4";
    opt.tag = "Transcode";

    // 基础转码参数
    opt.operation.transcode.container.format = "mp4";
    opt.operation.transcode.video.codec = "H.264";
    opt.operation.transcode.video.profile = "high";
    opt.operation.transcode.video.bit_rate = "1000";
    opt.operation.transcode.video.width = "1280";
    opt.operation.transcode.video.fps = "30";
    opt.operation.transcode.video.preset = "medium";
    opt.operation.transcode.audio.codec = "aac";
    opt.operation.transcode.audio.sample_format = "fltp";
    opt.operation.transcode.audio.bit_rate = "128";
    opt.operation.transcode.audio.channels = "4";
    opt.operation.transcode.trans_config.adj_dar_method = "scale";
    opt.operation.transcode.trans_config.is_check_audio_bit_rate = "false";
    opt.operation.transcode.trans_config.reso_adj_method = "1";
    opt.operation.transcode.time_interval.start = "0";
    opt.operation.transcode.time_interval.duration = "10";

    // 混音参数示例
    AudioMix audio_mix = AudioMix();
    audio_mix.audio_source = "https://test-12345.cos.ap-chongqing.myqcloud.com/test.wav";
    audio_mix.mix_mode = "Once";
    audio_mix.replace = "true";
    audio_mix.effect_config.enable_start_fade_in = "true";
    audio_mix.effect_config.start_fade_in_time = "3";
    audio_mix.effect_config.enable_end_fade_out = "false";
    audio_mix.effect_config.end_fade_out_time = "0";
    audio_mix.effect_config.enable_bgm_fade = "true";
    audio_mix.effect_config.bgm_fade_time = "1.7";
    opt.operation.transcode.audio_mix_array.push_back(audio_mix);

    // 去除水印参数
    opt.operation.remove_watermark.dx = "150";
    opt.operation.remove_watermark.dy = "150";
    opt.operation.remove_watermark.width = "75";
    opt.operation.remove_watermark.height = "75";

    // 数字水印参数
    opt.operation.digital_watermark.type = "Text";
    opt.operation.digital_watermark.message = "12345";
    opt.operation.digital_watermark.version = "V1";
    opt.operation.digital_watermark.ignore_error = "false";

    // 水印参数
    Watermark watermark = Watermark();
    watermark.type = "Text";
    watermark.loc_mode = "Absolute";
    watermark.dx = "128";
    watermark.dy = "128";
    watermark.pos = "TopRight";
    watermark.start_time = "0";
    watermark.end_time = "100.5";
    watermark.text.text = "水印内容";
    watermark.text.font_size = "30";
    watermark.text.font_color = "0x0B172F";
    watermark.text.font_type = "simfang.ttf";
    watermark.text.transparency = "30";
    opt.operation.watermarks.push_back(watermark);

    opt.operation.output.bucket = bucket_name;
    opt.operation.output.region = "ap-chongqing";
    opt.operation.output.object = "output/test.mp4";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    std::cout << "===================CreateTranscodeJobs==================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 创建视频转动图任务
void CreateAnimationJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.mp4";
    opt.tag = "Animation";
    opt.operation.animation.container.format = "gif";
    opt.operation.animation.video.codec = "gif";
    opt.operation.animation.video.width = "1280";
    opt.operation.animation.video.height = "960";
    opt.operation.animation.video.fps = "15";
    opt.operation.animation.video.animate_only_keep_key_frame = "true";
    opt.operation.animation.time_interval.start = "0";
    opt.operation.animation.time_interval.duration = "60";

    opt.operation.output.bucket = bucket_name;
    opt.operation.output.region = "ap-chongqing";
    opt.operation.output.object = "output/out.${ext}";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    std::cout << "===================CreateAnimationJobs==================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 创建视频拼接任务
void CreateConcatJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.mp4";
    opt.tag = "Concat";

    ConcatFragment fragment1 = ConcatFragment();
    fragment1.url = "http://test-12345.cos.ap-chongqing.myqcloud.com/start.mp4";
    opt.operation.concat.concat_fragment.push_back(fragment1);
    ConcatFragment fragment2 = ConcatFragment();
    fragment2.url = "http://test-12345.cos.ap-chongqing.myqcloud.com/end.mp4";
    opt.operation.concat.concat_fragment.push_back(fragment2);
    opt.operation.concat.audio.codec = "mp3";
    opt.operation.concat.video.codec = "H.264";
    opt.operation.concat.video.bit_rate = "1000";
    opt.operation.concat.video.width = "1280";
    opt.operation.concat.video.height = "720";
    opt.operation.concat.video.fps = "30";
    opt.operation.concat.container.format = "mp4";

    opt.operation.output.bucket = bucket_name;
    opt.operation.output.region = "ap-chongqing";
    opt.operation.output.object = "output/out.${ext}";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    std::cout << "===================CreateConcatJobs=====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 创建智能封面任务
void CreateSmartCoverJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.mp4";
    opt.tag = "SmartCover";
    opt.operation.smart_cover.format = "jpg";
    opt.operation.smart_cover.width = "1280";
    opt.operation.smart_cover.height = "960";
    opt.operation.smart_cover.count = "5";
    opt.operation.smart_cover.delete_duplicates = "true";

    opt.operation.output.bucket = bucket_name;
    opt.operation.output.region = "ap-chongqing";
    opt.operation.output.object = "output/out-${number}.jpg";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    std::cout << "===================CreateSmartCoverJobs=================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 创建数字水印任务
void CreateDigitalWatermarkJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.mp4";
    opt.tag = "DigitalWatermark";
    opt.operation.digital_watermark.type = "Text";
    opt.operation.digital_watermark.version = "V1";
    opt.operation.digital_watermark.message = "水印内容";

    opt.operation.output.bucket = bucket_name;
    opt.operation.output.region = "ap-chongqing";
    opt.operation.output.object = "output/out.${ext}";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    std::cout << "===================CreateDigitalWatermarkJobs===========" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 创建提取数字水印任务
void CreateExtractDigitalWatermarkJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.mp4";
    opt.tag = "ExtractDigitalWatermark";
    opt.operation.extract_digital_watermark.type = "Text";
    opt.operation.extract_digital_watermark.version = "V1";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    std::cout << "===================CreateExtractDigitalWatermarkJobs====" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 创建精彩集锦任务
void CreateVideoMontageJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.mp4";
    opt.tag = "VideoMontage";
    opt.operation.video_montage.container.format = "mp4";
    opt.operation.video_montage.video.codec = "H.264";
    opt.operation.video_montage.video.width = "1280";
    opt.operation.video_montage.video.bit_rate = "1000";
    opt.operation.video_montage.audio.codec = "aac";
    opt.operation.video_montage.audio.sample_format = "fltp";
    opt.operation.video_montage.audio.bit_rate = "128";
    opt.operation.video_montage.audio.channels = "4";

    opt.operation.output.bucket = bucket_name;
    opt.operation.output.region = "ap-chongqing";
    opt.operation.output.object = "output/out.${ext}";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    std::cout << "===================CreateVideoMontageJobs===============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 创建人声分离任务
void CreateVoiceSeparateJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.mp3";
    opt.tag = "VoiceSeparate";
    opt.operation.voice_separate.audio_mode = "IsAudio";
    opt.operation.voice_separate.audio_config.codec = "aac";
    opt.operation.voice_separate.audio_config.sample_rate = "11025";
    opt.operation.voice_separate.audio_config.bit_rate = "16";
    opt.operation.voice_separate.audio_config.channels = "2";

    opt.operation.output.bucket = bucket_name;
    opt.operation.output.region = "ap-chongqing";
    opt.operation.output.object = "output/out.${ext}";
    opt.operation.output.au_object = "output/au_object.${ext}";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    std::cout << "===================CreateVoiceSeparateJobs==============" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

// 创建转封装(分段)任务
void CreateSegmentJobsDemo(qcloud_cos::CosAPI& cos) {
    CreateDataProcessJobsReq req(bucket_name);
    CreateDataProcessJobsResp resp;

    JobsOptions opt;
    opt.input.bucket = bucket_name;
    opt.input.region = "ap-chongqing";
    opt.input.object = "test.mp4";
    opt.tag = "Segment";
    opt.operation.segment.duration = "15";
    opt.operation.segment.format = "mp4";

    opt.operation.output.bucket = bucket_name;
    opt.operation.output.region = "ap-chongqing";
    opt.operation.output.object = "output/out-${number}";
    req.setOperation(opt);

    CosResult result = cos.CreateDataProcessJobs(req, &resp);
    std::cout << "===================CreateSegmentJobs====================" << std::endl;
    PrintResult(result, resp);
    std::cout << "========================================================" << std::endl;
}

int main() {
    try {
        qcloud_cos::CosAPI cos = InitCosAPI();
        CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_ERR);

        // 下列调用均为演示用法，请根据实际情况取消对应注释并补充参数
        // CreateFileUncompressJobsDemo(cos);
        // DescribeFileUncompressJobsDemo(cos, "jxxxxxx");
        // DescribeDataProcessJobsDemo(cos, "jxxxxxx");
        // CancelDataProcessJobDemo(cos, "jxxxxxx");
        // CreateSnapshotJobsDemo(cos);
        // CreateTranscodeJobsDemo(cos);
        // CreateAnimationJobsDemo(cos);
        // CreateConcatJobsDemo(cos);
        // CreateSmartCoverJobsDemo(cos);
        // CreateDigitalWatermarkJobsDemo(cos);
        // CreateExtractDigitalWatermarkJobsDemo(cos);
        // CreateVideoMontageJobsDemo(cos);
        // CreateVoiceSeparateJobsDemo(cos);
        // CreateSegmentJobsDemo(cos);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "]]" << '\n';
    }
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
