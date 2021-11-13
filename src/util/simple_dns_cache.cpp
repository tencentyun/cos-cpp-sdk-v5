
#include "util/simple_dns_cache.h"

#include <stdlib.h>

#include <chrono>
#include <cstdlib>

#include "cos_defines.h"
#include "cos_sys_config.h"

namespace qcloud_cos {

SimpleDnsCache::SimpleDnsCache(unsigned max_size, unsigned expire_seconds)
    : m_max_size(max_size), m_expire_seconds(expire_seconds) {
  m_cache = std::make_shared<LruCache<std::string, HostEntryCache>>(m_max_size);
}

SimpleDnsCache::~SimpleDnsCache() {}

std::string SimpleDnsCache::Resolve(const std::string& host) {
  std::string ip_addr_str = "";
  if (host.empty()) {
    return ip_addr_str;
  }

  bool need_query_dns_server = false;
  HostEntryCache entry_cache;
  time_t current_ts = time(NULL);
  try {
    entry_cache = m_cache->Get(host);
    SDK_LOG_DBG("%s hit dns cache", host.c_str());
    if ((current_ts - entry_cache.cache_ts) > m_expire_seconds) {
      SDK_LOG_DBG("%s cache expired", host.c_str());
      need_query_dns_server = true;
    }
    // SDK_LOG_DBG("current_ts: %u, cache_ts: %u", current_ts,
    // entry_cache.cache_ts);
  } catch (const std::exception& ex) {
    SDK_LOG_DBG("%s not exists in cache", host.c_str());
    need_query_dns_server = true;
  }

  if (need_query_dns_server) {
    std::chrono::time_point<std::chrono::steady_clock> start_ts, end_ts;
    start_ts = std::chrono::steady_clock::now();
    Poco::Net::HostEntry host_entry = Poco::Net::DNS::hostByName(host);
    end_ts = std::chrono::steady_clock::now();
    unsigned time_consumed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_ts - start_ts)
            .count();
    SDK_LOG_DBG("query dns server for host: %s, consume: %dms", host.c_str(),
                time_consumed_ms);
    entry_cache.host_entry = host_entry;
    entry_cache.cache_ts = current_ts;
    m_cache->Put(host, entry_cache);
  }

  size_t address_size = entry_cache.host_entry.addresses().size();
  if (address_size > 0) {
    unsigned slot = 0;
    if (address_size > 1) {
      std::srand(current_ts);
      slot = std::rand() % (address_size - 1);
    }
    ip_addr_str = entry_cache.host_entry.addresses()[slot].toString();
  }
  SDK_LOG_DBG("ip_addr_str: %s", ip_addr_str.c_str());
  return ip_addr_str;
}

bool SimpleDnsCache::Exist(const std::string& host) {
  return m_cache->Exist(host);
}

}  // namespace qcloud_cos