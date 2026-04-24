// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Description: 分块上传测试
//   包含: MultiPutObject, MultiPutObject_OneStep, PutObjectResumableSingleThreadSync,
//         AbortMultiUpload, UploadPartCopyData, MultiUploadVaryName,
//         MultiUploadVaryPartSizeAndThreadPoolSize

#include "object_op_test_common.h"

namespace qcloud_cos {

TEST_F(ObjectOpTest, MultiPutObjectTest) {
  {
    uint64_t part_size = 20 * 1000 * 1000;
    uint64_t max_part_num = 3;
    std::string object_name = "object_test_multi";
    InitMultiUploadReq init_req(m_bucket_name, object_name);
    InitMultiUploadResp init_resp;
    CosResult init_result = m_client->InitMultiUpload(init_req, &init_resp);
    ASSERT_TRUE(init_result.IsSucc());

    std::vector<std::string> etags;
    std::vector<uint64_t> part_numbers;
    for (uint64_t part_cnt = 0; part_cnt < max_part_num; ++part_cnt) {
      std::string str(part_size * (part_cnt + 1), 'a');  // 分块大小倍增
      std::stringstream ss;
      ss << str;
      UploadPartDataReq req(m_bucket_name, object_name, init_resp.GetUploadId(),
                            ss);
      UploadPartDataResp resp;
      req.SetPartNumber(part_cnt + 1);
      req.SetRecvTimeoutInms(1000 * 200);

      CosResult result = m_client->UploadPartData(req, &resp);
      ASSERT_TRUE(result.IsSucc());
      etags.push_back(resp.GetEtag());
      part_numbers.push_back(part_cnt + 1);
    }

    // 测试ListParts
    {
      ListPartsReq req(m_bucket_name, object_name, init_resp.GetUploadId());
      ListPartsResp resp;

      CosResult result = m_client->ListParts(req, &resp);
      ASSERT_TRUE(result.IsSucc());
      EXPECT_EQ(m_bucket_name, resp.GetBucket());
      EXPECT_EQ(object_name, resp.GetKey());
      EXPECT_EQ(init_resp.GetUploadId(), resp.GetUploadId());
      const std::vector<Part>& parts = resp.GetParts();
      EXPECT_EQ(max_part_num, parts.size());
      for (size_t idx = 0; idx != parts.size(); ++idx) {
        EXPECT_EQ(part_numbers[idx], parts[idx].m_part_num);
        EXPECT_EQ(part_size * (idx + 1), parts[idx].m_size);
        EXPECT_EQ(etags[idx], parts[idx].m_etag);
      }
    }

    CompleteMultiUploadReq comp_req(m_bucket_name, object_name,
                                    init_resp.GetUploadId());
    CompleteMultiUploadResp comp_resp;
    comp_req.SetEtags(etags);
    comp_req.SetPartNumbers(part_numbers);

    CosResult result = m_client->CompleteMultiUpload(comp_req, &comp_resp);
    EXPECT_TRUE(result.IsSucc());
  }

  // 服务端加密
  {
    uint64_t part_size = 20 * 1000 * 1000;
    uint64_t max_part_num = 3;
    std::string object_name = "object_test_multi_and_enc";
    InitMultiUploadReq init_req(m_bucket_name, object_name);
    init_req.SetXCosServerSideEncryption("AES256");
    InitMultiUploadResp init_resp;
    CosResult init_result = m_client->InitMultiUpload(init_req, &init_resp);
    ASSERT_TRUE(init_result.IsSucc());
    EXPECT_EQ("AES256", init_resp.GetXCosServerSideEncryption());

    std::vector<std::string> etags;
    std::vector<uint64_t> part_numbers;
    for (uint64_t part_cnt = 0; part_cnt < max_part_num; ++part_cnt) {
      std::string str(part_size * (part_cnt + 1), 'b');  // 分块大小倍增
      std::stringstream ss;
      ss << str;
      UploadPartDataReq req(m_bucket_name, object_name, init_resp.GetUploadId(),
                            ss);
      UploadPartDataResp resp;
      req.SetPartNumber(part_cnt + 1);
      req.SetRecvTimeoutInms(1000 * 200);

      CosResult result = m_client->UploadPartData(req, &resp);
      ASSERT_TRUE(result.IsSucc());
      EXPECT_EQ("AES256", resp.GetXCosServerSideEncryption());
      etags.push_back(resp.GetEtag());
      part_numbers.push_back(part_cnt + 1);
    }

    // 测试ListParts
    {
      ListPartsReq req(m_bucket_name, object_name, init_resp.GetUploadId());
      ListPartsResp resp;

      CosResult result = m_client->ListParts(req, &resp);
      ASSERT_TRUE(result.IsSucc());
      EXPECT_EQ(m_bucket_name, resp.GetBucket());
      EXPECT_EQ(object_name, resp.GetKey());
      EXPECT_EQ(init_resp.GetUploadId(), resp.GetUploadId());
      const std::vector<Part>& parts = resp.GetParts();
      EXPECT_EQ(max_part_num, parts.size());
      for (size_t idx = 0; idx != parts.size(); ++idx) {
        EXPECT_EQ(part_numbers[idx], parts[idx].m_part_num);
        EXPECT_EQ(part_size * (idx + 1), parts[idx].m_size);
        EXPECT_EQ(etags[idx], parts[idx].m_etag);
      }
    }

    CompleteMultiUploadReq comp_req(m_bucket_name, object_name,
                                    init_resp.GetUploadId());
    CompleteMultiUploadResp comp_resp;
    comp_req.SetEtags(etags);
    comp_req.SetPartNumbers(part_numbers);

    CosResult result = m_client->CompleteMultiUpload(comp_req, &comp_resp);
    EXPECT_EQ("AES256", comp_resp.GetXCosServerSideEncryption());
    EXPECT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpTest, MultiPutObjectTest_OneStep) {
  {
    std::string filename = "multi_upload_object_one_step";
    std::string object_name = filename;
    // 1. 生成个临时文件, 用于分块上传
    {
      std::ofstream fs;
      fs.open(filename.c_str(), std::ios::out | std::ios::binary);
      std::string str(10 * 1000 * 1000, 'b');
      for (int idx = 0; idx < 10; ++idx) {
        fs << str;
      }
      fs.close();
    }

    // 2. 上传
    MultiPutObjectReq req(m_bucket_name, object_name, filename);
    //req.SetHttps();
    req.SetSSLCtxCallback(SslCtxCallback, nullptr);
    req.SetRecvTimeoutInms(1000 * 200);
    MultiPutObjectResp resp;

    CosResult result = m_client->MultiPutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());

    // 3. 删除临时文件
    if (-1 == remove(filename.c_str())) {
      std::cout << "Remove temp file=" << filename << " fail." << std::endl;
    }
  }

  {
    std::string filename = "multi_upload_object_enc_one_step";
    std::string object_name = filename;
    // 1. 生成个临时文件, 用于分块上传
    {
      std::ofstream fs;
      fs.open(filename.c_str(), std::ios::out | std::ios::binary);
      std::string str(10 * 1000 * 1000, 'b');
      for (int idx = 0; idx < 10; ++idx) {
        fs << str;
      }
      fs.close();
    }

    // 2. 上传
    MultiPutObjectReq req(m_bucket_name, object_name, filename);
    req.SetXCosServerSideEncryption("AES256");
    req.SetCheckPartCrc64(true);
    MultiPutObjectResp resp;

    CosResult result = m_client->MultiPutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("AES256", resp.GetXCosServerSideEncryption());

    // 3. 删除临时文件
    if (-1 == remove(filename.c_str())) {
      std::cout << "Remove temp file=" << filename << " fail." << std::endl;
    }
  }

  // 分块带各种头部上传
  {
    std::string filename = "multi_upload_object_with_header";
    std::string object_name = filename;
    // 1. 生成个临时文件, 用于分块上传
    {
      std::ofstream fs;
      fs.open(filename.c_str(), std::ios::out | std::ios::binary);
      std::string str(10 * 1000 * 1000, 'b');
      for (int idx = 0; idx < 10; ++idx) {
        fs << str;
      }
      fs.close();
    }

    // 2. 上传
    MultiPutObjectReq req(m_bucket_name, object_name, filename);
    req.SetXCosStorageClass(kStorageClassStandard);
    req.SetCacheControl("no-cache");
    req.SetCheckCRC64(true);
    req.SetCheckMD5(true);
    req.SetContentEncoding("deflate");
    req.SetContentType("text/plain");
    req.SetXCosAcl("public-read");
    req.SetXCosMeta("test", "test");
    MultiPutObjectResp resp;

    CosResult result = m_client->MultiPutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());

    // 3. 删除临时文件
    if (-1 == remove(filename.c_str())) {
      std::cout << "Remove temp file=" << filename << " fail." << std::endl;
    }
  }
}

