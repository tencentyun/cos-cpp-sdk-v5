#pragma once
#include <stdint.h>

#include <cstddef>

namespace qcloud_cos {
class CRC64 {
 public:
  static uint64_t CalcCRC(uint64_t crc, void *buf, size_t len);
  static uint64_t CombineCRC(uint64_t crc1, uint64_t crc2, uintmax_t len2);
  static uint64_t CalcCRC(uint64_t crc, void *buf, size_t len, bool little);
};
}  // namespace qcloud_cos
