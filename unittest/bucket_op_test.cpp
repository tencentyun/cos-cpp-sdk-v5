// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 08/23/17
// Description:

#include "gtest-1.7.0/include/gtest/gtest.h"

#include "cos_sys_config.h"
#include "mock_server.h"
#include "op/bucket_op.h"
#include "threadpool/boost/threadpool.hpp"
namespace qcloud_cos {

// 启动MockServer
void RunServer(Poco::Net::HTTPServer* http_server) {
    http_server->start();
}

class BucketOpTest : public testing::Test {
public:
    BucketOpTest() : m_config("./config.json"), m_bucket_op(m_config) {
    }
    ~BucketOpTest() {}

protected:
    static void SetUpTestCase() {
        m_http_server = new Poco::Net::HTTPServer(new MockRequestHandlerFactory,
                                      Poco::Net::ServerSocket(7777),
                                      new Poco::Net::HTTPServerParams);
        m_thread_pool.schedule(boost::bind(&RunServer, m_http_server) );
        CosSysConfig::SetDestDomain("127.0.0.1:7777");
        sleep(1);
    }

    static void TearDownTestCase() {
        m_thread_pool.wait();
        if (m_http_server != NULL) {
            m_http_server->stop();
        }
    }

protected:
    static boost::threadpool::pool m_thread_pool;
    CosConfig m_config;
    BucketOp m_bucket_op;
    static Poco::Net::HTTPServer* m_http_server;
};

boost::threadpool::pool BucketOpTest::m_thread_pool(2);
Poco::Net::HTTPServer* BucketOpTest::m_http_server = NULL;

TEST_F(BucketOpTest, GetBucketTest) {
    GetBucketReq req("bucket_test");
    GetBucketResp resp;
    req.SetPrefix("test/");
    req.SetDelimiter("delimiter");
    req.SetEncodingType("url");
    req.SetMarker("marker");
    req.SetMaxKeys(100);
    CosResult result = m_bucket_op.GetBucket(req, &resp);

    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("bucket_test", resp.GetName());
    EXPECT_EQ("url", resp.GetEncodingType());
    EXPECT_EQ("test/", resp.GetPrefix());
    EXPECT_TRUE(resp.GetMarker().empty());
    EXPECT_EQ(100, resp.GetMaxKeys());
    EXPECT_FALSE(resp.IsTruncated());
    EXPECT_EQ("1234.txt", resp.GetNextMarker());
    const std::vector<Content>& contents = resp.GetContents();
    EXPECT_EQ(2, contents.size());

    const Content& cnt_01 = contents[0];
    EXPECT_EQ("sevenyoutest01", cnt_01.m_key);
    EXPECT_EQ("2017-06-23T12:33:26.000Z", cnt_01.m_last_modified);
    EXPECT_EQ("39bfb88c11c65ed6424d2e1cd4db1826", cnt_01.m_etag);
    EXPECT_EQ("10485760", cnt_01.m_size);
    ASSERT_EQ(1, cnt_01.m_owner_ids.size());
    EXPECT_EQ("77777", (cnt_01.m_owner_ids)[0]);
    EXPECT_EQ("STANDARD", cnt_01.m_storage_class);
}

} // namespace qcloud_cos


int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
