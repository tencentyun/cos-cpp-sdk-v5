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
  static std::string GetEnvVar(const std::string& env_var_name);
  #if defined(__linux__)
  static bool IsDirectoryExists(const std::string& path);
  static bool MakeDirectory(const std::string& path);
  static bool RemoveDirectory(const std::string& path);
  #endif
};
#define GetEnvVar TestUtils::GetEnvVar

struct FileInfo {
  std::string m_object_name;
  std::string m_local_file;
  std::string m_local_file_download;
  uint64_t m_file_size;
  uint64_t m_file_crc64_origin;
  std::string m_file_md5_origin;
  int m_op_type;
};

}  // namespace qcloud_cos
