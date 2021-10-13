#pragma once

#include <string>
#include <iostream>

namespace qcloud_cos {
class TestUtils {
public:
    static void WriteRandomDatatoFile(const std::string &file, int len);
    static void RemoveFile(const std::string& file);
    static std::string GetRandomString(int size);
    static std::string CalcFileMd5(const std::string& file);
    static std::string CalcStreamMd5(std::istream& is);
    static std::string CalcStringMd5(const std::string& str);
    static std::string CalcStreamSHA1(std::istream& is);
    static std::string GetEnv(const std::string& env_var_name);
};
#define GetEnv TestUtils::GetEnv
}
