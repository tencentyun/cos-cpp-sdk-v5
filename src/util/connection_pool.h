#ifndef COS_CPP_SDK_V5_SRC_UTIL_CONNECTION_POOL_H_
#define COS_CPP_SDK_V5_SRC_UTIL_CONNECTION_POOL_H_

#include <stdint.h>

#include <atomic>
#include <chrono>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSClientSession.h"

namespace qcloud_cos {

/// \brief HTTP 连接池，支持 HTTP/HTTPS 持久连接复用。
///
/// 线程安全，通过 std::mutex 保护内部数据结构。
/// 以 "host:port:scheme[:verify:ca]" 为 key 为每个目标端点维护一组空闲连接。
/// HTTPS 场景下 key 必须包含证书校验开关与 CA 路径，避免安全属性不同的连接
/// 被互相复用。
///
/// 典型使用方式：
///   auto session = ConnectionPool::GetInstance().Acquire(key, ...);
///   if (!session) {
///     session.reset(new HTTPSClientSession(host, port, ctx));
///   }
///   // ... 发送请求、接收响应 ...
///   ConnectionPool::GetInstance().Release(key, std::move(session), is_healthy);
///
/// 注意：使用了自定义 ssl_ctx_cb 的连接不会被池化，
/// 因为 SSL 上下文已被回调修改，无法安全复用。
class ConnectionPool {
 public:
  struct PooledSession {
    std::unique_ptr<Poco::Net::HTTPClientSession> session;
    std::chrono::steady_clock::time_point last_used;
    std::chrono::steady_clock::time_point created;
  };

  /// \brief 获取单例
  static ConnectionPool& GetInstance();

  /// \brief 从池中获取一个到指定 key 的空闲连接。
  ///
  /// 取出时会校验空闲时间(MaxIdleMs)与生命周期(MaxAge)，过期连接直接丢弃
  /// 并继续尝试下一个，因此返回的连接一定在有效期内。
  /// \param key 连接池 key，格式为 "host:port:scheme[:verify:ca]"
  /// \param created_out 非空时输出该连接的真实建立时间，归还时需原样回传，
  ///        否则连接年龄会被重置，MaxAge 策略永远不会生效
  /// \return 如果池中有可用连接则返回之，否则返回 nullptr（调用方需自行创建）
  std::unique_ptr<Poco::Net::HTTPClientSession> Acquire(
      const std::string& key,
      std::chrono::steady_clock::time_point* created_out = nullptr);

  /// \brief 将连接归还到池中或销毁。
  /// \param key 连接池 key
  /// \param session 待归还的 session
  /// \param is_healthy 连接是否健康。若不健康则直接关闭而不归还池
  /// \param created 连接真实建立时间。默认值（epoch）表示新建连接，按当前
  ///        时间计算；复用连接必须传入 Acquire 输出的原始值
  void Release(const std::string& key,
               std::unique_ptr<Poco::Net::HTTPClientSession> session,
               bool is_healthy,
               std::chrono::steady_clock::time_point created =
                   std::chrono::steady_clock::time_point());

  /// \brief 上报一个"取出后即被判定陈旧而丢弃"的连接，用于修正统计。
  ///
  /// Acquire 成功已计入 Hit，调用方若因陈旧探测(如对端静默关闭)放弃该连接，
  /// 必须调用此方法，否则 Hit 计数无法反映真实复用率。
  void ReportStaleDiscard();

  /// \brief 驱逐所有超过空闲时间和最大生命周期的连接。
  ///
  /// Release() 中已内置低频自动清扫，通常无需外部调用；
  /// 需要立即回收时（如进入长时间空闲前）可主动触发。
  void EvictIdleConnections();

  /// \brief 关闭所有连接并清空池。
  void CloseAll();

  /// \brief 获取指定 key 的空闲连接数（主要用于测试/诊断）。
  size_t GetPoolSize(const std::string& key) const;

  /// \brief 获取总空闲连接数（主要用于测试/诊断）。
  size_t GetTotalPoolSize() const;

  // ---------- 统计 ----------
  // 计数器可能被任意线程无锁读取（测试/诊断），故使用 atomic 避免 data race
  uint64_t GetAcquireCount() const { return m_acquire_count.load(); }
  uint64_t GetReleaseCount() const { return m_release_count.load(); }
  uint64_t GetDiscardCount() const { return m_discard_count.load(); }
  uint64_t GetHitCount() const { return m_hit_count.load(); }
  uint64_t GetCreateCount() const {
    return m_acquire_count.load() - m_hit_count.load();
  }

 private:
  ConnectionPool() = default;
  ~ConnectionPool() = default;
  ConnectionPool(const ConnectionPool&) = delete;
  ConnectionPool& operator=(const ConnectionPool&) = delete;

  /// \brief 驱逐所有过期连接（调用方必须已持有 m_mutex_）。
  ///
  /// 被驱逐的连接不会在此析构, 而是移入 victims, 由调用方在释放锁之后
  /// 统一销毁 —— HTTPS 连接的析构含 SSL_shutdown 网络写, 对端停滞时最多
  /// 阻塞到 send timeout, 不得在持锁时进行。
  /// 注意：m_mutex_ 是非递归锁，锁内路径一律只能调用本函数，
  /// 绝不能调用会自行加锁的 EvictIdleConnections()/GetTotalPoolSize()。
  void EvictIdleConnectionsLocked(
      const std::chrono::steady_clock::time_point& now,
      std::vector<std::unique_ptr<Poco::Net::HTTPClientSession>>* victims);

  mutable std::mutex m_mutex_;
  std::unordered_map<std::string, std::list<PooledSession>> m_pool_;
  // 上次全局清扫时间，用于给 Release 中的自动清扫做时间门控
  std::chrono::steady_clock::time_point m_last_sweep_;

  // 统计计数器（写入在锁内，读取无锁，故使用 atomic 保证无 data race）
  std::atomic<uint64_t> m_acquire_count{0};
  std::atomic<uint64_t> m_release_count{0};
  std::atomic<uint64_t> m_discard_count{0};
  std::atomic<uint64_t> m_hit_count{0};
};

}  // namespace qcloud_cos

#endif  // COS_CPP_SDK_V5_SRC_UTIL_CONNECTION_POOL_H_
