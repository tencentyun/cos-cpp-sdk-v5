// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 08/11/17
// Description:

#include <iostream>
#include <thread>

#include "cos_sys_config.h"
#include "gtest/gtest.h"
#include "util/test_utils.h"
#include "util/auth_tool.h"
#include "util/file_util.h"
#include "util/lru_cache.h"
#include "util/simple_dns_cache.h"
#include "util/string_util.h"
#include "util/log_util.h"
#include "util/codec_util.h"

namespace qcloud_cos {

TEST(UtilTest, AuthToolTest) {
  std::string access_key = "access_key_test";
  std::string secret_key = "secret_key_test";
  std::string http_method = "GET";
  std::string in_uri = "xxxx";

  std::map<std::string, std::string> headers;
  headers["host"] = "hostname_test";
  headers["x-cos-meta-xx"] = "meta_test";
  std::map<std::string, std::string> params;
  params["first_param"] = "first_value";
  params["second_param"] = "second_value";

  std::string expected =
      "q-sign-algorithm=sha1&q-ak=access_key_test&q-sign-time="
      "1502493430;1502573430&q-key-time=1502493430;1502573430&"
      "q-header-list=host;x-cos-meta-xx&q-url-param-list=first"
      "_param;second_param&q-signature=7686473b75487163d6e204d"
      "3c68e5c3b74e06164";

  std::unordered_set<std::string> not_sign_headers;
  // 1. header、param均非空
  std::string sign_result =
      AuthTool::Sign(access_key, secret_key, http_method, in_uri, headers,
                     params, 1502493430, 1502573430,not_sign_headers);

  // sign_result.resize(sign_result.size() - 1);
  EXPECT_EQ(expected, sign_result);

  // 2.头部包含无需签名部分
  headers["unsigned_header"] = "unsigned_header_content";
  sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                               headers, params, 1502493430, 1502573430, not_sign_headers);

  // sign_result.resize(sign_result.size() - 1);
  EXPECT_EQ(expected, sign_result);

  // 3. params为空
  std::string no_param_expected =
      "q-sign-algorithm=sha1&q-ak=access_key_test&q-sig"
      "n-time=1502493430;1502573430&q-key-time=15024934"
      "30;1502573430&q-header-list=host;x-cos-meta-xx&q"
      "-url-param-list=&q-signature=8e196637993353bdff2"
      "466e0d34034130c9a2aca";
  params.clear();
  sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                               headers, params, 1502493430, 1502573430, not_sign_headers);

  // sign_result.resize(sign_result.size() - 1);
  EXPECT_EQ(no_param_expected, sign_result);

  // 4. headers/params为空
  std::string empty_expected =
      "q-sign-algorithm=sha1&q-ak=access_key_test&q-sign-t"
      "ime=1502493430;1502573430&q-key-time=1502493430;150"
      "2573430&q-header-list=&q-url-param-list=&q-signatur"
      "e=122c75ad3f7c230b8cc88f4f38410b9d3d347997";
  headers.clear();
  sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                               headers, params, 1502493430, 1502573430, not_sign_headers);

  // sign_result.resize(sign_result.size() - 1);
  EXPECT_EQ(empty_expected, sign_result);

  // 5. access_key为空
  access_key = "";
  sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                               headers, params, 1502493430, 1502573430, not_sign_headers);
  EXPECT_EQ("", sign_result);

  // 6. secret_key为空
  access_key = "access_key_test";
  secret_key = "";
  sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                               headers, params, 1502493430, 1502573430, not_sign_headers);
  EXPECT_EQ("", sign_result);

  // test sign headers
  headers["Origin"] = "test";
  headers["Content-Type"] = "test";
  headers["Transfer-Encoding"] = "test";
  headers["not-exists-header"] = "test";
  sign_result = AuthTool::Sign("access_key", "secret_key", "http_method",
                               "in_uri", headers, params, not_sign_headers);
  ASSERT_TRUE(sign_result.find("origin") != std::string::npos);
  ASSERT_TRUE(sign_result.find("content-type") != std::string::npos);
  ASSERT_TRUE(sign_result.find("transfer-encoding") != std::string::npos);
  ASSERT_TRUE(sign_result.find("not-exists-header") == std::string::npos);
}

