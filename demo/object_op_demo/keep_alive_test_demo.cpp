// Copyright (c) 2022, Tencent Inc.
// All rights reserved.

// 基于 AsyncMultiPutObject 的长连接（KeepAlive + 连接池）验证 demo。
//
// 验证原理：
//   开启 KeepAlive 后，SDK 会将 HTTP(S) 连接放入全局连接池复用。
//   本 demo 对同一个桶连续多次执行 AsyncMultiPutObject，
//   通过 ConnectionPool 的命中统计判断长连接是否生效：
//     - GetHitCount()      > 0  => 有连接被复用，长连接生效
//     - GetTotalPoolSize() > 0  => 有空闲连接被归还到池中
//   若命中数始终为 0，说明长连接未生效（请检查 KeepAlive 是否开启）。
//
// 使用步骤：
//   1. 修改 demo/config.json，填入真实 SecretId / SecretKey / Region；
//   2. 修改下方 kBucketName 为你的桶名（格式 <bucket>-<appid>）；
//   3. 编译：cmake --build build --target keep_alive_test_demo；
//   4. 在 config.json 所在目录运行：./bin/keep_alive_test_demo。

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"
#include "connection_pool.h"

using namespace qcloud_cos;

// ==================== 请按需修改 ====================
// 桶名，格式 <bucket>-<appid>，例如 examplebucket-1250000000
static const std::string kBucketName = "examplebucket-12500000000";
// 连续上传次数（次数越多越能体现连接复用）
static const int kUploadCount = 5;
// 本地测试文件大小（字节）。默认 1MB 为单分片；
// 如需触发多分片并发上传，可改为大于 UploadPartSize（默认 10MB）的值。
static const size_t kLocalFileSize = 1 * 1024 * 1024;
// ==================================================

namespace {

void PrintPoolStats(const std::string& stage) {
  ConnectionPool& pool = ConnectionPool::GetInstance();
  std::cout << "[" << stage << "] "
            << "Acquire=" << pool.GetAcquireCount()
            << " Hit=" << pool.GetHitCount()
            << " Create=" << pool.GetCreateCount()
            << " Release=" << pool.GetReleaseCount()
            << " Discard=" << pool.GetDiscardCount()
            << " IdlePoolSize=" << pool.GetTotalPoolSize()
            << std::endl;
}

bool MakeLocalTestFile(const std::string& path, size_t size) {
  std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
  if (!ofs.is_open()) {
    std::cerr << "Failed to create local file: " << path << std::endl;
    return false;
  }
  const size_t kBufSize = 1024 * 1024;
  std::vector<char> buf(kBufSize, 'a');
  size_t written = 0;
  while (written < size) {
    size_t n = std::min(kBufSize, size - written);
    ofs.write(buf.data(), static_cast<std::streamsize>(n));
    written += n;
  }
  ofs.close();
  return true;
}

}  // namespace

int main() {
  // 1. 初始化 SDK（读取 config.json）
  qcloud_cos::CosConfig config("demo/config.json");
  qcloud_cos::CosAPI cos(config);

  // 2. 开启长连接并配置连接池参数（必须在发起上传前调用）
  CosSysConfig::SetLogLevel((LOG_LEVEL)COS_LOG_DBG);
  CosSysConfig::SetKeepAlive(true);
  CosSysConfig::SetConnectionPoolSize(5);           // 每个 key 最多保留的空闲连接数
  CosSysConfig::SetConnectionPoolMaxIdleMs(60000);  // 空闲 60s 后驱逐
  CosSysConfig::SetConnectionPoolMaxAgeMs(300000);  // 连接最长存活 5min

  std::cout << "=== KeepAlive connection pool test (AsyncMultiPutObject) ==="
            << std::endl;
  std::cout << "Bucket=" << kBucketName << " UploadCount=" << kUploadCount
            << " FileSize=" << kLocalFileSize << std::endl;

  // 3. 生成本地测试文件
  const std::string local_file = "./keep_alive_test_file.bin";
  if (!MakeLocalTestFile(local_file, kLocalFileSize)) {
    return 1;
  }

  PrintPoolStats("before upload");

  // 4. 连续多次异步分片上传，观察连接复用
  int succ = 0;
  for (int i = 0; i < kUploadCount; ++i) {
    std::string object_name = "keep_alive_test_" + std::to_string(i);

    AsyncMultiPutObjectReq req(kBucketName, object_name, local_file);
    req.SetRecvTimeoutInms(1000 * 60);
    SharedAsyncContext context = cos.AsyncMultiPutObject(req);
    context->WaitUntilFinish();

    CosResult result = context->GetResult();
    if (result.IsSucc()) {
      ++succ;
      std::cout << "  upload #" << i << " [" << object_name << "] SUCC"
                << " RequestId=" << result.GetXCosRequestId() << std::endl;
    } else {
      std::cout << "  upload #" << i << " [" << object_name << "] FAIL"
                << " HttpStatus=" << result.GetHttpStatus()
                << " ErrorCode=" << result.GetErrorCode()
                << " ErrorMsg=" << result.GetErrorMsg()
                << " RequestId=" << result.GetXCosRequestId() << std::endl;
    }

    PrintPoolStats("after upload #" + std::to_string(i));
  }

  // 5. 输出结论
  ConnectionPool& pool = ConnectionPool::GetInstance();
  std::cout << "=== Result ===" << std::endl;
  std::cout << "  upload succ/fail: " << succ << "/" << (kUploadCount - succ)
            << std::endl;
  std::cout << "  acquire=" << pool.GetAcquireCount()
            << " hit=" << pool.GetHitCount()
            << " create=" << pool.GetCreateCount()
            << " idle_pool_size=" << pool.GetTotalPoolSize() << std::endl;

  if (pool.GetHitCount() > 0) {
    std::cout << "  [PASS] 长连接已生效：命中复用连接 " << pool.GetHitCount()
              << " 次" << std::endl;
  } else {
    std::cout << "  [FAIL] 长连接未生效：命中复用连接 0 次" << std::endl;
    std::cout << "         请确认已调用 CosSysConfig::SetKeepAlive(true)"
              << std::endl;
  }

  // 6. 清理本地文件
  std::remove(local_file.c_str());
  return 0;
}