TEST_F(ObjectOpTest, PutObjectResumableSingleThreadSyncTest) {
  {
    std::string filename = "single_upload_object_one_step";
    std::string object_name = filename;
    // 1. 生成个临时文件, 用于分块上传
    {
      std::ofstream fs;
      fs.open(filename.c_str(), std::ios::out | std::ios::binary);
      std::string str(10 * 1000 * 1000, 'b');
      for (int idx = 0; idx < 10; ++idx) {
        fs << str;
      }
      fs.close();
    }

    // 2. 上传
    qcloud_cos::PutObjectResumableSingleSyncReq req(m_bucket_name, object_name, filename);
    //req.SetHttps();
    req.AddHeader("x-cos-meta-ssss1","1xxxxxxx");
    req.AddHeader("x-cos-meta-ssss2","2xxxxxxx");
    req.AddHeader("x-cos-meta-ssss3","3xxxxxxx");
    req.AddHeader("x-cos-meta-ssss4","4xxxxxxx");
    uint64_t traffic_limit = 8192 * 1024*100;//100MB
    req.SetTrafficLimit(traffic_limit);
    req.SetCheckCRC64(true);
    qcloud_cos::PutObjectResumableSingleSyncResp resp;
    std::chrono::time_point<std::chrono::steady_clock> start_ts, end_ts;
    start_ts = std::chrono::steady_clock::now();
    qcloud_cos::CosResult result = m_client->PutObjectResumableSingleThreadSync(req, &resp);
    EXPECT_TRUE(result.IsSucc());

    // 3. 删除临时文件
    if (-1 == remove(filename.c_str())) {
      std::cout << "Remove temp file=" << filename << " fail." << std::endl;
    }
  }

  {
    std::string filename = "multi_upload_object_enc_one_step";
    std::string object_name = filename;
    // 1. 生成个临时文件, 用于分块上传
    {
      std::ofstream fs;
      fs.open(filename.c_str(), std::ios::out | std::ios::binary);
      std::string str(10 * 1000 * 1000, 'b');
      for (int idx = 0; idx < 10; ++idx) {
        fs << str;
      }
      fs.close();
    }

    // 2. 上传
    MultiPutObjectReq req(m_bucket_name, object_name, filename);
    req.SetXCosServerSideEncryption("AES256");
    MultiPutObjectResp resp;

    CosResult result = m_client->MultiPutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("AES256", resp.GetXCosServerSideEncryption());

    // 3. 删除临时文件
    if (-1 == remove(filename.c_str())) {
      std::cout << "Remove temp file=" << filename << " fail." << std::endl;
    }
  }
}

