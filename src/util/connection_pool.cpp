#include "connection_pool.h"

#include "cos_sys_config.h"

namespace qcloud_cos {

ConnectionPool& ConnectionPool::GetInstance() {
  static ConnectionPool instance;
  return instance;
}

std::unique_ptr<Poco::Net::HTTPClientSession> ConnectionPool::Acquire(
    const std::string& key,
    std::chrono::steady_clock::time_point* created_out) {
  // victims 在 lock_guard 之前声明: C++ 逆序析构保证其中的 session 在锁
  // 释放后才销毁 —— HTTPS 连接析构含 SSL_shutdown 网络写, 对端停滞时可
  // 阻塞到 send timeout, 不得在持锁时进行。
  std::vector<std::unique_ptr<Poco::Net::HTTPClientSession>> victims;
  std::lock_guard<std::mutex> lock(m_mutex_);
  ++m_acquire_count;

  auto iter = m_pool_.find(key);
  if (iter == m_pool_.end() || iter->second.empty()) {
    return nullptr;
  }

  const int64_t max_idle_ms =
      static_cast<int64_t>(CosSysConfig::GetConnectionPoolMaxIdleMs());
  const int64_t max_age_ms =
      static_cast<int64_t>(CosSysConfig::GetConnectionPoolMaxAgeMs());
  const auto now = std::chrono::steady_clock::now();

  // 从空闲列表尾部取出一个连接（LIFO 策略，最新归还的最可能还活着）。
  // 取用时必须校验空闲时间与生命周期: 驱逐仅发生在 Release/Evict 时，
  // 长时间无请求的 key 下会残留早已被服务端关闭的连接。
  std::unique_ptr<Poco::Net::HTTPClientSession> result;
  while (!iter->second.empty()) {
    PooledSession entry = std::move(iter->second.back());
    iter->second.pop_back();
    const int64_t idle_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now - entry.last_used).count();
    const int64_t age_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - entry.created).count();
    if (idle_ms > max_idle_ms || age_ms > max_age_ms) {
      ++m_discard_count;
      victims.push_back(std::move(entry.session));  // 锁外析构
      continue;
    }
    if (created_out != nullptr) {
      *created_out = entry.created;
    }
    result = std::move(entry.session);
    break;
  }

  if (iter->second.empty()) {
    m_pool_.erase(iter);
  }
  if (result) {
    ++m_hit_count;
  }
  return result;
}

void ConnectionPool::Release(
    const std::string& key,
    std::unique_ptr<Poco::Net::HTTPClientSession> session,
    bool is_healthy,
    std::chrono::steady_clock::time_point created) {
  if (!session) return;

  // victims 在 lock_guard 之前声明: C++ 逆序析构保证其中的 session 在锁
  // 释放后才销毁 —— HTTPS 连接析构含 SSL_shutdown 网络写, 对端停滞时可
  // 阻塞到 send timeout, 不得在持锁时进行。
  std::vector<std::unique_ptr<Poco::Net::HTTPClientSession>> victims;
  std::lock_guard<std::mutex> lock(m_mutex_);

  if (!is_healthy) {
    // 不健康的连接直接丢弃(移入 victims, 锁外析构)
    ++m_discard_count;
    victims.push_back(std::move(session));
    return;
  }

  size_t max_pool_size = CosSysConfig::GetConnectionPoolSize();
  size_t max_idle_ms = CosSysConfig::GetConnectionPoolMaxIdleMs();
  size_t max_age_ms = CosSysConfig::GetConnectionPoolMaxAgeMs();

  auto& idle_list = m_pool_[key];
  auto now = std::chrono::steady_clock::now();

  // 1. 统一先驱逐该 key 下已过期的空闲连接
  while (!idle_list.empty()) {
    auto& oldest = idle_list.front();
    auto idle_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - oldest.last_used).count();
    auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - oldest.created).count();
    if (idle_duration > static_cast<int64_t>(max_idle_ms) ||
        age > static_cast<int64_t>(max_age_ms)) {
      ++m_discard_count;
      victims.push_back(std::move(oldest.session));  // 锁外析构
      idle_list.pop_front();
    } else {
      break;
    }
  }

  // 2. 若驱逐后仍满, 挤掉最老的空闲连接为新连接腾空间
  if (idle_list.size() >= max_pool_size) {
    ++m_discard_count;
    victims.push_back(std::move(idle_list.front().session));  // 锁外析构
    idle_list.pop_front();
  }

  // 3. 归还连接
  // created 必须沿用连接真实建立时间（由 Acquire 透传回来），否则每次归还
  // 都会重置连接年龄, MaxAge 驱逐策略将永远不会生效。
  PooledSession entry;
  entry.session = std::move(session);
  entry.last_used = now;
  entry.created =
      (created == std::chrono::steady_clock::time_point()) ? now : created;
  idle_list.push_back(std::move(entry));
  ++m_release_count;

  // 4. 低频全局清扫: 上面第 1 步只清理了当前 key, 而再也不会被访问的 host
  // (切换 region、临时域名等) 其空闲 fd 会一直驻留到进程退出。此处做时间
  // 门控, 最多每秒扫一次, 摊销开销可忽略。
  // 注意: 已持有 m_mutex_, 必须调用免锁版本。
  if (std::chrono::duration_cast<std::chrono::milliseconds>(
          now - m_last_sweep_).count() >= 1000) {
    m_last_sweep_ = now;
    EvictIdleConnectionsLocked(now, &victims);
  }
}

