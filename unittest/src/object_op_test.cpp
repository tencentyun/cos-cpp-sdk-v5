// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:

#include <sstream>

#include <thread>
#include "Poco/MD5Engine.h"
#include "Poco/StreamCopier.h"
#include "cos_api.h"
#include "util/test_utils.h"
#include "util/file_util.h"
#include "util/simple_dns_cache.h"
#include "gtest/gtest.h"

/*
export CPP_SDK_V5_ACCESS_KEY=xxx
export CPP_SDK_V5_SECRET_KEY=xxx
export CPP_SDK_V5_REGION=ap-guangzhou
export CPP_SDK_V5_UIN=xxx
export CPP_SDK_V5_APPID=xxx
export COS_CPP_V5_TAG=""
export COS_CPP_V5_USE_DNS_CACHE="true"

export CPP_SDK_V5_OTHER_ACCESS_KEY=xxx
export CPP_SDK_V5_OTHER_SECRET_KEY=xxx
export CPP_SDK_V5_OTHER_REGION=ap-hongkong
export CPP_SDK_V5_OTHER_UIN=xxx
*/

namespace qcloud_cos {

class ObjectOpTest : public testing::Test {
 protected:
  static void SetUpTestCase() {
    std::cout << "================SetUpTestCase Begin===================="
              << std::endl;
    m_config = new CosConfig("./config.json");
    m_config->SetAccessKey(GetEnvVar("CPP_SDK_V5_ACCESS_KEY"));
    m_config->SetSecretKey(GetEnvVar("CPP_SDK_V5_SECRET_KEY"));
    m_config->SetRegion(GetEnvVar("CPP_SDK_V5_REGION"));
    if (GetEnvVar("COS_CPP_V5_USE_DNS_CACHE") == "true") {
      std::cout << "================USE DNS CACHE===================="
                << std::endl;
      CosSysConfig::SetUseDnsCache(true);
    }
    m_client = new CosAPI(*m_config);

    m_bucket_name = "coscppsdkv5ut" + GetEnvVar("COS_CPP_V5_TAG") + "-" +
                    GetEnvVar("CPP_SDK_V5_APPID");
    m_bucket_name2 = "coscppsdkv5utcopy" + GetEnvVar("COS_CPP_V5_TAG") + "-" +
                     GetEnvVar("CPP_SDK_V5_APPID");
    {
      PutBucketReq req(m_bucket_name);
      PutBucketResp resp;
      CosResult result = m_client->PutBucket(req, &resp);
      ASSERT_TRUE(result.IsSucc());
    }

    {
      PutBucketReq req(m_bucket_name2);
      PutBucketResp resp;
      CosResult result = m_client->PutBucket(req, &resp);
      ASSERT_TRUE(result.IsSucc());
    }

    std::cout << "================SetUpTestCase End===================="
              << std::endl;
  }

  static void TearDownTestCase() {
    std::cout << "================TearDownTestCase Begin===================="
              << std::endl;

    // 1. 删除所有Object
    {{GetBucketReq req(m_bucket_name);
    GetBucketResp resp;
    CosResult result = m_client->GetBucket(req, &resp);
    ASSERT_TRUE(result.IsSucc());

    const std::vector<Content>& contents = resp.GetContents();
    for (std::vector<Content>::const_iterator c_itr = contents.begin();
         c_itr != contents.end(); ++c_itr) {
      const Content& content = *c_itr;
      DeleteObjectReq del_req(m_bucket_name, content.m_key);
      DeleteObjectResp del_resp;
      CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
      EXPECT_TRUE(del_result.IsSucc());
      if (!del_result.IsSucc()) {
        std::cout << "DeleteObject Failed, check object=" << content.m_key
                  << std::endl;
      }
    }
  }

  {
    GetBucketReq req(m_bucket_name2);
    GetBucketResp resp;
    CosResult result = m_client->GetBucket(req, &resp);
    ASSERT_TRUE(result.IsSucc());

    const std::vector<Content>& contents = resp.GetContents();
    for (std::vector<Content>::const_iterator c_itr = contents.begin();
         c_itr != contents.end(); ++c_itr) {
      const Content& content = *c_itr;
      DeleteObjectReq del_req(m_bucket_name2, content.m_key);
      DeleteObjectResp del_resp;
      CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
      EXPECT_TRUE(del_result.IsSucc());
      if (!del_result.IsSucc()) {
        std::cout << "DeleteObject Failed, check object=" << content.m_key
                  << std::endl;
      }
    }
  }
}

// 2. 删除所有未complete的分块
{
  for (std::map<std::string, std::string>::const_iterator c_itr =
           m_to_be_aborted.begin();
       c_itr != m_to_be_aborted.end(); ++c_itr) {
    AbortMultiUploadReq req(m_bucket_name, c_itr->first, c_itr->second);
    AbortMultiUploadResp resp;

    CosResult result = m_client->AbortMultiUpload(req, &resp);
    EXPECT_TRUE(result.IsSucc());
    if (!result.IsSucc()) {
      std::cout << "AbortMultiUpload Failed, object=" << c_itr->first
                << ", upload_id=" << c_itr->second << std::endl;
    }
  }
}

{
  // 删除所有碎片
  std::vector<std::string> bucket_v = {m_bucket_name, m_bucket_name2};
  for (auto& bucket : bucket_v) {
    qcloud_cos::ListMultipartUploadReq list_mp_req(bucket);
    qcloud_cos::ListMultipartUploadResp list_mp_resp;
    qcloud_cos::CosResult list_mp_result =
        m_client->ListMultipartUpload(list_mp_req, &list_mp_resp);
    ASSERT_TRUE(list_mp_result.IsSucc());
    std::vector<Upload> rst = list_mp_resp.GetUpload();
    for (std::vector<qcloud_cos::Upload>::const_iterator itr = rst.begin();
         itr != rst.end(); ++itr) {
      AbortMultiUploadReq abort_mp_req(bucket, itr->m_key, itr->m_uploadid);
      AbortMultiUploadResp abort_mp_resp;
      CosResult abort_mp_result =
          m_client->AbortMultiUpload(abort_mp_req, &abort_mp_resp);
      EXPECT_TRUE(abort_mp_result.IsSucc());
      if (!abort_mp_result.IsSucc()) {
        std::cout << "AbortMultiUpload Failed, object=" << itr->m_key
                  << ", upload_id=" << itr->m_uploadid << std::endl;
      }
    }
  }
}

// 3. 删除Bucket
{
  {
    DeleteBucketReq req(m_bucket_name);
    DeleteBucketResp resp;
    CosResult result = m_client->DeleteBucket(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }

  {
    DeleteBucketReq req(m_bucket_name2);
    DeleteBucketResp resp;
    CosResult result = m_client->DeleteBucket(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
}

delete m_client;
delete m_config;
std::cout << "================TearDownTestCase End===================="
          << std::endl;
}  // namespace qcloud_cos

protected:
static CosConfig* m_config;
static CosAPI* m_client;
static std::string m_bucket_name;
static std::string m_bucket_name2;  // 用于copy

// 用于记录单测中未Complete的分块上传uploadID,便于清理
static std::map<std::string, std::string> m_to_be_aborted;
}
;

std::string ObjectOpTest::m_bucket_name = "";
std::string ObjectOpTest::m_bucket_name2 = "";
CosConfig* ObjectOpTest::m_config = NULL;
CosAPI* ObjectOpTest::m_client = NULL;
std::map<std::string, std::string> ObjectOpTest::m_to_be_aborted;

#if 1

TEST_F(ObjectOpTest, PutObjectByFileTest) {
  // 1. ObjectName为普通字符串
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "test_object", local_file);
    req.SetXCosStorageClass(kStorageClassStandard);
    PutObjectByFileResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    TestUtils::RemoveFile(local_file);
  }

  // 2. ObjectName为中文字符串
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "这是个中文Object", local_file);
    req.SetXCosStorageClass(kStorageClassStandard);
    PutObjectByFileResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    TestUtils::RemoveFile(local_file);
  }

