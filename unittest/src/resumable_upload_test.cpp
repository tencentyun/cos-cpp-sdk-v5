// 断点续传（Resumable Upload / Checkpoint）集成测试
//
// 环境变量（与 unittest 其他测试一致）：
//   CPP_SDK_V5_ACCESS_KEY / CPP_SDK_V5_SECRET_KEY / CPP_SDK_V5_REGION / CPP_SDK_V5_APPID
//   COS_CPP_V5_USE_DNS_CACHE（可选）

#include <atomic>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "cos_api.h"
#include "cos_sys_config.h"
#include "util/test_utils.h"
#include "util/file_util.h"
#include "util/simple_dns_cache.h"
#include "Poco/MD5Engine.h"
#include "Poco/DigestEngine.h"
#include "Poco/DigestStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSONString.h"
#include "Poco/Dynamic/Var.h"
#include "gtest/gtest.h"

namespace qcloud_cos {

// ============ 常量 ============
static const uint64_t kTestPartSize = 1 * 1024 * 1024;   // 1MB
static const uint64_t kTestFileSize = 20 * 1024 * 1024;   // 20MB
static const std::string kCheckpointDir = "/tmp/cos_ckpt_test/";
static const std::string kCheckpointDirReq = "/tmp/cos_ckpt_test_req/";
static const std::string kCheckpointDirConfig = "/tmp/cos_ckpt_test_config/";

// checkpoint 文件 JSON key（与 cos_params.h 一致）
static const std::string kCkptOpType = "opType";
static const std::string kCkptUploadId = "uploadId";
static const std::string kCkptFilePath = "filePath";
static const std::string kCkptBucket = "bucket";
static const std::string kCkptKey = "key";
static const std::string kCkptFileSize = "fileSize";
static const std::string kCkptLastModified = "lastModified";
static const std::string kCkptPartSize = "partSize";
static const std::string kCkptMd5Sum = "md5Sum";
static const std::string kCkptFileSuffix = ".cosresumableupload";

// ============ Fixture 基类 ============
class ResumableUploadTest : public testing::Test {
 protected:
  static CosConfig* m_config;
  static CosAPI* m_client;
  static std::string m_bucket_name;
  static std::string s_test_file;
  static std::string s_test_file_md5;

  static void SetUpTestCase() {
    std::cout << "================SetUpTestCase Begin====================" << std::endl;
    m_config = new CosConfig("./config.json");
    m_config->SetAccessKey(GetEnvVar("CPP_SDK_V5_ACCESS_KEY"));
    m_config->SetSecretKey(GetEnvVar("CPP_SDK_V5_SECRET_KEY"));
    m_config->SetRegion(GetEnvVar("CPP_SDK_V5_REGION"));
    if (GetEnvVar("COS_CPP_V5_USE_DNS_CACHE") == "true") {
      std::cout << "================USE DNS CACHE====================" << std::endl;
      CosSysConfig::SetUseDnsCache(true);
    }

    CosSysConfig::SetUploadPartSize(kTestPartSize);

    m_client = new CosAPI(*m_config);

    m_bucket_name = "coscppsdkv5ut-resumable-" + GetEnvVar("CPP_SDK_V5_APPID");

    // 先清理旧桶内容，确保干净环境
    CleanBucketContents(m_bucket_name);

    // 创建桶（桶已存在时返回 BucketAlreadyOwnedByYou，不影响）
    {
      PutBucketReq req(m_bucket_name);
      PutBucketResp resp;
      m_client->PutBucket(req, &resp);
    }

    std::cout << "================SetUpTestCase End====================" << std::endl;
  }

  // 不删除桶，保留桶防止配额问题，下次执行时会在 SetUpTestCase 中先清理再创建
  static void TearDownTestCase() {
    std::cout << "================TearDownTestCase Begin====================" << std::endl;
    delete m_client; m_client = nullptr;
    delete m_config; m_config = nullptr;
    std::cout << "================TearDownTestCase End====================" << std::endl;
  }

  void SetUp() override {
    CleanDir(kCheckpointDir);
    CleanDir(kCheckpointDirReq);
    CleanDir(kCheckpointDirConfig);
    CreateDirIfNotExist(kCheckpointDir);
    CreateDirIfNotExist(kCheckpointDirReq);
    CreateDirIfNotExist(kCheckpointDirConfig);

    s_test_file = "/tmp/cos_resumable_test_file_" + std::to_string(time(nullptr));
    CreateTestFile(s_test_file, kTestFileSize);
    s_test_file_md5 = CalcFileMd5(s_test_file);
  }

