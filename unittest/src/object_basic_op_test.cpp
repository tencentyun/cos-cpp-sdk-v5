// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Description: 基本对象操作测试
//   包含: PutObjectByFile, ObjectTagging, PutObjectByStream, IsObjectExist,
//         HeadObject, PutDirectory, MoveObject, DeleteObject, DeleteObjects,
//         GetObjectByStream, GetObjectUrl, PostObjectRestore,
//         GetObjectByFile, ResumableGetObject

#include "object_op_test_common.h"

namespace qcloud_cos {

TEST_F(ObjectOpTest, PutObjectByFileTest) {
  // 1. ObjectName为普通字符串
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "test_object", local_file);
    req.SetXCosStorageClass(kStorageClassStandard);
    PutObjectByFileResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    std::cout << "result: " << result.GetErrorMsg() << std::endl;
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


        std::cout << "start to download: " << "/././///abc/.//def//../../" << std::endl;
        CosSysConfig::SetObjectKeySimplifyCheck(true);
        GetObjectByFileReq get_req2(m_bucket_name, "/././///abc/.//def//../../", file_download);
        GetObjectByFileResp get_resp2;
        CosResult get_result2 = m_client->GetObject(get_req2, &get_resp2);
        ASSERT_TRUE(!get_result2.IsSucc());
        ASSERT_EQ("GetObjectKeyIllegal", get_result2.GetErrorCode());

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

  // 8. 带各种头部上传对象
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "test_object_with_header", local_file);
    req.SetXCosStorageClass(kStorageClassStandard);
    req.SetCacheControl("no-cache");
    req.SetCheckCRC64(true);
    req.SetCheckMD5(true);
    req.SetContentEncoding("deflate");
    req.SetContentType("text/plain");
    req.SetXCosAcl("public-read");
    req.SetXCosMeta("test", "test");
    PutObjectByFileResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    TestUtils::RemoveFile(local_file);
  }
}

TEST_F(ObjectOpTest, ObjectTaggingTest){
  //上传文件
  std::string object_tagging_test_name = "ObjectTaggingTest";
  {
    std::istringstream iss("put_obj_by_stream_with_test_obj_tagging");
    PutObjectByStreamReq req(m_bucket_name, object_tagging_test_name, iss);
    req.SetXCosStorageClass(kStorageClassStandard);
    PutObjectByStreamResp resp;
    CosResult result = m_client->PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
  //HeadObject
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, object_tagging_test_name);
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client->HeadObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("", resp.GetXCosTaggingCount());
  }
  //PutObjectTagging
  {
    qcloud_cos::PutObjectTaggingReq req(m_bucket_name, object_tagging_test_name);
    qcloud_cos::PutObjectTaggingResp resp;
    std::vector<Tag> tagset;
    Tag tag1;
    tag1.SetKey("age");
    tag1.SetValue("19");

    Tag tag2;
    tag2.SetKey("name");
    tag2.SetValue("xiaoming");

    Tag tag3;
    tag3.SetKey("sex");
    tag3.SetValue("male");

    tagset.push_back(tag1);
    tagset.push_back(tag2);
    tagset.push_back(tag3);
    req.SetTagSet(tagset);

    qcloud_cos::CosResult result = m_client->PutObjectTagging(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  //GetObjectTagging
  {
    qcloud_cos::GetObjectTaggingReq req(m_bucket_name, object_tagging_test_name);
    qcloud_cos::GetObjectTaggingResp resp;

    qcloud_cos::CosResult result = m_client->GetObjectTagging(req, &resp);
    EXPECT_TRUE(result.IsSucc());

    std::vector<Tag> tagSet = resp.GetTagSet();
    std::vector<Tag>::iterator it = tagSet.begin();

    EXPECT_EQ("age", it->GetKey());
    EXPECT_EQ("19", it->GetValue());
    ++it;
    EXPECT_EQ("name", it->GetKey());
    EXPECT_EQ("xiaoming", it->GetValue());
    ++it;
    EXPECT_EQ("sex", it->GetKey());
    EXPECT_EQ("male", it->GetValue());
  }
  //HeadObject
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, object_tagging_test_name);
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client->HeadObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("3", resp.GetXCosTaggingCount());
  }
  //DeleteObjectTagging
  {
    qcloud_cos::DeleteObjectTaggingReq req(m_bucket_name, object_tagging_test_name);
    qcloud_cos::DeleteObjectTaggingResp resp;

    qcloud_cos::CosResult result = m_client->DeleteObjectTagging(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
  //HeadObject
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, object_tagging_test_name);
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client->HeadObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("", resp.GetXCosTaggingCount());
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

  // 带version请求不存在的对象
  {
    HeadObjectReq head_req(m_bucket_name, "test_head_object");
    head_req.AddParam("versionId", "test");
    HeadObjectResp head_resp;
    CosResult  result = m_client->HeadObject(head_req, &head_resp);
    ASSERT_FALSE(result.IsSucc());
    EXPECT_EQ(404, result.GetHttpStatus());
  }
}

