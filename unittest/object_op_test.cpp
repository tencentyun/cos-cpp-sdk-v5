// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:

#include "gtest/gtest.h"

#include "common.h"
#include "cos_api.h"

namespace qcloud_cos {

class ObjectOpTest : public testing::Test {
protected:
    static void SetUpTestCase() {
        std::cout << "================SetUpTestCase Begin====================" << std::endl;
        m_config = new CosConfig("./config.json");
        m_config->SetAccessKey(GetEnv("CPP_SDK_V5_ACCESS_KEY"));
        m_config->SetSecretKey(GetEnv("CPP_SDK_V5_SECRET_KEY"));
        m_client = new CosAPI(*m_config);

        m_bucket_name = "coscppsdkv5ut" + GetEnv("COS_CPP_V5_TAG") + "-" + GetEnv("CPP_SDK_V5_APPID");
        m_bucket_name2 = "coscppsdkv5utcopy" + GetEnv("COS_CPP_V5_TAG") + "-" + GetEnv("CPP_SDK_V5_APPID");
        {
            PutBucketReq req(m_bucket_name);
            PutBucketResp resp;
            CosResult result = m_client->PutBucket(req, &resp);
            ASSERT_TRUE(result.IsSucc());
        }

        {
            PutBucketReq req(m_bucket_name2);
            PutBucketResp resp;
            CosResult result = m_client->PutBucket(req, &resp);
            ASSERT_TRUE(result.IsSucc());
        }

#if 0
        // 生成一个大文件
        {
            system("dd if=/dev/urandom of=sevenyou_1G.tmp bs=10M count=100");
        }
#endif

        std::cout << "================SetUpTestCase End====================" << std::endl;
    }

    static void TearDownTestCase() {
        std::cout << "================TearDownTestCase Begin====================" << std::endl;

        // 1. 删除所有Object
        {
            {
                GetBucketReq req(m_bucket_name);
                GetBucketResp resp;
                CosResult result = m_client->GetBucket(req, &resp);
                ASSERT_TRUE(result.IsSucc());

                const std::vector<Content>& contents = resp.GetContents();
                for (std::vector<Content>::const_iterator c_itr = contents.begin();
                     c_itr != contents.end(); ++c_itr) {
                    const Content& content = *c_itr;
                    DeleteObjectReq del_req(m_bucket_name, content.m_key);
                    DeleteObjectResp del_resp;
                    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
                    EXPECT_TRUE(del_result.IsSucc());
                    if (!del_result.IsSucc()) {
                        std::cout << "DeleteObject Failed, check object=" << content.m_key << std::endl;
                    }
                }
            }

            {
                GetBucketReq req(m_bucket_name2);
                GetBucketResp resp;
                CosResult result = m_client->GetBucket(req, &resp);
                ASSERT_TRUE(result.IsSucc());

                const std::vector<Content>& contents = resp.GetContents();
                for (std::vector<Content>::const_iterator c_itr = contents.begin();
                     c_itr != contents.end(); ++c_itr) {
                    const Content& content = *c_itr;
                    DeleteObjectReq del_req(m_bucket_name2, content.m_key);
                    DeleteObjectResp del_resp;
                    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
                    EXPECT_TRUE(del_result.IsSucc());
                    if (!del_result.IsSucc()) {
                        std::cout << "DeleteObject Failed, check object=" << content.m_key << std::endl;
                    }
                }
            }
        }

        // 2. 删除所有未complete的分块
        {
            for (std::map<std::string, std::string>::const_iterator c_itr = m_to_be_aborted.begin();
                 c_itr != m_to_be_aborted.end(); ++c_itr) {
                AbortMultiUploadReq req(m_bucket_name, c_itr->first, c_itr->second);
                AbortMultiUploadResp resp;

                CosResult result = m_client->AbortMultiUpload(req, &resp);
                EXPECT_TRUE(result.IsSucc());
                if (!result.IsSucc()) {
                    std::cout << "AbortMultiUpload Failed, object=" << c_itr->first
                        << ", upload_id=" << c_itr->second << std::endl;
                }
            }
        }

        // 3. 删除Bucket
        {
            {
                DeleteBucketReq req(m_bucket_name);
                DeleteBucketResp resp;
                CosResult result = m_client->DeleteBucket(req, &resp);
                ASSERT_TRUE(result.IsSucc());
            }

            {
                DeleteBucketReq req(m_bucket_name2);
                DeleteBucketResp resp;
                CosResult result = m_client->DeleteBucket(req, &resp);
                ASSERT_TRUE(result.IsSucc());
            }
        }

        delete m_client;
        delete m_config;
        std::cout << "================TearDownTestCase End====================" << std::endl;
    }

protected:
    static CosConfig* m_config;
    static CosAPI* m_client;
    static std::string m_bucket_name;
    static std::string m_bucket_name2; // 用于copy

