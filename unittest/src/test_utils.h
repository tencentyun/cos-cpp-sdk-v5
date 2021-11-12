#pragma once

#include <iostream>
#include <string>

namespace qcloud_cos {
class TestUtils {
 public:
  static void WriteStringtoFile(const std::string& file,
                                const std::string& str);
  static void WriteRandomDatatoFile(const std::string& file, unsigned len);
  static void RemoveFile(const std::string& file);
  static std::string GetRandomString(unsigned size);
  static std::string CalcFileMd5(const std::string& file);
  static std::string CalcStreamMd5(std::istream& is);
  static std::string CalcStringMd5(const std::string& str);
  static std::string CalcStreamSHA1(std::istream& is);
  static std::string GetEnv(const std::string& env_var_name);
};
#define GetEnv TestUtils::GetEnv
}  // namespace qcloud_cos
