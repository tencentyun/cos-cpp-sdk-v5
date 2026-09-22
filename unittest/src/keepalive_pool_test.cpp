// Copyright (c) 2026, Tencent Inc.
// All rights reserved.
//
// Description: 连接池（ConnectionPool）纯逻辑单测 —— 不依赖网络。
//
//   与 keepalive_*_test.cpp 的区别: 本文件不发起任何真实请求, 直接对
//   ConnectionPool 单例做白盒验证, 因此可在无网络/无密钥的环境下运行,
//   适合作为提交前的快速回归。
//
//   覆盖的历史缺陷:
//     1. CloseAll() 曾在持锁状态下调用 GetTotalPoolSize() 造成自死锁;
//        修复后该路径被 CosAPI 析构接入, 成为每次进程收尾的必经流程,
//        一旦退化会直接导致业务进程挂死。
//     2. Acquire() 曾不校验过期时间, 使 MaxIdleMs/MaxAge 配置失效。
//     3. Release() 曾无条件重置 created, 使 MaxAge 永远不触发。
//     4. 陈旧连接被丢弃时未修正 Hit 计数, 导致复用率统计失真。
//
//   死锁类用例的实现要点（踩过的坑）:
//     - 不能用 std::async + future::wait_for: 超时断言失败后, future 的
//       析构函数仍会阻塞等待任务结束, 测试进程照样挂死, 且无法输出结果;
//     - 不能在 SetUp/TearDown 中调用 CloseAll: 一旦它退化为死锁, 其它用例
//       会跟着卡住, 掩盖真正的失败点;
//     - 故改为 fork 子进程执行待测逻辑, 超时则 SIGKILL 子进程并判定失败,
//       父进程状态不受任何影响。

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#if defined(__unix__) || defined(__APPLE__)
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#define COS_UT_HAS_FORK 1
#endif

#include <atomic>
#include <future>

#include "Poco/Net/HTTPClientSession.h"
#include "cos_api.h"
#include "cos_sys_config.h"
#include "gtest/gtest.h"
#include "util/connection_pool.h"

namespace qcloud_cos {

namespace {

// 死锁检测超时时间: 正常执行为毫秒级, 超过即可判定卡死
const int kDeadlockTimeoutSec = 10;

enum IsolatedResult {
  kIsolatedOk = 0,       // 子进程正常退出且自检通过
  kIsolatedTimeout = 1,  // 子进程超时未退出 —— 判定死锁
  kIsolatedFailed = 2,   // 子进程自检失败或异常退出
};

const char* IsolatedResultName(IsolatedResult rc) {
  switch (rc) {
    case kIsolatedOk: return "Ok";
    case kIsolatedTimeout: return "Timeout";
    default: return "Failed";
  }
}

#ifdef COS_UT_HAS_FORK
/// 在子进程中执行 body 并限时等待。
/// body 返回 0 表示自检通过, 非 0 表示自检失败。
/// 子进程一律用 _exit 退出, 不触发父进程继承来的 atexit/静态析构。
IsolatedResult RunIsolated(int (*body)(), int timeout_sec) {
  pid_t pid = fork();
  if (pid < 0) {
    ADD_FAILURE() << "fork() failed, errno=" << errno;
    return kIsolatedFailed;
  }
  if (pid == 0) {
    int code = 91;
    try {
      code = body();
    } catch (...) {
      code = 92;
    }
    _exit(code);
  }

  const int kPollMs = 20;
  for (int waited_ms = 0; waited_ms < timeout_sec * 1000; waited_ms += kPollMs) {
    int status = 0;
    pid_t ret = waitpid(pid, &status, WNOHANG);
    if (ret == pid) {
      return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? kIsolatedOk
                                                             : kIsolatedFailed;
    }
    if (ret < 0) {
      return kIsolatedFailed;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(kPollMs));
  }

  kill(pid, SIGKILL);
  waitpid(pid, NULL, 0);
  return kIsolatedTimeout;
}
#endif  // COS_UT_HAS_FORK

/// 构造一个未建立连接的 session。
/// Poco::Net::HTTPClientSession 的构造函数不会发起 TCP 连接, 析构时
/// close() 对未连接的 socket 也是安全的, 因此可用于纯逻辑验证。
std::unique_ptr<Poco::Net::HTTPClientSession> MakeFakeSession() {
  return std::unique_ptr<Poco::Net::HTTPClientSession>(
      new Poco::Net::HTTPClientSession("127.0.0.1", 1));
}

/// 析构时故意阻塞的 session, 用于模拟 HTTPS 连接析构中 SSL_shutdown
/// 网络写被对端拖住的情形, 验证池不会在持锁状态下等待析构完成。
class SlowDtorSession : public Poco::Net::HTTPClientSession {
 public:
  static std::atomic<bool> s_dtor_running;
  static std::atomic<bool> s_dtor_done;

