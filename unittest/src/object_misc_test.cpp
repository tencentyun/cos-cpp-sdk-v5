// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Description: 其他杂项测试
//   包含: ObjectACL, GeneratePresignedUrl, PutObjectWithMultiMeta,
//         ObjectOptionsDefault, SelectObjectContent, TestPutObjectWithMeta,
//         TestMultiPutObjectWithMeta, AppendObject, UriTest, DnsCachePerfTest,
//         InvalidConfig, ObjectOptest1

#include "object_op_test_common.h"

namespace qcloud_cos {

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

  // 预签名带Header和Param
  {
    GeneratePresignedUrlReq req(m_bucket_name, "object_test", HTTP_GET);
    req.SetStartTimeInSec(1000);
    req.SetExpiredTimeInSec(5 * 60);
    req.AddHeader("Range", "bytes=0-1");
    req.AddHeader("x-cos-traffic-limit", "819200");
    req.AddParam("response-cache-control", "no-cache");
    req.AddParam("response-content-type", "text/plain");

    std::string presigned_url = m_client->GeneratePresignedUrl(req);
    EXPECT_FALSE(presigned_url.empty());
    EXPECT_TRUE(presigned_url.find("range") != std::string::npos);
    EXPECT_TRUE(presigned_url.find("x-cos-traffic-limit") != std::string::npos);
    EXPECT_TRUE(presigned_url.find("response-cache-control") != std::string::npos);
    EXPECT_TRUE(presigned_url.find("response-content-type") != std::string::npos);
  }
  
  // 使用临时Token生成预签名
  {
    CosConfig *config = new CosConfig("./config.json");
    config->SetAccessKey("access_key");
    config->SetSecretKey("secret_key");
    config->SetTmpToken("tmp-token");

    CosAPI *client = new CosAPI(*config);

    GeneratePresignedUrlReq req(m_bucket_name, "object_test", HTTP_GET);
    req.SetStartTimeInSec(2400);
    
    std::string presigned_url = client->GeneratePresignedUrl(req);
    EXPECT_FALSE(presigned_url.empty());
    EXPECT_NE(std::string::npos, presigned_url.find("x-cos-security-token=tmp-token"));
    SDK_LOG_ERR("end to generate: %s", presigned_url.c_str());
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
    //EXPECT_EQ(0, input_str.compare(StringUtil::Trim(strstream.str(), "\\n")));
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
    //EXPECT_EQ(
    //    0, StringUtil::Trim(strstream.str(), "\\n").compare("{\"aaa\":111}"));
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
    std::istringstream iss("aaa,111,bbb,222,ccc,333\naaa,111,bbb,222,ccc,333");
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
    //EXPECT_EQ(0, input_str.compare(StringUtil::Trim(strstream.str(), "\\\\n")));
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
      size_t file_size = ((rand() % 20) + 1) * size;
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
      //get_req.SetHttps();
      get_req.SetSSLCtxCallback(SslCtxCallback, nullptr);
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

TEST_F(ObjectOpTest, InvalidConfig) {
  {
    qcloud_cos::CosConfig config(123, "", "sk", "region");
    ASSERT_TRUE(config.GetAccessKey().empty());
    qcloud_cos::CosAPI cos(config);
    std::istringstream iss("put_obj_by_stream_string");
    PutObjectByStreamReq req("test-bucket-1253960454", "test_object", iss);
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
    PutObjectByStreamReq req("test-bucket-1253960454", "test_object", iss);
    PutObjectByStreamResp resp;
    CosResult result = cos.PutObject(req, &resp);
    result.DebugString();
    ASSERT_TRUE(!result.IsSucc());
    ASSERT_EQ(result.GetErrorMsg(),
              "Invalid access_key secret_key or region, please check your "
              "configuration");
  }
}

TEST_F(ObjectOpTest, ObjectOptest1){
  SharedConfig config = std::make_shared<CosConfig>("./config.json");
  ObjectOp *object_op = new ObjectOp(config);
  object_op->GetCosConfig();
  object_op->GetTmpToken();
  object_op->GetDestDomain();
  ASSERT_TRUE(object_op->IsDefaultHost("xxxxx-12234.cos.zzzzz-wwww.myqcloud.com"));

  std::string local_file = "./testfile2";
  TestUtils::WriteRandomDatatoFile(local_file, 1024);
  PutObjectByFileReq req(m_bucket_name, "test_object", local_file);
  object_op->CheckSinglePart(req, 5, 10, 10,"1024");
  TestUtils::RemoveFile(local_file);
  delete object_op;
}

}  // namespace qcloud_cos