  // 3. ObjectName为特殊字符串
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(
        m_bucket_name,
        "/→↓←→↖↗↙↘! \"#$%&'()*+,-./0123456789:;"
        "<=>@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~",
        local_file);
    req.SetXCosStorageClass(kStorageClassStandardIA);
    PutObjectByFileResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    TestUtils::RemoveFile(local_file);
  }

  // 4. 上传下载各种不同大小的文件
  {
    // std::vector<int> base_size_v = {1024};
    std::vector<int> base_size_v = {5,    35,    356,         1024,
                                    2545, 25678, 1024 * 1024, 5 * 1024 * 1024};

    for (auto& size : base_size_v) {
      for (int i = 1; i < 10; i++) {
        std::cout << "base_size: " << size << ", test_time: " << i << std::endl;
        size_t file_size = ((rand() % 100) + 1) * size;
        std::string object_name = "test_object_" + std::to_string(file_size);
        std::string local_file = "./" + object_name;
        std::cout << "generate file: " << local_file << std::endl;
        TestUtils::WriteRandomDatatoFile(local_file, file_size);

        // 上传对象
        std::cout << "start to upload: " << local_file << std::endl;
        PutObjectByFileReq put_req(m_bucket_name, object_name, local_file);
        put_req.SetRecvTimeoutInms(1000 * 200);
        PutObjectByFileResp put_resp;
        CosResult put_result = m_client->PutObject(put_req, &put_resp);
        ASSERT_TRUE(put_result.IsSucc());

        // 校验文件
        std::string file_md5_origin = TestUtils::CalcFileMd5(local_file);
        ASSERT_EQ(put_resp.GetEtag(), file_md5_origin);

        // 下载对象
        std::cout << "start to download: " << object_name << std::endl;
        std::string file_download = local_file + "_download";
        GetObjectByFileReq get_req(m_bucket_name, object_name, file_download);
        GetObjectByFileResp get_resp;
        CosResult get_result = m_client->GetObject(get_req, &get_resp);
        ASSERT_TRUE(get_result.IsSucc());
        std::string file_md5_download = TestUtils::CalcFileMd5(file_download);
        ASSERT_EQ(file_md5_download, file_md5_origin);
        ASSERT_EQ(file_md5_download, get_resp.GetEtag());

        // 删除对象
        std::cout << "start to delete: " << object_name << std::endl;
        CosResult del_result;
        qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
        qcloud_cos::DeleteObjectResp del_resp;
        del_result = m_client->DeleteObject(del_req, &del_resp);
        ASSERT_TRUE(del_result.IsSucc());

        // 删除本地文件
        TestUtils::RemoveFile(local_file);
        TestUtils::RemoveFile(file_download);
      }
    }
  }

  // 5. 服务端加密, 正确的加密算法AES256
  {
    std::istringstream iss("put_obj_by_stream_string");
    PutObjectByStreamReq req(m_bucket_name, "object_server_side_enc_test", iss);
    req.SetXCosStorageClass(kStorageClassStandard);
    req.SetXCosServerSideEncryption("AES256");
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("AES256", resp.GetXCosServerSideEncryption());
  }

  // 6. 服务端加密, 错误的加密算法AES789
  {
    std::istringstream iss("put_obj_by_stream_string");
    PutObjectByStreamReq req(m_bucket_name, "object_server_side_enc_wrong_test",
                             iss);
    req.SetXCosStorageClass(kStorageClassStandard);
    req.SetXCosServerSideEncryption("AES789");
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_FALSE(result.IsSucc());
    EXPECT_EQ(400, result.GetHttpStatus());
    // EXPECT_EQ("SSEContentNotSupported", result.GetErrorCode());
    EXPECT_EQ("InvalidArgument", result.GetErrorCode());
  }

  // 7. 关闭MD5上传校验
  {
    std::istringstream iss("put_obj_by_stream_string");
    PutObjectByStreamReq req(m_bucket_name, "object_file_not_count_contentmd5",
                             iss);
    req.TurnOffComputeConentMd5();
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpTest, PutObjectByStreamTest) {
  // 1. ObjectName为普通字符串
  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    PutObjectByStreamReq req(m_bucket_name, "object_test2", iss);
    req.SetXCosStorageClass(kStorageClassStandard);
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }

  // 2. ObjectName为中文字符串
  {
    std::istringstream iss("put_obj_by_stream_chinese_string");
    PutObjectByStreamReq req(m_bucket_name, "这是个中文Object2", iss);
    req.SetXCosStorageClass(kStorageClassStandardIA);
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }

  // 3. ObjectName为特殊字符串
  {
    std::istringstream iss("put_obj_by_stream_special_string");
    PutObjectByStreamReq req(
        m_bucket_name,
        "/→↓←→↖↗↙↘! \"#$%&'()*+,-./0123456789:;"
        "<=>@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~2",
        iss);
    req.SetXCosStorageClass(kStorageClassStandard);
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }

  // 4. 关闭MD5上传校验
  {
    std::istringstream iss("put_obj_by_stream_not_count_contentmd5");
    PutObjectByStreamReq req(m_bucket_name, "object_test3", iss);
    req.TurnOffComputeConentMd5();
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpTest, IsObjectExistTest) {
  std::istringstream iss("put_obj_by_stream_string");
  std::string object_name = "object_test";
  PutObjectByStreamReq req(m_bucket_name, object_name, iss);
  PutObjectByStreamResp resp;
  CosResult result = m_client->PutObject(req, &resp);
  ASSERT_TRUE(result.IsSucc());
  EXPECT_TRUE(m_client->IsObjectExist(m_bucket_name, object_name));
  EXPECT_FALSE(m_client->IsObjectExist(m_bucket_name, "not_exist_object"));
}

TEST_F(ObjectOpTest, HeadObjectTest) {
  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    PutObjectByStreamReq put_req(m_bucket_name, "test_head_object", iss);
    PutObjectByStreamResp put_resp;
    CosResult result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(result.IsSucc());

    HeadObjectReq head_req(m_bucket_name, "test_head_object");
    HeadObjectResp head_resp;
    result = m_client->HeadObject(head_req, &head_resp);
    ASSERT_TRUE(result.IsSucc());
  }

  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    PutObjectByStreamReq put_req(m_bucket_name, "test_head_object_with_sse",
                                 iss);
    put_req.SetXCosServerSideEncryption("AES256");
    PutObjectByStreamResp put_resp;
    CosResult result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(result.IsSucc());

    HeadObjectReq head_req(m_bucket_name, "test_head_object_with_sse");
    HeadObjectResp head_resp;
    result = m_client->HeadObject(head_req, &head_resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("AES256", head_resp.GetXCosServerSideEncryption());
  }
}
TEST_F(ObjectOpTest, PutDirectoryTest) {
  {
    std::string directory_name = "put_directory_test";
    PutDirectoryReq req(m_bucket_name, directory_name);
    PutDirectoryResp resp;
    CosResult result = m_client->PutDirectory(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpTest, MoveObjectTest) {
  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    PutObjectByStreamReq req(m_bucket_name, "move_object_src", iss);
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());

    MoveObjectReq mv_req(m_bucket_name,  "move_object_src", "move_object_dst");
    CosResult mv_result = m_client->MoveObject(mv_req);
    ASSERT_TRUE(mv_result.IsSucc());
  }
}

TEST_F(ObjectOpTest, DeleteObjectTest) {
  {
    // Delete empty string, test whether call the DeleteBucket interface
    std::string object_name = "";
    DeleteObjectReq req(m_bucket_name, object_name);
    DeleteObjectResp resp;
    CosResult result = m_client->DeleteObject(req, &resp);
    std::string errinfo = result.GetErrorMsg();
    EXPECT_EQ("Delete object's name is empty.", errinfo);
  }
}

