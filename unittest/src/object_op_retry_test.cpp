
#include <sstream>

#include <thread>
#include "Poco/MD5Engine.h"
#include "Poco/StreamCopier.h"
#include "cos_api.h"
#include "util/test_utils.h"
#include "util/file_util.h"
#include "util/simple_dns_cache.h"
#include "gtest/gtest.h"
#include "op/object_op.h"


namespace qcloud_cos {
class ObjectOpRetryTest : public ::testing::Test {
    protected:
      static void SetUpTestCase() {
        m_client_without_retry= createCosClient(0);
        m_client_with_retry= createCosClient(3);
        m_bucket_name = GetEnvVar("ErrBucket") + "-" + GetEnvVar("ErrAppid");
      }

      static CosAPI* createCosClient(int max_retry_num) {
        m_config = new CosConfig("./config.json");
        m_config->SetMaxRetryTimes(max_retry_num);
        m_config->SetAccessKey(GetEnvVar("CPP_SDK_V5_ACCESS_KEY"));
        m_config->SetSecretKey(GetEnvVar("CPP_SDK_V5_SECRET_KEY"));
        m_config->SetRegion(GetEnvVar("ErrRegion"));

        return new CosAPI(*m_config);
      }

      static void TearDownTestCase() {
        delete m_client_with_retry;
        delete m_client_without_retry;
        delete m_config;
      }

    protected:
      static CosConfig* m_config;
      static CosAPI* m_client_without_retry;
      static CosAPI* m_client_with_retry;
      static std::string m_bucket_name;
};

CosConfig* ObjectOpRetryTest::m_config;
CosAPI* ObjectOpRetryTest::m_client_without_retry;
CosAPI* ObjectOpRetryTest::m_client_with_retry;
std::string ObjectOpRetryTest::m_bucket_name;

TEST_F(ObjectOpRetryTest, GetObjectFor2xxTest) {
    {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "200", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "204", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "206", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpRetryTest, GetObjectWithChangeDomainFor2xxTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "200", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "204", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "206", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, GetObjectWithoutRetryFor5xxTest) {
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "500", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_without_retry->GetObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(500, result.GetHttpStatus());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "503", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_without_retry->GetObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(503, result.GetHttpStatus());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "504", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_without_retry->GetObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(504, result.GetHttpStatus());
  }
}

TEST_F(ObjectOpRetryTest, GetObjectWithRetryFor5xxTest) {
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "500", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "503", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "504", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpRetryTest, GetObjectWithRetryChangeDomainFor5xxTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "500", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "503", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "504", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, GetObjectWithRetryFor4xxTest) {
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "400", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(400, result.GetHttpStatus());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "403", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(403, result.GetHttpStatus());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "404", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(404, result.GetHttpStatus());
  }
}

TEST_F(ObjectOpRetryTest, GetObjectWithoutRetryFor3xxTest) {
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "301", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_without_retry->GetObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(301, result.GetHttpStatus());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "302", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_without_retry->GetObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(302, result.GetHttpStatus());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "307", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_without_retry->GetObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(307, result.GetHttpStatus());
  }
}

TEST_F(ObjectOpRetryTest, GetObjectWithRetryChangeDomainFor3xxTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "301", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "302", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "307", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, PutObjectWithoutRetryFor5xxTest) {
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "500", local_file);

    PutObjectByFileResp resp;
    CosResult result = m_client_without_retry->PutObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(500, result.GetHttpStatus());
  }
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "503", local_file);

    PutObjectByFileResp resp;
    CosResult result = m_client_without_retry->PutObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(503, result.GetHttpStatus());
  }
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "504", local_file);

    PutObjectByFileResp resp;
    CosResult result = m_client_without_retry->PutObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(504, result.GetHttpStatus());
  }
}

TEST_F(ObjectOpRetryTest, PutObjectWithRetryFor5xxTest) {
  {
    std::istringstream iss("put object test");
    PutObjectByStreamReq req(m_bucket_name, "500", iss);

    qcloud_cos::PutObjectByStreamResp resp;
    CosResult result = m_client_with_retry->PutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "503", local_file);

    PutObjectByFileResp resp;
    CosResult result = m_client_with_retry->PutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "504", local_file);

    PutObjectByFileResp resp;
    CosResult result = m_client_with_retry->PutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpRetryTest, PutObjectWithRetryChangeDomainFor5xxTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "500", local_file);

    qcloud_cos::PutObjectByFileResp resp;
    CosResult result = m_client_with_retry->PutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    std::istringstream iss("put object test");
    PutObjectByStreamReq req(m_bucket_name, "503", iss);

    qcloud_cos::PutObjectByStreamResp resp;
    CosResult result = m_client_with_retry->PutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "504", local_file);

    qcloud_cos::PutObjectByFileResp resp;
    CosResult result = m_client_with_retry->PutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, PutObjectWithoutRetryFor3xxTest) {
  {
      std::string local_file = "./testfile";
      TestUtils::WriteRandomDatatoFile(local_file, 1024);
      PutObjectByFileReq req(m_bucket_name, "301", local_file);

      PutObjectByFileResp resp;
      CosResult result = m_client_without_retry->PutObject(req, &resp);
      EXPECT_FALSE(result.IsSucc());
      EXPECT_EQ(301, result.GetHttpStatus());
  }
  {
      std::string local_file = "./testfile";
      TestUtils::WriteRandomDatatoFile(local_file, 1024);
      PutObjectByFileReq req(m_bucket_name, "302", local_file);

      PutObjectByFileResp resp;
      CosResult result = m_client_without_retry->PutObject(req, &resp);
      EXPECT_FALSE(result.IsSucc());
      EXPECT_EQ(302, result.GetHttpStatus());
  }
  {
    std::istringstream iss("put object test");
    PutObjectByStreamReq req(m_bucket_name, "307", iss);

    PutObjectByStreamResp resp;
    CosResult result = m_client_without_retry->PutObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(307, result.GetHttpStatus());
  }
}

