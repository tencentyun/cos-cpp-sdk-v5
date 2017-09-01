// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:

#include "gtest-1.7.0/include/gtest/gtest.h"

#include "cos_sys_config.h"
#include "mock_server.h"
#include "op/object_op.h"
#include "threadpool/boost/threadpool.hpp"

namespace qcloud_cos {

// 启动MockServer
void RunServer(Poco::Net::HTTPServer* http_server) {
    http_server->start();
}

class ObjectOpTest : public testing::Test {
public:
    ObjectOpTest() : m_config("./config.json"), m_object_op(m_config) {
    }
    ~ObjectOpTest() {}

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
    ObjectOp m_object_op;
    std::vector<std::string> m_etags;
    std::vector<uint64_t> m_part_numbers;
    static Poco::Net::HTTPServer* m_http_server;
};

boost::threadpool::pool ObjectOpTest::m_thread_pool(2);
Poco::Net::HTTPServer* ObjectOpTest::m_http_server = NULL;

TEST_F(ObjectOpTest, HeadObjectTest) {
    HeadObjectReq req("bucket_test", "object_test_1m");
    HeadObjectResp resp;
    CosResult result = m_object_op.HeadObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());

    EXPECT_EQ(kMockLastModified, resp.GetLastModified());
    EXPECT_EQ(kMockHeadContentType, resp.GetContentType());
    EXPECT_EQ(kMockHeadETag, resp.GetEtag());
    EXPECT_EQ(kMockObjectTypeNormal, resp.GetXCosObjectType());
    EXPECT_EQ(kMockHeadReqId, resp.GetXCosRequestId());
    EXPECT_EQ(1048576, resp.GetContentLength());
    EXPECT_EQ(kStorageClassStandard, resp.GetXCosStorageClass());
}

TEST_F(ObjectOpTest, PutObjectByFileTest) {
    PutObjectByFileReq req("bucket_test", "object_test_1m", "sevenyou.txt");
    req.SetXCosStorageClass(kStorageClassStandardIA);
    PutObjectByFileResp resp;
    CosResult result = m_object_op.PutObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());

    EXPECT_EQ(kMockPutObjectContentType, resp.GetContentType());
    EXPECT_EQ(kMockPutObjectETag, resp.GetEtag());
}

TEST_F(ObjectOpTest, GetObjectByFileTest) {
    GetObjectByFileReq req("bucket_test", "object_test_1m", "./sevenyou2.txt");
    GetObjectByFileResp resp;

    CosResult result = m_object_op.GetObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ(kMockGetObjectContentType, resp.GetContentType());
    EXPECT_EQ(kMockHeadETag, resp.GetEtag());
    EXPECT_EQ(kMockGetObjectReqId, resp.GetXCosRequestId());
    EXPECT_EQ(1048576, resp.GetContentLength());
    EXPECT_EQ(kStorageClassStandardIA, resp.GetXCosStorageClass());
    EXPECT_EQ(kMockObjectTypeNormal, resp.GetXCosObjectType());
}