#if defined(__linux__)
TEST_F(ObjectOpTest, DeleteObjectsTest) {
  // 批量上传+批量删除
  {
    std::string directory_name = "delete_objects_testfile_directory";
    if(!TestUtils::IsDirectoryExists(directory_name) && TestUtils::MakeDirectory(directory_name)){
      std::string object_name1 = "testfile1";
      std::string object_name2 = "testfile2";
      std::string object_name3 = "testfile3";
      std::string object_name4 = "testfile4";
      std::string local_file1 = "./" + directory_name + "/" + object_name1;
      std::string local_file2 = "./" + directory_name + "/" + object_name2;
      std::string local_file3 = "./" + directory_name + "/" + object_name3;
      std::string local_file4 = "./" + directory_name + "/" + object_name4;
      TestUtils::WriteRandomDatatoFile(local_file1, 1024);
      TestUtils::WriteRandomDatatoFile(local_file2, 1024);
      TestUtils::WriteRandomDatatoFile(local_file3, 1024);
      TestUtils::WriteRandomDatatoFile(local_file4, 1024);
    
      PutObjectsByDirectoryReq req(m_bucket_name, directory_name);
      PutObjectsByDirectoryResp resp;
      CosResult result = m_client->PutObjects(req, &resp);
      ASSERT_TRUE(result.IsSucc());
      TestUtils::RemoveFile(local_file1);
      TestUtils::RemoveFile(local_file2);
      TestUtils::RemoveFile(local_file3);
      TestUtils::RemoveFile(local_file4);
      TestUtils::RemoveDirectory(directory_name);

      DeleteObjectsByPrefixReq del_req(m_bucket_name, directory_name);
      DeleteObjectsByPrefixResp del_resp;
      CosResult del_result = m_client->DeleteObjects(del_req, &del_resp);
      ASSERT_TRUE(del_result.IsSucc());
    }
  }
  //批量删除
  {
    std::string object_name1 = "testfile1";
    std::string object_name2 = "testfile2";
    std::string object_name3 = "testfile3";
    std::string object_name4 = "testfile4";
    std::istringstream iss("put_obj_by_stream_normal_string");
    PutObjectByStreamReq put_req1(m_bucket_name, object_name1, iss);
    PutObjectByStreamResp put_resp1;
    CosResult put_result1 = m_client->PutObject(put_req1, &put_resp1);
    ASSERT_TRUE(put_result1.IsSucc());
    PutObjectByStreamReq put_req2(m_bucket_name, object_name2, iss);
    PutObjectByStreamResp put_resp2;
    CosResult put_result2 = m_client->PutObject(put_req2, &put_resp2);
    ASSERT_TRUE(put_result2.IsSucc());
    PutObjectByStreamReq put_req3(m_bucket_name, object_name3, iss);
    PutObjectByStreamResp put_resp3;
    CosResult put_result3 = m_client->PutObject(put_req3, &put_resp3);
    ASSERT_TRUE(put_result3.IsSucc());
    PutObjectByStreamReq put_req4(m_bucket_name, object_name4, iss);
    PutObjectByStreamResp put_resp4;
    CosResult put_result4 = m_client->PutObject(put_req4, &put_resp4);
    ASSERT_TRUE(put_result4.IsSucc());

    std::vector<std::string> objects;
    std::vector<ObjectVersionPair> to_be_deleted;
    objects.push_back(object_name1);
    objects.push_back(object_name2);
    objects.push_back(object_name3);
    objects.push_back(object_name4);
    for (size_t idx = 0; idx < objects.size(); ++idx) {
        ObjectVersionPair pair;
        pair.m_object_name = objects[idx];
        to_be_deleted.push_back(pair);
    }
    qcloud_cos::DeleteObjectsReq req(m_bucket_name, to_be_deleted);  
    qcloud_cos::DeleteObjectsResp resp;                 
    qcloud_cos::CosResult del_result = m_client->DeleteObjects(req, &resp);
    ASSERT_TRUE(del_result.IsSucc());
  }
}
#endif

TEST_F(ObjectOpTest, GetObjectByStreamTest) {
  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    std::string object_name = "get_object_by_stream_test";
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());

    std::ostringstream os;
    GetObjectByStreamReq get_req(m_bucket_name, object_name, os);
    GetObjectByStreamResp get_resp;
    CosResult get_result = m_client->GetObject(get_req, &get_resp);
    ASSERT_TRUE(get_result.IsSucc());

    DeleteObjectReq del_req(m_bucket_name, object_name);
    DeleteObjectResp del_resp;
    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
    ASSERT_TRUE(del_result.IsSucc());
  }

  // 下载服务端加密的文件
  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    std::string object_name = "get_sse_object_test";
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    put_req.SetXCosServerSideEncryption("AES256");
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());

    std::ostringstream os;
    GetObjectByStreamReq get_req(m_bucket_name, object_name, os);
    GetObjectByStreamResp get_resp;
    CosResult get_result = m_client->GetObject(get_req, &get_resp);
    ASSERT_TRUE(get_result.IsSucc());
    EXPECT_EQ("AES256", get_resp.GetXCosServerSideEncryption());

    DeleteObjectReq del_req(m_bucket_name, object_name);
    DeleteObjectResp del_resp;
    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
    ASSERT_TRUE(del_result.IsSucc());
  }
}

TEST_F(ObjectOpTest, GetObjectUrlTest) {
  std::string destdomain = m_config->GetDestDomain().empty() ? 
                            CosSysConfig::GetDestDomain() : m_config->GetDestDomain();
  {
    std::string object_url = m_client->GetObjectUrl("get_object_url_test_bucket_name", 
                                                    "get_object_url_test_object_name", 
                                                    true, 
                                                    "get_object_url_test_region");
    std::string myUrl = destdomain;
    if(destdomain.empty()){
      myUrl = "get_object_url_test_bucket_name.cos.get_object_url_test_region.myqcloud.com/get_object_url_test_object_name";
    }
    ASSERT_EQ(object_url, "https://"+myUrl);
  }

  {
    std::string object_url = m_client->GetObjectUrl("get_object_url_test_bucket_name", 
                                                    "get_object_url_test_object_name", 
                                                    false, 
                                                    "");
    std::string myUrl = destdomain;
    if(destdomain.empty()){
      myUrl = "get_object_url_test_bucket_name.cos." + GetEnvVar("CPP_SDK_V5_REGION") + ".myqcloud.com/get_object_url_test_object_name";
    }
    ASSERT_EQ(object_url, "http://"+myUrl);
  }
}

