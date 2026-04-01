#pragma once

#include "Poco/MD5Engine.h"
#include "cos_api.h"
#include "op/object_op.h"
#include "util/file_util.h"
#include "util/simple_dns_cache.h"
#include "util/test_utils.h"
#include "gtest/gtest.h"
#include <sstream>
#include <thread>

/*
export CPP_SDK_V5_ACCESS_KEY=xxx
export CPP_SDK_V5_SECRET_KEY=xxx
export CPP_SDK_V5_REGION=ap-guangzhou
export CPP_SDK_V5_UIN=xxx
export CPP_SDK_V5_APPID=xxx
export COS_CPP_V5_USE_DNS_CACHE="true"

export CPP_SDK_V5_OTHER_ACCESS_KEY=xxx
export CPP_SDK_V5_OTHER_SECRET_KEY=xxx
export CPP_SDK_V5_OTHER_REGION=ap-hongkong
export CPP_SDK_V5_OTHER_UIN=xxx
*/

namespace qcloud_cos {

// SSL上下文回调函数（供测试用例使用）
int SslCtxCallback(void* ssl_ctx, void* data);

class ObjectOpTest : public testing::Test {
protected:
    static void SetUpTestCase();
    static void TearDownTestCase();

private:
    static void InitConfig();

protected:
    static CosConfig* m_config;
    static CosAPI* m_client;
    static std::string m_bucket_name;
    static std::string m_bucket_name2; // 用于copy

    // 用于记录单测中未Complete的分块上传uploadID,便于清理
    static std::map<std::string, std::string> m_to_be_aborted;
};

} // namespace qcloud_cos