TEST_F(ObjectOpTest, UploadPartCopyDataTest) {
  //上传一个对象
  {
    std::string local_file = "./object_test_upload_part_copy_data_source";
    TestUtils::WriteRandomDatatoFile(local_file, 10 * 1024 * 1024);
    PutObjectByFileReq req(m_bucket_name, "object_test_upload_part_copy_data_source", local_file);
    req.SetXCosStorageClass(kStorageClassStandard);
    PutObjectByFileResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    TestUtils::RemoveFile(local_file);
  }
  //分块copy
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "object_test_upload_part_copy_data_source");
    qcloud_cos::HeadObjectResp resp;
    uint64_t part_size = 1024 * 1024;
    uint64_t copy_size = 10 * 1024 * 1024;
    uint64_t no_copy_size = copy_size;
  
    std::string object_name_copy = "object_test_upload_part_copy_data_copy";
    InitMultiUploadReq init_req(m_bucket_name, object_name_copy);
    InitMultiUploadResp init_resp;
    CosResult init_result = m_client->InitMultiUpload(init_req, &init_resp);
    ASSERT_TRUE(init_result.IsSucc());
    std::string host = CosSysConfig::GetHost(m_config->GetAppId(), m_config->GetRegion(),
                                            m_bucket_name);
    std::vector<std::string> etags;
    std::vector<uint64_t> part_numbers;
    for (uint64_t part_cnt = 0; no_copy_size > 0; ++part_cnt) {
      UploadPartCopyDataReq req(m_bucket_name, object_name_copy, init_resp.GetUploadId(),
                                          part_cnt + 1);
      req.SetXCosCopySource(host + "/object_test_upload_part_copy_data_source");
      std::string range = "bytes=" + std::to_string(part_cnt * part_size) + "-";
      if(no_copy_size > part_size) {
        range += std::to_string(((part_cnt + 1) * part_size) - 1);
      }else range += std::to_string(copy_size - 1);
      req.SetXCosCopySourceRange(range);
      UploadPartCopyDataResp resp;
      CosResult result = m_client->UploadPartCopyData(req, &resp);
      ASSERT_TRUE(result.IsSucc());
      if(no_copy_size > part_size) {
        no_copy_size -= part_size;
      }else {
        no_copy_size = 0;
      }
      etags.push_back(resp.GetEtag());
      part_numbers.push_back(part_cnt + 1);
    }
    qcloud_cos::CompleteMultiUploadReq com_req(m_bucket_name, object_name_copy, init_resp.GetUploadId());
    qcloud_cos::CompleteMultiUploadResp com_resp;
    com_req.SetEtags(etags);
    com_req.SetPartNumbers(part_numbers);
    qcloud_cos::CosResult com_result = m_client->CompleteMultiUpload(com_req, &com_resp);
    ASSERT_TRUE(com_result.IsSucc());
  }
}

