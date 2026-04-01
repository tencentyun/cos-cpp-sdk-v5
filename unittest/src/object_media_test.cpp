// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Description: 媒体/文档/文件处理/图片处理测试
//   包含: PutBucketToCI, ImageProcess, MediaTest, DocTest, FileProcessTest

#include "object_op_test_common.h"

namespace qcloud_cos {

TEST_F(ObjectOpTest, PutBucketToCITest) {
    bool use_dns_cache = CosSysConfig::GetUseDnsCache();
    CosSysConfig::SetUseDnsCache(false);
    {
        PutBucketToCIReq req(m_bucket_name);
        PutBucketToCIResp resp;
        req.SetHttps();
        CosResult result = m_client->PutBucketToCI(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        sleep(480);
    }
    CosSysConfig::SetUseDnsCache(use_dns_cache);
}

TEST_F(ObjectOpTest, ImageProcessTest) {
    bool use_dns_cache = CosSysConfig::GetUseDnsCache();
    CosSysConfig::SetUseDnsCache(false);
    std::string object_name = "test.jpg";
    // 上传处理
    {
        PutImageByFileReq req(m_bucket_name, object_name, "../../demo/test_file/test.jpg");
        PutImageByFileResp resp;

        PicOperation pic_operation;
        PicRules rule1;
        std::string newname = "/" + object_name + "_put_qrcode.jpg";
        rule1.fileid = newname;
        rule1.rule = "QRcode/cover/1";
        pic_operation.AddRule(rule1);
        req.SetPicOperation(pic_operation);
        CosResult result = m_client->PutImage(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        ASSERT_TRUE(m_client->IsObjectExist(m_bucket_name, newname));
        resp.GetUploadResult().to_string();
    }
    // 云上处理
    {
        CloudImageProcessReq req(m_bucket_name, object_name);
        CloudImageProcessResp resp;
        PicOperation pic_operation;
        PicRules rule;
        std::string newname = "/" + object_name + "_cut.jpg";
        rule.fileid = newname;
        rule.rule = "imageMogr2/cut/300x300";
        pic_operation.AddRule(rule);
        req.SetPicOperation(pic_operation);
        CosResult result = m_client->CloudImageProcess(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        ASSERT_TRUE(m_client->IsObjectExist(m_bucket_name, newname));
    }
    // 下载处理
    {
        GetQRcodeReq req(m_bucket_name, object_name);
        GetQRcodeResp resp;
        CosResult result = m_client->GetQRcode(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        ASSERT_EQ(resp.GetResult().qr_code_info[0].code_url, "testimage");
        resp.GetResult().to_string();
    }
    CosSysConfig::SetUseDnsCache(use_dns_cache);
}

// 媒体接口

TEST_F(ObjectOpTest, MediaTest) {
    bool use_dns_cache = CosSysConfig::GetUseDnsCache();
    CosSysConfig::SetUseDnsCache(false);
    std::string m_region = GetEnvVar("CPP_SDK_V5_REGION");
    std::string audio_object_name = "audio.mp3";

    std::string snapshot_job_id = "";
    std::string transcode_job_id = "";
    std::string animation_job_id = "";
    std::string concat_job_id = "";
    std::string smart_cover_job_id = "";
    std::string digital_watermark_job_id = "";
    std::string extract_digital_watermark_job_id = "";
    std::string video_montage_job_id = "";
    std::string voice_seperate_job_id = "";
    std::string segment_job_id = "";

    // 上传媒体
    {
      PutObjectByFileReq put_req(m_bucket_name, audio_object_name, "../../demo/test_file/audio.mp3");
      put_req.SetRecvTimeoutInms(1000 * 200);
      PutObjectByFileResp put_resp;
      CosResult put_result = m_client->PutObject(put_req, &put_resp);
      std::cout << put_result.GetErrorMsg() << std::endl;
      ASSERT_TRUE(put_result.IsSucc());
    }

    std::string image_object_name = "test.jpg";
    // 上传媒体
    {
      PutObjectByFileReq put_req(m_bucket_name, image_object_name, "../../demo/test_file/test.jpg");
      put_req.SetRecvTimeoutInms(1000 * 200);
      PutObjectByFileResp put_resp;
      CosResult put_result = m_client->PutObject(put_req, &put_resp);
      ASSERT_TRUE(put_result.IsSucc());
    }

    std::string object_name = "video.mp4";
    //上传媒体
    {
      PutObjectByFileReq put_req(m_bucket_name, object_name, "../../demo/test_file/video.mp4");
      put_req.SetRecvTimeoutInms(1000 * 200);
      PutObjectByFileResp put_resp;
      CosResult put_result = m_client->PutObject(put_req, &put_resp);
      ASSERT_TRUE(put_result.IsSucc());
    }

    std::string sub_title_name = "test.srt";
    //上传媒体
    {
      PutObjectByFileReq put_req(m_bucket_name, sub_title_name, "../../demo/test_file/test.srt");
      put_req.SetRecvTimeoutInms(1000 * 200);
      PutObjectByFileResp put_resp;
      CosResult put_result = m_client->PutObject(put_req, &put_resp);
      ASSERT_TRUE(put_result.IsSucc());
    }

    // 绑定媒体服务
    {
        for (int i = 0; i < 12; i++) {
            CreateMediaBucketReq req(m_bucket_name);
            CreateMediaBucketResp resp;
            CosResult result = m_client->CreateMediaBucket(req, &resp);
            if (!result.GetErrorMsg().empty() && result.GetErrorMsg().find("created by current account") != std::string::npos) {
                std::cout <<"create media bucket failed, retry after 10s: " << result.GetErrorMsg() << std::endl;
                sleep(10);
                continue;
            }
            std::cout << result.GetErrorMsg() << std::endl;
            ASSERT_TRUE(result.IsSucc());
            ASSERT_EQ(resp.GetResult().media_bucket.name, m_bucket_name);
            ASSERT_EQ(resp.GetResult().media_bucket.region, GetEnvVar("CPP_SDK_V5_REGION"));
        }
    }
      //info
      {
        GetMediaInfoReq req(m_bucket_name, object_name);
        GetMediaInfoResp resp;
        CosResult result = m_client->GetMediaInfo(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        ASSERT_EQ(resp.GetResult().media_info.stream.video.codec_name, "h264");
        ASSERT_EQ(resp.GetResult().media_info.stream.video.height, 360);
        ASSERT_EQ(resp.GetResult().media_info.stream.video.width, 640);
        ASSERT_EQ(resp.GetResult().media_info.stream.audio.codec_name, "aac");
        ASSERT_EQ(resp.GetResult().media_info.format.num_stream, 4);
      }
      //截图
      {
        GetSnapshotReq req(m_bucket_name, object_name, "local_file_snapshot.jpg");
        GetSnapshotResp resp;
        req.SetTime(10);
        CosResult result = m_client->GetSnapshot(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        ASSERT_EQ(resp.GetContentType(), "image/jpeg");
        TestUtils::RemoveFile("local_file_snapshot.jpg");
      }

      // 异步截图
      {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;
        JobsOptions opt;
        std::string output = "snapshot/test.jpg";
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = object_name;
        opt.tag = "Snapshot";
        opt.operation.snapshot.mode = "Interval";
        opt.operation.snapshot.start = "0";
        opt.operation.snapshot.time_interval = "5";
        opt.operation.snapshot.count = "1";

        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = output;
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        snapshot_job_id = resp.GetJobsDetail().job_id;
      }

      // 视频转码
      {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = object_name;
        opt.tag = "Transcode";

        // 使用转码参数提交任务
        opt.operation.transcode.container.format = "mkv";
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

        // 混音参数
        AudioMix audio_mix_1 = AudioMix();
        audio_mix_1.audio_source = "https://" + m_bucket_name + ".cos." + m_region + ".myqcloud.com/audio.mp3";
        audio_mix_1.mix_mode = "Once";
        audio_mix_1.replace = "true";
        audio_mix_1.effect_config.enable_start_fade_in = "true";
        audio_mix_1.effect_config.start_fade_in_time = "3";
        audio_mix_1.effect_config.enable_end_fade_out = "false";
        audio_mix_1.effect_config.end_fade_out_time = "0";
        audio_mix_1.effect_config.enable_bgm_fade = "true";
        audio_mix_1.effect_config.bgm_fade_time = "1.7";
        opt.operation.transcode.audio_mix_array.push_back(audio_mix_1);

        // 去除水印参数
        opt.operation.remove_watermark.dx = "150";
        opt.operation.remove_watermark.dy = "150";
        opt.operation.remove_watermark.width = "75";
        opt.operation.remove_watermark.height = "75";

        // 数字水印参数
        opt.operation.digital_watermark.type = "Text";
        opt.operation.digital_watermark.message = "12345678";
        opt.operation.digital_watermark.version = "V1";
        opt.operation.digital_watermark.ignore_error = "false";

        // 使用水印参数
        Watermark watermark_1 = Watermark();
        watermark_1.type = "Text";
        watermark_1.loc_mode = "Absolute";
        watermark_1.dx = "128";
        watermark_1.dy = "128";
        watermark_1.pos = "TopRight";
        watermark_1.start_time = "0";
        watermark_1.end_time = "100.5";
        watermark_1.text.text = "水印内容";
        watermark_1.text.font_size = "30";
        watermark_1.text.font_color = "0x0B172F";
        watermark_1.text.font_type = "simfang.ttf";
        watermark_1.text.transparency = "30";
        opt.operation.watermarks.push_back(watermark_1);

        Watermark watermark_2 = Watermark();
        watermark_2.type = "Image";
        watermark_2.loc_mode = "Absolute";
        watermark_2.dx = "128";
        watermark_2.dy = "128";
        watermark_2.pos = "TopRight";
        watermark_2.start_time = "0";
        watermark_2.end_time = "100.5";
        watermark_2.image.url = "https://" + m_bucket_name + ".cos." + m_region + ".myqcloud.com/test.jpg";
        watermark_2.image.mode = "Proportion";
        watermark_2.image.width = "10";
        watermark_2.image.height = "10";
        watermark_2.image.transparency = "30";
        opt.operation.watermarks.push_back(watermark_2);

        // 字幕参数
        Subtitle subtitle1 = Subtitle();
        subtitle1.url = "https://" + m_bucket_name + ".cos." + m_region + ".myqcloud.com/test.srt";

        Subtitle subtitle2 = Subtitle();
        subtitle2.url = "https://" + m_bucket_name + ".cos." + m_region + ".myqcloud.com/test.srt";

        Subtitles subtitles = Subtitles();
        subtitles.subtitle.push_back(subtitle1);
        subtitles.subtitle.push_back(subtitle2);
        opt.operation.subtitles = subtitles;

        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = "output/transcode.mp4";
        req.setOperation(opt);
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        transcode_job_id = resp.GetJobsDetail().job_id;
      }

      {
        // hls 加密
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = object_name;
        opt.tag = "Transcode";

        // 使用转码参数提交任务
        opt.operation.transcode.container.format = "hls";
        opt.operation.transcode.container.clip_config.duration = "5";

        opt.operation.transcode.video.codec = "H.264";
        opt.operation.transcode.audio.codec = "aac";
        opt.operation.transcode.audio.sample_rate = "16000";
        opt.operation.transcode.audio.bit_rate = "128";
        opt.operation.transcode.audio.channels = "2";
        opt.operation.transcode.trans_config.hls_encrypt.is_hls_encrypt = "true";
        opt.operation.transcode.trans_config.hls_encrypt.url_key = "http://abc.com/";
        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = "output/transcode.m3u8";
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        ASSERT_TRUE(result.IsSucc());
      }

      {
        // dash 加密
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = object_name;
        opt.tag = "Transcode";

        // 使用转码参数提交任务
        opt.operation.transcode.container.format = "dash";
        opt.operation.transcode.container.clip_config.duration = "5";
        opt.operation.transcode.video.codec = "H.264";
        opt.operation.transcode.audio.codec = "aac";
        opt.operation.transcode.audio.sample_rate = "16000";
        opt.operation.transcode.audio.bit_rate = "128";
        opt.operation.transcode.audio.channels = "2";
        opt.operation.transcode.trans_config.dash_encrypt.is_encrypt = "true";
        opt.operation.transcode.trans_config.dash_encrypt.url_key = "http://abc.com/";
        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = "output/transcode.dash";
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        ASSERT_TRUE(result.IsSucc());
      }

      // 动图
      {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = object_name;
        opt.tag = "Animation";
        opt.operation.animation.container.format = "gif";
        opt.operation.animation.video.codec = "gif";
        opt.operation.animation.video.width = "1280";
        opt.operation.animation.video.height = "960";
        opt.operation.animation.video.fps = "15";
        opt.operation.animation.video.animate_only_keep_key_frame = "true";
        opt.operation.animation.time_interval.start = "0";
        opt.operation.animation.time_interval.duration = "60";

        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = "animation/out.gif";
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        animation_job_id = resp.GetJobsDetail().job_id;
      }

      // 拼接
      {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = object_name;
        opt.tag = "Concat";
        ConcatFragment fragment1 = ConcatFragment();
        fragment1.url = "https://" + m_bucket_name + ".cos." + m_region + ".myqcloud.com/video.mp4";
        fragment1.start_time = '10';
        fragment1.end_time = '20';
        opt.operation.concat.concat_fragment.push_back(fragment1);
        opt.operation.concat.audio.codec = "mp3";
        opt.operation.concat.video.codec = "H.264";
        opt.operation.concat.video.bit_rate = "1000";
        opt.operation.concat.video.width = "1280";
        opt.operation.concat.video.height = "720";
        opt.operation.concat.video.fps = "30";
        opt.operation.concat.container.format = "mp4";
        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = "concat/out.mp4";
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        concat_job_id = resp.GetJobsDetail().job_id;
      }

      // 智能封面
      {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = object_name;
        opt.tag = "SmartCover";

        opt.operation.smart_cover.format = "jpg";
        opt.operation.smart_cover.width = "1280";
        opt.operation.smart_cover.height = "960";
        opt.operation.smart_cover.count = "1";
        opt.operation.smart_cover.delete_duplicates = "true";
        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = "smartcover/out.jpg";
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        smart_cover_job_id = resp.GetJobsDetail().job_id;
      }

      // 数字水印
      {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = object_name;
        opt.tag = "DigitalWatermark";
        // 使用参数形式提交任务
        opt.operation.digital_watermark.type = "Text";
        opt.operation.digital_watermark.version = "V1";
        opt.operation.digital_watermark.message = "水印内容";

        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = "digitalwatermark/out.mp4";
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        // ASSERT_TRUE(result.IsSucc());
        // digital_watermark_job_id = resp.GetJobsDetail().job_id;
      }

      // 提取数字水印
      {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = "digitalwatermark/out.mp4";
        opt.tag = "ExtractDigitalWatermark";
        // 使用参数形式提交任务
        opt.operation.extract_digital_watermark.type = "Text";
        opt.operation.extract_digital_watermark.version = "V1";

        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        // ASSERT_TRUE(result.IsSucc());
        // extract_digital_watermark_job_id = resp.GetJobsDetail().job_id;
      }

      // 精彩集锦任务
      {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = object_name;
        opt.tag = "VideoMontage";
        // 使用模版ID提交任务
        // opt.operation.template_id = "XXXXXXXXXXXXXXXXXXX";
        // 使用参数形式提交任务
        opt.operation.video_montage.container.format = "mp4";
        opt.operation.video_montage.video.codec = "H.264";
        opt.operation.video_montage.video.width = "1280";
        opt.operation.video_montage.video.bit_rate = "1000";
        opt.operation.video_montage.audio.codec = "aac";
        opt.operation.video_montage.audio.sample_format = "fltp";
        opt.operation.video_montage.audio.bit_rate = "128";
        opt.operation.video_montage.audio.channels = "4";

        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = "videomotage/out.mp4";
        req.setOperation(opt);

        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        // ASSERT_TRUE(result.IsSucc());
        // video_montage_job_id = resp.GetJobsDetail().job_id;
      }

      // 转封装
      {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;
        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = object_name;
        opt.tag = "Segment";
        // 使用模版ID提交任务
        // opt.operation.template_id = "XXXXXXXXXXXXXXXXXXX";
        // 使用参数形式提交任务
        opt.operation.segment.duration = "15";
        opt.operation.segment.format = "mp4";
        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = "output/segment/out-${number}";
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        // ASSERT_TRUE(result.IsSucc());
        // segment_job_id = resp.GetJobsDetail().job_id;
      }

      std::string m3u8_object_name = "pm3u8.m3u8";
      //上传媒体
      {
        PutObjectByFileReq put_req(m_bucket_name, m3u8_object_name, "../../demo/test_file/douyin.m3u8");
        put_req.SetRecvTimeoutInms(1000 * 200);
        PutObjectByFileResp put_resp;
        CosResult put_result = m_client->PutObject(put_req, &put_resp);
        // ASSERT_TRUE(put_result.IsSucc());
      }

      //上传媒体
      {
        PutObjectByFileReq put_req(m_bucket_name, "douyin-00000.ts", "../../demo/test_file/douyin-00000.ts");
        put_req.SetRecvTimeoutInms(1000 * 200);
        PutObjectByFileResp put_resp;
        CosResult put_result = m_client->PutObject(put_req, &put_resp);
        // ASSERT_TRUE(put_result.IsSucc());
      }

      // pm3u8
      {
        GetPm3u8Req req(m_bucket_name, m3u8_object_name, "./local_file_pm3u8.m3u8");
        GetPm3u8Resp resp;
        req.SetExpires(3600);
        CosResult result = m_client->GetPm3u8(req, &resp);
        // ASSERT_TRUE(result.IsSucc());
        TestUtils::RemoveFile("./local_file_pm3u8.m3u8");
      }

      // 人声分离
      {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = m_region;
        opt.input.object = audio_object_name;
        opt.tag = "VoiceSeparate";
        // 使用参数形式提交任务
        opt.operation.voice_separate.audio_mode = "IsAudio";
        opt.operation.voice_separate.audio_config.codec = "aac";
        opt.operation.voice_separate.audio_config.sample_rate = "11025";
        opt.operation.voice_separate.audio_config.bit_rate = "16";
        opt.operation.voice_separate.audio_config.channels = "2";
        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = m_region;
        opt.operation.output.object = "output/out.mp3";
        opt.operation.output.au_object = "output/au.mp3";
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        // ASSERT_TRUE(result.IsSucc());
        // voice_seperate_job_id = resp.GetJobsDetail().job_id;
      }

      // 查询任务
      {
        DescribeDataProcessJobReq snapshot_req(m_bucket_name);
        DescribeDataProcessJobResp snapshot_resp;

        snapshot_req.SetJobId(snapshot_job_id);
        CosResult result = m_client->DescribeDataProcessJob(snapshot_req, &snapshot_resp);
        ASSERT_TRUE(result.IsSucc());
        ASSERT_EQ(snapshot_resp.GetJobsDetail().state, "Success");

        DescribeDataProcessJobReq transcode_req(m_bucket_name);
        DescribeDataProcessJobResp transcode_resp;

        transcode_req.SetJobId(transcode_job_id);
        result = m_client->DescribeDataProcessJob(transcode_req, &transcode_resp);
        ASSERT_TRUE(result.IsSucc());

        DescribeDataProcessJobReq animation_req(m_bucket_name);
        DescribeDataProcessJobResp animation_resp;

        animation_req.SetJobId(animation_job_id);
        result = m_client->DescribeDataProcessJob(animation_req, &animation_resp);
        ASSERT_TRUE(result.IsSucc());

        DescribeDataProcessJobReq concat_req(m_bucket_name);
        DescribeDataProcessJobResp concat_resp;

        concat_req.SetJobId(concat_job_id);
        result = m_client->DescribeDataProcessJob(concat_req, &concat_resp);
        ASSERT_TRUE(result.IsSucc());

        DescribeDataProcessJobReq smart_cover_req(m_bucket_name);
        DescribeDataProcessJobResp smart_cover_resp;

        smart_cover_req.SetJobId(smart_cover_job_id);
        result = m_client->DescribeDataProcessJob(smart_cover_req, &smart_cover_resp);
        ASSERT_TRUE(result.IsSucc());

        DescribeDataProcessJobReq digital_watermark_req(m_bucket_name);
        DescribeDataProcessJobResp digital_watermark_resp;

        digital_watermark_req.SetJobId(digital_watermark_job_id);
        result = m_client->DescribeDataProcessJob(digital_watermark_req, &digital_watermark_resp);
        //ASSERT_TRUE(result.IsSucc());

        DescribeDataProcessJobReq extract_dw_req(m_bucket_name);
        DescribeDataProcessJobResp extract_dw_resp;

        extract_dw_req.SetJobId(extract_digital_watermark_job_id);
        result = m_client->DescribeDataProcessJob(extract_dw_req, &extract_dw_resp);
        //ASSERT_TRUE(result.IsSucc());

        DescribeDataProcessJobReq video_montage_req(m_bucket_name);
        DescribeDataProcessJobResp video_montage_resp;

        video_montage_req.SetJobId(video_montage_job_id);
        result = m_client->DescribeDataProcessJob(video_montage_req, &video_montage_resp);
        //ASSERT_TRUE(result.IsSucc());

        DescribeDataProcessJobReq voice_seperate_req(m_bucket_name);
        DescribeDataProcessJobResp voice_seperate_resp;

        voice_seperate_req.SetJobId(voice_seperate_job_id);
        result = m_client->DescribeDataProcessJob(voice_seperate_req, &voice_seperate_resp);
        //ASSERT_TRUE(result.IsSucc());

        DescribeDataProcessJobReq segment_req(m_bucket_name);
        DescribeDataProcessJobResp segment_resp;

        segment_req.SetJobId(segment_job_id);
        result = m_client->DescribeDataProcessJob(segment_req, &segment_resp);
        //ASSERT_TRUE(result.IsSucc());
      }

    CosSysConfig::SetUseDnsCache(use_dns_cache);
}

// 文档接口

TEST_F(ObjectOpTest, DocTest) {
    bool use_dns_cache = CosSysConfig::GetUseDnsCache();
    CosSysConfig::SetUseDnsCache(false);
    std::string object_name = "document.docx";
    std::string output_object = "/test-ci/test-create-doc-process-${Number}";
    std::string queue_id = "";
    std::string doc_job_id;

    // 上传媒体
    {
        PutObjectByFileReq put_req(m_bucket_name, object_name, "../../demo/test_file/document.docx");
        put_req.SetRecvTimeoutInms(1000 * 200);
        PutObjectByFileResp put_resp;
        CosResult put_result = m_client->PutObject(put_req, &put_resp);
        ASSERT_TRUE(put_result.IsSucc());
    }
    // 绑定文档预览服务
    {
        CreateDocBucketReq req(m_bucket_name);
        CreateDocBucketResp resp;
        CosResult result = m_client->CreateDocBucket(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        ASSERT_EQ(resp.GetResult().doc_bucket.name, m_bucket_name);
        ASSERT_EQ(resp.GetResult().doc_bucket.region, GetEnvVar("CPP_SDK_V5_REGION"));
        resp.GetResult().to_string();
    }
    // 查询文档预览桶列表
    {
        DescribeDocProcessBucketsReq req;
        DescribeDocProcessBucketsResp resp;
        CosResult result = m_client->DescribeDocProcessBuckets(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        resp.GetResult().to_string();
    }
    // 查询文档预览队列
    {
        DescribeDocProcessQueuesReq req(m_bucket_name);
        DescribeDocProcessQueuesResp resp;
        CosResult result = m_client->DescribeDocProcessQueues(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        ASSERT_EQ(resp.GetPageNumber(), 1);
        ASSERT_EQ(resp.GetQueueList().bucket_id, m_bucket_name);
        ASSERT_EQ(resp.GetQueueList().category, "DocProcessing");
        queue_id = resp.GetQueueList().queue_id;
        resp.GetQueueList().to_string();
        resp.GetNonExistPIDs().to_string();
    }
    // 更新文档预览队列
    {
        UpdateDocProcessQueueReq req(m_bucket_name);
        UpdateDocProcessQueueResp resp;
        req.SetName("queue-doc-process-1");
        req.SetQueueId(queue_id);
        req.SetState("Active");
        NotifyConfig notify_config;
        notify_config.url = "http://example.com";
        notify_config.state = "On";
        notify_config.type = "Url";
        notify_config.event = "TaskFinish";
        req.SetNotifyConfig(notify_config);
        CosResult result = m_client->UpdateDocProcessQueue(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        ASSERT_EQ(resp.GetQueueList().bucket_id, m_bucket_name);
        ASSERT_EQ(resp.GetQueueList().name, "queue-doc-process-1");
        ASSERT_EQ(resp.GetQueueList().state, "Active");
    }

    // 创建文档处理任务
    {
        CreateDocProcessJobsReq req(m_bucket_name);
        CreateDocProcessJobsResp resp;
        Input input;
        input.object = object_name;

        Operation operation;
        Output output;
        output.bucket = m_bucket_name;
        output.region = GetEnvVar("CPP_SDK_V5_REGION");
        output.object = output_object;

        operation.output = output;
        req.SetOperation(operation);
        req.SetQueueId(queue_id);
        req.SetTag("DocProcess");
        req.SetInput(input);
        CosResult result = m_client->CreateDocProcessJobs(req, &resp);
        doc_job_id = resp.GetJobsDetail().job_id;
        ASSERT_TRUE(result.IsSucc());
        ASSERT_EQ(resp.GetJobsDetail().state, "Submitted");
        resp.GetJobsDetail().to_string();
    }

    // 查询文档预览任务
    {
        DescribeDocProcessJobReq req(m_bucket_name);
        DescribeDocProcessJobResp resp;
        req.SetJobId(doc_job_id);
        CosResult result = m_client->DescribeDocProcessJob(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        resp.GetJobsDetail().to_string();
    }

    // 查询文档预览任务列表
    {
        DescribeDocProcessJobsReq req(m_bucket_name);
        DescribeDocProcessJobsResp resp;
        req.SetQueueId(queue_id);
        CosResult result = m_client->DescribeDocProcessJobs(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        for (size_t i = 0; i < resp.GetJobsDetails().size(); i++) {
            resp.GetJobsDetails().at(i).to_string();
        }
    }

    // 同步文档预览
    {
        std::string local_file = "./test_preview.jpg";
        DocPreviewReq req(m_bucket_name, object_name, local_file);
        DocPreviewResp resp;
        req.SetSrcType("docx");
        req.SetPage(1);
        req.SetSheet(1);
        req.SetDstType("jpg");
        req.SetExcelPaperDirection(0);
        req.SetQuality(100);
        req.SetScale(100);
        CosResult result = m_client->DocPreview(req, &resp);
        resp.GetSheetName();
        resp.GetTotalPage();
        resp.GetErrNo();
        resp.GetTotalSheet();
        TestUtils::RemoveFile(local_file);
    }

    CosSysConfig::SetUseDnsCache(use_dns_cache);
}

// 文件处理接口

TEST_F(ObjectOpTest, FileProcessTest) {
    bool use_dns_cache = CosSysConfig::GetUseDnsCache();
    CosSysConfig::SetUseDnsCache(false);
    std::string object_name = "test.zip";
    std::string output_object_prefix = "/test-ci/test-create-file-process-${Number}";
    std::string queue_id = "";
    std::string file_uncompress_job_id;

    // 上传媒体
    {
        PutObjectByFileReq put_req(m_bucket_name, object_name, "../../demo/test_file/test.zip");
        put_req.SetRecvTimeoutInms(1000 * 200);
        PutObjectByFileResp put_resp;
        CosResult put_result = m_client->PutObject(put_req, &put_resp);
        ASSERT_TRUE(put_result.IsSucc());
    }
    // 绑定文件处理服务
    {
        CreateFileBucketReq req(m_bucket_name);
        CreateFileBucketResp resp;
        CosResult result = m_client->CreateFileBucket(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        ASSERT_EQ(resp.GetResult().file_bucket.name, m_bucket_name);
        ASSERT_EQ(resp.GetResult().file_bucket.region, GetEnvVar("CPP_SDK_V5_REGION"));
        resp.GetResult().to_string();
    }
    // 查询文件处理桶列表
    {
        DescribeFileBucketsReq req;
        DescribeFileBucketsResp resp;
        CosResult result = m_client->DescribeFileBuckets(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        resp.GetResult().to_string();
    }

    {
        CreateDataProcessJobsReq req(m_bucket_name);
        CreateDataProcessJobsResp resp;

        JobsOptions opt;
        opt.input.bucket = m_bucket_name;
        opt.input.region = GetEnvVar("CPP_SDK_V5_REGION");
        opt.input.object = object_name;
        opt.tag = "FileUncompress";
        // 使用参数形式提交任务
        opt.operation.file_uncompress_config.prefix = output_object_prefix;
        opt.operation.file_uncompress_config.prefix_replaced = "1";
        opt.operation.output.bucket = m_bucket_name;
        opt.operation.output.region = GetEnvVar("CPP_SDK_V5_REGION");
        req.setOperation(opt);
        CosResult result = m_client->CreateDataProcessJobs(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        file_uncompress_job_id = resp.GetJobsDetail().job_id;
    }

    // 查询文件解压任务
    {
        DescribeDataProcessJobReq req(m_bucket_name);
        DescribeDataProcessJobResp resp;
        req.SetJobId(file_uncompress_job_id);
        CosResult result = m_client->DescribeDataProcessJob(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        resp.GetJobsDetail().to_string();
    }
    CosSysConfig::SetUseDnsCache(use_dns_cache);
}

} // namespace qcloud_cos
