// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 08/11/17
// Description:

#include "gtest/gtest.h"

#include "util/auth_tool.h"
#include <iostream>

namespace qcloud_cos {

TEST(AuthToolTest, NormalTest) {
    std::string access_key = "access_key_test";
    std::string secret_key = "secret_key_test";
    std::string http_method = "GET";
    std::string in_uri = "xxxx";

    std::map<std::string, std::string> headers;
    headers["host"] = "hostname_test";
    headers["x-cos-meta-xx"] = "meta_test";
    std::map<std::string, std::string> params;
    params["first_param"] = "first_value";
    params["second_param"] = "second_value";

    std::string expected = "q-sign-algorithm=sha1&q-ak=access_key_test&q-sign-time="
                           "1502493430;1502573430&q-key-time=1502493430;1502573430&"
                           "q-header-list=host;x-cos-meta-xx&q-url-param-list=first"
                           "_param;second_param&q-signature=7686473b75487163d6e204d"
                           "3c68e5c3b74e06164";

    // 1. header、param均非空
    std::string sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                                             headers, params, 1502493430, 1502573430);

    //sign_result.resize(sign_result.size() - 1);
    EXPECT_EQ(expected, sign_result);

    // 2.头部包含无需签名部分
    headers["unsigned_header"] = "unsigned_header_content";
    sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                                             headers, params, 1502493430, 1502573430);

    //sign_result.resize(sign_result.size() - 1);
    EXPECT_EQ(expected, sign_result);

    // 3. params为空
    std::string no_param_expected = "q-sign-algorithm=sha1&q-ak=access_key_test&q-sig"
                                    "n-time=1502493430;1502573430&q-key-time=15024934"
                                    "30;1502573430&q-header-list=host;x-cos-meta-xx&q"
                                    "-url-param-list=&q-signature=8e196637993353bdff2"
                                    "466e0d34034130c9a2aca";
    params.clear();
    sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                                 headers, params, 1502493430, 1502573430);

    //sign_result.resize(sign_result.size() - 1);
    EXPECT_EQ(no_param_expected, sign_result);

    // 4. headers/params为空
    std::string empty_expected = "q-sign-algorithm=sha1&q-ak=access_key_test&q-sign-t"
                                 "ime=1502493430;1502573430&q-key-time=1502493430;150"
                                 "2573430&q-header-list=&q-url-param-list=&q-signatur"
                                 "e=122c75ad3f7c230b8cc88f4f38410b9d3d347997";
    headers.clear();
    sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                                 headers, params, 1502493430, 1502573430);

    //sign_result.resize(sign_result.size() - 1);
    EXPECT_EQ(empty_expected, sign_result);

    // 5. access_key为空
    access_key = "";
    sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                                 headers, params, 1502493430, 1502573430);
    EXPECT_EQ("", sign_result);

    // 6. secret_key为空
    access_key = "access_key_test";
    secret_key = "";
    sign_result = AuthTool::Sign(access_key, secret_key, http_method, in_uri,
                                 headers, params, 1502493430, 1502573430);
    EXPECT_EQ("", sign_result);
}

} // namespace qcloud_cos
