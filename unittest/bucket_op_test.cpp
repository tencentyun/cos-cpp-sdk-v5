// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 08/23/17
// Description:

#include "gtest-1.7.0/include/gtest/gtest.h"

#include "cos_api.h"
#include "cos_sys_config.h"
#include "mock_server.h"
#include "threadpool/boost/threadpool.hpp"
#include "util/string_util.h"

namespace qcloud_cos {

class BucketOpTest : public testing::Test {
protected:
    static void SetUpTestCase() {
        std::cout << "================SetUpTestCase Begin====================" << std::endl;
        m_config = new CosConfig("./config.json");
        m_client = new CosAPI(*m_config);
        m_config2 = new CosConfig("./config_bucket_test.json");
        m_client2 = new CosAPI(*m_config2);
        std::cout << "================SetUpTestCase End====================" << std::endl;
    }

    static void TearDownTestCase() {
        std::cout << "================TearDownTestCase Begin====================" << std::endl;
        delete m_client;
        delete m_config;
        delete m_client2;
        delete m_config2;
        std::cout << "================TearDownTestCase End====================" << std::endl;
    }

protected:
    static CosConfig* m_config;
    static CosAPI* m_client;
    static std::string m_bucket_name;
    static std::string m_owner;

    static CosConfig* m_config2;
    static CosAPI* m_client2;
    static std::string m_bucket_name2;
    static std::string m_owner2;
};

CosConfig* BucketOpTest::m_config = NULL;
CosConfig* BucketOpTest::m_config2 = NULL;
CosAPI* BucketOpTest::m_client = NULL;
CosAPI* BucketOpTest::m_client2 = NULL;
std::string BucketOpTest::m_bucket_name = "coscppsdkv5ut-1251668577";
std::string BucketOpTest::m_bucket_name2 = "coscppsdkv5utotherregion-1251668577";
std::string BucketOpTest::m_owner = "";
std::string BucketOpTest::m_owner2 = "";

TEST_F(BucketOpTest, PutBucketTest) {
    {
        PutBucketReq req(m_bucket_name);
        PutBucketResp resp;
        CosResult result = m_client->PutBucket(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }

    {
        PutBucketReq req(m_bucket_name2);
        PutBucketResp resp;
        req.SetXCosAcl("public-read-write");
        CosResult result = m_client2->PutBucket(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }
}

TEST_F(BucketOpTest, GetBucketTest) {
    // 查询空的Bucket
    {
        GetBucketReq req(m_bucket_name);
        GetBucketResp resp;
        req.SetPrefix("test/");
        req.SetDelimiter("/");
        req.SetEncodingType("url");
        req.SetMarker("marker");
        req.SetMaxKeys(100);
        CosResult result = m_client->GetBucket(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }

    // 添加Object
    {
        std::string prefixA = "prefixA_";
        std::string prefixB = "prefixB_";
        for (int i = 0; i < 5; ++i) {
            {
                std::istringstream iss(prefixA + StringUtil::IntToString(i));
                PutObjectByStreamReq req(m_bucket_name, prefixA + StringUtil::IntToString(i), iss);
                req.SetXCosStorageClass(kStorageClassStandardIA);
                PutObjectByStreamResp resp;
                CosResult result = m_client->PutObject(req, &resp);
                EXPECT_TRUE(result.IsSucc());
            }

            {
                std::istringstream iss(prefixB + StringUtil::IntToString(i));
                PutObjectByStreamReq req(m_bucket_name, prefixB + StringUtil::IntToString(i), iss);
                req.SetXCosStorageClass(kStorageClassStandardIA);
                PutObjectByStreamResp resp;
                CosResult result = m_client->PutObject(req, &resp);
                EXPECT_TRUE(result.IsSucc());
            }
        }
    }

    // GetBucket
    {
        GetBucketReq req(m_bucket_name);
        GetBucketResp resp;
        req.SetPrefix("prefixB_");
        req.SetMaxKeys(1);
        CosResult result = m_client->GetBucket(req, &resp);
        EXPECT_TRUE(result.IsSucc());
        EXPECT_EQ(m_bucket_name, resp.GetName());
        EXPECT_EQ("prefixB_", resp.GetPrefix());
        EXPECT_TRUE(resp.IsTruncated());
        EXPECT_EQ("prefixB_0", resp.GetNextMarker());
        const std::vector<Content>& contents = resp.GetContents();
        EXPECT_EQ(1, contents.size());
    }

    // GetBucket
    {
        GetBucketReq req(m_bucket_name);
        GetBucketResp resp;
        req.SetPrefix("prefixA_");
        req.SetMaxKeys(5);
        CosResult result = m_client->GetBucket(req, &resp);
        EXPECT_TRUE(result.IsSucc());
        EXPECT_EQ(m_bucket_name, resp.GetName());
        EXPECT_EQ("prefixA_", resp.GetPrefix());
        EXPECT_EQ("prefixA_4", resp.GetNextMarker());
        EXPECT_TRUE(resp.IsTruncated());
        const std::vector<Content>& contents = resp.GetContents();
        EXPECT_EQ(5, contents.size());
    }

    // GetBucket
    {
        GetBucketReq req(m_bucket_name);
        GetBucketResp resp;
        req.SetPrefix("prefix");
        CosResult result = m_client->GetBucket(req, &resp);
        EXPECT_TRUE(result.IsSucc());
        EXPECT_EQ(m_bucket_name, resp.GetName());
        EXPECT_EQ("prefix", resp.GetPrefix());
        EXPECT_FALSE(resp.IsTruncated());
        EXPECT_EQ("", resp.GetNextMarker());
        const std::vector<Content>& contents = resp.GetContents();
        EXPECT_EQ(10, contents.size());

        const Content& cnt_01 = contents[0];
        EXPECT_EQ("prefixA_0", cnt_01.m_key);
        EXPECT_EQ("3be03ea31c1d6ce899f419c04cbf1ea9", cnt_01.m_etag);
        EXPECT_EQ("9", cnt_01.m_size);
        EXPECT_EQ("STANDARD_IA", cnt_01.m_storage_class);
    }
}

TEST_F(BucketOpTest, PutBucketVersioningTest) {
    PutBucketVersioningReq req(m_bucket_name);
    PutBucketVersioningResp resp;
    req.SetStatus(true);

    CosResult result = m_client->PutBucketVersioning(req, &resp);
    EXPECT_TRUE(result.IsSucc());
}

TEST_F(BucketOpTest, GetBucketVersioningTest) {
    sleep(3);
    // 1. 查看开启版本管理的Bucket状态
    {
        GetBucketVersioningReq req(m_bucket_name);
        GetBucketVersioningResp resp;

        CosResult result = m_client->GetBucketVersioning(req, &resp);
        EXPECT_TRUE(result.IsSucc());
        EXPECT_EQ(1, resp.GetStatus());
    }

    // 2. 查看未开启版本管理的Bucket状态
    {
        GetBucketVersioningReq req(m_bucket_name2);
        GetBucketVersioningResp resp;

        CosResult result = m_client2->GetBucketVersioning(req, &resp);
        EXPECT_TRUE(result.IsSucc());
        EXPECT_EQ(0, resp.GetStatus());
    }

    // 3. 暂停版本管理
    {
        {
            PutBucketVersioningReq req(m_bucket_name);
            PutBucketVersioningResp resp;
            req.SetStatus(false);

            CosResult result = m_client->PutBucketVersioning(req, &resp);
            EXPECT_TRUE(result.IsSucc());
        }

        {
            GetBucketVersioningReq req(m_bucket_name);
            GetBucketVersioningResp resp;

            CosResult result = m_client->GetBucketVersioning(req, &resp);
            EXPECT_TRUE(result.IsSucc());
            EXPECT_EQ(2, resp.GetStatus());
        }
    }

    // 4. 重新开启
    {
        {
            PutBucketVersioningReq req(m_bucket_name);
            PutBucketVersioningResp resp;
            req.SetStatus(true);

            CosResult result = m_client->PutBucketVersioning(req, &resp);
            EXPECT_TRUE(result.IsSucc());
        }

        sleep(3);
        {
            GetBucketVersioningReq req(m_bucket_name);
            GetBucketVersioningResp resp;

            CosResult result = m_client->GetBucketVersioning(req, &resp);
            EXPECT_TRUE(result.IsSucc());
            EXPECT_EQ(1, resp.GetStatus());
        }
    }
}

TEST_F(BucketOpTest, PutBucketReplicationTest) {
    // 前置条件源和目标Bucket必须开启多版本
    {
        {
            PutBucketVersioningReq req(m_bucket_name);
            PutBucketVersioningResp resp;
            req.SetStatus(true);

            CosResult result = m_client->PutBucketVersioning(req, &resp);
            EXPECT_TRUE(result.IsSucc());
        }
        {
            PutBucketVersioningReq req(m_bucket_name2);
            PutBucketVersioningResp resp;
            req.SetStatus(true);

            CosResult result = m_client2->PutBucketVersioning(req, &resp);
            EXPECT_TRUE(result.IsSucc());
        }
    }

    {
        PutBucketReplicationReq req(m_bucket_name);
        PutBucketReplicationResp resp;

        std::string appid = StringUtil::Uint64ToString(m_config2->GetAppId());
        std::string bucket_name = m_bucket_name2;
        if (appid == "0") {
            ssize_t pos = bucket_name.find("-");
            if (pos != std::string::npos) {
                appid = m_bucket_name2.substr(pos + 1);
                bucket_name = m_bucket_name2.substr(0, pos);
            }
        }

        std::string dest = "qcs:id/0:cos:" + StringUtil::StringRemovePrefix(m_config2->GetRegion(), "cos.")
            + ":appid/" + appid + ":" + bucket_name;
        std::cout << "dest=" << dest << std::endl;
        ReplicationRule rule00("prefix_A", dest, "Standard", "rule_00");
        ReplicationRule rule01("prefix_B", dest, "Standard_IA", "rule_01");
        req.AddReplicationRule(rule00);
        req.AddReplicationRule(rule01);

        CosResult result = m_client->PutBucketReplication(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }
}

TEST_F(BucketOpTest, GetBucketReplicationTest) {
    GetBucketReplicationReq req(m_bucket_name);
    GetBucketReplicationResp resp;

    CosResult result = m_client->GetBucketReplication(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    const std::vector<ReplicationRule>& rules = resp.GetRules();
    ASSERT_EQ(2, rules.size());

    EXPECT_TRUE(rules[0].m_is_enable);
    EXPECT_EQ("rule_00", rules[0].m_id);
    EXPECT_EQ("prefix_A", rules[0].m_prefix);
    EXPECT_EQ("Standard", rules[0].m_dest_storage_class);

    EXPECT_TRUE(rules[1].m_is_enable);
    EXPECT_EQ("rule_01", rules[1].m_id);
    EXPECT_EQ("prefix_B", rules[1].m_prefix);
    EXPECT_EQ("Standard_IA", rules[1].m_dest_storage_class);
}

TEST_F(BucketOpTest, DeleteBucketReplicationTest) {
    DeleteBucketReplicationReq req(m_bucket_name);
    DeleteBucketReplicationResp resp;

    CosResult result = m_client->DeleteBucketReplication(req, &resp);
    ASSERT_TRUE(result.IsSucc());
}

TEST_F(BucketOpTest, PutBucketLifecycleTest) {
    PutBucketLifecycleReq req(m_bucket_name);
    PutBucketLifecycleResp resp;
    {
        LifecycleRule rule;
        rule.SetIsEnable(true);
        rule.SetId("lifecycle_rule00");
        LifecycleFilter filter;
        filter.SetPrefix("sevenyou_e0");
        //LifecycleTag tag0 = {"tag_key0", "tag_value0" };
        //LifecycleTag tag1 = {"tag_key1", "tag_value1" };
        //filter.AddTag(tag0);
        //filter.AddTag(tag1);
        rule.SetFilter(filter);

        LifecycleExpiration expiration;
        expiration.SetDays(7);
        rule.SetExpiration(expiration);

        LifecycleTransition transition;
        transition.SetDays(30);
        transition.SetStorageClass("STANDARD_IA");
        rule.AddTransition(transition);

        LifecycleNonCurrTransition non_cur_transition;
        non_cur_transition.SetDays(30);
        non_cur_transition.SetStorageClass("STANDARD_IA");
        rule.SetNonCurrTransition(non_cur_transition);

        LifecycleNonCurrExpiration non_cur_expiration;
        non_cur_expiration.SetDays(8);
        rule.SetNonCurrExpiration(non_cur_expiration);

        AbortIncompleteMultipartUpload abort_upload;
        abort_upload.m_days_after_init = 3;
        rule.SetAbortIncompleteMultiUpload(abort_upload);

        req.AddRule(rule);
    }

    {
        LifecycleRule rule;
        rule.SetIsEnable(true);
        rule.SetId("lifecycle_rule01");
        LifecycleFilter filter;
        filter.SetPrefix("sevenyou_e1");
        rule.SetFilter(filter);
        LifecycleTransition transition;
        transition.SetDays(60);
        transition.SetStorageClass("NEARLINE");
        rule.AddTransition(transition);
#if 0
        LifecycleTransition transition2;
        transition2.SetDays(30);
        transition2.SetStorageClass("STANDARD_IA");
        rule.AddTransition(transition2);
#endif
        req.AddRule(rule);
    }

    {
        LifecycleRule rule;
        rule.SetIsEnable(false);
        rule.SetId("lifecycle_rule02");
        LifecycleFilter filter;
        filter.SetPrefix("sevenyou_e2");
        rule.SetFilter(filter);

        LifecycleTransition transition;
        transition.SetDays(30);
        transition.SetStorageClass("STANDARD_IA");
        rule.AddTransition(transition);

        req.AddRule(rule);
    }

    CosResult result = m_client->PutBucketLifecycle(req, &resp);
    EXPECT_TRUE(result.IsSucc());
}

TEST_F(BucketOpTest, GetBucketLifecycleTest) {
    GetBucketLifecycleReq req(m_bucket_name);
    GetBucketLifecycleResp resp;
    CosResult result = m_client->GetBucketLifecycle(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    const std::vector<LifecycleRule>& rules = resp.GetRules();
    ASSERT_EQ(3, rules.size());

    // 验证第一条rule
    {
        const LifecycleRule& rule = rules[0];
        EXPECT_TRUE(rule.IsEnable());
        EXPECT_EQ("lifecycle_rule00", rule.GetId());
        EXPECT_EQ("sevenyou_e0", rule.GetFilter().GetPrefix());
        // Tag不能与Abort/DeleteMarker一起使用
        //ASSERT_EQ(2, rule.GetFilter().GetTags().size());
        //EXPECT_EQ("tag0_key", rule.GetFilter().GetTags()[0].key);
        //EXPECT_EQ("tag0_value", rule.GetFilter().GetTags()[0].value);
        //EXPECT_EQ("tag1_key", rule.GetFilter().GetTags()[1].key);
        //EXPECT_EQ("tag1_value", rule.GetFilter().GetTags()[1].value);

        const LifecycleExpiration& expiration = rule.GetExpiration();
        EXPECT_EQ(7, expiration.GetDays());

        const std::vector<LifecycleTransition>& transitions = rule.GetTransitions();
        EXPECT_EQ(30, transitions[0].GetDays());
        EXPECT_EQ("STANDARD_IA", transitions[0].GetStorageClass());

        const LifecycleNonCurrExpiration& non_cur_expiration = rule.GetNonCurrExpiration();
        EXPECT_EQ(8, non_cur_expiration.GetDays());

        const LifecycleNonCurrTransition& non_cur_transition = rule.GetNonCurrTransition();
        EXPECT_EQ(30, non_cur_transition.GetDays());
        EXPECT_EQ("STANDARD_IA", non_cur_transition.GetStorageClass());
    }

    EXPECT_TRUE(rules[1].IsEnable());
    EXPECT_EQ("lifecycle_rule01", rules[1].GetId());

    EXPECT_FALSE(rules[2].IsEnable());
    EXPECT_EQ("lifecycle_rule02", rules[2].GetId());
}

TEST_F(BucketOpTest, DeleteBucketLifecycleTest) {
    DeleteBucketLifecycleReq req(m_bucket_name);
    DeleteBucketLifecycleResp resp;
    CosResult result = m_client->DeleteBucketLifecycle(req, &resp);
    EXPECT_TRUE(result.IsSucc());
}

TEST_F(BucketOpTest, PutBucketACLTest) {
    {
        PutBucketACLReq req(m_bucket_name);
        PutBucketACLResp resp;

        Owner owner = {"qcs::cam::uin/" + m_owner + ":uin/" + m_owner,
            "qcs::cam::uin/" + m_owner + ":uin/" + m_owner};
        Grant grant;
        req.SetOwner(owner);
        grant.m_perm = "READ";
        grant.m_grantee.m_type = "RootAccount";
        grant.m_grantee.m_uri = "http://cam.qcloud.com/groups/global/AllUsers";
        grant.m_grantee.m_id = "qcs::cam::uin/" + m_owner2 + ":uin/" + m_owner2;
        grant.m_grantee.m_display_name = "qcs::cam::uin/" + m_owner2 + ":uin/" + m_owner2;
        req.AddAccessControlList(grant);

        CosResult result = m_client->PutBucketACL(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }
}

TEST_F(BucketOpTest, GetBucketACLTest) {
    GetBucketACLReq req(m_bucket_name);
    GetBucketACLResp resp;

    CosResult result = m_client->GetBucketACL(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    const std::vector<qcloud_cos::Grant>& grants = resp.GetAccessControlList();
    EXPECT_EQ(1, grants.size());
}

TEST_F(BucketOpTest, DeleteBucketTest) {
    // 未删除Object， 删除Bucket返回false
    {
        DeleteBucketReq req(m_bucket_name);
        DeleteBucketResp resp;
        CosResult result = m_client->DeleteBucket(req, &resp);
        EXPECT_FALSE(result.IsSucc());
    }

    // 清空Object
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
            CosResult result = m_client2->GetBucket(req, &resp);
            EXPECT_TRUE(result.IsSucc());

            const std::vector<Content>& contents = resp.GetContents();
            for (std::vector<Content>::const_iterator c_itr = contents.begin();
                 c_itr != contents.end(); ++c_itr) {
                const Content& content = *c_itr;
                DeleteObjectReq del_req(m_bucket_name2, content.m_key);
                DeleteObjectResp del_resp;
                CosResult del_result = m_client2->DeleteObject(del_req, &del_resp);
                EXPECT_TRUE(del_result.IsSucc());
                if (!del_result.IsSucc()) {
                    std::cout << "DeleteObject Failed, check object=" << content.m_key << std::endl;
                }
            }
        }
    }

    // 删除Bucket
    {
        {
            DeleteBucketReq req(m_bucket_name);
            DeleteBucketResp resp;
            CosResult result = m_client->DeleteBucket(req, &resp);
            EXPECT_TRUE(result.IsSucc());
        }
        {
            DeleteBucketReq req(m_bucket_name2);
            DeleteBucketResp resp;
            CosResult result = m_client2->DeleteBucket(req, &resp);
            EXPECT_TRUE(result.IsSucc());
        }
    }
}

} // namespace qcloud_cos

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