TEST_F(ObjectOpRetryTest, PutObjectWithRetryChangeDomainFor3xxTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "301", local_file);

    PutObjectByFileResp resp;
    CosResult result = m_client_with_retry->PutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    std::string local_file = "./testfile";
    TestUtils::WriteRandomDatatoFile(local_file, 1024);
    PutObjectByFileReq req(m_bucket_name, "302", local_file);

    PutObjectByFileResp resp;
    CosResult result = m_client_with_retry->PutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    std::istringstream iss("put object test");
    PutObjectByStreamReq req(m_bucket_name, "307", iss);

    PutObjectByStreamResp resp;
    CosResult result = m_client_with_retry->PutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, HeadObjectWithoutRetryFor5xxTest) {
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "500");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_without_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(500, result.GetHttpStatus());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "503");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_without_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(503, result.GetHttpStatus());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "504");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_without_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(504, result.GetHttpStatus());
  }
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryFor5xxTest) {
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "500");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "503");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
    std::cout << "result.GetErrorMsg() = " << result.GetErrorMsg() << std::endl;
    std::cout << "result.GetHttpStatus() = " << result.GetHttpStatus() << std::endl;
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "504");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryChangeDomainFor5xxTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "500");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "503");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
    std::cout << "result.GetErrorMsg() = " << result.GetErrorMsg() << std::endl;
    std::cout << "result.GetHttpStatus() = " << result.GetHttpStatus() << std::endl;
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "504");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, HeadObjectWithoutRetryFor4xxTest) {
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "400");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_without_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(400, result.GetHttpStatus());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "403");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_without_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(403, result.GetHttpStatus());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "404");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_without_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(404, result.GetHttpStatus());
  }
}

TEST_F(ObjectOpRetryTest, HeadObjectWithoutRetryFor3xxTest) {
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "301");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_without_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(301, result.GetHttpStatus());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "302");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_without_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(302, result.GetHttpStatus());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "307");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_without_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(307, result.GetHttpStatus());
  }
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryChangeDomainFor3xxTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "301");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "302");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "307");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, GetObjectFor2xxrTest) {
    {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "200r", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "204r", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "206r", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpRetryTest, GetObjectWithChangeDomainFor2xxrTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "200r", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "204r", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "206r", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, GetObjectWithRetryChangeDomainFor3xxrTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "301r", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::GetObjectByFileReq req(m_bucket_name, "302r", "./test.txt");
    qcloud_cos::GetObjectByFileResp resp;

    qcloud_cos::CosResult result = m_client_with_retry->GetObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryFor4xxrTest) {
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "400r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(400, result.GetHttpStatus());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "403r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(403, result.GetHttpStatus());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "404r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(404, result.GetHttpStatus());
  }
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryChangeDomainFor4xxrTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "400r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(400, result.GetHttpStatus());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "403r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(403, result.GetHttpStatus());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "404r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_FALSE(result.IsSucc());
    EXPECT_EQ(404, result.GetHttpStatus());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryFor5xxrTest) {
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "500r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "503r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
    std::cout << "result.GetErrorMsg() = " << result.GetErrorMsg() << std::endl;
    std::cout << "result.GetHttpStatus() = " << result.GetHttpStatus() << std::endl;
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "504r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryChangeDomainFor5xxrTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "500r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "503r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
    std::cout << "result.GetErrorMsg() = " << result.GetErrorMsg() << std::endl;
    std::cout << "result.GetHttpStatus() = " << result.GetHttpStatus() << std::endl;
  }
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "504r");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryForTimeoutTest) {
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "timeout");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryForShutdownTest) {
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "shutdown");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryChangeDomainForTimeoutTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "timeout");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}

TEST_F(ObjectOpRetryTest, HeadObjectWithRetryChangeDomainForShutdownTest) {
  CosSysConfig::SetRetryChangeDomain(true);
  {
    qcloud_cos::HeadObjectReq req(m_bucket_name, "shutdown");
    qcloud_cos::HeadObjectResp resp;
    qcloud_cos::CosResult result = m_client_with_retry->HeadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());
  }
  CosSysConfig::SetRetryChangeDomain(false);
}
}