TEST_F(ObjectOpTest, AbortMultiUploadTest) {
  uint64_t part_size = 20 * 1000 * 1000;
  uint64_t max_part_num = 3;
  std::string object_name = "object_test_abort_multi";
  InitMultiUploadReq init_req(m_bucket_name, object_name);
  InitMultiUploadResp init_resp;
  CosResult init_result = m_client->InitMultiUpload(init_req, &init_resp);
  ASSERT_TRUE(init_result.IsSucc());

  std::vector<std::string> etags;
  std::vector<uint64_t> part_numbers;
  std::string filename = "object_test_abort_multi";
  // 1. 生成个临时文件, 用于分块上传
  std::ofstream fs;
  fs.open(filename.c_str(), std::ios::out | std::ios::binary);

  for (uint64_t part_cnt = 0; part_cnt < max_part_num; ++part_cnt) {
    std::string str(part_size * (part_cnt + 1), 'a');  // 分块大小倍增
    fs << str;
    std::stringstream ss;
    ss << str;
    UploadPartDataReq req(m_bucket_name, object_name, init_resp.GetUploadId(),
                          ss);
    UploadPartDataResp resp;
    req.SetPartNumber(part_cnt + 1);
    req.SetRecvTimeoutInms(1000 * 200);

    CosResult result = m_client->UploadPartData(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    etags.push_back(resp.GetEtag());
    part_numbers.push_back(part_cnt + 1);
  }
  std::string str(10 * 1000 * 1000, 'b');
  fs << str;
  

  MultiPutObjectReq req1(m_bucket_name, object_name, filename);
  req1.IsHttps();
  bool resume_flag = false;
  std::vector<std::string> already_exist_parts(kMaxPartNumbers);
  // check the breakpoint
  
  std::shared_ptr<CosConfig> config1 = std::make_shared<CosConfig>();
  config1->SetAccessKey(GetEnvVar("CPP_SDK_V5_ACCESS_KEY"));
  config1->SetSecretKey(GetEnvVar("CPP_SDK_V5_SECRET_KEY"));
  config1->SetRegion(GetEnvVar("CPP_SDK_V5_REGION"));
  ObjectOp m_object_op(config1);
  //req1.SetHttps();
  req1.SetSSLCtxCallback(SslCtxCallback, nullptr);
  std::string resume_uploadid = m_object_op.GetResumableUploadID(req1, m_bucket_name, object_name, false);
  if (!resume_uploadid.empty()) {
    resume_flag = m_object_op.CheckUploadPart(req1, m_bucket_name, object_name,
                                  resume_uploadid, already_exist_parts);
  }
  fs.close();

  AbortMultiUploadReq abort_req(m_bucket_name, object_name,
                                init_resp.GetUploadId());
  AbortMultiUploadResp abort_resp;

// 3. 删除临时文件
  CosResult result = m_client->AbortMultiUpload(abort_req, &abort_resp);
  if (-1 == remove(filename.c_str())) {
        std::cout << "Remove temp file=" << filename << " fail." << std::endl;
      }
  ASSERT_TRUE(result.IsSucc());
}

TEST_F(ObjectOpTest, MultiUploadVaryName) {
  std::vector<std::string> object_name_list = {"test_multiupload_object",
                                               "测试上传中文", "测试上传韩文",
                                               "のテストアップロード"};
  size_t test_file_size = 10 * 1024 * 1024;
  for (auto& object_name : object_name_list) {
    std::cout << "test object_name: " << object_name << std::endl;
    std::string local_file = "./" + object_name;
    std::cout << "generate file: " << local_file << std::endl;
    TestUtils::WriteRandomDatatoFile(local_file, test_file_size);
    uint64_t file_crc64_origin = FileUtil::GetFileCrc64(local_file);
    MultiPutObjectReq multiupload_req(m_bucket_name, object_name, local_file);
    MultiPutObjectResp multiupload_resp;
    ASSERT_TRUE(multiupload_req.CheckCRC64());

    // upload object
    CosResult multiupload_result =
        m_client->MultiPutObject(multiupload_req, &multiupload_resp);
    ASSERT_TRUE(multiupload_result.IsSucc());
    ASSERT_TRUE(!multiupload_resp.GetXCosRequestId().empty());
    ASSERT_TRUE(multiupload_resp.GetContentLength() == 0);
    ASSERT_TRUE(!multiupload_resp.GetConnection().empty());
    ASSERT_TRUE(!multiupload_resp.GetDate().empty());
    ASSERT_EQ(multiupload_resp.GetServer(), "tencent-cos");
    ASSERT_EQ(multiupload_resp.GetXCosHashCrc64Ecma(),
              std::to_string(file_crc64_origin));

    // head object
    std::cout << "head object: " << object_name << std::endl;
    HeadObjectReq head_req(m_bucket_name, object_name);
    HeadObjectResp head_resp;
    CosResult head_result = m_client->HeadObject(head_req, &head_resp);
    ASSERT_TRUE(head_result.IsSucc());
    ASSERT_TRUE(!head_result.GetXCosRequestId().empty());
    ASSERT_EQ(head_resp.GetContentLength(), test_file_size);
    ASSERT_TRUE(!head_resp.GetDate().empty());
    ASSERT_EQ(head_resp.GetServer(), "tencent-cos");
    ASSERT_EQ(head_resp.GetXCosHashCrc64Ecma(),
              std::to_string(file_crc64_origin));

    // delete object
    std::cout << "delete object: " << object_name << std::endl;
    qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
    qcloud_cos::DeleteObjectResp del_resp;
    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
    // checkout common header
    ASSERT_TRUE(del_result.IsSucc());
    ASSERT_TRUE(!del_resp.GetXCosRequestId().empty());
    ASSERT_TRUE(!del_resp.GetConnection().empty());
    ASSERT_TRUE(!del_resp.GetDate().empty());
    ASSERT_EQ(del_resp.GetServer(), "tencent-cos");

    // remove local file
    TestUtils::RemoveFile(local_file);
  }

  {
    // upload not exist file
    std::string object_name = "not_exist_file";
    std::string local_file_not_exist = "./not_exist_file";
    MultiPutObjectReq multiupload_req(m_bucket_name, object_name,
                                      local_file_not_exist);
    MultiPutObjectResp multiupload_resp;
    CosResult result = m_client->MultiPutObject(multiupload_req, &multiupload_resp);
    ASSERT_TRUE(!result.IsSucc());
    ASSERT_TRUE(result.GetErrorMsg().find("Failed to open file") !=
                std::string::npos);
  }
}

TEST_F(ObjectOpTest, MultiUploadVaryPartSizeAndThreadPoolSize) {
  std::vector<unsigned> part_size_list = {1024 * 1024, 1024 * 1024 * 4,
                                          1024 * 1024 * 10, 1024 * 1024 * 20};
  std::vector<unsigned> thread_pool_size_list = {1, 4, 10, 16};
  size_t test_file_size = 100 * 1024 * 1024;
  for (auto& part_size : part_size_list) {
    for (auto& thead_pool_size : thread_pool_size_list) {
      std::cout << "part_size : " << part_size
                << ", thead_pool_size: " << thead_pool_size << std::endl;
      CosSysConfig::SetUploadPartSize(part_size);
      CosSysConfig::SetUploadThreadPoolSize(thead_pool_size);
      std::string object_name = "test_multiupload_object";
      std::string local_file = "./" + object_name;
      std::cout << "generate file: " << local_file << std::endl;
      TestUtils::WriteRandomDatatoFile(local_file, test_file_size);
      uint64_t file_crc64_origin = FileUtil::GetFileCrc64(local_file);
      MultiPutObjectReq multiupload_req(m_bucket_name, object_name, local_file);
      MultiPutObjectResp multiupload_resp;
      ASSERT_TRUE(multiupload_req.CheckCRC64());
      multiupload_req.SetCheckPartCrc64(true);

      // upload object
      CosResult multiupload_result =
          m_client->MultiPutObject(multiupload_req, &multiupload_resp);
      ASSERT_TRUE(multiupload_result.IsSucc());
      ASSERT_TRUE(!multiupload_resp.GetXCosRequestId().empty());
      ASSERT_TRUE(multiupload_resp.GetContentLength() == 0);
      ASSERT_TRUE(!multiupload_resp.GetConnection().empty());
      ASSERT_TRUE(!multiupload_resp.GetDate().empty());
      ASSERT_EQ(multiupload_resp.GetServer(), "tencent-cos");
      ASSERT_EQ(multiupload_resp.GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));

      // head object
      std::cout << "head object: " << object_name << std::endl;
      HeadObjectReq head_req(m_bucket_name, object_name);
      HeadObjectResp head_resp;
      CosResult head_result = m_client->HeadObject(head_req, &head_resp);
      ASSERT_TRUE(head_result.IsSucc());
      ASSERT_TRUE(!head_result.GetXCosRequestId().empty());
      ASSERT_EQ(head_resp.GetContentLength(), test_file_size);
      ASSERT_TRUE(!head_resp.GetDate().empty());
      ASSERT_EQ(head_resp.GetServer(), "tencent-cos");
      ASSERT_EQ(head_resp.GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));

      // download object
      std::cout << "download object: " << object_name << std::endl;
      CosSysConfig::SetDownThreadPoolSize(thead_pool_size);
      CosSysConfig::SetDownSliceSize(part_size);
      std::string file_download = local_file + "_download";
      qcloud_cos::MultiGetObjectReq get_req(m_bucket_name, object_name,
                                         file_download);
      qcloud_cos::MultiGetObjectResp get_resp;
      CosResult get_result = m_client->MultiGetObject(get_req, &get_resp);

      {
        //合并路径
        qcloud_cos::MultiGetObjectReq get_req2(m_bucket_name, "/././///abc/.//def//../../",
                                         file_download);
        qcloud_cos::MultiGetObjectResp get_resp2;
        CosResult get_result2 = m_client->MultiGetObject(get_req2, &get_resp2);
        ASSERT_TRUE(!get_result2.IsSucc());
        ASSERT_EQ("GetObjectKeyIllegal", get_result2.GetErrorCode());
      }
      // checkout common header
      ASSERT_TRUE(get_result.IsSucc());
      ASSERT_TRUE(!get_resp.GetXCosRequestId().empty());
      ASSERT_TRUE(!get_resp.GetConnection().empty());
      ASSERT_TRUE(!get_resp.GetDate().empty());
      ASSERT_EQ(get_resp.GetServer(), "tencent-cos");
      ASSERT_EQ(get_resp.GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));
      ASSERT_EQ(file_crc64_origin, FileUtil::GetFileCrc64(file_download));
      ASSERT_EQ(FileUtil::GetFileMd5(local_file),
                FileUtil::GetFileMd5(file_download));

      // delete object
      std::cout << "delete object: " << object_name << std::endl;
      qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
      qcloud_cos::DeleteObjectResp del_resp;
      CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
      // checkout common header
      ASSERT_TRUE(del_result.IsSucc());
      ASSERT_TRUE(!del_resp.GetXCosRequestId().empty());
      ASSERT_TRUE(!del_resp.GetConnection().empty());
      ASSERT_TRUE(!del_resp.GetDate().empty());
      ASSERT_EQ(del_resp.GetServer(), "tencent-cos");

      // remove local file
      TestUtils::RemoveFile(local_file);
      TestUtils::RemoveFile(file_download);
    }
  }
}

}  // namespace qcloud_cos
