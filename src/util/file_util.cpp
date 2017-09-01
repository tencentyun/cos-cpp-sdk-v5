#include "util/file_util.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "cos_defines.h"
#include "cos_sys_config.h"
#include "util/codec_util.h"
#include "util/string_util.h"

namespace qcloud_cos {

std::string FileUtil::GetFileContent(const std::string& local_file_path) {
    std::ifstream file_input(local_file_path.c_str(), std::ios::in | std::ios::binary);
    std::ostringstream out;

    out << file_input.rdbuf();
    std::string content = out.str();

    file_input.close();
    file_input.clear();

    return content;
}

uint64_t FileUtil::GetFileLen(const std::string& local_file_path) {
    std::ifstream file_input(local_file_path.c_str(), std::ios::in | std::ios::binary);
    file_input.seekg(0, std::ios::end);
    uint64_t file_len = file_input.tellg();
    file_input.close();
    return file_len;
}
} //namespace qcloud_cos
