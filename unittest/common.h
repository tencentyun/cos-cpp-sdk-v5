// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:

#include <stdlib.h>

#include <string>

namespace qcloud_cos {

std::string GetEnv(const std::string& env_var_name) {
    char const* tmp = getenv(env_var_name.c_str());
    if (tmp == NULL) {
        return "NOT_EXIST_ENV_" + env_var_name;
    }

    return std::string(tmp);
}

}
