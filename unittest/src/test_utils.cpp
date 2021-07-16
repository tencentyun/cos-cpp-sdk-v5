#include "test_utils.h"

#include <stdlib.h>
#include <stdio.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/StreamCopier.h"

namespace qcloud_cos {

void TestUtils::WriteRandomDatatoFile(const std::string &file, int len) {
    std::fstream fs(file, std::ios::out | std::ios::binary | std::ios::trunc);
    fs << TestUtils::GetRandomString(len);
    fs.close();
}

void TestUtils::RemoveFile(const std::string& file) {
    ::remove(file.c_str());
}

std::string TestUtils::GetRandomString(int len) {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmopqrstuvwxyz";
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

std::string TestUtils::GetEnv(const std::string& env_var_name) {
    char const* tmp = getenv(env_var_name.c_str());
    if (tmp == NULL) {
        return "NOT_EXIST_ENV_" + env_var_name;
    }

    return std::string(tmp);
}
}