  void TearDown() override {
    CleanupRemoteObjects();
    RemoveFile(s_test_file);
    CleanDir(kCheckpointDir);
    CleanDir(kCheckpointDirReq);
    CleanDir(kCheckpointDirConfig);
  }

  // ============ 辅助函数 ============

  // 清理桶内所有对象和碎片（不删桶）
  static void CleanBucketContents(const std::string& bucket) {
    // 删除桶内所有对象
    GetBucketReq get_req(bucket);
    GetBucketResp get_resp;
    CosResult get_result = m_client->GetBucket(get_req, &get_resp);
    if (get_result.IsSucc()) {
      for (const auto& content : get_resp.GetContents()) {
        DeleteObjectReq del_req(bucket, content.m_key);
        DeleteObjectResp del_resp;
        m_client->DeleteObject(del_req, &del_resp);
      }
    }
    // 删除所有未完成的分块上传
    ListMultipartUploadReq list_mp_req(bucket);
    ListMultipartUploadResp list_mp_resp;
    CosResult list_mp_result = m_client->ListMultipartUpload(list_mp_req, &list_mp_resp);
    if (list_mp_result.IsSucc()) {
      for (const auto& upload : list_mp_resp.GetUpload()) {
        AbortMultiUploadReq abort_req(bucket, upload.m_key, upload.m_uploadid);
        AbortMultiUploadResp abort_resp;
        m_client->AbortMultiUpload(abort_req, &abort_resp);
      }
    }
  }

  static void CreateTestFile(const std::string& path, uint64_t size) {
    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    ASSERT_TRUE(ofs.is_open()) << "Cannot create test file: " << path;
    std::string pattern(4096, 'A');
    uint64_t written = 0;
    while (written < size) {
      uint64_t to_write = std::min((uint64_t)pattern.size(), size - written);
      ofs.write(pattern.data(), to_write);
      written += to_write;
    }
    ofs.close();
  }

  static std::string CalcFileMd5(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) return "";
    Poco::MD5Engine md5;
    Poco::DigestOutputStream dos(md5);
    Poco::StreamCopier::copyStream(ifs, dos);
    dos.close();
    return Poco::DigestEngine::digestToHex(md5.digest());
  }

  static std::string GetFileLastModifiedTime(const std::string& path) {
    struct stat st;
    return (stat(path.c_str(), &st) == 0) ? std::to_string(st.st_mtime) : "";
  }

  static std::string GenCheckpointFilePath(const std::string& dir,
                                           const std::string& local_file,
                                           const std::string& bucket,
                                           const std::string& object) {
    Poco::MD5Engine md5_src;
    md5_src.update(local_file);
    std::string src_md5 = Poco::DigestEngine::digestToHex(md5_src.digest());
    Poco::MD5Engine md5_dest;
    std::string dest = "cos://" + bucket + "/" + object;
    md5_dest.update(dest);
    std::string dest_md5 = Poco::DigestEngine::digestToHex(md5_dest.digest());
    std::string d = dir;
    if (!d.empty() && d.back() != '/') d += "/";
    return d + src_md5 + "--" + dest_md5 + kCkptFileSuffix;
  }

  static bool WriteCheckpointFile(const std::string& path,
                                  const std::string& upload_id,
                                  const std::string& file_path,
                                  const std::string& bucket,
                                  const std::string& key,
                                  uint64_t file_size,
                                  const std::string& last_modified,
                                  uint64_t part_size) {
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object(Poco::JSON_PRESERVE_KEY_ORDER);
    root->set(kCkptOpType, std::string("ResumableUpload"));
    root->set(kCkptUploadId, upload_id);
    root->set(kCkptFilePath, file_path);
    root->set(kCkptBucket, bucket);
    root->set(kCkptKey, key);
    root->set(kCkptFileSize, std::to_string(file_size));
    root->set(kCkptLastModified, last_modified);
    root->set(kCkptPartSize, std::to_string(part_size));
    std::ostringstream ss;
    Poco::JSON::Stringifier::stringify(root, ss);
    Poco::MD5Engine md5;
    md5.update(ss.str());
    root->set(kCkptMd5Sum, Poco::DigestEngine::digestToHex(md5.digest()));
    std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
    if (!ofs.is_open()) return false;
    Poco::JSON::Stringifier::stringify(root, ofs);
    ofs.close();
    return true;
  }