TEST(UtilTest, MD5Test) {
  const std::string test_file = "/tmp/testmd5";
  TestUtils::WriteStringtoFile(test_file, "aaaaaaaaa");
  ASSERT_EQ(FileUtil::GetFileMd5(test_file),
            "552e6a97297c53e592208cf97fbb3b60");
  TestUtils::RemoveFile(test_file);
}

TEST(UtilTest, CR64Test) {
  const std::string test_file = "/tmp/testcrc64";
  TestUtils::WriteStringtoFile(test_file, "0123456789");
  ASSERT_EQ(FileUtil::GetFileCrc64(test_file), 2838902930144391966);
  TestUtils::RemoveFile(test_file);
}

TEST(UtilTest, FileLenTest) {
  const std::string test_file = "/tmp/testfilelen";
  const size_t test_file_len = 11111;
  TestUtils::WriteRandomDatatoFile(test_file, test_file_len);
  ASSERT_EQ(FileUtil::GetFileLen(test_file), test_file_len);
  TestUtils::RemoveFile(test_file);
}

TEST(UtilTest, LruCacheTest) {
  const size_t test_size = 50;
  const size_t test_put_record = 100;
  LruCache<int, int> testcache1(test_size);
  for (size_t i = 1; i <= test_put_record; ++i) {
    testcache1.Put(i, i);
    ASSERT_TRUE(testcache1.Exist(i));
    ASSERT_EQ(testcache1.Get(i), i);
    EXPECT_THROW(testcache1.Get(i + 1), std::range_error);
    if (i <= test_size) {
      ASSERT_EQ(testcache1.Size(), i);
      for (size_t j = 1; j <= i; ++j) {
        ASSERT_TRUE(testcache1.Exist(j));
        ASSERT_EQ(testcache1.Get(j), j);
      }
    } else {
      ASSERT_EQ(testcache1.Size(), test_size);
      for (size_t j = 1; j <= i - test_size; ++j) {
        ASSERT_TRUE(!testcache1.Exist(j));
        EXPECT_THROW(testcache1.Get(j), std::range_error);
      }
    }
  }

  LruCache<int, int> testcache2(test_size);
  for (size_t i = 1; i <= test_put_record; ++i) {
    testcache2.Put(1, i);
    ASSERT_EQ(testcache2.Size(), 1);
    ASSERT_EQ(testcache2.Get(1), i);
  }
}

static unsigned GetResolveTime(SimpleDnsCache& dns_cache,
                               const std::string& host) {
  std::chrono::time_point<std::chrono::steady_clock> start_ts, end_ts;
  start_ts = std::chrono::steady_clock::now();
  dns_cache.Resolve(host);
  end_ts = std::chrono::steady_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(end_ts -
                                                               start_ts)
      .count();
}

TEST(UtilTest, DnsCacheTest) {
  CosSysConfig::SetDnsCacheSize(100);
  ASSERT_EQ(CosSysConfig::GetDnsCacheSize(), 100);
  CosSysConfig::SetUseDnsCache(true);
  ASSERT_EQ(CosSysConfig::GetUseDnsCache(), true);
  CosSysConfig::SetUseDnsCache(false);
  ASSERT_EQ(CosSysConfig::GetUseDnsCache(), false);
  CosSysConfig::SetDnsCacheExpireSeconds(3600);
  ASSERT_EQ(CosSysConfig::GetDnsCacheExpireSeconds(), 3600);

  const int dns_expire_seconds = 5;
  SimpleDnsCache dns_cache(CosSysConfig::GetDnsCacheSize(), dns_expire_seconds);
  const std::string cos_domain_gz = "cos.ap-guangzhou.myqcloud.com";

  GetResolveTime(dns_cache, cos_domain_gz);

  ASSERT_TRUE(dns_cache.Exist(cos_domain_gz));

  // resolve 1000 times
  for (auto i = 0; i < 100; ++i) {
    ASSERT_TRUE(GetResolveTime(dns_cache, cos_domain_gz) <= 1);
    ASSERT_TRUE(!dns_cache.Resolve(cos_domain_gz).empty());
  }

  // wait for expiring
  std::this_thread::sleep_for(std::chrono::seconds(dns_expire_seconds + 1));
  ASSERT_TRUE(GetResolveTime(dns_cache, cos_domain_gz) > 10);

}