TEST_F(ObjectOpTest, PutDirectoryTest) {
  std::string directory_name = "put_directory_test";
  PutDirectoryReq req(m_bucket_name, directory_name);
  PutDirectoryResp resp;
  CosResult result = m_client->PutDirectory(req, &resp);
  ASSERT_TRUE(result.IsSucc());
}

TEST_F(ObjectOpTest, MoveObjectTest) {
  std::istringstream iss("put_obj_by_stream_normal_string");
  PutObjectByStreamReq req(m_bucket_name, "move_object_src", iss);
  PutObjectByStreamResp resp;
  CosResult result = m_client->PutObject(req, &resp);
  ASSERT_TRUE(result.IsSucc());

  MoveObjectReq mv_req(m_bucket_name,  "move_object_src", "move_object_dst");
  try{
    // 非法bucket_name
    MoveObjectReq test_req("bucket_name-12500000000@xxxx", "move_object_src", "move_object_dst");
  }
  catch(const std::exception& e){
    EXPECT_STREQ(e.what(), "Invalid bucket_name argument :bucket_name-12500000000@xxxx");
  }
  mv_req.SetSSLCtxCallback(SslCtxCallback, nullptr);
  CosResult mv_result = m_client->MoveObject(mv_req);
  ASSERT_TRUE(mv_result.IsSucc());
}