TEST_F(ObjectOpTest, InitMultiUploadTest) {
    InitMultiUploadReq req("bucket_test", "object_test_multi");
    InitMultiUploadResp resp;
    req.SetCacheControl("no-cache");
    req.SetContentDisposition("attachment;filename=object_test_multi.txt");
    req.SetContentEncoding("gzip");
    req.SetContentType("application/octet-stream");
    req.SetExpires("Sat, 22 Jul 2017 08:42:09 GMT");
    req.SetXCosMeta("test_meta_key", "test_meta_value");
    req.SetXCosStorageClass(kMockObjectTypeNormal);
    req.SetXCosAcl("public-read-write");

    CosResult result = m_object_op.InitMultiUpload(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ(kMockInitMultiUploadContentType, resp.GetContentType());
    EXPECT_EQ(kMockInitMultiUploadReqId, resp.GetXCosRequestId());
    EXPECT_EQ(kMockUploadId, resp.GetUploadId());
    EXPECT_EQ("object_test_multi", resp.GetKey());
    EXPECT_EQ("bucket_test", resp.GetBucket());
}

TEST_F(ObjectOpTest, UploadPartDataTest) {
    // Part 1
    {
        // 直接用GetObject下载的文件
        std::fstream is("./sevenyou2.txt");
        UploadPartDataReq req("bucket_test", "object_test_multi", kMockUploadId, is);
        UploadPartDataResp resp;
        req.SetPartNumber(1);

        CosResult result = m_object_op.UploadPartData(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        EXPECT_EQ(kMockUploadPartContentType, resp.GetContentType());
        EXPECT_EQ(kMockUploadPartReqId, resp.GetXCosRequestId());
        EXPECT_EQ(kMockUploadPartETag, resp.GetEtag());
        m_etags.push_back(resp.GetEtag());
        m_part_numbers.push_back(1);
        is.close();
    }

    // Part 2
    {
        // 直接用GetObject下载的文件
        std::fstream is("./sevenyou2.txt");
        UploadPartDataReq req("bucket_test", "object_test_multi", kMockUploadId, is);
        UploadPartDataResp resp;
        req.SetPartNumber(2);

        CosResult result = m_object_op.UploadPartData(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        EXPECT_EQ(kMockUploadPartContentType, resp.GetContentType());
        EXPECT_EQ(kMockUploadPartReqId, resp.GetXCosRequestId());
        EXPECT_EQ(kMockUploadPartETag, resp.GetEtag());
        m_etags.push_back(resp.GetEtag());
        m_part_numbers.push_back(2);
        is.close();
    }
}

TEST_F(ObjectOpTest, CompleteMultiUploadTest) {
    CompleteMultiUploadReq req("bucket_test", "object_test_multi", kMockUploadId);
    CompleteMultiUploadResp resp;
    req.SetEtags(m_etags);
    req.SetPartNumbers(m_part_numbers);

    CosResult result = m_object_op.CompleteMultiUpload(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("bucket_test-7777.cn-north.myqcloud.com/object_test_multi", resp.GetLocation());
    EXPECT_EQ("object_test_multi", resp.GetKey());
    EXPECT_EQ("bucket_test", resp.GetBucket());
    EXPECT_EQ(kMockCompleteETag, resp.GetEtag());
    EXPECT_EQ(kMockCompleteReqId, resp.GetXCosRequestId());
}

TEST_F(ObjectOpTest, MultiUploadObjectTest) {
    MultiUploadObjectReq req("bucket_test", "object_test_multi", "./sevenyou.txt");
    MultiUploadObjectResp resp;

    CosResult result = m_object_op.MultiUploadObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ("bucket_test-7777.cn-north.myqcloud.com/object_test_multi", resp.GetLocation());
    EXPECT_EQ("object_test_multi", resp.GetKey());
    EXPECT_EQ("bucket_test", resp.GetBucket());
    EXPECT_EQ(kMockCompleteETag, resp.GetEtag());
    EXPECT_EQ(kMockCompleteReqId, resp.GetXCosRequestId());
}

TEST_F(ObjectOpTest, AbortMultiUploadTest) {
    AbortMultiUploadReq req("bucket_test", "object_test_multi", kMockUploadId);
    AbortMultiUploadResp resp;

    CosResult result = m_object_op.AbortMultiUpload(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    EXPECT_EQ(kMockAbortReqId, resp.GetXCosRequestId());
}

TEST_F(ObjectOpTest, AbortMultiUploadFailedTest) {
    AbortMultiUploadReq req("bucket-----test", "object_test_multi", kMockUploadId);
    AbortMultiUploadResp resp;

    CosResult result = m_object_op.AbortMultiUpload(req, &resp);
    ASSERT_FALSE(result.IsSucc());
    EXPECT_EQ("InvalidBucketName", result.GetErrorCode());
    EXPECT_EQ("bucket name is invalid", result.GetErrorMsg());
    EXPECT_EQ("error_request_resource", result.GetResourceAddr());
    EXPECT_EQ("error_request_id", result.GetXCosRequestId());
    EXPECT_EQ("error_request_trace_id", result.GetXCosTraceId());
}

} // namespace qcloud_cos


int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