TEST(UtilTest, StringUtilTest) {
  StringUtil string_util;
  {
    std::string str = "";
    string_util.Trim(str);
  }
  {
    std::string body = "<LiveChannelConfiguration><Description>111</Description><Switch>prefix</Switch><Target><Type>prefixA_0</Type><FragDuration>97</FragDuration><FragCount>78</FragCount><PlaylistName>xxxxx</PlaylistName><PublishUrls><Url>test12</Url></PublishUrls><PlayUrls><Url>test1</Url></PlayUrls></Target><Unknown>123</Unknown>\n</LiveChannelConfiguration>\n\n";
    rapidxml::xml_document<> doc;
    StringUtil::StringToXml(&body[0], &doc);
    StringUtil::XmlToString(doc);
  }
  {
    std::string str = StringUtil::FloatToString(1.1);
    ASSERT_EQ(str, "1.100000");
  }
  {
    std::vector<std::string> str_vec = {"hello", "world"};
    std::string delimiter = ",";

    std::string expected = "hello,world";
    std::string actual = StringUtil::JoinStrings(str_vec, delimiter);

    EXPECT_EQ(expected, actual);
  }
  {
    std::string str = "HelloWorld";
    std::string suffix = "world";

    bool expected = true;
    bool actual = StringUtil::StringEndsWithIgnoreCase(str, suffix);

    EXPECT_EQ(expected, actual);
  }
  {
    std::string str = "hello,world,this,is,a,test";
    std::string sep = ",";
    std::vector<std::string> expected = {"hello", "world", "this", "is", "a", "test"};

    std::vector<std::string> actual;
    StringUtil::SplitString(str, sep, &actual);

    EXPECT_EQ(expected, actual);
  }
  {
    EXPECT_EQ("GET", StringUtil::HttpMethodToString(HTTP_GET));
    EXPECT_EQ("HEAD", StringUtil::HttpMethodToString(HTTP_HEAD));
    EXPECT_EQ("PUT", StringUtil::HttpMethodToString(HTTP_PUT));
    EXPECT_EQ("POST", StringUtil::HttpMethodToString(HTTP_POST));
    EXPECT_EQ("DELETE", StringUtil::HttpMethodToString(HTTP_DELETE));
    EXPECT_EQ("OPTIONS", StringUtil::HttpMethodToString(HTTP_OPTIONS));
    EXPECT_EQ("UNKNOWN", StringUtil::HttpMethodToString(static_cast<HTTP_METHOD>(-1)));
  }
  {
    std::string etag1 = "\"abcdef1234567890\"";
    std::string etag2 = "\"abcdef1234567890-1\"";

    bool expected1 = false;
    bool expected2 = true;

    bool actual1 = StringUtil::IsMultipartUploadETag(etag1);
    bool actual2 = StringUtil::IsMultipartUploadETag(etag2);

    EXPECT_EQ(expected1, actual1);
    EXPECT_EQ(expected2, actual2);
  }
  {
    std::string input1 = "Hello, world!";
    std::string input2 = "This is a test.";

    std::istringstream is1(input1);
    std::istringstream is2(input2);

    std::string expected1 = "6cd3556deb0da54bca060b4c39479839";
    std::string expected2 = "120ea8a25e5d487bf68b5f7096440019";

    std::string actual1 = TestUtils::CalcStreamMd5(is1);
    std::string actual2 = TestUtils::CalcStreamMd5(is2);

    EXPECT_EQ(expected1, actual1);
    EXPECT_EQ(expected2, actual2);
  }
  {
    std::string input1 = "Hello, world!";
    std::string input2 = "This is a test.";

    std::istringstream is1(input1);
    std::istringstream is2(input2);

    std::string expected1 = "943a702d06f34599aee1f8da8ef9f7296031d699";
    std::string expected2 = "afa6c8b3a2fae95785dc7d9685a57835d703ac88";

    std::string actual1 = TestUtils::CalcStreamSHA1(is1);
    std::string actual2 = TestUtils::CalcStreamSHA1(is2);

    EXPECT_EQ(expected1, actual1);
    EXPECT_EQ(expected2, actual2);
  }
  {
    std::string env_var_name = "TEST_ENV_VAR";
    std::string env_var_value = "Hello, world!";
    setenv(env_var_name.c_str(), env_var_value.c_str(), 1);

    // 测试存在的环境变量
    std::string actual1 = TestUtils::GetEnvVar(env_var_name);
    EXPECT_EQ(env_var_value, actual1);

    // 测试不存在的环境变量
    std::string env_var_name2 = "NOT_EXIST_ENV_VAR";
    std::string expected2 = "NOT_EXIST_ENV_" + env_var_name2;
    std::string actual2 = TestUtils::GetEnvVar(env_var_name2);
    EXPECT_EQ(expected2, actual2);

    // 清理环境变量
    unsetenv(env_var_name.c_str());
  }
  #if defined(__linux__)
  {
    std::string path = "/tmp/test_dir";

    // 创建目录
    mkdir(path.c_str(), 0775);

    // 测试存在的目录
    bool actual1 = TestUtils::IsDirectoryExists(path);
    EXPECT_TRUE(actual1);

    // 删除目录
    rmdir(path.c_str());

    // 测试不存在的目录
    bool actual2 = TestUtils::IsDirectoryExists(path);
    EXPECT_FALSE(actual2);
  }
  {
    std::string path = "/tmp/test_dir";

    // 删除目录（如果存在）
    rmdir(path.c_str());

    // 测试创建目录
    bool actual = TestUtils::MakeDirectory(path);
    EXPECT_TRUE(actual);

    // 检查目录是否存在
    bool exists = TestUtils::IsDirectoryExists(path);
    EXPECT_TRUE(exists);

    // 清理
    rmdir(path.c_str());
  }
  {
    std::string path = "/tmp/test_dir";

    // 创建目录
    mkdir(path.c_str(), 0775);

    // 测试删除目录
    bool actual = TestUtils::RemoveDirectory(path);
    EXPECT_TRUE(actual);

    // 检查目录是否存在
    bool exists = TestUtils::IsDirectoryExists(path);
    EXPECT_FALSE(exists);
  }
  	#endif
}

