
#include "util/file_util.h"

#include <sys/stat.h>
#include <sys/types.h>
#if defined(_WIN32)
#include <codecvt>
#endif
#include <fstream>
#include <iostream>
#include <sstream>

#include "Poco/DigestStream.h"
#include "Poco/MD5Engine.h"
#include "Poco/StreamCopier.h"
#include "cos_defines.h"
#include "cos_sys_config.h"
#include "util/codec_util.h"
#include "util/crc64.h"
#include "util/string_util.h"


namespace qcloud_cos {

std::string FileUtil::GetFileContent(const std::string& local_file_path) {
  std::ifstream file_input(local_file_path.c_str(),
                           std::ios::in | std::ios::binary);
  std::ostringstream out;

  out << file_input.rdbuf();
  std::string content = out.str();

  file_input.close();
  file_input.clear();

  return content;
}

uint64_t FileUtil::GetFileLen(const std::string& path) {
  std::ifstream file_input(path, std::ios::in | std::ios::binary);
  file_input.seekg(0, std::ios::end);
  uint64_t file_len = file_input.tellg();
  file_input.close();
  return file_len;
}

#if defined(_WIN32)
uint64_t FileUtil::GetFileLen(const std::wstring& path) {
  std::ifstream file_input(path, std::ios::in | std::ios::binary);
  file_input.seekg(0, std::ios::end);
  uint64_t file_len = file_input.tellg();
  file_input.close();
  return file_len;
}
#endif

bool FileUtil::IsDirectoryExists(const std::string& path) {
  struct stat info;
  if (0 == stat(path.c_str(), &info) && info.st_mode & S_IFDIR) {
    return true;
  } else {
    return false;
  }
}

bool FileUtil::IsDirectory(const std::string& path) {
  return IsDirectoryExists(path);
}

std::string FileUtil::GetDirectory(const std::string& path) {
  size_t found = path.find_last_of("/\\");
  return (path.substr(0, found));
}

uint64_t FileUtil::GetFileCrc64(const std::string& file) {
  std::fstream f(file, std::ios::in | std::ios::binary);
  const static int buffer_size = 2048;
  char buffer[buffer_size];
  uint64_t crc64 = 0;
  while (f.good()) {
    f.read(buffer, buffer_size);
    auto bytes_read = f.gcount();
    if (bytes_read > 0) {
      crc64 = CRC64::CalcCRC(crc64, static_cast<void*>(buffer),
                             static_cast<size_t>(bytes_read));
    }
  }
  f.close();
  return crc64;
}

#if defined(_WIN32)
uint64_t FileUtil::GetFileCrc64(const std::wstring& file) {
  std::fstream f(file, std::ios::in | std::ios::binary);
  const static int buffer_size = 2048;
  char buffer[buffer_size];
  uint64_t crc64 = 0;
  while (f.good()) {
    f.read(buffer, buffer_size);
    auto bytes_read = f.gcount();
    if (bytes_read > 0) {
      crc64 = CRC64::CalcCRC(crc64, static_cast<void*>(buffer),
                             static_cast<size_t>(bytes_read));
    }
  }
  f.close();
  return crc64;
}

std::wstring FileUtil::GetWideCharFilePath(const std::string file_path) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.from_bytes(file_path);
}
#endif

std::string FileUtil::GetFileMd5(const std::string& file) {
  std::ifstream ifs(file);
  Poco::MD5Engine md5;
  Poco::DigestOutputStream md5_dos(md5);
  Poco::StreamCopier::copyStream(ifs, md5_dos);
  ifs.close();
  md5_dos.close();
  return Poco::DigestEngine::digestToHex(md5.digest());
}

}  // namespace qcloud_cos
