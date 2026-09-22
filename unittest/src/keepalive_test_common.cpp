// Copyright (c) 2026, Tencent Inc.
// All rights reserved.
//
// Description: 长连接（KeepAlive + 连接池）专项单测公共 Fixture 实现。

#include "keepalive_test_common.h"

#include <cstdlib>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace qcloud_cos {

// 静态成员定义（只在此文件中定义一次）
std::string KeepAliveOpTest::m_bucket_name = "";
std::string KeepAliveOpTest::m_bucket_name_cfg = "";
std::string KeepAliveOpTest::m_bucket_name_tmp = "";
CosConfig* KeepAliveOpTest::m_config = NULL;
CosAPI* KeepAliveOpTest::m_client = NULL;
unsigned KeepAliveOpTest::m_saved_pool_size = 0;
uint64_t KeepAliveOpTest::m_saved_max_idle_ms = 0;
uint64_t KeepAliveOpTest::m_saved_max_age_ms = 0;

void KeepAliveOpTest::SetUpTestCase() {
  std::cout << "========KeepAliveOpTest SetUpTestCase Begin========"
            << std::endl;
  m_config = new CosConfig("./config.json");
  m_config->SetIsUseIntranetAddr(false);
  m_config->SetIntranetAddr("");
  // 指定测试环境 IP 直连（可选）: 设置 COS_TEST_DEST_IP 后, 所有请求的 TCP
  // 连接指向该地址(支持 "ip" 或 "ip:port" 形式), Host 头与签名仍使用桶域名,
  // 便于将测试流量打到指定测试环境网关; 不设置则走默认 DNS 解析
  const char* dest_ip_env = getenv("COS_TEST_DEST_IP");
  if (dest_ip_env != nullptr && dest_ip_env[0] != '\0') {
    m_config->SetIntranetAddr(dest_ip_env);
    m_config->SetIsUseIntranetAddr(true);
    std::cout << "[KeepAliveTest] DestIP: " << dest_ip_env
              << " (all requests connect to this address)" << std::endl;
  }
  m_config->SetDomainSameToHost(false);
  m_config->SetAccessKey(GetEnvVar("CPP_SDK_V5_ACCESS_KEY"));
  m_config->SetSecretKey(GetEnvVar("CPP_SDK_V5_SECRET_KEY"));
  m_config->SetRegion(GetEnvVar("CPP_SDK_V5_REGION"));

  // 保存被本套件修改的全局配置原值, TearDownTestCase 中恢复
  m_saved_pool_size = CosSysConfig::GetConnectionPoolSize();
  m_saved_max_idle_ms = CosSysConfig::GetConnectionPoolMaxIdleMs();
  m_saved_max_age_ms = CosSysConfig::GetConnectionPoolMaxAgeMs();

  // 开启长连接并配置连接池参数（必须在发起请求前设置）
  CosSysConfig::SetKeepAlive(true);
  CosSysConfig::SetKeepIdle(20);
  CosSysConfig::SetKeepIntvl(5);
  CosSysConfig::SetConnectionPoolSize(16);
  // 50s: 与 SDK 默认值一致, 为 COS 网关 60s 空闲超时留出余量
  CosSysConfig::SetConnectionPoolMaxIdleMs(50000);
  CosSysConfig::SetConnectionPoolMaxAgeMs(300000);
  // 分块/分片调小，让分块上传与断点下载产生多次请求，充分验证连接复用
  CosSysConfig::SetUploadCopyPartSize(kPartSize1M);
  CosSysConfig::SetDownThreadPoolSize(4);
  CosSysConfig::SetDownSliceSize(kPartSize1M);
  CosSysConfig::SetLogCallback(nullptr);
  // 日志级别: 默认 DBG 便于调试长连接; 可设置环境变量 COS_TEST_LOG_LEVEL=1 (ERR)
  // 得到干净输出。注意: DBG 下 SDK 解析 GetService 时会对每个未知字段打印一次
  // 全量响应 body(账号桶多时输出巨大), 属 SDK 已知行为 (测试规范 2.4)
  const char* log_level_env = getenv("COS_TEST_LOG_LEVEL");
  int log_level = log_level_env ? atoi(log_level_env) : (int)qcloud_cos::COS_LOG_DBG;
  CosSysConfig::SetLogLevel((qcloud_cos::LOG_LEVEL)log_level);

  m_client = new CosAPI(*m_config);

  const std::string appid = GetEnvVar("CPP_SDK_V5_APPID");
  m_bucket_name = "coscppsdkv5ut-ka-obj-" + appid;
  m_bucket_name_cfg = "coscppsdkv5ut-ka-cfg-" + appid;
  m_bucket_name_tmp = "coscppsdkv5ut-ka-tmp-" + appid;

  // 清理桶内数据并确保桶存在（保留桶不删除, 与 ObjectOpTest 做法一致:
  // 避免每次运行重新建桶触发服务端元数据同步延迟, 也可防止配额用完）
  const std::vector<std::string> buckets_to_handle = {
      m_bucket_name, m_bucket_name_cfg, m_bucket_name_tmp};
  for (const auto& bucket : buckets_to_handle) {
    CleanupBucket(bucket, /*delete_bucket=*/false);

    PutBucketReq req(bucket);
    PutBucketResp resp;
    CosResult result = m_client->PutBucket(req, &resp);
    if (!result.IsSucc()) {
      std::cout << "PutBucket " << bucket
                << " failed(ignored if exists): " << result.GetErrorMsg()
                << " HttpStatus=" << result.GetHttpStatus()
                << " RequestId=" << result.GetXCosRequestId() << std::endl;
    }
  }

  // 等待桶元数据在服务端生效（新建桶后立即读可能短暂返回 404, 读最终一致）
  for (const auto& bucket : buckets_to_handle) {
    bool bucket_ready = false;
    for (int retry = 0; retry < 30; ++retry) {
      HeadBucketReq head_req(bucket);
      HeadBucketResp head_resp;
      if (m_client->HeadBucket(head_req, &head_resp).IsSucc()) {
        bucket_ready = true;
        break;
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    EXPECT_TRUE(bucket_ready) << "bucket " << bucket
                              << " not ready after 30s";
  }
  std::cout << "========KeepAliveOpTest SetUpTestCase End========" << std::endl;
}

void KeepAliveOpTest::CleanupBucket(const std::string& bucket_name,
                                    bool delete_bucket) {
  // 1. 删除桶内所有对象（含版本对象）
  {
    GetBucketObjectVersionsReq ver_req(bucket_name);
    GetBucketObjectVersionsResp ver_resp;
    CosResult ver_result = m_client->GetBucketObjectVersions(ver_req, &ver_resp);
    if (ver_result.IsSucc()) {
      const std::vector<COSVersionSummary>& summs = ver_resp.GetVersionSummary();
      for (const auto& summ : summs) {
        DeleteObjectReq del_req(bucket_name, summ.m_key);
        if (!summ.m_version_id.empty()) {
          del_req.SetXCosVersionId(summ.m_version_id);
        }
        DeleteObjectResp del_resp;
        m_client->DeleteObject(del_req, &del_resp);
      }
    }
  }

  // 2. 中止所有未完成的分块上传
  {
    ListMultipartUploadReq list_mp_req(bucket_name);
    ListMultipartUploadResp list_mp_resp;
    CosResult list_mp_result =
        m_client->ListMultipartUpload(list_mp_req, &list_mp_resp);
    if (list_mp_result.IsSucc()) {
      std::vector<Upload> rst = list_mp_resp.GetUpload();
      for (const auto& itr : rst) {
        AbortMultiUploadReq abort_req(bucket_name, itr.m_key, itr.m_uploadid);
        AbortMultiUploadResp abort_resp;
        m_client->AbortMultiUpload(abort_req, &abort_resp);
      }
    }
  }

  // 3. 关闭版本控制（若开启过），避免影响后续删桶
  {
    PutBucketVersioningReq ver_req(bucket_name);
    PutBucketVersioningResp ver_resp;
    ver_req.SetStatus(false);
    m_client->PutBucketVersioning(ver_req, &ver_resp);
  }

  // 4. 删除桶
  if (delete_bucket) {
    DeleteBucketReq del_req(bucket_name);
    DeleteBucketResp del_resp;
    m_client->DeleteBucket(del_req, &del_resp);
  }
}

void KeepAliveOpTest::TearDownTestCase() {
  std::cout << "========KeepAliveOpTest TearDownTestCase Begin========"
            << std::endl;
  // 只清空桶内数据, 保留桶（原因见 SetUpTestCase 注释）
  const std::vector<std::string> buckets_to_handle = {
      m_bucket_name, m_bucket_name_cfg, m_bucket_name_tmp};
  for (const auto& bucket : buckets_to_handle) {
    CleanupBucket(bucket, /*delete_bucket=*/false);
  }
  delete m_client;
  delete m_config;
  // 恢复 SetUpTestCase 修改过的全局配置, 避免污染同进程后续的测试套件
  // (如 ConnectionPoolTest 对默认值的断言)
  CosSysConfig::SetKeepAlive(false);
  CosSysConfig::SetConnectionPoolSize(m_saved_pool_size);
  CosSysConfig::SetConnectionPoolMaxIdleMs(m_saved_max_idle_ms);
  CosSysConfig::SetConnectionPoolMaxAgeMs(m_saved_max_age_ms);
  std::cout << "========KeepAliveOpTest TearDownTestCase End========"
            << std::endl;
}

void KeepAliveOpTest::SetUp() {
  // 防止聚合运行时其它 suite 修改了全局开关，每个用例前确保长连接开启
  CosSysConfig::SetKeepAlive(true);
  // 避免请求过快
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void KeepAliveOpTest::TearDown() {
  // 还原默认值，避免影响同进程运行的其它测试套件
  CosSysConfig::SetKeepAlive(false);
}

KeepAlivePoolSnapshot KeepAliveOpTest::GetPoolSnapshot() {
  ConnectionPool& pool = ConnectionPool::GetInstance();
  KeepAlivePoolSnapshot snapshot;
  snapshot.acquire_count = pool.GetAcquireCount();
  snapshot.hit_count = pool.GetHitCount();
  snapshot.release_count = pool.GetReleaseCount();
  snapshot.discard_count = pool.GetDiscardCount();
  snapshot.idle_pool_size = pool.GetTotalPoolSize();
  return snapshot;
}

bool KeepAliveOpTest::ExpectKeepAliveHit(
    const KeepAlivePoolSnapshot& before, const std::string& op_name) {
  const KeepAlivePoolSnapshot after = GetPoolSnapshot();
  const uint64_t acquire_delta = after.acquire_count - before.acquire_count;
  const uint64_t hit_delta = after.hit_count - before.hit_count;

  EXPECT_GE(acquire_delta, 1u)
      << "[" << op_name
      << "] 连接池 Acquire 计数未增加, 请求未经过连接池, "
         "请确认已开启 CosSysConfig::SetKeepAlive(true)";
  EXPECT_GE(hit_delta, 1u)
      << "[" << op_name << "] 连接池命中(Hit)计数未增加, 长连接未生效,"
         << " acquire_delta=" << acquire_delta
      << ", idle_pool_size=" << after.idle_pool_size;
  return acquire_delta >= 1 && hit_delta >= 1;
}

bool KeepAliveOpTest::ExpectSucc(const CosResult& result,
                                 const std::string& op_name) {
  EXPECT_TRUE(result.IsSucc())
      << "[" << op_name << "] failed: " << result.GetErrorMsg()
      << " HttpStatus=" << result.GetHttpStatus()
      << " RequestId=" << result.GetXCosRequestId();
  return result.IsSucc();
}

CosResult KeepAliveOpTest::CallWithRetry(
    const std::function<CosResult()>& fn, int max_attempts) {
  CosResult result;
  for (int attempt = 0; attempt < max_attempts; ++attempt) {
    result = fn();
    if (result.IsSucc()) {
      break;
    }
    std::cout << "[retry] attempt " << (attempt + 1) << "/" << max_attempts
              << " failed: " << result.GetErrorMsg()
              << " HttpStatus=" << result.GetHttpStatus()
              << " RequestId=" << result.GetXCosRequestId() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
  return result;
}

}  // namespace qcloud_cos
