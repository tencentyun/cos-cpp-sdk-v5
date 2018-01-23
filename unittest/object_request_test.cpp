// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:

#include "gtest-1.7.0/include/gtest/gtest.h"

#include <iostream>

#include "request/object_req.h"

namespace qcloud_cos {

TEST(ObjectReqTest, NormalTest) {
    std::string bucket_name = "ut_bucket_01";
    std::string object_name = "ut_object_01";
    std::string local_file_path = "ut_file_01";
    {
        GetObjectByFileReq req(bucket_name, object_name);
        EXPECT_EQ("GET", req.GetMethod());
        EXPECT_EQ(bucket_name, req.GetBucketName());
        EXPECT_EQ("/" + object_name, req.GetPath());
        EXPECT_EQ("./" + object_name, req.GetLocalFilePath());

        req.SetLocalFilePath(local_file_path);
        EXPECT_EQ(local_file_path, req.GetLocalFilePath());

        req.AddParam("response-content-type", "application/octet-stream");
        req.AddParam("response-content-language", "Content-language");
        EXPECT_EQ(2, req.GetParams().size());
        std::map<std::string, std::string> params;
        params.insert(std::make_pair("response-content-type", "application/xml"));
        req.AddParams(params);
        EXPECT_EQ(2, req.GetParams().size());
        EXPECT_EQ("application/xml", req.GetParam("response-content-type"));
        EXPECT_EQ("", req.GetParam("EMPTY_STRING"));
        req.ClearParams();

        req.AddHeader("Range", "0-100");
        req.AddHeader("If-Unmodified-Since", "Unmodified-Date");
        // 还有UA
        EXPECT_EQ(3, req.GetHeaders().size());
        std::map<std::string, std::string> headers;
        headers.insert(std::make_pair("ADD_HEADER", "add_header"));
        req.AddHeaders(headers);
        EXPECT_EQ(4, req.GetHeaders().size());
        EXPECT_EQ("0-100", req.GetHeader("Range"));
        EXPECT_EQ("", req.GetHeader("EMPTY_STRING"));
        req.ClearHeaders();
        EXPECT_EQ(0, req.GetHeaders().size());
    }

    {
        PutObjectByFileReq req(bucket_name, object_name);
        EXPECT_EQ("PUT", req.GetMethod());
        EXPECT_EQ(bucket_name, req.GetBucketName());
        EXPECT_EQ("/" + object_name, req.GetPath());
        EXPECT_EQ("./" + object_name, req.GetLocalFilePath());

        req.SetLocalFilePath(local_file_path);
        EXPECT_EQ(local_file_path, req.GetLocalFilePath());
    }

    {
        HeadObjectReq req(bucket_name, object_name);
        EXPECT_EQ("HEAD", req.GetMethod());
    }

    {
        InitMultiUploadReq req(bucket_name, object_name);
        EXPECT_EQ("POST", req.GetMethod());
    }

    {
        std::ifstream is;
        UploadPartDataReq req(bucket_name, object_name, "ut_upload_id", is);
        EXPECT_EQ("PUT", req.GetMethod());
        EXPECT_EQ("ut_upload_id", req.GetUploadId());
        req.SetUploadId("ut_upload_id_2");
        EXPECT_EQ("ut_upload_id_2", req.GetUploadId());
        EXPECT_EQ(1, req.GetPartNumber());
        req.SetPartNumber(2);
        EXPECT_EQ(2, req.GetPartNumber());
    }

    {
        CompleteMultiUploadReq req(bucket_name, object_name, "ut_upload_id");
        EXPECT_EQ("POST", req.GetMethod());
        EXPECT_EQ("ut_upload_id", req.GetUploadId());
        req.SetUploadId("ut_upload_id_2");
        EXPECT_EQ("ut_upload_id_2", req.GetUploadId());

        EXPECT_EQ(0, req.GetPartNumbers().size());
        std::vector<uint64_t> part_numbers_1;
        part_numbers_1.push_back(1);
        part_numbers_1.push_back(2);
        req.SetPartNumbers(part_numbers_1);
        EXPECT_EQ(2, req.GetPartNumbers().size());
        std::vector<uint64_t> part_numbers_2;
        part_numbers_2.push_back(3);
        req.SetPartNumbers(part_numbers_2);
        EXPECT_EQ(1, req.GetPartNumbers().size());

        EXPECT_EQ(0, req.GetEtags().size());
        std::vector<std::string> etags_1;
        etags_1.push_back("etag_1");
        etags_1.push_back("etag_2");
        req.SetEtags(etags_1);
        EXPECT_EQ(2, req.GetEtags().size());
        std::vector<std::string> etags_2;
        etags_2.push_back("etag_3");
        req.SetEtags(etags_2);
        EXPECT_EQ(1, req.GetEtags().size());


        req.AddPartEtagPair(4, "etag_4");
        EXPECT_TRUE(req.GetPartNumbers().size() == req.GetEtags().size());
    }
}

} // namespace qcloud_cos

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