void ConnectionPool::ReportStaleDiscard() {
  // Acquire 已计入 Hit, 此处回退, 保证 Hit 只统计真实完成复用的连接
  --m_hit_count;
  ++m_discard_count;
}

void ConnectionPool::EvictIdleConnections() {
  // victims 在 lock_guard 之前声明: C++ 逆序析构保证其中的 session 在锁
  // 释放后才销毁(原因见 Release 中的说明)。
  std::vector<std::unique_ptr<Poco::Net::HTTPClientSession>> victims;
  std::lock_guard<std::mutex> lock(m_mutex_);
  EvictIdleConnectionsLocked(std::chrono::steady_clock::now(), &victims);
}

void ConnectionPool::EvictIdleConnectionsLocked(
    const std::chrono::steady_clock::time_point& now,
    std::vector<std::unique_ptr<Poco::Net::HTTPClientSession>>* victims) {
  // 调用方必须已持有 m_mutex_; 此处不得再加锁, 也不得调用 GetTotalPoolSize()。
  // 被驱逐的连接移入 victims, 由调用方在锁释放后统一析构。
  size_t max_idle_ms = CosSysConfig::GetConnectionPoolMaxIdleMs();
  size_t max_age_ms = CosSysConfig::GetConnectionPoolMaxAgeMs();

  auto pool_iter = m_pool_.begin();
  while (pool_iter != m_pool_.end()) {
    auto& idle_list = pool_iter->second;
    while (!idle_list.empty()) {
      auto& oldest = idle_list.front();
      auto idle_duration =
          std::chrono::duration_cast<std::chrono::milliseconds>(
              now - oldest.last_used).count();
      auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
          now - oldest.created).count();
      if (idle_duration > static_cast<int64_t>(max_idle_ms) ||
          age > static_cast<int64_t>(max_age_ms)) {
        ++m_discard_count;
        victims->push_back(std::move(oldest.session));  // 锁外析构
        idle_list.pop_front();
      } else {
        break;
      }
    }
    if (idle_list.empty()) {
      pool_iter = m_pool_.erase(pool_iter);
    } else {
      ++pool_iter;
    }
  }
}

void ConnectionPool::CloseAll() {
  // 注意: 不能在持锁状态下调用 GetTotalPoolSize() —— 它内部会再次加锁
  // 同一非递归 mutex, 造成死锁。此处改为锁内直接遍历统计。
  // to_destroy 在 lock_guard 之前声明: C++ 逆序析构保证其中的连接在锁释放
  // 后才销毁 —— 一次析构全部连接时, 任何一条 HTTPS 连接的 SSL_shutdown
  // 阻塞都不再影响其它线程的 Acquire/Release。
  std::unordered_map<std::string, std::list<PooledSession>> to_destroy;
  std::lock_guard<std::mutex> lock(m_mutex_);
  size_t total = 0;
  for (const auto& kv : m_pool_) {
    total += kv.second.size();
  }
  m_discard_count += total;
  to_destroy.swap(m_pool_);
}

size_t ConnectionPool::GetPoolSize(const std::string& key) const {
  std::lock_guard<std::mutex> lock(m_mutex_);
  auto iter = m_pool_.find(key);
  if (iter == m_pool_.end()) {
    return 0;
  }
  return iter->second.size();
}

size_t ConnectionPool::GetTotalPoolSize() const {
  std::lock_guard<std::mutex> lock(m_mutex_);
  size_t total = 0;
  for (const auto& kv : m_pool_) {
    total += kv.second.size();
  }
  return total;
}

}  // namespace qcloud_cos