    // 用于记录单测中未Complete的分块上传uploadID,便于清理
    static std::map<std::string, std::string> m_to_be_aborted;
};

std::string ObjectOpTest::m_bucket_name = "";
std::string ObjectOpTest::m_bucket_name2 = "";
CosConfig* ObjectOpTest::m_config = NULL;
CosAPI* ObjectOpTest::m_client = NULL;
std::map<std::string, std::string> ObjectOpTest::m_to_be_aborted;

TEST_F(ObjectOpTest, PutObjectByFileTest) {
    // 1. ObjectName为普通字符串
    {
        PutObjectByFileReq req(m_bucket_name, "object_test", "sevenyou.txt");
        req.SetXCosStorageClass(kStorageClassNearline);
        PutObjectByFileResp resp;
        CosResult result = m_client->PutObject(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }

    // 2. ObjectName为中文字符串
    {
        PutObjectByFileReq req(m_bucket_name, "这是个中文Object", "sevenyou.txt");
        req.SetXCosStorageClass(kStorageClassStandard);
        PutObjectByFileResp resp;
        CosResult result = m_client->PutObject(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }

    // 3. ObjectName为特殊字符串
    {
        PutObjectByFileReq req(m_bucket_name, "/→↓←→↖↗↙↘! \"#$%&'()*+,-./0123456789:;"
                               "<=>@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~",
                               "sevenyou.txt");
        req.SetXCosStorageClass(kStorageClassStandardIA);
        PutObjectByFileResp resp;
        CosResult result = m_client->PutObject(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }

#if 0
    // 4. 上传大文件1G
    {
        PutObjectByFileReq req(m_bucket_name, "sevenyou_1G.tmp", "./sevenyou_1G.tmp");
        req.SetXCosStorageClass(kStorageClassStandard);
        PutObjectByFileResp resp;
        CosResult result = m_client->PutObject(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }
#endif
}

TEST_F(ObjectOpTest, PutObjectByStreamTest) {
    // 1. ObjectName为普通字符串
    {
        std::istringstream iss("put_obj_by_stream_normal_string");
        PutObjectByStreamReq req(m_bucket_name, "object_test2", iss);
        req.SetXCosStorageClass(kStorageClassStandard);
        PutObjectByStreamResp resp;
        CosResult result = m_client->PutObject(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }

    // 2. ObjectName为中文字符串
    {
        std::istringstream iss("put_obj_by_stream_chinese_string");
        PutObjectByStreamReq req(m_bucket_name, "这是个中文Object2", iss);
        req.SetXCosStorageClass(kStorageClassStandardIA);
        PutObjectByStreamResp resp;
        CosResult result = m_client->PutObject(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }

    // 3. ObjectName为特殊字符串
    {
        std::istringstream iss("put_obj_by_stream_special_string");
        PutObjectByStreamReq req(m_bucket_name, "/→↓←→↖↗↙↘! \"#$%&'()*+,-./0123456789:;"
                               "<=>@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~2",
                               iss);
        req.SetXCosStorageClass(kStorageClassNearline);
        PutObjectByStreamResp resp;
        CosResult result = m_client->PutObject(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }

}

TEST_F(ObjectOpTest, IsObjectExistTest) {
    EXPECT_TRUE(m_client->IsObjectExist(m_bucket_name, "object_test"));
    EXPECT_FALSE(m_client->IsObjectExist(m_bucket_name, "not_exist_object"));
}

TEST_F(ObjectOpTest, HeadObjectTest) {
    HeadObjectReq req(m_bucket_name, "object_test");
    HeadObjectResp resp;
    CosResult result = m_client->HeadObject(req, &resp);
    ASSERT_TRUE(result.IsSucc());
}

TEST_F(ObjectOpTest, GetObjectByFileTest) {
    {
        GetObjectByFileReq req(m_bucket_name, "object_test", "sevenyou2.txt");
        GetObjectByFileResp resp;
        CosResult result = m_client->GetObject(req, &resp);

        ASSERT_TRUE(result.IsSucc());
    }

    {
        GetObjectByFileReq req(m_bucket_name, "sevenyou_1G.tmp", "sevenyou_1G.tmp_download");
        GetObjectByFileResp resp;
        CosResult result = m_client->GetObject(req, &resp);

        ASSERT_TRUE(result.IsSucc());
    }
}

TEST_F(ObjectOpTest, MultiUploadObjectTest) {
    uint64_t part_size = 20 * 1000 * 1000;
    uint64_t max_part_num = 3;
    std::string object_name = "object_test_multi";
    InitMultiUploadReq init_req(m_bucket_name, object_name);
    InitMultiUploadResp init_resp;
    CosResult init_result = m_client->InitMultiUpload(init_req, &init_resp);
    ASSERT_TRUE(init_result.IsSucc());

    std::vector<std::string> etags;
    std::vector<uint64_t> part_numbers;
    for (uint64_t part_cnt = 0; part_cnt < max_part_num; ++part_cnt) {
        std::string str(part_size * (part_cnt + 1), 'a'); // 分块大小倍增
        std::stringstream ss;
        ss << str;
        UploadPartDataReq req(m_bucket_name, object_name, init_resp.GetUploadId(), ss);
        UploadPartDataResp resp;
        req.SetPartNumber(part_cnt + 1);

        CosResult result = m_client->UploadPartData(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        etags.push_back(resp.GetEtag());
        part_numbers.push_back(part_cnt + 1);
    }

    // 测试ListParts
    {
        ListPartsReq req(m_bucket_name, object_name, init_resp.GetUploadId());
        ListPartsResp resp;

        CosResult result = m_client->ListParts(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        EXPECT_EQ(m_bucket_name, resp.GetBucket());
        EXPECT_EQ(object_name, resp.GetKey());
        EXPECT_EQ(init_resp.GetUploadId(), resp.GetUploadId());
        const std::vector<Part>& parts = resp.GetParts();
        EXPECT_EQ(max_part_num, parts.size());
        for (size_t idx = 0; idx != parts.size(); ++idx) {
            EXPECT_EQ(part_numbers[idx], parts[idx].m_part_num);
            EXPECT_EQ(part_size * (idx + 1), parts[idx].m_size);
            EXPECT_EQ(etags[idx], parts[idx].m_etag);
        }
    }

    CompleteMultiUploadReq comp_req(m_bucket_name, "object_test_multi", init_resp.GetUploadId());
    CompleteMultiUploadResp comp_resp;
    comp_req.SetEtags(etags);
    comp_req.SetPartNumbers(part_numbers);

    CosResult result = m_client->CompleteMultiUpload(comp_req, &comp_resp);
    EXPECT_TRUE(result.IsSucc());
}

TEST_F(ObjectOpTest, MultiUploadObjectTest_OneStep) {
    std::string filename = "multi_upload_object_one_step";
    std::string object_name = filename;
    // 1. 生成个临时文件, 用于分块上传
    {
        std::ofstream fs;
        fs.open(filename.c_str(), std::ios::out | std::ios::binary);
        std::string str(10 * 1000 * 1000, 'b');
        for (int idx = 0; idx < 10; ++idx) {
            fs << str;
        }
        fs.close();
    }

    // 2. 上传
    MultiUploadObjectReq req(m_bucket_name, object_name, filename);
    MultiUploadObjectResp resp;

    CosResult result = m_client->MultiUploadObject(req, &resp);
    EXPECT_TRUE(result.IsSucc());

    // 3. 删除临时文件
    if (-1 == remove(filename.c_str())) {
        std::cout << "Remove temp file=" << filename << " fail." << std::endl;
    }
}

TEST_F(ObjectOpTest, AbortMultiUploadTest) {
    uint64_t part_size = 20 * 1000 * 1000;
    uint64_t max_part_num = 3;
    std::string object_name = "object_test_abort_multi";
    InitMultiUploadReq init_req(m_bucket_name, object_name);
    InitMultiUploadResp init_resp;
    CosResult init_result = m_client->InitMultiUpload(init_req, &init_resp);
    ASSERT_TRUE(init_result.IsSucc());

    std::vector<std::string> etags;
    std::vector<uint64_t> part_numbers;
    for (uint64_t part_cnt = 0; part_cnt < max_part_num; ++part_cnt) {
        std::string str(part_size * (part_cnt + 1), 'a'); // 分块大小倍增
        std::stringstream ss;
        ss << str;
        UploadPartDataReq req(m_bucket_name, object_name, init_resp.GetUploadId(), ss);
        UploadPartDataResp resp;
        req.SetPartNumber(part_cnt + 1);

        CosResult result = m_client->UploadPartData(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        etags.push_back(resp.GetEtag());
        part_numbers.push_back(part_cnt + 1);
    }

    AbortMultiUploadReq abort_req(m_bucket_name, object_name, init_resp.GetUploadId());
    AbortMultiUploadResp abort_resp;

    CosResult result = m_client->AbortMultiUpload(abort_req, &abort_resp);
    ASSERT_TRUE(result.IsSucc());
}

TEST_F(ObjectOpTest, ObjectACLTest) {
    // 1. Put
    {
        PutObjectACLReq req(m_bucket_name, "object_test");
        PutObjectACLResp resp;
        std::string uin(GetEnv("CPP_SDK_V5_UIN"));
        std::string grant_uin(GetEnv("CPP_SDK_V5_OTHER_UIN"));

        qcloud_cos::Owner owner = {"qcs::cam::uin/" + uin + ":uin/" + uin,
            "qcs::cam::uin/" + uin + ":uin/" + uin};
        Grant grant;
        req.SetOwner(owner);
        grant.m_perm = "READ";
        grant.m_grantee.m_type = "RootAccount";
        grant.m_grantee.m_uri = "http://cam.qcloud.com/groups/global/AllUsers";
        grant.m_grantee.m_id = "qcs::cam::uin/" + grant_uin + ":uin/" + grant_uin;
        grant.m_grantee.m_display_name = "qcs::cam::uin/" + grant_uin + ":uin/" + grant_uin;
        req.AddAccessControlList(grant);

        CosResult result = m_client->PutObjectACL(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }

    // 2. Get
    {
        sleep(3);
        GetObjectACLReq req(m_bucket_name, "object_test");
        GetObjectACLResp resp;
        CosResult result = m_client->GetObjectACL(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }
}

TEST_F(ObjectOpTest, PutObjectCopyTest) {
    std::string object_name = "object_test";
    PutObjectCopyReq req(m_bucket_name2, "object_test_copy_from_bucket1");
    PutObjectCopyResp resp;
    std::string source = m_bucket_name + "." + m_config->GetRegion()
        + ".mycloud.com/" + object_name;
    req.SetXCosCopySource(source);

    CosResult result = m_client->PutObjectCopy(req, &resp);
    EXPECT_TRUE(result.IsSucc());
}

TEST_F(ObjectOpTest, GeneratePresignedUrlTest) {
    {
        GeneratePresignedUrlReq req(m_bucket_name, "object_test", HTTP_GET);
        req.SetStartTimeInSec(0);
        req.SetExpiredTimeInSec(5 * 60);

        std::string presigned_url = m_client->GeneratePresignedUrl(req);
        EXPECT_FALSE(presigned_url.empty());

        // TODO(sevenyou) 先直接调 curl 命令看下是否正常
        std::string curl_url = "curl " + presigned_url;
        int ret = system(curl_url.c_str());
        EXPECT_EQ(0, ret);
    }

    {
        std::string presigned_url = m_client->GeneratePresignedUrl(m_bucket_name, "object_test", 0, 0);
        // TODO(sevenyou) 先直接调 curl 命令看下是否正常
        std::string curl_url = "curl " + presigned_url;
        int ret = system(curl_url.c_str());
        EXPECT_EQ(0, ret);
    }
}

} // namespace qcloud_cos