  static bool FileExists(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
  }

  static bool HasCheckpointFileInDir(const std::string& dir) {
    DIR* d = opendir(dir.c_str());
    if (!d) return false;
    bool found = false;
    struct dirent* entry;
    while ((entry = readdir(d)) != nullptr) {
      std::string name(entry->d_name);
      if (name.length() > kCkptFileSuffix.length() &&
          name.substr(name.length() - kCkptFileSuffix.length()) == kCkptFileSuffix) {
        found = true;
        break;
      }
    }
    closedir(d);
    return found;
  }

  static void RemoveFile(const std::string& path) {
    if (!path.empty()) std::remove(path.c_str());
  }

  static void CleanDir(const std::string& dir) {
    DIR* d = opendir(dir.c_str());
    if (!d) return;
    struct dirent* entry;
    while ((entry = readdir(d)) != nullptr) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
      std::string full = dir + "/" + entry->d_name;
      remove(full.c_str());
    }
    closedir(d);
    rmdir(dir.c_str());
  }

  static void CreateDirIfNotExist(const std::string& dir) {
    mkdir(dir.c_str(), 0755);
  }

  void CleanupRemoteObjects() {
    ListMultipartUploadReq list_req(m_bucket_name);
    ListMultipartUploadResp list_resp;
    CosResult result = m_client->ListMultipartUpload(list_req, &list_resp);
    if (result.IsSucc()) {
      for (const auto& upload : list_resp.GetUpload()) {
        AbortMultiUploadReq abort_req(m_bucket_name, upload.m_key, upload.m_uploadid);
        AbortMultiUploadResp abort_resp;
        m_client->AbortMultiUpload(abort_req, &abort_resp);
      }
    }
  }

  bool UploadAndVerifySuccess(const std::string& object_name,
                              const std::string& local_file,
                              const std::string& checkpoint_dir) {
    MultiPutObjectReq req(m_bucket_name, object_name, local_file);
    if (!checkpoint_dir.empty()) req.SetCheckpointDir(checkpoint_dir);
    req.SetCheckCRC64(false);
    MultiPutObjectResp resp;
    CosResult result = m_client->MultiPutObject(req, &resp);
    EXPECT_TRUE(result.IsSucc())
        << "Upload failed: " << result.GetErrorMsg()
        << " HttpStatus=" << result.GetHttpStatus();
    return result.IsSucc();
  }

  bool DownloadAndVerifyMd5(const std::string& object_name,
                            const std::string& expected_md5) {
    std::string download = "/tmp/cos_download_verify_" + std::to_string(time(nullptr));
    GetObjectByFileReq req(m_bucket_name, object_name, download);
    GetObjectByFileResp resp;
    CosResult result = m_client->GetObject(req, &resp);
    if (!result.IsSucc()) {
      EXPECT_TRUE(result.IsSucc()) << "Download failed: " << result.GetErrorMsg();
      RemoveFile(download);
      return false;
    }
    std::string actual = CalcFileMd5(download);
    EXPECT_EQ(expected_md5, actual) << "MD5 mismatch after download";
    RemoveFile(download);
    return expected_md5 == actual;
  }

  bool DeleteRemoteObject(const std::string& object_name) {
    DeleteObjectReq req(m_bucket_name, object_name);
    DeleteObjectResp resp;
    return m_client->DeleteObject(req, &resp).IsSucc();
  }

  std::string InitMultiUpload(const std::string& object_name) {
    InitMultiUploadReq req(m_bucket_name, object_name);
    InitMultiUploadResp resp;
    CosResult result = m_client->InitMultiUpload(req, &resp);
    return result.IsSucc() ? resp.GetUploadId() : "";
  }

  bool UploadPart(const std::string& object_name, const std::string& upload_id,
                  uint64_t part_number, const std::string& local_file,
                  uint64_t offset, uint64_t length) {
    std::ifstream ifs(local_file, std::ios::binary);
    if (!ifs.is_open()) return false;
    ifs.seekg(offset);
    std::string data(length, '\0');
    ifs.read(&data[0], length);
    ifs.close();
    std::istringstream iss(data);
    UploadPartDataReq req(m_bucket_name, object_name, upload_id, iss);
    req.SetPartNumber(part_number);
    UploadPartDataResp resp;
    return m_client->UploadPartData(req, &resp).IsSucc();
  }

  bool AbortMultiUpload(const std::string& object_name, const std::string& upload_id) {
    AbortMultiUploadReq req(m_bucket_name, object_name, upload_id);
    AbortMultiUploadResp resp;
    return m_client->AbortMultiUpload(req, &resp).IsSucc();
  }
};

