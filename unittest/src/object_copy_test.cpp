// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Description: 复制操作测试
//   包含: CopyTest, CopyTest2, CopyTest3, PutObjectCopy

#include "object_op_test_common.h"

namespace qcloud_cos {

TEST_F(ObjectOpTest, CopyTest) {
  //上传一个对象
  {
    std::string local_file = "./object_test_copy_data_source";
    TestUtils::WriteRandomDatatoFile(local_file, 1024 * 1024);
    PutObjectByFileReq req(m_bucket_name, "object_test_copy_data_source", local_file);
    //req.SetHttps();
    req.SetSSLCtxCallback(SslCtxCallback, nullptr);
    req.SetXCosStorageClass(kStorageClassStandard);
    PutObjectByFileResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    TestUtils::RemoveFile(local_file);
  }
  {
    std::string host = CosSysConfig::GetHost(m_config->GetAppId(), m_config->GetRegion(),
                                            m_bucket_name);
    CopyReq req(m_bucket_name, "object_test_copy_data_copy");
    //req.SetHttps();
    req.SetSSLCtxCallback(SslCtxCallback, nullptr);
    CopyResp resp;
    req.SetXCosCopySource(host + "/object_test_copy_data_source");
    CosResult result = m_client->Copy(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpTest, CopyTest2) {
  //上传一个对象
  std::string bucketname_src = "cppsdkcopysrctest-" +
                     GetEnvVar("CPP_SDK_V5_APPID");
  std::string host;
  {
    CosConfig* m_config2;
    CosAPI* m_client2;
    m_config2 = new CosConfig("./config.json");
    m_config2->SetAccessKey(GetEnvVar("CPP_SDK_V5_ACCESS_KEY"));
    m_config2->SetSecretKey(GetEnvVar("CPP_SDK_V5_SECRET_KEY"));
    m_config2->SetRegion("ap-beijing");
    m_client2 = new CosAPI(*m_config2);
    {
      PutBucketReq req(bucketname_src);
      PutBucketResp resp;
      CosResult result = m_client2->PutBucket(req, &resp);
    }
    std::string local_file = "./object_test_copy_data_source2";
    TestUtils::WriteRandomDatatoFile(local_file, 1024 * 1024);
    PutObjectByFileReq req(bucketname_src, "object_test_copy_data_source2", local_file);
    req.SetXCosStorageClass(kStorageClassStandard);
    PutObjectByFileResp resp;
    CosResult result = m_client2->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    TestUtils::RemoveFile(local_file);
    host = CosSysConfig::GetHost(m_config2->GetAppId(), m_config2->GetRegion(),
                                            bucketname_src);
    delete m_config2;
    delete m_client2;
  }
  {
    CopyReq req(m_bucket_name, "object_test_copy_data_copy2");
    //req.SetHttps();
    req.SetSSLCtxCallback(SslCtxCallback, nullptr);
    CopyResp resp;
    req.SetXCosCopySource(host + "/object_test_copy_data_source2");
    CosResult result = m_client->Copy(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
  {
    DeleteObjectReq del_req(m_bucket_name, "object_test_copy_data_copy2");
    DeleteObjectResp del_resp;
    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
  }
}


TEST_F(ObjectOpTest, CopyTest3) {
  std::string bucketname_src = "cppsdkcopysrctest2-" +
                     GetEnvVar("CPP_SDK_V5_APPID");
  std::string host;
  host = CosSysConfig::GetHost(0, "ap-beijing",
                                            "cppsdkcopysrctest2-"+GetEnvVar("CPP_SDK_V5_APPID"));
  {
    CopyReq req(m_bucket_name, "object_test_copy_data_copy3");
    //req.SetHttps();
    req.SetSSLCtxCallback(SslCtxCallback, nullptr);
    CopyResp resp;
    req.SetXCosCopySource(host + "/object_test_copy_data_copy3");
    CosResult result = m_client->Copy(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
  {
    DeleteObjectReq del_req(m_bucket_name, "object_test_copy_data_copy3");
    DeleteObjectResp del_resp;
    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
  }
}

TEST_F(ObjectOpTest, PutObjectCopyTest) {
  std::istringstream iss("put_obj_by_stream_string");
  std::string object_name = "object_test";
  PutObjectByStreamReq req(m_bucket_name, object_name, iss);
  PutObjectByStreamResp resp;
  CosResult result = m_client->PutObject(req, &resp);
  ASSERT_TRUE(result.IsSucc());

  {
    PutObjectCopyReq req(m_bucket_name2, "object_test_copy_from_bucket1");
    PutObjectCopyResp resp;
    std::string source = m_bucket_name + "." + m_config->GetRegion() +
                         ".mycloud.com/" + object_name;
    req.SetXCosCopySource(source);

    CosResult result = m_client->PutObjectCopy(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }

  {
    PutObjectCopyReq req(m_bucket_name2, "object_enc_test_copy_from_bucket1");
    PutObjectCopyResp resp;
    std::string source = m_bucket_name + "." + m_config->GetRegion() +
                         ".mycloud.com/" + object_name;
    req.SetXCosCopySource(source);
    req.SetXCosServerSideEncryption("AES256");

    CosResult result = m_client->PutObjectCopy(req, &resp);
    EXPECT_TRUE(result.IsSucc());
    EXPECT_EQ("AES256", resp.GetXCosServerSideEncryption());
  }
}

}  // namespace qcloud_cos
