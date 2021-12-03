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
#include "test_utils.h"
#include "util/auth_tool.h"
#include "util/file_util.h"
#include "util/lru_cache.h"
#include "util/simple_dns_cache.h"

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

  // 1. header、param均非空
  std::string sign_result =
      AuthTool::Sign(access_key, secret_key, http_method, in_uri, headers,
                     params, 1502493430, 1502573430);

  // sign_result.resize(sign_result.size() - 1);
  EXPECT_EQ(expected, sign_result);

  // 2.头部包含无需签名部分
  headers["unsigned_header"] = "unsigned_header_content";
  sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                               headers, params, 1502493430, 1502573430);

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
                               headers, params, 1502493430, 1502573430);

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
                               headers, params, 1502493430, 1502573430);

  // sign_result.resize(sign_result.size() - 1);
  EXPECT_EQ(empty_expected, sign_result);

  // 5. access_key为空
  access_key = "";
  sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                               headers, params, 1502493430, 1502573430);
  EXPECT_EQ("", sign_result);

  // 6. secret_key为空
  access_key = "access_key_test";
  secret_key = "";
  sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                               headers, params, 1502493430, 1502573430);
  EXPECT_EQ("", sign_result);

  // test sign headers
  headers["Origin"] = "test";
  headers["Content-Type"] = "test";
  headers["Transfer-Encoding"] = "test";
  headers["not-exists-header"] = "test";
  sign_result = AuthTool::Sign("access_key", "secret_key", "http_method",
                               "in_uri", headers, params);
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
  for (auto i = 1; i <= test_put_record; ++i) {
    testcache1.Put(i, i);
    ASSERT_TRUE(testcache1.Exist(i));
    ASSERT_EQ(testcache1.Get(i), i);
    EXPECT_THROW(testcache1.Get(i + 1), std::range_error);
    if (i <= test_size) {
      ASSERT_EQ(testcache1.Size(), i);
      for (auto j = 1; j <= i; ++j) {
        ASSERT_TRUE(testcache1.Exist(j));
        ASSERT_EQ(testcache1.Get(j), j);
      }
    } else {
      ASSERT_EQ(testcache1.Size(), test_size);
      for (auto j = 1; j <= i - test_size; ++j) {
        ASSERT_TRUE(!testcache1.Exist(j));
        EXPECT_THROW(testcache1.Get(j), std::range_error);
      }
    }
  }

  LruCache<int, int> testcache2(test_size);
  for (auto i = 1; i <= test_put_record; ++i) {
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

  ASSERT_TRUE(GetResolveTime(dns_cache, cos_domain_gz) > 10);

  ASSERT_TRUE(dns_cache.Exist(cos_domain_gz));

  // resolve 1000 times
  for (auto i = 0; i < 100; ++i) {
    ASSERT_TRUE(GetResolveTime(dns_cache, cos_domain_gz) <= 1);
    ASSERT_TRUE(!dns_cache.Resolve(cos_domain_gz).empty());
  }

  // wait for expiring
  std::this_thread::sleep_for(std::chrono::seconds(dns_expire_seconds + 1));
  ASSERT_TRUE(GetResolveTime(dns_cache, cos_domain_gz) > 10);

  // resolve 1000 times
  for (auto i = 0; i < 100; ++i) {
    ASSERT_TRUE(GetResolveTime(dns_cache, cos_domain_gz) <= 1);
    ASSERT_TRUE(!dns_cache.Resolve(cos_domain_gz).empty());
  }
}
}  // namespace qcloud_cos