TEST_F(ObjectOpTest, PostObjectRestoreTest) {
  //上传
  {
    std::istringstream iss("put object");
    PutObjectByStreamReq req(m_bucket_name, "post_object_restore", iss);
    req.SetXCosStorageClass("ARCHIVE");
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetXCosStorageClass(), "ARCHIVE");
  }
  //post
  {
    PostObjectRestoreReq req(m_bucket_name,  "post_object_restore");
    req.SetExiryDays(30);
    req.SetTier("Standard");
    PostObjectRestoreResp resp;
    CosResult result = m_client->PostObjectRestore(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
  //删除
  {
    DeleteObjectReq req(m_bucket_name, "post_object_restore");
    DeleteObjectResp resp;
    CosResult result = m_client->DeleteObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpTest, PutBucketToCITest) {
  bool use_dns_cache = CosSysConfig::GetUseDnsCache();
  CosSysConfig::SetUseDnsCache(false);
  {
    PutBucketToCIReq req(m_bucket_name);
    PutBucketToCIResp resp;
    CosResult result = m_client->PutBucketToCI(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetUseDnsCache(use_dns_cache);           
}

TEST_F(ObjectOpTest, ImageProcessTest) {
  bool use_dns_cache = CosSysConfig::GetUseDnsCache();
  CosSysConfig::SetUseDnsCache(false);
  std::string object_name = "test.jpg";
  //上传处理
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
  //云上处理
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
  //下载处理
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

//媒体接口
TEST_F(ObjectOpTest, MediaTest) {
  bool use_dns_cache = CosSysConfig::GetUseDnsCache();
  CosSysConfig::SetUseDnsCache(false);
  std::string object_name = "video.mp4";
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
  // std::string voice_seperate_job_id = "";
  std::string segment_job_id = "";

  // 上传媒体
  {
    PutObjectByFileReq put_req(m_bucket_name, audio_object_name, "../../demo/test_file/audio.mp3");
    put_req.SetRecvTimeoutInms(1000 * 200);
    PutObjectByFileResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
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

  //上传媒体
  {
    PutObjectByFileReq put_req(m_bucket_name, object_name, "../../demo/test_file/video.mp4");
    put_req.SetRecvTimeoutInms(1000 * 200);
    PutObjectByFileResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());
  }

  //绑定媒体服务
  {
    CreateMediaBucketReq req(m_bucket_name);
    CreateMediaBucketResp resp;
    CosResult result = m_client->CreateMediaBucket(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(resp.GetResult().media_bucket.name, m_bucket_name);
    ASSERT_EQ(resp.GetResult().media_bucket.region, GetEnvVar("CPP_SDK_V5_REGION"));
    resp.GetResult().to_string();
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
    resp.GetResult().to_string();
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

    opt.operation.output.bucket = m_bucket_name;
    opt.operation.output.region = m_region;
    opt.operation.output.object = "output/transcode.mp4";
    req.setOperation(opt);
    req.setOperation(opt);
    CosResult result = m_client->CreateDataProcessJobs(req, &resp);    
    ASSERT_TRUE(result.IsSucc());
    transcode_job_id = resp.GetJobsDetail().job_id;
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
    ASSERT_TRUE(result.IsSucc());
    digital_watermark_job_id = resp.GetJobsDetail().job_id;
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
    ASSERT_TRUE(result.IsSucc());
    extract_digital_watermark_job_id = resp.GetJobsDetail().job_id;
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
    ASSERT_TRUE(result.IsSucc());
    video_montage_job_id = resp.GetJobsDetail().job_id;
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
    ASSERT_TRUE(result.IsSucc());
    segment_job_id = resp.GetJobsDetail().job_id;
  }

  std::string m3u8_object_name = "pm3u8.m3u8";
  //上传媒体
  {
    PutObjectByFileReq put_req(m_bucket_name, m3u8_object_name, "../../demo/test_file/douyin.m3u8");
    put_req.SetRecvTimeoutInms(1000 * 200);
    PutObjectByFileResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());
  }

  //上传媒体
  {
    PutObjectByFileReq put_req(m_bucket_name, "douyin-00000.ts", "../../demo/test_file/douyin-00000.ts");
    put_req.SetRecvTimeoutInms(1000 * 200);
    PutObjectByFileResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());
  }

  // pm3u8
  {
    GetPm3u8Req req(m_bucket_name, m3u8_object_name, "./local_file_pm3u8.m3u8");
    GetPm3u8Resp resp;
    req.SetExpires(3600);
    CosResult result = m_client->GetPm3u8(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    TestUtils::RemoveFile("./local_file_pm3u8.m3u8");
  }

  // 人声分离
  // {
  //   CreateDataProcessJobsReq req(m_bucket_name);
  //   CreateDataProcessJobsResp resp;

  //   JobsOptions opt;
  //   opt.input.bucket = m_bucket_name;
  //   opt.input.region = m_region;
  //   opt.input.object = audio_object_name;
  //   opt.tag = "VoiceSeparate";
  //   // 使用参数形式提交任务
  //   opt.operation.voice_separate.audio_mode = "IsAudio";
  //   opt.operation.voice_separate.audio_config.codec = "aac";
  //   opt.operation.voice_separate.audio_config.sample_rate = "11025";
  //   opt.operation.voice_separate.audio_config.bit_rate = "16";
  //   opt.operation.voice_separate.audio_config.channels = "2";
  //   opt.operation.output.bucket = m_bucket_name;
  //   opt.operation.output.region = m_region;
  //   opt.operation.output.object = "output/out.mp3";
  //   opt.operation.output.au_object = "output/au.mp3";
  //   req.setOperation(opt);
  //   CosResult result = m_client->CreateDataProcessJobs(req, &resp);
  //   ASSERT_TRUE(result.IsSucc());
  //   voice_seperate_job_id = resp.GetJobsDetail().job_id;
  // }

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
    ASSERT_EQ(transcode_resp.GetJobsDetail().state, "Success");    

    DescribeDataProcessJobReq animation_req(m_bucket_name);
    DescribeDataProcessJobResp animation_resp;

    animation_req.SetJobId(animation_job_id);
    result = m_client->DescribeDataProcessJob(animation_req, &animation_resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(animation_resp.GetJobsDetail().state, "Success");

    DescribeDataProcessJobReq concat_req(m_bucket_name);
    DescribeDataProcessJobResp concat_resp;

    concat_req.SetJobId(concat_job_id);
    result = m_client->DescribeDataProcessJob(concat_req, &concat_resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(concat_resp.GetJobsDetail().state, "Success");

    DescribeDataProcessJobReq smart_cover_req(m_bucket_name);
    DescribeDataProcessJobResp smart_cover_resp;

    smart_cover_req.SetJobId(smart_cover_job_id);
    result = m_client->DescribeDataProcessJob(smart_cover_req, &smart_cover_resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(smart_cover_resp.GetJobsDetail().state, "Success");

    DescribeDataProcessJobReq digital_watermark_req(m_bucket_name);
    DescribeDataProcessJobResp digital_watermark_resp;

    digital_watermark_req.SetJobId(digital_watermark_job_id);
    result = m_client->DescribeDataProcessJob(digital_watermark_req, &digital_watermark_resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(digital_watermark_resp.GetJobsDetail().state, "Success");

    DescribeDataProcessJobReq extract_dw_req(m_bucket_name);
    DescribeDataProcessJobResp extract_dw_resp;

    extract_dw_req.SetJobId(extract_digital_watermark_job_id);
    result = m_client->DescribeDataProcessJob(extract_dw_req, &extract_dw_resp);
    ASSERT_TRUE(result.IsSucc());

    DescribeDataProcessJobReq video_montage_req(m_bucket_name);
    DescribeDataProcessJobResp video_montage_resp;

    video_montage_req.SetJobId(video_montage_job_id);
    result = m_client->DescribeDataProcessJob(video_montage_req, &video_montage_resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(video_montage_resp.GetJobsDetail().state, "Success");    

    // req.SetJobId(voice_seperate_job_id);
    // result = m_client->DescribeDataProcessJob(req, &resp);
    // ASSERT_TRUE(result.IsSucc());
    // ASSERT_EQ(resp.GetJobsDetail().state, "Success");  

    DescribeDataProcessJobReq segment_req(m_bucket_name);
    DescribeDataProcessJobResp segment_resp;

    segment_req.SetJobId(segment_job_id);
    result = m_client->DescribeDataProcessJob(segment_req, &segment_resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_EQ(segment_resp.GetJobsDetail().state, "Success");  
  }

  CosSysConfig::SetUseDnsCache(use_dns_cache);
}

//文档接口
TEST_F(ObjectOpTest, DocTest) {
  bool use_dns_cache = CosSysConfig::GetUseDnsCache();
  CosSysConfig::SetUseDnsCache(false);
  std::string object_name = "document.docx";
  std::string output_object = "/test-ci/test-create-doc-process-${Number}";
  std::string queue_id = "";
  std::string doc_job_id;

  //上传媒体
  {
    PutObjectByFileReq put_req(m_bucket_name, object_name, "../../demo/test_file/document.docx");
    put_req.SetRecvTimeoutInms(1000 * 200);
    PutObjectByFileResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());
  }
  //绑定文档预览服务
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



//审核接口
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

TEST_F(ObjectOpTest, GetObjectByFileTest) {
  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    std::string object_name = "get_object_by_file_test";
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());

    std::string file_download = "file_download";
    GetObjectByFileReq get_req(m_bucket_name, object_name, file_download);
    GetObjectByFileResp get_resp;
    CosResult get_result = m_client->GetObject(get_req, &get_resp);
    ASSERT_TRUE(get_result.IsSucc());

    std::string file_md5_download = TestUtils::CalcFileMd5(file_download);
    ASSERT_EQ(file_md5_download, get_resp.GetEtag());

    DeleteObjectReq del_req(m_bucket_name, object_name);
    DeleteObjectResp del_resp;
    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
    ASSERT_TRUE(del_result.IsSucc());

    TestUtils::RemoveFile(file_download);
  }

  // 下载服务端加密的文件
  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    std::string object_name = "get_sse_object_test";
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    put_req.SetXCosServerSideEncryption("AES256");
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());

    std::string file_download = "sse_file_download";
    GetObjectByFileReq get_req(m_bucket_name, object_name, file_download);
    GetObjectByFileResp get_resp;
    CosResult get_result = m_client->GetObject(get_req, &get_resp);
    ASSERT_TRUE(get_result.IsSucc());
    EXPECT_EQ("AES256", get_resp.GetXCosServerSideEncryption());

    std::string file_md5_download = TestUtils::CalcFileMd5(file_download);
    ASSERT_EQ(file_md5_download, get_resp.GetEtag());

    DeleteObjectReq del_req(m_bucket_name, object_name);
    DeleteObjectResp del_resp;
    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
    ASSERT_TRUE(del_result.IsSucc());

    TestUtils::RemoveFile(file_download);
  }
}

TEST_F(ObjectOpTest, ResumableGetObjectTest) {
  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    std::string object_name = "resumable_get_object_test";
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());

    std::string file_download = "resumable_get_object_test_file_download";
    GetObjectByFileReq get_req(m_bucket_name, object_name, file_download);
    GetObjectByFileResp get_resp;
    CosResult get_result = m_client->ResumableGetObject(get_req, &get_resp);
    
    ASSERT_TRUE(get_result.IsSucc());

    std::string file_md5_download = TestUtils::CalcFileMd5(file_download);
    ASSERT_EQ(file_md5_download, get_resp.GetEtag());

    DeleteObjectReq del_req(m_bucket_name, object_name);
    DeleteObjectResp del_resp;
    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
    ASSERT_TRUE(del_result.IsSucc());

    TestUtils::RemoveFile(file_download);
  }
}


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
    TestUtils::WriteRandomDatatoFile(local_file, 100 * 1024 * 1024);
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
    uint64_t part_size = 30 * 1024 * 1024;
    uint64_t copy_size = 100 * 1024 * 1024;
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
TEST_F(ObjectOpTest, CopyTest) {
  //上传一个对象
  {
    std::string local_file = "./object_test_copy_data_source";
    TestUtils::WriteRandomDatatoFile(local_file, 100 * 1024 * 1024);
    PutObjectByFileReq req(m_bucket_name, "object_test_copy_data_source", local_file);
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
    CopyResp resp;
    req.SetXCosCopySource(host + "/object_test_copy_data_source");
    CosResult result = m_client->Copy(req, &resp);
    ASSERT_TRUE(result.IsSucc());
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

  AbortMultiUploadReq abort_req(m_bucket_name, object_name,
                                init_resp.GetUploadId());
  AbortMultiUploadResp abort_resp;

  CosResult result = m_client->AbortMultiUpload(abort_req, &abort_resp);
  ASSERT_TRUE(result.IsSucc());
}

TEST_F(ObjectOpTest, ObjectACLTest) {
  // 1. Put
  {
    std::istringstream iss("put_obj_by_stream_string");
    std::string object_name = "object_test";
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());

    PutObjectACLReq put_acl_req(m_bucket_name, object_name);
    PutObjectACLResp put_acl_resp;
    std::string uin(GetEnvVar("CPP_SDK_V5_UIN"));
    std::string grant_uin(GetEnvVar("CPP_SDK_V5_OTHER_UIN"));

    qcloud_cos::Owner owner = {"qcs::cam::uin/" + uin + ":uin/" + uin,
                               "qcs::cam::uin/" + uin + ":uin/" + uin};
    Grant grant;
    put_acl_req.SetOwner(owner);
    grant.m_perm = "READ";
    grant.m_grantee.m_type = "RootAccount";
    grant.m_grantee.m_uri = "http://cam.qcloud.com/groups/global/AllUsers";
    grant.m_grantee.m_id = "qcs::cam::uin/" + grant_uin + ":uin/" + grant_uin;
    grant.m_grantee.m_display_name =
        "qcs::cam::uin/" + grant_uin + ":uin/" + grant_uin;
    put_acl_req.AddAccessControlList(grant);

    CosResult put_acl_result =
        m_client->PutObjectACL(put_acl_req, &put_acl_resp);
    EXPECT_TRUE(put_acl_result.IsSucc());
  }

  // 2. Get
  {
    // sleep(5);
    GetObjectACLReq req(m_bucket_name, "object_test");
    GetObjectACLResp resp;
    CosResult result = m_client->GetObjectACL(req, &resp);
    EXPECT_TRUE(result.IsSucc());
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

TEST_F(ObjectOpTest, GeneratePresignedUrlTest) {
  bool use_dns_cache = CosSysConfig::GetUseDnsCache();
  CosSysConfig::SetUseDnsCache(false);
  {
    GeneratePresignedUrlReq req(m_bucket_name, "object_test", HTTP_GET);
    req.SetStartTimeInSec(0);
    req.SetExpiredTimeInSec(5 * 60);

    std::string presigned_url = m_client->GeneratePresignedUrl(req);
    EXPECT_FALSE(presigned_url.empty());
    EXPECT_TRUE(StringUtil::StringStartsWith(presigned_url, "https"));

    // TODO(sevenyou) 先直接调 curl 命令看下是否正常
    std::string curl_url = "curl " + presigned_url;
    int ret = system(curl_url.c_str());
    EXPECT_EQ(0, ret);
  }

  {
    std::string presigned_url =
        m_client->GeneratePresignedUrl(m_bucket_name, "object_test", 0, 0);
    // TODO(sevenyou) 先直接调 curl 命令看下是否正常
    std::string curl_url = "curl " + presigned_url;
    int ret = system(curl_url.c_str());
    EXPECT_EQ(0, ret);
  }

  {
    GeneratePresignedUrlReq req(m_bucket_name, "object_test", HTTP_GET);
    req.SetUseHttps(false);
    std::string presigned_url = m_client->GeneratePresignedUrl(req);
    EXPECT_TRUE(StringUtil::StringStartsWith(presigned_url, "http"));
    EXPECT_TRUE(presigned_url.find("host") != std::string::npos);
  }

  {
    GeneratePresignedUrlReq req(m_bucket_name, "object_test", HTTP_GET);
    req.SetSignHeaderHost(false);
    std::string presigned_url = m_client->GeneratePresignedUrl(req);
    EXPECT_TRUE(StringUtil::StringStartsWith(presigned_url, "https"));
    EXPECT_TRUE(presigned_url.find("host") == std::string::npos);
  }
  CosSysConfig::SetUseDnsCache(use_dns_cache);
}

TEST_F(ObjectOpTest, PutObjectWithMultiMeta) {
  // put object
  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    PutObjectByStreamReq req(m_bucket_name, "object_test_with_multiheader",
                             iss);
    req.SetContentDisposition("attachment; filename=example");
    req.SetContentType("image/jpeg");
    req.SetContentEncoding("compress");
    req.SetXCosMeta("key1", "val1");
    req.SetXCosMeta("key2", "val2");
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
  // head object
  {
    HeadObjectReq req(m_bucket_name, "object_test_with_multiheader");
    HeadObjectResp resp;
    CosResult result = m_client->HeadObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("image/jpeg", resp.GetContentType());
    EXPECT_EQ("attachment; filename=example", resp.GetContentDisposition());
    EXPECT_EQ("compress", resp.GetContentEncoding());
    EXPECT_EQ(resp.GetXCosMeta("key1"), "val1");
    EXPECT_EQ(resp.GetXCosMeta("key2"), "val2");
  }
}

TEST_F(ObjectOpTest, ObjectOptionsDefault) {
  // put object
  {
    std::istringstream iss("test string");
    PutObjectByStreamReq req(m_bucket_name, "object_test_origin", iss);
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
  // test default option
  {
    OptionsObjectReq req(m_bucket_name, "object_test_origin");
    req.SetOrigin("https://console.cloud.tencent.com");
    req.SetAccessControlRequestMethod("GET");
    req.SetAccessControlRequestHeaders("Content-Length");
    OptionsObjectResp resp;
    CosResult result = m_client->OptionsObject(req, &resp);
    EXPECT_EQ(resp.GetAccessControAllowOrigin(),
              "https://console.cloud.tencent.com");
    EXPECT_EQ(resp.GetAccessControlAllowMethods(), "GET,PUT,POST,HEAD,DELETE");
    EXPECT_EQ(resp.GetAccessControlAllowHeaders(), "Content-Length");
    ASSERT_TRUE(result.IsSucc());
  }

  // put bucket cors and option object
  {
    PutBucketCORSReq req(m_bucket_name);
    PutBucketCORSResp resp;
    CORSRule rule;
    rule.m_id = "cors_rule_00";
    rule.m_max_age_secs = "600";
    rule.m_allowed_headers.push_back("x-cos-header-test1");
    rule.m_allowed_headers.push_back("x-cos-header-test2");
    rule.m_allowed_origins.push_back("http://www.123.com");
    rule.m_allowed_origins.push_back("http://www.abc.com");
    rule.m_allowed_methods.push_back("PUT");
    rule.m_allowed_methods.push_back("GET");
    rule.m_expose_headers.push_back("x-cos-expose-headers");
    req.AddRule(rule);
    CosResult result = m_client->PutBucketCORS(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }

  // options object allow
  {
    OptionsObjectReq req(m_bucket_name, "object_test_origin");
    req.SetOrigin("http://www.123.com");
    req.SetAccessControlRequestMethod("GET");
    req.SetAccessControlRequestHeaders("x-cos-header-test1");
    OptionsObjectResp resp;
    CosResult result = m_client->OptionsObject(req, &resp);
    EXPECT_EQ(resp.GetAccessControAllowOrigin(), "http://www.123.com");
    EXPECT_EQ(resp.GetAccessControlAllowMethods(), "PUT,GET");
    EXPECT_EQ(resp.GetAccessControlAllowHeaders(),
              "x-cos-header-test1,x-cos-header-test2");
    EXPECT_EQ(resp.GetAccessControlExposeHeaders(), "x-cos-expose-headers");
    EXPECT_EQ(resp.GetAccessControlMaxAge(), "600");
    ASSERT_TRUE(result.IsSucc());
  }
  // options object allow
  {
    OptionsObjectReq req(m_bucket_name, "object_test_origin");
    req.SetOrigin("http://www.abc.com");
    req.SetAccessControlRequestMethod("PUT");
    req.SetAccessControlRequestHeaders("x-cos-header-test2");
    OptionsObjectResp resp;
    CosResult result = m_client->OptionsObject(req, &resp);
    EXPECT_EQ(resp.GetAccessControAllowOrigin(), "http://www.abc.com");
    EXPECT_EQ(resp.GetAccessControlAllowMethods(), "PUT,GET");
    EXPECT_EQ(resp.GetAccessControlAllowHeaders(),
              "x-cos-header-test1,x-cos-header-test2");
    EXPECT_EQ(resp.GetAccessControlExposeHeaders(), "x-cos-expose-headers");
    EXPECT_EQ(resp.GetAccessControlMaxAge(), "600");
    ASSERT_TRUE(result.IsSucc());
  }

  // options object not allow
  {
    OptionsObjectReq req(m_bucket_name, "object_test_origin");
    req.SetOrigin("http://www.1234.com");
    req.SetAccessControlRequestMethod("GET");
    req.SetAccessControlRequestHeaders("x-cos-header-test");
    OptionsObjectResp resp;
    CosResult result = m_client->OptionsObject(req, &resp);
    EXPECT_EQ(resp.GetAccessControAllowOrigin(), "");
    EXPECT_EQ(resp.GetAccessControlAllowMethods(), "");
    EXPECT_EQ(resp.GetAccessControlAllowHeaders(), "");
    EXPECT_EQ(resp.GetAccessControlExposeHeaders(), "");
    EXPECT_EQ(resp.GetAccessControlMaxAge(), "");
    ASSERT_TRUE(!result.IsSucc());
    EXPECT_EQ(result.GetHttpStatus(), 403);
  }
}

TEST_F(ObjectOpTest, SelectObjectContent) {
  std::string input_str;
  // put json object
  {
    std::istringstream iss("{\"aaa\":111,\"bbb\":222,\"ccc\":\"333\"}");
    input_str = iss.str();
    PutObjectByStreamReq req(m_bucket_name, "testjson", iss);
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
  // select object content, input json, output json
  {
    SelectObjectContentReq req(m_bucket_name, "testjson", JSON, COMPRESS_NONE,
                               JSON);
    SelectObjectContentResp resp;
    req.SetSqlExpression("Select * from COSObject");
    CosResult result = m_client->SelectObjectContent(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    resp.PrintResult();
    EXPECT_EQ(0, resp.WriteResultToLocalFile("select_result.json"));
    std::ifstream ifs("select_result.json");
    std::stringstream strstream;
    // read the file
    strstream << ifs.rdbuf();
    // compare
    EXPECT_EQ(0, input_str.compare(StringUtil::Trim(strstream.str(), "\\n")));
    EXPECT_EQ(0, ::remove("select_result.json"));
  }
  // select object content using filter, input json, output json,
  {
    SelectObjectContentReq req(m_bucket_name, "testjson", JSON, COMPRESS_NONE,
                               JSON);
    SelectObjectContentResp resp;
    req.SetSqlExpression("Select testjson.aaa from COSObject testjson");
    CosResult result = m_client->SelectObjectContent(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    resp.PrintResult();
    EXPECT_EQ(0, resp.WriteResultToLocalFile("select_result.json"));
    std::ifstream ifs("select_result.json");
    std::stringstream strstream;
    strstream << ifs.rdbuf();
    // compare
    EXPECT_EQ(
        0, StringUtil::Trim(strstream.str(), "\\n").compare("{\"aaa\":111}"));
    EXPECT_EQ(0, ::remove("select_result.json"));
  }

  // select object content using filter, input json, output json,
  {
    SelectObjectContentReq req(m_bucket_name, "testjson", JSON, COMPRESS_NONE,
                               CSV);
    SelectObjectContentResp resp;
    req.SetSqlExpression("Select * from COSObject testjson");
    CosResult result = m_client->SelectObjectContent(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    resp.PrintResult();
    EXPECT_EQ(0, resp.WriteResultToLocalFile("select_result.csv"));
    // std::ifstream ifs("select_result.csv");
    // std::stringstream strstream;
    // strstream << ifs.rdbuf();
    // compare
    // EXPECT_EQ(0, StringUtil::Trim(strstream.str(),
    // "\\n").compare("{\"aaa\":111}"));
    EXPECT_EQ(0, ::remove("select_result.csv"));
  }
  // put csv object
  {
    std::istringstream iss("aaa,111,bbb,222,ccc,333");
    input_str = iss.str();
    PutObjectByStreamReq req(m_bucket_name, "testcsv", iss);
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
  // select object content, input csv, output csv
  {
    SelectObjectContentReq req(m_bucket_name, "testcsv", CSV, COMPRESS_NONE,
                               CSV);
    SelectObjectContentResp resp;
    req.SetSqlExpression("Select * from COSObject");
    CosResult result = m_client->SelectObjectContent(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    resp.PrintResult();
    EXPECT_EQ(0, resp.WriteResultToLocalFile("select_result.csv"));
    std::ifstream ifs("select_result.csv");
    std::stringstream strstream;
    strstream << ifs.rdbuf();
    // compare
    EXPECT_EQ(0, input_str.compare(StringUtil::Trim(strstream.str(), "\\\\n")));
    EXPECT_EQ(0, ::remove("select_result.csv"));
  }
}

TEST_F(ObjectOpTest, TestPutObjectWithMeta) {
  std::vector<int> base_size_v = {1024};
  for (auto& size : base_size_v) {
    for (int i = 0; i < 5; i++) {
      std::cout << "base_size: " << size << ", test_time: " << i << std::endl;
      size_t file_size = ((rand() % 100) + 1) * size;
      std::string object_name =
          "test_putobjectwithmeta_" + std::to_string(file_size);
      std::string local_file = "./" + object_name;

      std::cout << "generate file: " << local_file << std::endl;
      TestUtils::WriteRandomDatatoFile(local_file, file_size);

      // put object
      qcloud_cos::PutObjectByFileReq put_req(m_bucket_name, object_name,
                                             local_file);
      put_req.SetXCosStorageClass(kStorageClassStandardIA);
      put_req.SetCacheControl("max-age=86400");
      put_req.SetXCosMeta("key1", "val1");
      put_req.SetXCosMeta("key2", "val2");
      put_req.SetXCosAcl(kAclPublicRead);
      put_req.SetExpires("1000");
      put_req.SetContentEncoding("gzip");
      put_req.SetContentDisposition("attachment; filename=example");
      put_req.SetContentType("image/jpeg");
      qcloud_cos::PutObjectByFileResp put_resp;
      std::cout << "upload object: " << object_name << ", size: " << file_size
                << std::endl;
      CosResult put_result = m_client->PutObject(put_req, &put_resp);
      ASSERT_TRUE(put_result.IsSucc());
      ASSERT_TRUE(!put_resp.GetXCosRequestId().empty());
      ASSERT_TRUE(put_resp.GetContentLength() == 0);
      ASSERT_TRUE(!put_resp.GetConnection().empty());
      ASSERT_TRUE(!put_resp.GetDate().empty());
      ASSERT_EQ(put_resp.GetServer(), "tencent-cos");

      // check crc64 and md5
      uint64_t file_crc64_origin = FileUtil::GetFileCrc64(local_file);
      ASSERT_EQ(put_resp.GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));
      std::string file_md5_origin = FileUtil::GetFileMd5(local_file);
      ASSERT_EQ(put_resp.GetEtag(), file_md5_origin);

      // head object
      std::cout << "head object: " << object_name << std::endl;
      HeadObjectReq head_req(m_bucket_name, object_name);
      HeadObjectResp head_resp;
      CosResult head_result = m_client->HeadObject(head_req, &head_resp);

      // check common headers
      ASSERT_TRUE(head_result.IsSucc());
      ASSERT_TRUE(!head_resp.GetXCosRequestId().empty());
      ASSERT_TRUE(!head_resp.GetConnection().empty());
      ASSERT_TRUE(!head_resp.GetDate().empty());
      ASSERT_EQ(head_resp.GetServer(), "tencent-cos");
      // checkout crcr64 and md5
      ASSERT_EQ(head_resp.GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));
      ASSERT_EQ(head_resp.GetEtag(), file_md5_origin);

      // check meta
      ASSERT_EQ(head_resp.GetXCosMeta("key1"), "val1");
      ASSERT_EQ(head_resp.GetXCosMeta("key2"), "val2");
      ASSERT_EQ(head_resp.GetXCosStorageClass(), kStorageClassStandardIA);
      ASSERT_EQ(head_resp.GetExpires(), "1000");
      ASSERT_EQ(head_resp.GetContentLength(), file_size);
      ASSERT_EQ(head_resp.GetContentEncoding(), "gzip");
      ASSERT_EQ(head_resp.GetContentDisposition(),
                "attachment; filename=example");
      ASSERT_EQ(head_resp.GetContentType(), "image/jpeg");

      // TODO check acl
      // TODO check if-modified-since

      // get object
      std::string local_file_download = local_file + "_download";
      std::cout << "get object: " << object_name << std::endl;
      qcloud_cos::GetObjectByFileReq get_req(m_bucket_name, object_name,
                                             local_file_download);
      qcloud_cos::GetObjectByFileResp get_resp;
      CosResult get_result = m_client->GetObject(get_req, &get_resp);
      // checkout common header
      ASSERT_TRUE(get_result.IsSucc());
      ASSERT_TRUE(!get_resp.GetXCosRequestId().empty());
      ASSERT_TRUE(!get_resp.GetConnection().empty());
      ASSERT_TRUE(!get_resp.GetDate().empty());
      ASSERT_EQ(get_resp.GetServer(), "tencent-cos");

      // checkout crcr64 and md5
      ASSERT_EQ(get_resp.GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));
      ASSERT_EQ(get_resp.GetEtag(), file_md5_origin);
      std::string file_md5_download =
          TestUtils::CalcFileMd5(local_file_download);
      ASSERT_EQ(file_md5_origin, file_md5_download);
      uint64_t file_crc64_download =
          FileUtil::GetFileCrc64(local_file_download);
      ASSERT_EQ(file_crc64_origin, file_crc64_download);

      // check meta
      ASSERT_EQ(get_resp.GetXCosMeta("key1"), "val1");
      ASSERT_EQ(get_resp.GetXCosMeta("key2"), "val2");
      ASSERT_EQ(get_resp.GetXCosStorageClass(), kStorageClassStandardIA);
      ASSERT_EQ(get_resp.GetExpires(), "1000");
      ASSERT_EQ(get_resp.GetContentLength(), file_size);
      ASSERT_EQ(get_resp.GetContentEncoding(), "gzip");
      ASSERT_EQ(get_resp.GetContentDisposition(),
                "attachment; filename=example");
      ASSERT_EQ(get_resp.GetContentType(), "image/jpeg");

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

      // remote local file
      TestUtils::RemoveFile(local_file);
      TestUtils::RemoveFile(local_file_download);
    }
  }
}

TEST_F(ObjectOpTest, TestMultiPutObjectWithMeta) {
  std::vector<int> base_size_v = {1024 * 1024};
  for (auto& size : base_size_v) {
    for (int i = 0; i < 5; i++) {
      std::cout << "base_size: " << size << ", test_time: " << i << std::endl;
      size_t file_size = ((rand() % 100) + 1) * size;
      std::string object_name =
          "test_putobjectwithmeta_" + std::to_string(file_size);
      std::string local_file = "./" + object_name;

      std::cout << "generate file: " << local_file << std::endl;
      TestUtils::WriteRandomDatatoFile(local_file, file_size);

      // put object
      qcloud_cos::MultiPutObjectReq put_req(m_bucket_name, object_name,
                                            local_file);
      put_req.SetXCosStorageClass(kStorageClassStandardIA);
      put_req.SetCacheControl("max-age=86400");
      put_req.SetXCosMeta("key1", "val1");
      put_req.SetXCosMeta("key2", "val2");
      put_req.SetXCosAcl(kAclPublicRead);
      put_req.SetExpires("1000");
      put_req.SetContentEncoding("gzip");
      put_req.SetContentDisposition("attachment; filename=example");
      put_req.SetContentType("image/jpeg");
      qcloud_cos::MultiPutObjectResp put_resp;
      std::cout << "upload object: " << object_name << ", size: " << file_size
                << std::endl;
      CosResult put_result = m_client->MultiPutObject(put_req, &put_resp);
      ASSERT_TRUE(put_result.IsSucc());
      ASSERT_TRUE(!put_resp.GetXCosRequestId().empty());
      ASSERT_EQ(put_resp.GetContentLength(), 0);
      ASSERT_TRUE(!put_resp.GetConnection().empty());
      ASSERT_TRUE(!put_resp.GetDate().empty());
      ASSERT_EQ(put_resp.GetServer(), "tencent-cos");

      // check crc64 and md5
      uint64_t file_crc64_origin = FileUtil::GetFileCrc64(local_file);
      ASSERT_EQ(put_resp.GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));
      std::string file_md5_origin = FileUtil::GetFileMd5(local_file);
      // multipart upload etag not equal to md5
      ASSERT_NE(put_resp.GetEtag(), file_md5_origin);

      // head object
      std::cout << "head object: " << object_name << std::endl;
      HeadObjectReq head_req(m_bucket_name, object_name);
      HeadObjectResp head_resp;
      CosResult head_result = m_client->HeadObject(head_req, &head_resp);

      // check common headers
      ASSERT_TRUE(head_result.IsSucc());
      ASSERT_TRUE(!head_resp.GetXCosRequestId().empty());
      ASSERT_TRUE(!head_resp.GetConnection().empty());
      ASSERT_TRUE(!head_resp.GetDate().empty());
      ASSERT_EQ(head_resp.GetServer(), "tencent-cos");
      // checkout crcr64 and md5
      ASSERT_EQ(head_resp.GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));
      // multipart upload etag not equal to md5
      ASSERT_NE(head_resp.GetEtag(), file_md5_origin);

      // check meta
      ASSERT_EQ(head_resp.GetXCosMeta("key1"), "val1");
      ASSERT_EQ(head_resp.GetXCosMeta("key2"), "val2");
      ASSERT_EQ(head_resp.GetXCosStorageClass(), kStorageClassStandardIA);
      ASSERT_EQ(head_resp.GetExpires(), "1000");
      ASSERT_EQ(head_resp.GetContentLength(), file_size);
      ASSERT_EQ(head_resp.GetContentEncoding(), "gzip");
      ASSERT_EQ(head_resp.GetContentDisposition(),
                "attachment; filename=example");
      ASSERT_EQ(head_resp.GetContentType(), "image/jpeg");

      // TODO check acl
      // TODO check if-modified-since

      // get object
      std::string local_file_download = local_file + "_download";
      std::cout << "get object: " << object_name << std::endl;
      qcloud_cos::MultiGetObjectReq get_req(m_bucket_name, object_name,
                                            local_file_download);
      qcloud_cos::MultiGetObjectResp get_resp;
      CosResult get_result = m_client->MultiGetObject(get_req, &get_resp);
      // checkout common header
      ASSERT_TRUE(get_result.IsSucc());
      ASSERT_TRUE(!get_resp.GetXCosRequestId().empty());
      ASSERT_TRUE(!get_resp.GetConnection().empty());
      ASSERT_TRUE(!get_resp.GetDate().empty());
      ASSERT_EQ(get_resp.GetServer(), "tencent-cos");

      // checkout crcr64 and md5
      ASSERT_EQ(get_resp.GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));
      ASSERT_NE(get_resp.GetEtag(), file_md5_origin);
      std::string file_md5_download =
          TestUtils::CalcFileMd5(local_file_download);
      ASSERT_EQ(file_md5_origin, file_md5_download);
      uint64_t file_crc64_download =
          FileUtil::GetFileCrc64(local_file_download);
      ASSERT_EQ(file_crc64_origin, file_crc64_download);

      // check meta
      ASSERT_EQ(get_resp.GetXCosMeta("key1"), "val1");
      ASSERT_EQ(get_resp.GetXCosMeta("key2"), "val2");
      ASSERT_EQ(get_resp.GetXCosStorageClass(), kStorageClassStandardIA);
      ASSERT_EQ(get_resp.GetExpires(), "1000");
      //ASSERT_EQ(get_resp.GetContentLength(), file_size);
      ASSERT_EQ(get_resp.GetContentEncoding(), "gzip");
      ASSERT_EQ(get_resp.GetContentDisposition(),
                "attachment; filename=example");
      ASSERT_EQ(get_resp.GetContentType(), "image/jpeg");

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
      TestUtils::RemoveFile(local_file_download);
    }
  }
}

TEST_F(ObjectOpTest, AppendObjectTest) {
  const int append_times = 100;
  int append_position = 0;
  int total_object_len = 0;
  std::string object_name = "test_append_object";
  for (int i = 0; i < append_times; i++) {
    int random_str_len = (rand() % 1024) + 1;
    std::cout << "append size: " << random_str_len << std::endl;
    total_object_len += random_str_len;
    std::string test_str = TestUtils::GetRandomString(random_str_len);
    std::istringstream iss(test_str);
    AppendObjectReq append_req(m_bucket_name, object_name, iss);
    append_req.SetPosition(std::to_string(append_position));
    AppendObjectResp append_resp;
    CosResult append_result = m_client->AppendObject(append_req, &append_resp);
    ASSERT_TRUE(append_result.IsSucc());
    ASSERT_TRUE(!append_resp.GetXCosRequestId().empty());
    ASSERT_EQ(append_resp.GetContentLength(), 0);
    ASSERT_TRUE(!append_resp.GetConnection().empty());
    ASSERT_TRUE(!append_resp.GetDate().empty());
    ASSERT_EQ(append_resp.GetServer(), "tencent-cos");
    ASSERT_EQ(append_resp.GetNextPosition(), std::to_string(total_object_len));

    // check md5
    ASSERT_EQ(append_resp.GetXCosContentSha1(),
              TestUtils::CalcStringMd5(test_str));

    // append again with old position, reuturn 409
    std::istringstream err_iss(test_str);
    AppendObjectReq err_append_req(m_bucket_name, object_name, err_iss);
    AppendObjectResp err_append_resp;
    CosResult err_append_result =
        m_client->AppendObject(err_append_req, &err_append_resp);
    ASSERT_TRUE(!err_append_result.IsSucc());
    ASSERT_EQ(err_append_result.GetHttpStatus(), 409);

    // head object
    HeadObjectReq head_req(m_bucket_name, object_name);
    HeadObjectResp head_resp;
    CosResult head_result = m_client->HeadObject(head_req, &head_resp);
    ASSERT_TRUE(head_result.IsSucc());
    ASSERT_TRUE(!head_resp.GetXCosRequestId().empty());
    ASSERT_EQ(head_resp.GetContentLength(), total_object_len);
    ASSERT_TRUE(!append_resp.GetConnection().empty());
    ASSERT_TRUE(!append_resp.GetDate().empty());
    ASSERT_EQ(head_resp.GetServer(), "tencent-cos");
    // ASSERT_EQ(head_resp.GetEtag(), TestUtils::CalcStreamMd5(iss));
    ASSERT_EQ(head_resp.GetXCosObjectType(), kObjectTypeAppendable);

    // update position
    append_position = total_object_len;
  }

  // delete object
  DeleteObjectReq delete_req(m_bucket_name, object_name);
  DeleteObjectResp delete_resp;
  CosResult delete_result = m_client->DeleteObject(delete_req, &delete_resp);
  ASSERT_TRUE(delete_result.IsSucc());
  ASSERT_TRUE(!delete_resp.GetXCosRequestId().empty());
  ASSERT_EQ(delete_resp.GetContentLength(), 0);
  ASSERT_TRUE(!delete_resp.GetConnection().empty());
  ASSERT_TRUE(!delete_resp.GetDate().empty());
  ASSERT_EQ(delete_resp.GetServer(), "tencent-cos");
}

TEST_F(ObjectOpTest, UriTest) {
  BaseOp base_op;
  std::string host = "cos.ap-guangzhou.myqcloud.com";
  std::string path = "/a/b/c";
  CosSysConfig::SetUseDnsCache(false);
  ASSERT_EQ(base_op.GetRealUrl(host, path, false),
            "http://cos.ap-guangzhou.myqcloud.com/a/b/c");
  ASSERT_EQ(base_op.GetRealUrl(host, path, true),
            "https://cos.ap-guangzhou.myqcloud.com/a/b/c");
  // set private ip
  CosSysConfig::SetIsUseIntranet(true);
  CosSysConfig::SetIntranetAddr("1.1.1.1");
  ASSERT_EQ(base_op.GetRealUrl(host, path, false), "http://1.1.1.1/a/b/c");

  // set domain
  CosSysConfig::SetDestDomain("mydomain.com");
  ASSERT_EQ(base_op.GetRealUrl(host, path, false), "http://1.1.1.1/a/b/c");
  CosSysConfig::SetIsUseIntranet(false);
  ASSERT_EQ(base_op.GetRealUrl(host, path, false), "http://mydomain.com/a/b/c");

  // set dns cache
  CosSysConfig::SetIsUseIntranet(false);
  CosSysConfig::SetDestDomain("");
  CosSysConfig::SetUseDnsCache(true);
  for (auto i = 0; i < 10000; ++i) {
    ASSERT_NE(base_op.GetRealUrl(host, path, false), "http:///a/b/c");
  }
  CosSysConfig::SetUseDnsCache(false);
}

TEST_F(ObjectOpTest, DnsCachePerfTest) {
  const int test_times = 100;
  unsigned cosume_ms = 0;
  CosSysConfig::SetUseDnsCache(false);
  std::chrono::time_point<std::chrono::steady_clock> start_ts, end_ts;
  start_ts = std::chrono::steady_clock::now();
  for (int i = 0; i < test_times; i++) {
    std::istringstream iss("put_obj_by_stream_normal_string");
    PutObjectByStreamReq put_req(m_bucket_name, "test_put_without_dns_cache",
                                 iss);
    PutObjectByStreamResp put_resp;
    CosResult result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(result.IsSucc());
  }

  end_ts = std::chrono::steady_clock::now();
  cosume_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - start_ts)
          .count();
  std::cout << "put object without dns cache, comsume: " << cosume_ms
            << std::endl;

  CosSysConfig::SetUseDnsCache(true);
  start_ts = std::chrono::steady_clock::now();
  for (int i = 0; i < test_times; i++) {
    std::istringstream iss("put_obj_by_stream_normal_string");
    PutObjectByStreamReq put_req(m_bucket_name, "test_put_with_dns_cache", iss);
    PutObjectByStreamResp put_resp;
    CosResult result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(result.IsSucc());
  }
  end_ts = std::chrono::steady_clock::now();

  cosume_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - start_ts)
          .count();
  std::cout << "put object with dns cache, comsume: " << cosume_ms << std::endl;
  CosSysConfig::SetUseDnsCache(false);
}

TEST_F(ObjectOpTest, MultiUploadVaryName) {
  std::vector<std::string> object_name_list = {"test_multiupload_object",
                                               "测试上传中文", "测试上传韩文",
                                               "のテストアップロード"};
  size_t test_file_size = 100 * 1024 * 1024;
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

TEST_F(ObjectOpTest, InvalidConfig) {
  {
    qcloud_cos::CosConfig config(123, "", "sk", "region");
    ASSERT_TRUE(config.GetAccessKey().empty());
    qcloud_cos::CosAPI cos(config);
    std::istringstream iss("put_obj_by_stream_string");
    PutObjectByStreamReq req("test_bucket", "test_object", iss);
    PutObjectByStreamResp resp;
    CosResult result = cos.PutObject(req, &resp);
    ASSERT_TRUE(!result.IsSucc());
    ASSERT_EQ(result.GetErrorMsg(),
              "Invalid access_key secret_key or region, please check your "
              "configuration");
  }
  {
    qcloud_cos::CosConfig config(123, "ak", "", "region");
    ASSERT_TRUE(config.GetSecretKey().empty());
    qcloud_cos::CosAPI cos(config);
    std::istringstream iss("put_obj_by_stream_string");
    PutObjectByStreamReq req("test_bucket", "test_object", iss);
    PutObjectByStreamResp resp;
    CosResult result = cos.PutObject(req, &resp);
    ASSERT_TRUE(!result.IsSucc());
    ASSERT_EQ(result.GetErrorMsg(),
              "Invalid access_key secret_key or region, please check your "
              "configuration");
  }
}
#endif

}  // namespace qcloud_cos