  SlowDtorSession() : Poco::Net::HTTPClientSession("127.0.0.1", 1) {}
  ~SlowDtorSession() {
    s_dtor_running = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    s_dtor_done = true;
  }
};

std::atomic<bool> SlowDtorSession::s_dtor_running(false);
std::atomic<bool> SlowDtorSession::s_dtor_done(false);

}  // namespace

class ConnectionPoolTest : public testing::Test {
 protected:
  void SetUp() override {
    // 保存原配置, 避免污染同进程内的其它测试套件
    m_saved_pool_size = CosSysConfig::GetConnectionPoolSize();
    m_saved_max_idle_ms = CosSysConfig::GetConnectionPoolMaxIdleMs();
    m_saved_max_age_ms = CosSysConfig::GetConnectionPoolMaxAgeMs();
  }

  void TearDown() override {
    // 只清理本用例自己的 key（而非 CloseAll），这样即使 CloseAll 退化为
    // 死锁, 也只会让对应用例失败, 不会连累其它用例
    ConnectionPool& pool = ConnectionPool::GetInstance();
    const std::string key = TestKey();
    while (std::unique_ptr<Poco::Net::HTTPClientSession> s = pool.Acquire(key)) {
      // 取出即丢弃
    }
    CosSysConfig::SetConnectionPoolSize(m_saved_pool_size);
    CosSysConfig::SetConnectionPoolMaxIdleMs(m_saved_max_idle_ms);
    CosSysConfig::SetConnectionPoolMaxAgeMs(m_saved_max_age_ms);
  }

  /// 每个用例使用独立 key，避免用例间相互干扰，也免去清空整个池的需要
  static std::string TestKey() {
    const testing::TestInfo* info =
        testing::UnitTest::GetInstance()->current_test_info();
    return std::string("ut.pool.") + (info ? info->name() : "unknown") +
           ":443:https:v1:";
  }