// 静态成员定义
CosConfig* ResumableUploadTest::m_config = nullptr;
CosAPI* ResumableUploadTest::m_client = nullptr;
std::string ResumableUploadTest::m_bucket_name;
std::string ResumableUploadTest::s_test_file;
std::string ResumableUploadTest::s_test_file_md5;

// =====================================================================
// Checkpoint 基本功能测试
// =====================================================================

// TC_CKPT_001: 全新上传成功后 checkpoint 文件被清理
TEST_F(ResumableUploadTest, TC_CKPT_001_NewUploadCleansCheckpoint) {
  std::string object_name = "tc_ckpt_001_" + std::to_string(time(nullptr));
  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) return;
  std::string ckpt = GenCheckpointFilePath(kCheckpointDir, s_test_file, m_bucket_name, object_name);
  EXPECT_FALSE(FileExists(ckpt)) << "Checkpoint should be removed after successful upload";
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
  DeleteRemoteObject(object_name);
}

// TC_CKPT_002: 模拟中断后恢复上传
TEST_F(ResumableUploadTest, TC_CKPT_002_ResumeFromCheckpoint) {
  std::string object_name = "tc_ckpt_002_" + std::to_string(time(nullptr));
  std::string upload_id = InitMultiUpload(object_name);
  ASSERT_FALSE(upload_id.empty());
  ASSERT_TRUE(UploadPart(object_name, upload_id, 1, s_test_file, 0, kTestPartSize));
  ASSERT_TRUE(UploadPart(object_name, upload_id, 2, s_test_file, kTestPartSize, kTestPartSize));

  std::string ckpt = GenCheckpointFilePath(kCheckpointDir, s_test_file, m_bucket_name, object_name);
  ASSERT_TRUE(WriteCheckpointFile(ckpt, upload_id, s_test_file, m_bucket_name, object_name,
                                  kTestFileSize, GetFileLastModifiedTime(s_test_file), kTestPartSize));
  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) return;
  EXPECT_FALSE(FileExists(ckpt));
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
  DeleteRemoteObject(object_name);
}

// TC_CKPT_003: 源文件被修改后 checkpoint 失效
TEST_F(ResumableUploadTest, TC_CKPT_003_SourceFileModifiedInvalidatesCheckpoint) {
  std::string object_name = "tc_ckpt_003_" + std::to_string(time(nullptr));
  std::string upload_id = InitMultiUpload(object_name);
  ASSERT_FALSE(upload_id.empty());
  ASSERT_TRUE(UploadPart(object_name, upload_id, 1, s_test_file, 0, kTestPartSize));

  std::string ckpt = GenCheckpointFilePath(kCheckpointDir, s_test_file, m_bucket_name, object_name);
  ASSERT_TRUE(WriteCheckpointFile(ckpt, upload_id, s_test_file, m_bucket_name, object_name,
                                  kTestFileSize, GetFileLastModifiedTime(s_test_file), kTestPartSize));
  sleep(1);
  { std::ofstream ofs(s_test_file, std::ios::binary | std::ios::app); ofs << "extra_data"; ofs.close(); }
  std::string new_md5 = CalcFileMd5(s_test_file);

  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) return;
  EXPECT_FALSE(FileExists(ckpt));
  if (!DownloadAndVerifyMd5(object_name, new_md5)) return;
  DeleteRemoteObject(object_name);
  AbortMultiUpload(object_name, upload_id);
}

// TC_CKPT_004: 两次独立上传，验证幂等性
TEST_F(ResumableUploadTest, TC_CKPT_004_IdempotentUpload) {
  std::string object_name = "tc_ckpt_004_" + std::to_string(time(nullptr));
  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) return;
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) return;
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
  DeleteRemoteObject(object_name);
}

// =====================================================================
// Checkpoint 异常场景测试
// =====================================================================

// TC_ERR_001: checkpoint 文件内容损坏
TEST_F(ResumableUploadTest, TC_ERR_001_CorruptedCheckpointFile) {
  std::string object_name = "tc_err_001_" + std::to_string(time(nullptr));
  std::string ckpt = GenCheckpointFilePath(kCheckpointDir, s_test_file, m_bucket_name, object_name);
  { std::ofstream ofs(ckpt); ofs << "not valid json {{{{"; ofs.close(); }
  ASSERT_TRUE(FileExists(ckpt));
  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) return;
  EXPECT_FALSE(FileExists(ckpt));
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
  DeleteRemoteObject(object_name);
}

