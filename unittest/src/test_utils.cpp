#include "test_utils.h"

#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <sstream>

#include "Poco/DigestStream.h"
#include "Poco/MD5Engine.h"
#include "Poco/SHA1Engine.h"
#include "Poco/StreamCopier.h"

namespace qcloud_cos {

void TestUtils::WriteStringtoFile(const std::string& file,
                                  const std::string& str) {
  std::fstream fs(file, std::ios::out | std::ios::binary | std::ios::trunc);
  fs << str;
  fs.close();
}
void TestUtils::WriteRandomDatatoFile(const std::string& file, unsigned len) {
  std::fstream fs(file, std::ios::out | std::ios::binary | std::ios::trunc);
  fs << TestUtils::GetRandomString(len);
  fs.close();
}

void TestUtils::RemoveFile(const std::string& file) { ::remove(file.c_str()); }

std::string TestUtils::GetRandomString(unsigned len) {
  static const char alphanum[] =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmopqrstuvwxyz";
  std::stringstream ss;
  for (int i = 0; i < len; ++i) {
    ss << alphanum[rand() % (sizeof(alphanum) - 1)];
  }
  return ss.str();
}

std::string TestUtils::CalcFileMd5(const std::string& file) {
  std::ifstream ifs(file);
  Poco::MD5Engine md5;
  Poco::DigestOutputStream md5_dos(md5);
  Poco::StreamCopier::copyStream(ifs, md5_dos);
  ifs.close();
  md5_dos.close();
  return Poco::DigestEngine::digestToHex(md5.digest());
}

std::string TestUtils::CalcStreamMd5(std::istream& is) {
  Poco::MD5Engine md5;
  Poco::DigestOutputStream md5_dos(md5);
  Poco::StreamCopier::copyStream(is, md5_dos);
  md5_dos.close();
  return Poco::DigestEngine::digestToHex(md5.digest());
}

std::string TestUtils::CalcStringMd5(const std::string& str) {
  Poco::MD5Engine md5;
  md5.update(str);
  return Poco::DigestEngine::digestToHex(md5.digest());
}

std::string TestUtils::CalcStreamSHA1(std::istream& is) {
  Poco::SHA1Engine sha1;
  Poco::DigestOutputStream sha1_dos(sha1);
  Poco::StreamCopier::copyStream(is, sha1_dos);
  sha1_dos.close();
  return Poco::DigestEngine::digestToHex(sha1.digest());
}

std::string TestUtils::GetEnvVar(const std::string& env_var_name) {
  char const* tmp = getenv(env_var_name.c_str());
  if (tmp == NULL) {
    return "NOT_EXIST_ENV_" + env_var_name;
  }

  return std::string(tmp);
}
}  // namespace qcloud_cos
