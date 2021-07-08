#ifndef FILE_UTIL_H
#define FILE_UTIL_H
#include <stdint.h>

#include <fstream>
#include <iostream>
#include <string>

namespace qcloud_cos {

    class FileUtil {
    public:
        //获取文件内容
        static std::string GetFileContent(const std::string& path);

        //返回文件大小
        static uint64_t GetFileLen(const std::string& path);

        static bool IsDirectoryExists(const std::string& path);

        static bool IsDirectory(const std::string& path);

        static std::string GetDirectory(const std::string& path);

        static uint64_t GetFileCrc64(const std::string& file);
    };
} // namespace qcloud_cos

#endif