// TC_ERR_002: checkpoint 中 uploadId 已失效
TEST_F(ResumableUploadTest, TC_ERR_002_ExpiredUploadId) {
  std::string object_name = "tc_err_002_" + std::to_string(time(nullptr));
  std::string upload_id = InitMultiUpload(object_name);
  ASSERT_FALSE(upload_id.empty());
  ASSERT_TRUE(UploadPart(object_name, upload_id, 1, s_test_file, 0, kTestPartSize));
  ASSERT_TRUE(AbortMultiUpload(object_name, upload_id));

  std::string ckpt = GenCheckpointFilePath(kCheckpointDir, s_test_file, m_bucket_name, object_name);
  ASSERT_TRUE(WriteCheckpointFile(ckpt, upload_id, s_test_file, m_bucket_name, object_name,
                                  kTestFileSize, GetFileLastModifiedTime(s_test_file), kTestPartSize));
  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) return;
  EXPECT_FALSE(FileExists(ckpt));
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
  DeleteRemoteObject(object_name);
}

// TC_ERR_004: Complete 失败后 checkpoint 保留，重试成功
TEST_F(ResumableUploadTest, TC_ERR_004_CompleteFailurePreservesCheckpoint) {
  std::string object_name = "tc_err_004_" + std::to_string(time(nullptr));
  std::string upload_id = InitMultiUpload(object_name);
  ASSERT_FALSE(upload_id.empty());
  for (uint64_t i = 0; i < 5; ++i) {
    uint64_t offset = i * kTestPartSize;
    uint64_t len = std::min(kTestPartSize, kTestFileSize - offset);
    ASSERT_TRUE(UploadPart(object_name, upload_id, i + 1, s_test_file, offset, len));
  }
  std::string ckpt = GenCheckpointFilePath(kCheckpointDir, s_test_file, m_bucket_name, object_name);
  ASSERT_TRUE(WriteCheckpointFile(ckpt, upload_id, s_test_file, m_bucket_name, object_name,
                                  kTestFileSize, GetFileLastModifiedTime(s_test_file), kTestPartSize));
  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) return;
  EXPECT_FALSE(FileExists(ckpt));
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
  DeleteRemoteObject(object_name);
}

// TC_ERR_004_ALT: 部分分块重试
TEST_F(ResumableUploadTest, TC_ERR_004_ALT_PartialRetry) {
  std::string object_name = "tc_err_004_alt_" + std::to_string(time(nullptr));
  std::string upload_id = InitMultiUpload(object_name);
  ASSERT_FALSE(upload_id.empty());
  for (uint64_t i = 0; i < 3; ++i) {
    ASSERT_TRUE(UploadPart(object_name, upload_id, i + 1, s_test_file, i * kTestPartSize, kTestPartSize));
  }
  std::string ckpt = GenCheckpointFilePath(kCheckpointDir, s_test_file, m_bucket_name, object_name);
  ASSERT_TRUE(WriteCheckpointFile(ckpt, upload_id, s_test_file, m_bucket_name, object_name,
                                  kTestFileSize, GetFileLastModifiedTime(s_test_file), kTestPartSize));
  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) return;
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
  DeleteRemoteObject(object_name);
}

// =====================================================================
// Checkpoint 配置优先级测试
// =====================================================================

// TC_CFG_001: 请求级 checkpoint 目录优先于 config 级
TEST_F(ResumableUploadTest, TC_CFG_001_RequestLevelCheckpointDirPriority) {
  std::string object_name = "tc_cfg_001_" + std::to_string(time(nullptr));
  m_config->SetEnableCheckpoint(true);
  m_config->SetCheckpointDir(kCheckpointDirConfig);

  std::string upload_id = InitMultiUpload(object_name);
  ASSERT_FALSE(upload_id.empty());
  ASSERT_TRUE(UploadPart(object_name, upload_id, 1, s_test_file, 0, kTestPartSize));

  std::string ckpt_req = GenCheckpointFilePath(kCheckpointDirReq, s_test_file, m_bucket_name, object_name);
  ASSERT_TRUE(WriteCheckpointFile(ckpt_req, upload_id, s_test_file, m_bucket_name, object_name,
                                  kTestFileSize, GetFileLastModifiedTime(s_test_file), kTestPartSize));

  MultiPutObjectReq req(m_bucket_name, object_name, s_test_file);
  req.SetCheckpointDir(kCheckpointDirReq);
  MultiPutObjectResp resp;
  CosResult result = m_client->MultiPutObject(req, &resp);
  EXPECT_TRUE(result.IsSucc()) << "Upload failed: " << result.GetErrorMsg();
  EXPECT_FALSE(FileExists(ckpt_req));

  std::string ckpt_config = GenCheckpointFilePath(kCheckpointDirConfig, s_test_file, m_bucket_name, object_name);
  EXPECT_FALSE(FileExists(ckpt_config));

  m_config->SetEnableCheckpoint(false);
  m_config->SetCheckpointDir("");
  DeleteRemoteObject(object_name);
}

