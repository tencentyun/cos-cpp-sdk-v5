#pragma once

#include <string>

namespace qcloud_cos {
class TestUtils {
public:
    static void WriteRandomDatatoFile(const std::string &file, int len);
    static void RemoveFile(const std::string& file);
    static std::string GetRandomString(int size);
    static std::string CalcFileMd5(const std::string& file);
    static std::string GetEnv(const std::string& env_var_name);
};
#define GetEnv TestUtils::GetEnv
}
