#pragma once

#include <iostream>
#include <list>
#include <mutex>
#include <stdexcept>
#include <unordered_map>

namespace qcloud_cos {

template <typename KeyType, typename ValueType>
class LruCache {
 public:
  using KeyValuePair = std::pair<KeyType, ValueType>;
  using ListIterator = typename std::list<KeyValuePair>::iterator;

  LruCache(size_t size) : m_max_size(size) {}

  ~LruCache() {}

  void Put(const KeyType& key, const ValueType& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_entry_list.emplace_front(key, value);
    auto it = m_entry_map.find(key);
    if (it != m_entry_map.end()) {
      m_entry_list.erase(it->second);
    } else {
      if (m_entry_list.size() > m_max_size) {
        m_entry_map.erase(m_entry_list.back().first);
        m_entry_list.pop_back();
      }
    }
    m_entry_map[key] = m_entry_list.begin();
  }

  const ValueType Get(const KeyType& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_entry_map.find(key);
    if (it == m_entry_map.end()) {
      throw std::range_error("No such key in cache");
    } else {
      m_entry_list.emplace_front(key, it->second->second);
      m_entry_list.erase(it->second);
      m_entry_map[key] = m_entry_list.begin();
      return m_entry_list.begin()->second;
    }
  }

  bool Exist(const KeyType& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_entry_map.find(key) != m_entry_map.end();
  }

  size_t Size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_entry_map.size();
  }

 private:
  std::list<KeyValuePair> m_entry_list;
  std::unordered_map<KeyType, ListIterator> m_entry_map;
  size_t m_max_size;
  mutable std::mutex m_mutex;
};

}  // namespace qcloud_cos