 private:
  unsigned m_saved_pool_size = 0;
  uint64_t m_saved_max_idle_ms = 0;
  uint64_t m_saved_max_age_ms = 0;
};

// TC-KA-POOL-001: CloseAll() 不得死锁（历史 bug: 持锁调用 GetTotalPoolSize）
TEST_F(ConnectionPoolTest, TC_KA_POOL_001_CloseAllShouldNotDeadlock) {
#ifndef COS_UT_HAS_FORK
  std::cout << "[SKIPPED] 当前平台不支持 fork, 跳过死锁检测" << std::endl;
#else
  IsolatedResult rc = RunIsolated([]() -> int {
    ConnectionPool& pool = ConnectionPool::GetInstance();
    // 放入若干连接, 确保 CloseAll 走到实际清理分支而非空池快速返回
    for (int i = 0; i < 3; ++i) {
      pool.Release("ut.pool.closeall:443:https:v1:", MakeFakeSession(), true);
    }
    if (pool.GetTotalPoolSize() != 3) {
      return 3;
    }
    pool.CloseAll();  // 历史死锁点
    // CloseAll 返回后再取一次总数, 顺带验证锁已正常释放
    return pool.GetTotalPoolSize() == 0 ? 0 : 4;
  }, kDeadlockTimeoutSec);

  ASSERT_NE(kIsolatedTimeout, rc)
      << "Deadlock detected: ConnectionPool::CloseAll() did not return within "
      << kDeadlockTimeoutSec
      << "s, it is likely re-locking the non-recursive m_mutex_ "
         "(e.g. by calling GetTotalPoolSize() while holding the lock).";
  ASSERT_EQ(kIsolatedOk, rc)
      << "CloseAll() self-check failed in child process, result="
      << IsolatedResultName(rc);
#endif
}

// TC-KA-POOL-002: CosAPI 析构会触发 CloseAll, 该路径同样不得死锁
//
// CosUInit() 内部持有全局初始化锁 g_init_lock, 若在锁内调用 CloseAll,
// 既会形成锁嵌套, 也会让 SSL_shutdown 等网络写操作阻塞并发建连的线程。
TEST_F(ConnectionPoolTest, TC_KA_POOL_002_CosApiDestructShouldNotDeadlock) {
#ifndef COS_UT_HAS_FORK
  std::cout << "[SKIPPED] 当前平台不支持 fork, 跳过死锁检测" << std::endl;
#else
  IsolatedResult rc = RunIsolated([]() -> int {
    ConnectionPool& pool = ConnectionPool::GetInstance();
    pool.Release("ut.pool.cosapi:443:https:v1:", MakeFakeSession(), true);
    if (pool.GetTotalPoolSize() != 1) {
      return 3;
    }
    {
      // 构造/析构均不发起网络请求, CheckRegion 仅做字符串校验
      CosConfig config(1250000000, "ut_ak", "ut_sk", "ap-guangzhou");
      CosAPI api(config);
      (void)api;
    }
    // 最后一个 CosAPI 析构时应已清空连接池
    return pool.GetTotalPoolSize() == 0 ? 0 : 4;
  }, kDeadlockTimeoutSec);

  ASSERT_NE(kIsolatedTimeout, rc)
      << "Deadlock detected: CosAPI destruction did not return within "
      << kDeadlockTimeoutSec
      << "s, check the lock order between CosUInit()'s g_init_lock and "
         "ConnectionPool::CloseAll().";
  ASSERT_EQ(kIsolatedOk, rc)
      << "CosAPI destruction did not drain the connection pool, result="
      << IsolatedResultName(rc);
#endif
}

// TC-KA-POOL-003: Acquire 必须丢弃空闲超时的连接（MaxIdleMs 生效）
TEST_F(ConnectionPoolTest, TC_KA_POOL_003_AcquireShouldDropIdleExpired) {
  ConnectionPool& pool = ConnectionPool::GetInstance();
  const std::string key = TestKey();
  CosSysConfig::SetConnectionPoolMaxIdleMs(50);
  CosSysConfig::SetConnectionPoolMaxAgeMs(3600 * 1000);

  pool.Release(key, MakeFakeSession(), true);
  ASSERT_EQ(1u, pool.GetPoolSize(key));

  const uint64_t hit_before = pool.GetHitCount();
  const uint64_t discard_before = pool.GetDiscardCount();
  std::this_thread::sleep_for(std::chrono::milliseconds(150));

  // 该 key 下无新连接归还, 若 Acquire 不自校验, 过期连接会被直接交给调用方
  std::unique_ptr<Poco::Net::HTTPClientSession> session = pool.Acquire(key);
  EXPECT_TRUE(session == NULL)
      << "idle-expired connection should not be handed out";
  EXPECT_EQ(hit_before, pool.GetHitCount())
      << "discarded connection must not be counted as a hit";
  EXPECT_EQ(discard_before + 1, pool.GetDiscardCount());
  EXPECT_EQ(0u, pool.GetPoolSize(key));
}

// TC-KA-POOL-004: 复用连接的 created 必须透传, 否则 MaxAge 永不触发
TEST_F(ConnectionPoolTest, TC_KA_POOL_004_MaxAgeShouldSurviveReuse) {
  ConnectionPool& pool = ConnectionPool::GetInstance();
  const std::string key = TestKey();
  CosSysConfig::SetConnectionPoolMaxIdleMs(3600 * 1000);
  CosSysConfig::SetConnectionPoolMaxAgeMs(300);

  // T0: 首次归还, created = T0
  pool.Release(key, MakeFakeSession(), true);

  // T0+200ms: 取出, 此时 age=200ms < 300ms, 应命中
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  std::chrono::steady_clock::time_point created;
  std::unique_ptr<Poco::Net::HTTPClientSession> session =
      pool.Acquire(key, &created);
  ASSERT_FALSE(session == NULL) << "connection within MaxAge should hit";
  ASSERT_TRUE(created != std::chrono::steady_clock::time_point())
      << "Acquire must output the real creation time";

  // 带着原始 created 归还
  pool.Release(key, std::move(session), true, created);

  // T0+400ms: age=400ms > 300ms, 必须被驱逐。
  // 若 Release 把 created 重置为归还时刻, 此处 age 仅 200ms, 会错误命中。
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  std::unique_ptr<Poco::Net::HTTPClientSession> expired = pool.Acquire(key);
  EXPECT_TRUE(expired == NULL)
      << "created was reset on release, MaxAge never triggers";
}

// TC-KA-POOL-005: 陈旧连接丢弃后需修正 Hit 计数, 保证复用率统计真实
TEST_F(ConnectionPoolTest, TC_KA_POOL_005_ReportStaleDiscardFixesHitCount) {
  ConnectionPool& pool = ConnectionPool::GetInstance();
  const std::string key = TestKey();
  CosSysConfig::SetConnectionPoolMaxIdleMs(3600 * 1000);
  CosSysConfig::SetConnectionPoolMaxAgeMs(3600 * 1000);

  pool.Release(key, MakeFakeSession(), true);

  const uint64_t hit_before = pool.GetHitCount();
  const uint64_t discard_before = pool.GetDiscardCount();

  std::unique_ptr<Poco::Net::HTTPClientSession> session = pool.Acquire(key);
  ASSERT_FALSE(session == NULL);
  ASSERT_EQ(hit_before + 1, pool.GetHitCount());

  // 调用方探测到连接已被对端静默关闭, 放弃复用
  session.reset();
  pool.ReportStaleDiscard();

  EXPECT_EQ(hit_before, pool.GetHitCount())
      << "stale connection must not inflate the hit count";
  EXPECT_EQ(discard_before + 1, pool.GetDiscardCount());
}

// TC-KA-POOL-006: Release 中的低频清扫必须回收"其它 key"的过期连接
//
// Release 第 1 步只清理当前 key，若没有全局清扫，再也不会被访问的 host
// （切换 region、临时域名等）其空闲 fd 会驻留到进程退出。
TEST_F(ConnectionPoolTest, TC_KA_POOL_006_ReleaseShouldSweepOtherKeys) {
  ConnectionPool& pool = ConnectionPool::GetInstance();
  const std::string stale_key = TestKey() + "abandoned";
  const std::string active_key = TestKey() + "active";
  CosSysConfig::SetConnectionPoolMaxIdleMs(50);
  CosSysConfig::SetConnectionPoolMaxAgeMs(3600 * 1000);

  // 一个此后再也不会被 Acquire/Release 的 key
  pool.Release(stale_key, MakeFakeSession(), true);
  ASSERT_EQ(1u, pool.GetPoolSize(stale_key));

  // 等待两件事同时满足（期间不触碰该 key）：
  //   1. stale_key 的连接空闲超过 MaxIdleMs(50ms)；
  //   2. 全局清扫的 1 秒时间门控打开。
  // 门控意味着被遗弃的连接最多滞留约 1 秒才被回收，属预期行为。
  std::this_thread::sleep_for(std::chrono::milliseconds(1100));

  // 在另一个 key 上发生一次 Release，应触发全局清扫
  pool.Release(active_key, MakeFakeSession(), true);

  EXPECT_EQ(0u, pool.GetPoolSize(stale_key))
      << "abandoned key's expired connection was never reclaimed";
  EXPECT_EQ(1u, pool.GetPoolSize(active_key))
      << "the just-released connection must not be swept";

  // 清理本用例残留
  while (std::unique_ptr<Poco::Net::HTTPClientSession> s =
             pool.Acquire(active_key)) {
  }
}

// TC-KA-POOL-007: 清扫路径不得死锁（EvictIdleConnectionsLocked 免锁约定）
//
// Release 已持有 m_mutex_，若清扫误调用会自行加锁的 EvictIdleConnections()，
// 会在非递归 mutex 上自死锁。
TEST_F(ConnectionPoolTest, TC_KA_POOL_007_ReleaseSweepShouldNotDeadlock) {
#ifndef COS_UT_HAS_FORK
  std::cout << "[SKIPPED] 当前平台不支持 fork, 跳过死锁检测" << std::endl;
#else
  IsolatedResult rc = RunIsolated([]() -> int {
    ConnectionPool& pool = ConnectionPool::GetInstance();
    CosSysConfig::SetConnectionPoolMaxIdleMs(10);
    // 多个 key，确保清扫真正遍历到多个链表
    for (int i = 0; i < 5; ++i) {
      pool.Release("ut.pool.sweep" + std::to_string(i) + ":443:https:v1:",
                   MakeFakeSession(), true);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // 该次 Release 会在持锁状态下触发全局清扫
    pool.Release("ut.pool.sweep.trigger:443:https:v1:", MakeFakeSession(),
                 true);
    // 公有版本单独加锁，也要能正常返回
    pool.EvictIdleConnections();
    return 0;
  }, kDeadlockTimeoutSec);

  ASSERT_NE(kIsolatedTimeout, rc)
      << "Deadlock detected: the sweep inside Release() did not return within "
      << kDeadlockTimeoutSec
      << "s, it must call EvictIdleConnectionsLocked() (lock-free) rather "
         "than EvictIdleConnections().";
  ASSERT_EQ(kIsolatedOk, rc)
      << "sweep self-check failed, result=" << IsolatedResultName(rc);
#endif
}

// TC-KA-POOL-008: MaxIdleMs 默认值必须小于 COS 网关空闲超时(60s)
//
// 该值同时会同步到 Poco 的 keepAliveTimeout（见 http_sender.cpp）。
// 若等于或大于网关超时，连接会在池中被服务端单方面关闭，复用时偶发失败。
//
// 注意: 本用例断言的是"当前生效值"而非编译期默认值, 依赖各测试套件遵守
// "修改全局配置后必须恢复"的约定 —— 聚合运行(keepalive-all-test)时,
// KeepAliveOpTest 会先于本套件执行并修改 CosSysConfig, 其 TearDownTestCase
// 负责恢复原值; 独立运行(keepalive_pool_test)时读到的就是编译期默认值。
TEST_F(ConnectionPoolTest, TC_KA_POOL_008_DefaultMaxIdleLeavesGatewayMargin) {
  const uint64_t max_idle_ms = CosSysConfig::GetConnectionPoolMaxIdleMs();
  EXPECT_LT(max_idle_ms, 60000u)
      << "ConnectionPoolMaxIdleMs=" << max_idle_ms
      << "ms leaves no margin before the 60s gateway idle timeout";
  EXPECT_GT(max_idle_ms, 0u);
}

// TC-KA-POOL-009: 被丢弃连接的析构不得持有池锁
//
// HTTPS 连接析构含 SSL_shutdown 网络写, 对端停滞时可阻塞到 send timeout。
// 若析构发生在持锁期间(如在锁内 pop_front/erase/clear), 所有线程的
// Acquire/Release 都会被卡住。本用例用"析构故意阻塞 500ms"的 session 验证:
// 析构进行中时, 其它线程的 Acquire 必须仍能立刻完成。
//
// 注意: 必须经由 CloseAll(或池内驱逐)触发析构才能测到锁内路径 —— 经由
// Release 的"不健康连接"路径不行, 那里的 session 是函数参数, C++ 保证
// 参数在局部变量(含 lock_guard)析构之后才销毁, 本就不在锁内。
TEST_F(ConnectionPoolTest, TC_KA_POOL_009_VictimDestructionMustNotHoldLock) {
  ConnectionPool& pool = ConnectionPool::GetInstance();
  const std::string key = TestKey();
  CosSysConfig::SetConnectionPoolMaxIdleMs(3600 * 1000);
  CosSysConfig::SetConnectionPoolMaxAgeMs(3600 * 1000);

  // 健康连接入池, 随后由 CloseAll 负责析构(锁内 clear 或锁外 swap)
  pool.Release(key,
               std::unique_ptr<Poco::Net::HTTPClientSession>(new SlowDtorSession()),
               true);
  ASSERT_EQ(1u, pool.GetPoolSize(key));

  SlowDtorSession::s_dtor_running = false;
  SlowDtorSession::s_dtor_done = false;

  std::future<void> closer = std::async(std::launch::async, [&] {
    pool.CloseAll();
  });

  // 等待析构开始(此时连接已离池, 若实现正确, 池锁应已释放)
  for (int i = 0; i < 2000 && !SlowDtorSession::s_dtor_running.load(); ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  ASSERT_TRUE(SlowDtorSession::s_dtor_running.load())
      << "victim destructor never started";

  // 析构还要阻塞约 500ms。此时对另一个 key 做 Acquire:
  // 若析构发生在持锁路径上(如锁内 m_pool_.clear()), 这里的 Acquire 会被
  // 卡到析构结束(>=400ms); 若析构在锁外(swap 出去后销毁), 立即返回 nullptr。
  const auto t0 = std::chrono::steady_clock::now();
  std::unique_ptr<Poco::Net::HTTPClientSession> blocked_probe =
      pool.Acquire(key + "other");
  const auto elapsed_ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::steady_clock::now() - t0)
          .count();

  EXPECT_LT(elapsed_ms, 400)
      << "Acquire was blocked for " << elapsed_ms
      << "ms while a victim session was being destroyed - "
         "destruction must happen outside the pool mutex";

  // 等待清理线程(及其析构)结束, 不在用例间遗留后台活动
  closer.wait();
  EXPECT_TRUE(SlowDtorSession::s_dtor_done.load());
}

}  // namespace qcloud_cos
