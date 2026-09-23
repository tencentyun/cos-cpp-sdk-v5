// Copyright (c) 2026, Tencent Inc.
// All rights reserved.
//
// Description: 长连接（KeepAlive + 连接池）专项单测公共 Fixture。
//
// 环境变量（与 object_op_test 一致）：
// export CPP_SDK_V5_ACCESS_KEY=xxx
// export CPP_SDK_V5_SECRET_KEY=xxx
// export CPP_SDK_V5_REGION=ap-guangzhou
// export CPP_SDK_V5_APPID=xxx
// export CPP_SDK_V5_UIN=xxx

#pragma once

#include <chrono>
#include <functional>
#include <string>
#include <thread>

#include "cos_api.h"
#include "util/connection_pool.h"
#include "util/test_utils.h"
#include "gtest/gtest.h"

namespace qcloud_cos {

// 连接池统计快照，用于用例执行前后的差值验证
struct KeepAlivePoolSnapshot {
  uint64_t acquire_count = 0;  // Acquire 总次数（走了连接池路径的请求数）
  uint64_t hit_count = 0;      // 从池中复用连接的次数
  uint64_t release_count = 0;  // 归还到池中的连接数
  uint64_t discard_count = 0;  // 被丢弃的连接数
  size_t idle_pool_size = 0;   // 当前池中空闲连接数
};

/*
 * 长连接（KeepAlive + 连接池）专项单测 Fixture。
 *
 * 与 ObjectOpTest 的区别：
 *   1. 开启 CosSysConfig::SetKeepAlive(true)；
 *   2. 提供连接池统计辅助函数，验证各接口在长连接下功能正常且连接被复用。
 *
 * 长连接生效的验证方法：
 *   - 用例开始时记录 ConnectionPool 统计快照；
 *   - 执行业务请求（至少两次串行请求，或一次内部并发多请求的操作）；
 *   - ExpectKeepAliveHit() 校验 Acquire 与 Hit 计数均增加：
 *       acquire_delta >= 1：请求走了连接池路径（KeepAlive 已开启）；
 *       hit_delta     >= 1：空闲连接被复用（长连接真正生效）。
 *
 * 注意：使用自定义 ssl_ctx_cb 的请求不会被池化（by design），
 * 因此本套件不设置 SSLCtxCallback。
 */
class KeepAliveOpTest : public testing::Test {
 protected:
  static void SetUpTestCase();
  static void TearDownTestCase();

  void SetUp() override;
  void TearDown() override;

  // ---------- 长连接验证辅助 ----------
  // 辅助函数返回 bool，调用处判断后 return（测试编写规范 3.1）

  /// 获取连接池统计快照
  static KeepAlivePoolSnapshot GetPoolSnapshot();

  /// 校验 [before, now] 期间发生了连接复用（Acquire/Hit 计数均增加）
  static bool ExpectKeepAliveHit(const KeepAlivePoolSnapshot& before,
                                 const std::string& op_name);

  /// 校验请求成功，失败消息带 HttpStatus/RequestId（测试编写规范 3.2）
  static bool ExpectSucc(const CosResult& result, const std::string& op_name);

  /// 带有限重试地执行请求（规避新建桶元数据同步延迟导致的 404，读最终一致）
  static CosResult CallWithRetry(const std::function<CosResult()>& fn,
                                 int max_attempts = 3);

  // ---------- 桶清理辅助 ----------
  /// 清空桶内对象（含版本对象）、中止未完成分块、关闭版本控制；
  /// delete_bucket 为 true 时进一步删除桶
  static void CleanupBucket(const std::string& bucket_name,
                            bool delete_bucket = true);

 protected:
  static CosConfig* m_config;
  static CosAPI* m_client;
  static std::string m_bucket_name;      // 对象操作测试桶
  static std::string m_bucket_name_cfg;  // 桶配置操作测试桶
  static std::string m_bucket_name_tmp;  // 建桶/删桶流程测试桶

  // SetUpTestCase 修改全局配置前的原始值, TearDownTestCase 恢复用。
  // CosSysConfig 是进程级全局单例, 聚合运行(keepalive-all-test)时若不
  // 恢复, 会污染后续测试套件(如 ConnectionPoolTest 对默认值的断言)。
  static unsigned m_saved_pool_size;
  static uint64_t m_saved_max_idle_ms;
  static uint64_t m_saved_max_age_ms;
};

/*
 * RAII 辅助：在作用域内临时压低日志级别，析构时恢复原级别。
 *
 * 用途：GetService 等接口的响应解析中，SDK 对每个未知字段（OFSType/AZType/
 * BucketType 等服务端新增字段）会以 WARN 级别打印一次"完整响应 body"
 * （SDK 已知行为, 账号下桶多时输出巨大）。当测试运行在 DBG 级别时这些
 * WARN 会全部输出导致刷屏。在相关用例开头声明 ScopedQuietLog 即可只在
 * 该用例内压到 ERR, 不影响其他用例的 DBG 调试输出。
 */
class ScopedQuietLog {
 public:
  explicit ScopedQuietLog(int level = COS_LOG_ERR)
      : m_prev_level(CosSysConfig::GetLogLevel()) {
    CosSysConfig::SetLogLevel((LOG_LEVEL)level);
  }

  ~ScopedQuietLog() { CosSysConfig::SetLogLevel((LOG_LEVEL)m_prev_level); }

 private:
  ScopedQuietLog(const ScopedQuietLog&) = delete;
  ScopedQuietLog& operator=(const ScopedQuietLog&) = delete;

  int m_prev_level;
};

}  // namespace qcloud_cos
