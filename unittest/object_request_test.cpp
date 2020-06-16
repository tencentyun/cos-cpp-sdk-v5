// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:

#include "gtest/gtest.h"

#include <iostream>

#include "request/object_req.h"
#include "cos_defines.h"

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

    //test traffic limit
    {
        GetObjectByFileReq req1(bucket_name, object_name);
        req1.SetTrafficLimitByParam("1048576");
        EXPECT_EQ("1048576", req1.GetParam("x-cos-traffic-limit"));
        req1.SetTrafficLimitByHeader("1024");
        EXPECT_EQ("", req1.GetHeader("x-cos-traffic-limit"));

        
        MultiGetObjectReq req2(bucket_name, object_name, "/tmp/test");
        req2.SetTrafficLimitByHeader("1048576");
        EXPECT_EQ("1048576", req2.GetHeader("x-cos-traffic-limit"));
        req2.SetTrafficLimitByParam("2048");
        EXPECT_EQ("", req2.GetParam("x-cos-traffic-limit"));

        std::istringstream iss("");
        PutObjectByStreamReq req3(bucket_name, object_name, iss);
        req3.SetTrafficLimitByParam("4096");
        EXPECT_EQ("4096", req3.GetParam("x-cos-traffic-limit"));
        req3.SetTrafficLimitByHeader("8192");
        EXPECT_EQ("", req3.GetHeader("x-cos-traffic-limit"));

        
        PutObjectByFileReq req4(bucket_name, object_name, "/tmp/test");
        req4.SetTrafficLimitByHeader("65536");
        EXPECT_EQ("65536", req4.GetHeader("x-cos-traffic-limit"));
        req4.SetTrafficLimitByParam("2048");
        EXPECT_EQ("", req4.GetParam("x-cos-traffic-limit"));
    }
}

 TEST(ObjectReqTest, SelectObjectContent) {
     std::string bucket_name = "ut_bucket_01";
     std::string object_name = "ut_object_01";
    // test select object content
    {
        SelectObjectContentReq req(bucket_name, object_name);
        req.SetSqlExpression("Select * from COSObject");
        std::string req_body;
        req.GenerateRequestBody(&req_body);
    }
    {
        SelectObjectContentReq req(bucket_name, object_name, CSV, JSON);
        req.SetSqlExpression("Select * from COSObject");
        std::string req_body;
        req.GenerateRequestBody(&req_body);
    }
    {
        SelectObjectContentReq req(bucket_name, object_name, JSON, CSV);
        req.SetSqlExpression("Select * from COSObject");
        std::string req_body;
        req.GenerateRequestBody(&req_body);
    }
    {
        SelectObjectContentReq req(bucket_name, object_name, JSON, JSON);
        req.SetSqlExpression("Select * from COSObject");
        std::string req_body;
        req.GenerateRequestBody(&req_body);
    }
    {
        SelectObjectContentReq req(bucket_name, object_name);
        std::ostringstream input_xml;
        input_xml << "<InputSerialization>";
        input_xml << "<CompressionType>GZIP</CompressionType>";
        input_xml << "<CSV>";
        input_xml << "<FileHeaderInfo>IGNORE</FileHeaderInfo>";
        input_xml << "<RecordDelimiter>\\n</RecordDelimiter>";
        input_xml << "<FieldDelimiter>,</FieldDelimiter>";
        input_xml << "<QuoteCharacter>,</QuoteCharacter>";
        input_xml << "<QuoteEscapeCharacter>\"</QuoteEscapeCharacter>";
        input_xml << "<Comments>#</Comments>";
        input_xml << "<AllowQuotedRecordDelimiter>FALSE</AllowQuotedRecordDelimiter>";
        input_xml << "</CSV>";
        input_xml << "</InputSerialization>";
        
        std::ostringstream output_xml;
        output_xml << "<OutputSerialization>";
        output_xml << "<CSV>";
        output_xml << "<QuoteFields>ASNEEDED</QuoteFields>";
        output_xml << "<RecordDelimiter>\\n</RecordDelimiter>";
        output_xml << "<FieldDelimiter>,</FieldDelimiter>";
        output_xml << "<QuoteCharacter>\"</QuoteCharacter>";
        output_xml << "<QuoteEscapeCharacter>\"</QuoteEscapeCharacter>";
        output_xml << "</CSV>";
        output_xml << "</OutputSerialization>";
        req.SetInputSerialization(input_xml.str());
        req.SetOutputSerialization(output_xml.str());
        req.SetSqlExpression("Select * from COSObject");
        std::string req_body;
        req.GenerateRequestBody(&req_body);
    }
}

        
} // namespace qcloud_cos