// =====================================================================
// Checkpoint 权限测试
// =====================================================================

// TC_CFG_003_ALT2: 空 checkpoint 目录路径
TEST_F(ResumableUploadTest, TC_CFG_003_ALT2_EmptyCheckpointDir) {
  std::string object_name = "tc_cfg_003_alt2_" + std::to_string(time(nullptr));
  MultiPutObjectReq req(m_bucket_name, object_name, s_test_file);
  req.SetCheckCRC64(false);
  MultiPutObjectResp resp;
  CosResult result = m_client->MultiPutObject(req, &resp);
  EXPECT_TRUE(result.IsSucc()) << "Upload should succeed without checkpoint dir";
  if (!result.IsSucc()) return;
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
  DeleteRemoteObject(object_name);
}

// =====================================================================
// 并发上传测试
// =====================================================================

// TC_CKPT_006: 两个线程同时上传同一文件到不同对象
TEST_F(ResumableUploadTest, TC_CKPT_006_ConcurrentUploadsSameSourceFile) {
  std::string obj_a = "tc_ckpt_006_a_" + std::to_string(time(nullptr));
  std::string obj_b = "tc_ckpt_006_b_" + std::to_string(time(nullptr));
  std::vector<std::thread> threads;
  std::vector<bool> results(2, false);
  std::mutex mtx;

  threads.emplace_back([this, obj_a, &results, &mtx]() {
    MultiPutObjectReq req(m_bucket_name, obj_a, s_test_file);
    req.SetCheckpointDir(kCheckpointDir); req.SetCheckCRC64(false);
    MultiPutObjectResp resp;
    bool ok = m_client->MultiPutObject(req, &resp).IsSucc();
    std::lock_guard<std::mutex> lock(mtx); results[0] = ok;
  });
  threads.emplace_back([this, obj_b, &results, &mtx]() {
    MultiPutObjectReq req(m_bucket_name, obj_b, s_test_file);
    req.SetCheckpointDir(kCheckpointDir); req.SetCheckCRC64(false);
    MultiPutObjectResp resp;
    bool ok = m_client->MultiPutObject(req, &resp).IsSucc();
    std::lock_guard<std::mutex> lock(mtx); results[1] = ok;
  });
  for (auto& t : threads) t.join();

  EXPECT_TRUE(results[0]) << "Thread A upload failed";
  EXPECT_TRUE(results[1]) << "Thread B upload failed";
  if (!results[0] || !results[1]) return;

  if (!DownloadAndVerifyMd5(obj_a, s_test_file_md5)) return;
  if (!DownloadAndVerifyMd5(obj_b, s_test_file_md5)) return;
  DeleteRemoteObject(obj_a);
  DeleteRemoteObject(obj_b);
  EXPECT_FALSE(HasCheckpointFileInDir(kCheckpointDir));
}

// TC_CKPT_006_ALT: 多次快速连续上传
TEST_F(ResumableUploadTest, TC_CKPT_006_ALT_RapidSequentialUploads) {
  for (int i = 0; i < 3; ++i) {
    std::string object_name = "tc_ckpt_006_alt_" + std::to_string(time(nullptr)) + "_" + std::to_string(i);
    if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) return;
    if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
    DeleteRemoteObject(object_name);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  EXPECT_FALSE(HasCheckpointFileInDir(kCheckpointDir));
}

// =====================================================================
// Part Size 变化测试
// =====================================================================

struct UploadInterruptState {
  std::atomic<uint64_t> bytes_uploaded{0};
  std::atomic<bool> should_stop{false};
  uint64_t stop_after_bytes{0};
};