TEST(UtilTest, LogUtilTest){
  {
    std::string actual = qcloud_cos::LogUtil::GetLogPrefix(qcloud_cos::COS_LOG_INFO);
    std::string expected_prefix = "INFO";
    EXPECT_NE(actual.find(expected_prefix), std::string::npos);
  }
  {
    std::string actual = qcloud_cos::LogUtil::FormatLog(qcloud_cos::COS_LOG_ERR, "Test error: %d", 42);
    std::string expected_prefix = "ERR";
    std::string expected_message = "Test error: 42";
    EXPECT_NE(actual.find(expected_prefix), std::string::npos);
    EXPECT_NE(actual.find(expected_message), std::string::npos);
  }
  {
    qcloud_cos::LogUtil::Syslog(qcloud_cos::COS_LOG_INFO, "Test message");
  }
}
TEST(UtilTest, FileUtilTest){
  {
    // 创建一个临时文件
    std::string temp_file_path = "/tmp/test_file.txt";
    std::ofstream temp_file(temp_file_path);
    temp_file << "Hello, world!";
    temp_file.close();

    // 测试GetFileContent函数
    std::string actual = qcloud_cos::FileUtil::GetFileContent(temp_file_path);
    std::string expected = "Hello, world!";
    EXPECT_EQ(expected, actual);

    // 删除临时文件
    std::remove(temp_file_path.c_str());
  }
  {
    std::string path1 = "/tmp/test_file.txt";
    std::string expected1 = "/tmp";
    std::string actual1 = qcloud_cos::FileUtil::GetDirectory(path1);
    EXPECT_EQ(expected1, actual1);
  }
}

TEST(UtilTest, CodecUtilTest){
  {
    unsigned char bin[] = {0x12, 0x34, 0x56, 0x78};
    unsigned int binLen = sizeof(bin) / sizeof(bin[0]);
    char hex[9];
    hex[8] = '\0';

    qcloud_cos::CodecUtil::BinToHex(bin, binLen, hex);
    std::string expected = "12345678";
    std::string actual(hex);
    EXPECT_EQ(expected, actual);
  }
}
}  // namespace qcloud_cos