TEST_F(ObjectOpTest, DeleteObjectTest) {
  // Delete empty string, test whether call the DeleteBucket interface
  std::string object_name = "";
  DeleteObjectReq req(m_bucket_name, object_name);
  DeleteObjectResp resp;
  CosResult result = m_client->DeleteObject(req, &resp);
  std::string errinfo = result.GetErrorMsg();
  EXPECT_EQ("Delete object's name is empty.", errinfo);
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
      try{
        // 非法bucket_name
        PutObjectsByDirectoryReq test_req("bucket_name-12500000000@xxxx",directory_name);
      }
      catch(const std::exception& e){
        EXPECT_STREQ(e.what(), "Invalid bucket_name argument :bucket_name-12500000000@xxxx");
      }
      try{
        // 非法bucket_name
        PutObjectsByDirectoryReq test_req("bucket_name-12500000000@xxxx",directory_name,"111");
      }
      catch(const std::exception& e){
        EXPECT_STREQ(e.what(), "Invalid bucket_name argument :bucket_name-12500000000@xxxx");
      }
      PutObjectsByDirectoryResp resp;
      CosResult result = m_client->PutObjects(req, &resp);
      ASSERT_TRUE(result.IsSucc());
      TestUtils::RemoveFile(local_file1);
      TestUtils::RemoveFile(local_file2);
      TestUtils::RemoveFile(local_file3);
      TestUtils::RemoveFile(local_file4);
      TestUtils::RemoveDirectory(directory_name);

      DeleteObjectsByPrefixReq del_req(m_bucket_name, directory_name);
      try{
        // 非法bucket_name
        DeleteObjectsByPrefixReq test_req("bucket_name-12500000000@xxxx",directory_name);
      }
      catch(const std::exception& e){
        EXPECT_STREQ(e.what(), "Invalid bucket_name argument :bucket_name-12500000000@xxxx");
      }
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
    try{
      // 非法bucket_name
      DeleteObjectsReq get_req("bucket_name-12500000000@xxxx");
    }
    catch(const std::exception& e){
      EXPECT_STREQ(e.what(), "Invalid bucket_name argument :bucket_name-12500000000@xxxx");
    }
    try{
      // 非法bucket_name
      DeleteObjectsReq get_req("bucket_name-12500000000@xxxx",to_be_deleted);
    }
    catch(const std::exception& e){
      EXPECT_STREQ(e.what(), "Invalid bucket_name argument :bucket_name-12500000000@xxxx");
    }
    try{
      // 非法bucket_name
      req.SetBucketName("bucket_name-12500000000@xxxx");
    }
    catch(const std::exception& e){
      EXPECT_STREQ(e.what(), "Invalid bucket_name argument :bucket_name-12500000000@xxxx");
    }
    req.SetBucketName(m_bucket_name);
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
    try{
      // 非法bucket_name
      GetObjectByStreamReq get_req("bucket_name-12500000000@xxxx", object_name, os);
    }
    catch(const std::exception& e){
      EXPECT_STREQ(e.what(), "Invalid bucket_name argument :bucket_name-12500000000@xxxx");
    }
    try{
      // 非法bucket_name
      get_req.SetBucketName("bucket_name-12500000000@xxxx");
    }
    catch(const std::exception& e){
      EXPECT_STREQ(e.what(), "Invalid bucket_name argument :bucket_name-12500000000@xxxx");
    }
    get_req.SetBucketName(m_bucket_name);
    GetObjectByStreamResp get_resp;
    CosResult get_result = m_client->GetObject(get_req, &get_resp);
    ASSERT_TRUE(get_result.IsSucc());

    {
      //合并路径
      qcloud_cos::GetObjectByStreamReq get_req2(m_bucket_name, "/././///abc/.//def//../../",
                                        os);
      qcloud_cos::GetObjectByStreamResp get_resp2;
      CosResult get_result2 = m_client->GetObject(get_req2, &get_resp2);
      ASSERT_TRUE(!get_result2.IsSucc());
      ASSERT_EQ("GetObjectKeyIllegal", get_result2.GetErrorCode());
    }

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

  // 下载带header 和 param（range）
  {
    std::istringstream iss("put_obj_by_stream_normal_string");
    std::string object_name = "get_object_with_header_param_test";
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    ASSERT_TRUE(put_result.IsSucc());

    std::ostringstream os;
    GetObjectByStreamReq get_req(m_bucket_name, object_name, os);
    get_req.SetResponseContentLang("en-US");
    get_req.SetResponseContentType("image/jpg");
    get_req.SetSignHeaderHost(true);
    get_req.AddHeader("Range", "bytes=0-1");
    get_req.SetCheckMD5(false);
    GetObjectByStreamResp get_resp;
    CosResult get_result = m_client->GetObject(get_req, &get_resp);
    ASSERT_TRUE(get_result.IsSucc());

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
  // 回热带versionid，版本不存在
  {
    PostObjectRestoreReq req(m_bucket_name,  "post_object_restore_not_exist");
    req.SetExiryDays(30);
    req.SetTier("Standard");
    req.AddParam("versionId", "test");
    PostObjectRestoreResp resp;
    CosResult result = m_client->PostObjectRestore(req, &resp);
    ASSERT_FALSE(result.IsSucc());
    EXPECT_EQ(404, result.GetHttpStatus());
  }
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
  std::istringstream iss("put_obj_by_stream_normal_string");
  std::string object_name = "resumable_get_object_test";
  PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
  PutObjectByStreamResp put_resp;
  CosResult put_result = m_client->PutObject(put_req, &put_resp);
  ASSERT_TRUE(put_result.IsSucc());

  std::string file_download = "resumable_get_object_test_file_download";
  GetObjectByFileReq get_req(m_bucket_name, object_name, file_download);
  GetObjectByFileResp get_resp;
  //get_req.SetHttps();
  get_req.SetSSLCtxCallback(SslCtxCallback, nullptr);
  CosResult get_result = m_client->ResumableGetObject(get_req, &get_resp);
  
  ASSERT_TRUE(get_result.IsSucc());

  {
    //合并路径
    qcloud_cos::GetObjectByFileReq get_req2(m_bucket_name, "/././///abc/.//def//../../",
                                      file_download);
    qcloud_cos::GetObjectByFileResp get_resp2;
    CosResult get_result2 = m_client->ResumableGetObject(get_req2, &get_resp2);
    ASSERT_TRUE(!get_result2.IsSucc());
    ASSERT_EQ("GetObjectKeyIllegal", get_result2.GetErrorCode());
  }

  std::string file_md5_download = TestUtils::CalcFileMd5(file_download);
  ASSERT_EQ(file_md5_download, get_resp.GetEtag());

  DeleteObjectReq del_req(m_bucket_name, object_name);
  DeleteObjectResp del_resp;
  CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
  ASSERT_TRUE(del_result.IsSucc());

  TestUtils::RemoveFile(file_download);
}

}  // namespace qcloud_cos