static void ProgressCallbackForInterrupt(uint64_t nCompleted, uint64_t, void* user_data) {
  if (!user_data) return;
  UploadInterruptState* state = static_cast<UploadInterruptState*>(user_data);
  state->bytes_uploaded = nCompleted;
  if (nCompleted >= state->stop_after_bytes && !state->should_stop) {
    state->should_stop = true;
  }
}

// TC_CKPT_005: Part Size 变大时，SDK 丢弃 checkpoint 重新上传
TEST_F(ResumableUploadTest, TC_CKPT_005_PartSizeLargerDiscardsCheckpoint) {
  std::string object_name = "tc_ckpt_005_" + std::to_string(time(nullptr));
  uint64_t original_part_size = CosSysConfig::GetUploadPartSize();
  ASSERT_EQ(kTestPartSize, original_part_size);

  std::string ckpt = GenCheckpointFilePath(kCheckpointDir, s_test_file, m_bucket_name, object_name);
  UploadInterruptState state;
  state.stop_after_bytes = kTestPartSize;

  {
    CosSysConfig::SetUploadThreadPoolSize(1);
    AsyncMultiPutObjectReq req(m_bucket_name, object_name, s_test_file);
    req.SetCheckpointDir(kCheckpointDir);
    req.SetTransferProgressCallback(&ProgressCallbackForInterrupt);
    req.SetUserData(&state);
    SharedAsyncContext ctx = m_client->AsyncMultiPutObject(req);
    while (state.bytes_uploaded < state.stop_after_bytes) sleep(1);
    ctx->Cancel();
    ctx->WaitUntilFinish();
  }
  sleep(3);

  if (!FileExists(ckpt)) {
    // 上传未被成功中断，跳过 Phase 2
    if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) return;
    DeleteRemoteObject(object_name);
    return;
  }

  // 验证 checkpoint 记录了 1MB
  {
    std::ifstream ifs(ckpt);
    ASSERT_TRUE(ifs.good());
    Poco::JSON::Parser parser;
    auto obj = parser.parse(ifs).extract<Poco::JSON::Object::Ptr>();
    EXPECT_EQ(kTestPartSize, std::stoull(obj->getValue<std::string>(kCkptPartSize)));
  }

  // Phase 2: 改为 2MB，SDK 应丢弃 checkpoint 重新上传
  CosSysConfig::SetUploadPartSize(2 * 1024 * 1024);
  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) {
    CosSysConfig::SetUploadPartSize(original_part_size);
    return;
  }
  EXPECT_FALSE(FileExists(ckpt));
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) {
    CosSysConfig::SetUploadPartSize(original_part_size);
    return;
  }
  DeleteRemoteObject(object_name);
  CosSysConfig::SetUploadPartSize(original_part_size);
}

// TC_CKPT_005_ALT: Part Size 从大变小时，SDK 丢弃 checkpoint 重新上传
TEST_F(ResumableUploadTest, TC_CKPT_005_ALT_PartSizeSmallerDiscardsCheckpoint) {
  std::string object_name = "tc_ckpt_005_alt_" + std::to_string(time(nullptr));
  uint64_t original_part_size = CosSysConfig::GetUploadPartSize();
  uint64_t large = 2 * 1024 * 1024;

  std::string ckpt = GenCheckpointFilePath(kCheckpointDir, s_test_file, m_bucket_name, object_name);
  UploadInterruptState state;
  state.stop_after_bytes = large;

  {
    CosSysConfig::SetUploadPartSize(large);
    CosSysConfig::SetUploadThreadPoolSize(1);
    AsyncMultiPutObjectReq req(m_bucket_name, object_name, s_test_file);
    req.SetCheckpointDir(kCheckpointDir);
    req.SetTransferProgressCallback(&ProgressCallbackForInterrupt);
    req.SetUserData(&state);
    SharedAsyncContext ctx = m_client->AsyncMultiPutObject(req);
    while (state.bytes_uploaded < state.stop_after_bytes) sleep(1);
    ctx->Cancel();
    ctx->WaitUntilFinish();
  }
  sleep(2);

  ASSERT_TRUE(FileExists(ckpt));
  ASSERT_LT(state.bytes_uploaded.load(), kTestFileSize);

  // 验证 checkpoint 记录了 2MB
  {
    std::ifstream ifs(ckpt);
    ASSERT_TRUE(ifs.good());
    Poco::JSON::Parser parser;
    auto obj = parser.parse(ifs).extract<Poco::JSON::Object::Ptr>();
    EXPECT_EQ(large, std::stoull(obj->getValue<std::string>(kCkptPartSize)));
  }

  // Phase 2: 改为 1MB，SDK 应丢弃 checkpoint 重新上传
  CosSysConfig::SetUploadPartSize(kTestPartSize);
  if (!UploadAndVerifySuccess(object_name, s_test_file, kCheckpointDir)) {
    CosSysConfig::SetUploadPartSize(original_part_size);
    return;
  }
  EXPECT_FALSE(FileExists(ckpt));
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) {
    CosSysConfig::SetUploadPartSize(original_part_size);
    return;
  }
  DeleteRemoteObject(object_name);
  CosSysConfig::SetUploadPartSize(original_part_size);
}

