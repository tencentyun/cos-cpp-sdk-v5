// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Description: 审核接口测试
//   包含: AuditingTest

#include "object_op_test_common.h"

namespace qcloud_cos {

TEST_F(ObjectOpTest, AuditingTest) {  
  bool use_dns_cache = CosSysConfig::GetUseDnsCache();
  CosSysConfig::SetUseDnsCache(false);
  //请勿改变审核测试前后的顺序
  std::string image_object_name = "test.jpg";
  std::string video_object_name = "video.mp4";
  //图片同步审核
  {
    GetImageAuditingReq req(m_bucket_name);
    GetImageAuditingResp resp;
    req.SetObjectKey(image_object_name);
    req.SetDataId("data_id_example");
    req.SetDetectType("ads,porn,politics,terrorism");
    req.SetInterval(1);
    req.SetMaxFrames(2);
    CosResult result = m_client->GetImageAuditing(req, &resp);
    resp.GetJobsDetail().to_string();
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetJobsDetail().GetDataId(), "data_id_example");
    ASSERT_EQ(resp.GetJobsDetail().GetState(), "Success");
    ASSERT_EQ(resp.GetJobsDetail().GetObject(), image_object_name);

    DescribeImageAuditingJobReq describe_req(m_bucket_name);
    DescribeImageAuditingJobResp describe_resp;
    describe_req.SetJobId(resp.GetJobsDetail().GetJobId());
    CosResult describe_result = m_client->DescribeImageAuditingJob(describe_req, &describe_resp);
    ASSERT_TRUE(describe_result.IsSucc());
    ASSERT_EQ(describe_resp.GetJobsDetail().GetDataId(), "data_id_example");
  }
  //批量图片审核
  {
    BatchImageAuditingReq req(m_bucket_name);
    BatchImageAuditingResp resp;
    AuditingInput input = AuditingInput();
    input.SetObject(image_object_name);
    input.SetDataId("data_id_example2");
    UserInfo user_info;
    user_info.SetAppId("appid");
    user_info.SetDeviceId("deviceid");
    user_info.SetIp("ip");
    user_info.SetNickName("nickname");
    user_info.SetRoom("room");
    user_info.SetTokenId("tokenid");
    user_info.SetType("type");
    input.SetUserInfo(user_info);
    req.AddInput(input);
    req.SetDetectType("Ads,Porn,Politics,Terrorism");
    CosResult result = m_client->BatchImageAuditing(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetJobsDetails()[0].GetDataId(), "data_id_example2");
    ASSERT_EQ(resp.GetJobsDetails()[0].GetState(), "Success");
    ASSERT_EQ(resp.GetJobsDetails()[0].GetObject(), image_object_name);
  }
  //提交视频审核任务
  std::string video_job_id;
  {
    CreateVideoAuditingJobReq req(m_bucket_name);
    CreateVideoAuditingJobResp resp;
    req.SetObject(video_object_name);
    req.SetDataId("DataId");
    SnapShotConf snap_shot = SnapShotConf();
    snap_shot.SetCount(5);
    snap_shot.SetMode("Interval");
    req.SetSnapShot(snap_shot);
    req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
    req.SetDetectContent(0);
    UserInfo user_info;
    user_info.SetAppId("appid");
    user_info.SetDeviceId("deviceid");
    user_info.SetIp("ip");
    user_info.SetNickName("nickname");
    user_info.SetRoom("room");
    user_info.SetTokenId("tokenid");
    user_info.SetType("type");
    req.SetUserInfo(user_info);
    CosResult result = m_client->CreateVideoAuditingJob(req, &resp);
    video_job_id = resp.GetJobsDetail().GetJobId();
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetJobsDetail().GetState(), "Submitted");
  }
  //提交音频审核任务
  std::string audio_object_name = "audio.mp3";
  std::string audio_job_id;
  {
    {
      PutObjectByFileReq put_req(m_bucket_name, audio_object_name, "../../demo/test_file/audio.mp3");
      put_req.SetRecvTimeoutInms(1000 * 200);
      PutObjectByFileResp put_resp;
      CosResult put_result = m_client->PutObject(put_req, &put_resp);
      ASSERT_TRUE(put_result.IsSucc());
    }
    {
      CreateAudioAuditingJobReq req(m_bucket_name);
      CreateAudioAuditingJobResp resp;
      req.SetObject(audio_object_name);
      req.SetDataId("DataId");
      req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
      req.SetCallBackVersion("Simple");
      CosResult result = m_client->CreateAudioAuditingJob(req, &resp);
      audio_job_id = resp.GetJobsDetail().GetJobId();
      ASSERT_TRUE(result.IsSucc());
      ASSERT_EQ(resp.GetJobsDetail().GetState(), "Submitted");
    }


  }
  //提交文本审核任务
  std::string text_object_name = "text.txt";
  std::string text_job_id;
  {
    {
      PutObjectByFileReq put_req(m_bucket_name, text_object_name, "../../demo/test_file/text.txt");
      put_req.SetRecvTimeoutInms(1000 * 200);
      PutObjectByFileResp put_resp;
      CosResult put_result = m_client->PutObject(put_req, &put_resp);
      ASSERT_TRUE(put_result.IsSucc());
    }
    {
      CreateTextAuditingJobReq req(m_bucket_name);
      CreateTextAuditingJobResp resp;
      req.SetObject(text_object_name);
      req.SetDataId("DataId");
      req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
      req.SetCallBackVersion("Simple");
      CosResult result = m_client->CreateTextAuditingJob(req, &resp);
      text_job_id = resp.GetJobsDetail().GetJobId();
      ASSERT_TRUE(result.IsSucc());
      ASSERT_EQ(resp.GetJobsDetail().GetState(), "Submitted");
    }
  }
  //提交文档审核任务
  std::string document_object_name = "document.docx";
  std::string document_job_id;
  {
    {
      PutObjectByFileReq put_req(m_bucket_name, document_object_name, "../../demo/test_file/document.docx");
      put_req.SetRecvTimeoutInms(1000 * 200);
      PutObjectByFileResp put_resp;
      CosResult put_result = m_client->PutObject(put_req, &put_resp);
      ASSERT_TRUE(put_result.IsSucc());
    }
    {
      CreateDocumentAuditingJobReq req(m_bucket_name);
      CreateDocumentAuditingJobResp resp;
      req.SetObject(document_object_name);
      req.SetDataId("DataId");
      req.SetType("docx");
      req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
      CosResult result = m_client->CreateDocumentAuditingJob(req, &resp);
      document_job_id = resp.GetJobsDetail().GetJobId();
      ASSERT_TRUE(result.IsSucc());
      ASSERT_EQ(resp.GetJobsDetail().GetState(), "Submitted");
    }
  }
  //提交网页审核任务
  std::string url_job_id;
  {
    CreateWebPageAuditingJobReq req(m_bucket_name);
    CreateWebPageAuditingJobResp resp;
    req.SetUrl("https://www.baidu.com/");
    req.SetDataId("DataId");
    req.SetDetectType("Porn,Ads,Politics,Terrorism,Abuse,Illegal");
    CosResult result = m_client->CreateWebPageAuditingJob(req, &resp);
    url_job_id = resp.GetJobsDetail().GetJobId();
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetJobsDetail().GetState(), "Submitted");
  }
  std::this_thread::sleep_for(std::chrono::seconds(10));
  //查询视频审核结果
  {
    DescribeVideoAuditingJobReq req(m_bucket_name);
    DescribeVideoAuditingJobResp resp;
    req.SetJobId(video_job_id);
    CosResult result = m_client->DescribeVideoAuditingJob(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetJobsDetail().GetState(), "Success");
    resp.GetJobsDetail().to_string();
  }
  //查询音频审核结果
  {
    DescribeAudioAuditingJobReq req(m_bucket_name);
    DescribeAudioAuditingJobResp resp;
    req.SetJobId(audio_job_id);
    CosResult result = m_client->DescribeAudioAuditingJob(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetJobsDetail().GetState(), "Success");
  }
  //查询文本审核结果
  {
    DescribeTextAuditingJobReq req(m_bucket_name);
    DescribeTextAuditingJobResp resp;
    req.SetJobId(text_job_id);
    CosResult result = m_client->DescribeTextAuditingJob(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetJobsDetail().GetState(), "Success");
    resp.GetJobsDetail().to_string();
  }
  //查询文档审核结果
  {
    DescribeDocumentAuditingJobReq req(m_bucket_name);
    DescribeDocumentAuditingJobResp resp;
    req.SetJobId(document_job_id);
    CosResult result = m_client->DescribeDocumentAuditingJob(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetJobsDetail().GetState(), "Success");
    resp.GetJobsDetail().GetLabels().to_string();
    if (resp.GetJobsDetail().GetPageSegment().HasResults()){
      resp.GetJobsDetail().GetPageSegment().to_string();
      resp.GetJobsDetail().GetPageSegment().GetResults()[0].to_string();
      resp.GetJobsDetail().to_string();
    }
  }
  //查询网页审核结果
  {
    DescribeWebPageAuditingJobReq req(m_bucket_name);
    DescribeWebPageAuditingJobResp resp;
    req.SetJobId(url_job_id);
    CosResult result = m_client->DescribeWebPageAuditingJob(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetJobsDetail().GetState(), "Success");
    resp.GetJobsDetail().to_string();
  }
  CosSysConfig::SetUseDnsCache(use_dns_cache);
}

}  // namespace qcloud_cos