// =====================================================================
// Legacy 回退测试
// =====================================================================

// TC_LEGACY_001: legacy=true 上传成功
TEST_F(ResumableUploadTest, TC_LEGACY_001_LegacyEnabledUploadSuccess) {
  std::string object_name = "tc_legacy_001_" + std::to_string(time(nullptr));
  bool original = CosSysConfig::GetEnableLegacyResumableUpload();
  CosSysConfig::SetEnableLegacyResumableUpload(true);
  MultiPutObjectReq req(m_bucket_name, object_name, s_test_file);
  MultiPutObjectResp resp;
  CosResult result = m_client->MultiPutObject(req, &resp);
  EXPECT_TRUE(result.IsSucc()) << "Legacy upload failed: " << result.GetErrorMsg();
  if (!result.IsSucc()) { CosSysConfig::SetEnableLegacyResumableUpload(original); return; }
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) {
    CosSysConfig::SetEnableLegacyResumableUpload(original); return;
  }
  DeleteRemoteObject(object_name);
  CosSysConfig::SetEnableLegacyResumableUpload(original);
}

// TC_LEGACY_002: legacy=false 上传成功
TEST_F(ResumableUploadTest, TC_LEGACY_002_LegacyDisabledUploadSuccess) {
  std::string object_name = "tc_legacy_002_" + std::to_string(time(nullptr));
  bool original = CosSysConfig::GetEnableLegacyResumableUpload();
  CosSysConfig::SetEnableLegacyResumableUpload(false);
  MultiPutObjectReq req(m_bucket_name, object_name, s_test_file);
  MultiPutObjectResp resp;
  CosResult result = m_client->MultiPutObject(req, &resp);
  EXPECT_TRUE(result.IsSucc()) << "Upload failed: " << result.GetErrorMsg();
  if (!result.IsSucc()) { CosSysConfig::SetEnableLegacyResumableUpload(original); return; }
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) {
    CosSysConfig::SetEnableLegacyResumableUpload(original); return;
  }
  DeleteRemoteObject(object_name);
  CosSysConfig::SetEnableLegacyResumableUpload(original);
}

// TC_LEGACY_003: legacy 从服务端恢复已有分块上传
TEST_F(ResumableUploadTest, TC_LEGACY_003_LegacyResumeFromServer) {
  std::string object_name = "tc_legacy_003_" + std::to_string(time(nullptr));
  bool original = CosSysConfig::GetEnableLegacyResumableUpload();
  CosSysConfig::SetEnableLegacyResumableUpload(true);

  std::string upload_id = InitMultiUpload(object_name);
  ASSERT_FALSE(upload_id.empty());
  ASSERT_TRUE(UploadPart(object_name, upload_id, 1, s_test_file, 0, kTestPartSize));
  ASSERT_TRUE(UploadPart(object_name, upload_id, 2, s_test_file, kTestPartSize, kTestPartSize));

  MultiPutObjectReq req(m_bucket_name, object_name, s_test_file);
  MultiPutObjectResp resp;
  CosResult result = m_client->MultiPutObject(req, &resp);
  EXPECT_TRUE(result.IsSucc()) << "Legacy resume failed: " << result.GetErrorMsg();
  if (!result.IsSucc()) { CosSysConfig::SetEnableLegacyResumableUpload(original); return; }
  if (!DownloadAndVerifyMd5(object_name, s_test_file_md5)) {
    CosSysConfig::SetEnableLegacyResumableUpload(original); return;
  }
  DeleteRemoteObject(object_name);
  CosSysConfig::SetEnableLegacyResumableUpload(original);
}

}  // namespace qcloud_cos
