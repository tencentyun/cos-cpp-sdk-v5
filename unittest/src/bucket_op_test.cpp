// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 08/23/17
// Description:

#include "gtest/gtest.h"

#include "cos_api.h"
#include "test_utils.h"
#include "util/string_util.h"

namespace qcloud_cos {

/*
export CPP_SDK_V5_ACCESS_KEY=xxx
export CPP_SDK_V5_SECRET_KEY=xxx
export CPP_SDK_V5_REGION=ap-guangzhou
export CPP_SDK_V5_UIN=xxx
export CPP_SDK_V5_APPID=xxx
export COS_CPP_V5_TAG=""

export CPP_SDK_V5_OTHER_ACCESS_KEY=xxx
export CPP_SDK_V5_OTHER_SECRET_KEY=xxx
export CPP_SDK_V5_OTHER_REGION=ap-hongkong
export CPP_SDK_V5_OTHER_UIN=xxx
*/

class BucketOpTest : public testing::Test {

protected:
    static void SetUpTestCase() {
        std::cout << "================SetUpTestCase Begin====================" << std::endl; 
        m_config = new CosConfig("./config.json");
        m_config->SetAccessKey(GetEnv("CPP_SDK_V5_ACCESS_KEY"));
        m_config->SetSecretKey(GetEnv("CPP_SDK_V5_SECRET_KEY"));
        m_config->SetRegion(GetEnv("CPP_SDK_V5_REGION"));
        m_client = new CosAPI(*m_config);
        m_config2 = new CosConfig("./config.json");
        m_config2->SetAccessKey(GetEnv("CPP_SDK_V5_OTHER_ACCESS_KEY"));
        m_config2->SetSecretKey(GetEnv("CPP_SDK_V5_OTHER_SECRET_KEY"));
        m_config2->SetRegion(GetEnv("CPP_SDK_V5_OTHER_REGION"));
        m_client2 = new CosAPI(*m_config2);

        m_bucket_name = "coscppsdkv5ut" + GetEnv("COS_CPP_V5_TAG") + "-" + GetEnv("CPP_SDK_V5_APPID");
        m_bucket_name2 = "coscppsdkv5utotherregion" + GetEnv("COS_CPP_V5_TAG") + "-" + GetEnv("CPP_SDK_V5_APPID");
        m_bucket_name_nil = "coscppsdkv5utt" + GetEnv("COS_CPP_V5_TAG") + "-" + GetEnv("CPP_SDK_V5_APPID");
        m_bucket_name_wrong = "coscppsdkv5utt" + GetEnv("COS_CPP_V5_TAG") + "-" + GetEnv("CPP_SDK_V5_APPID") + "1";
        m_owner = GetEnv("CPP_SDK_V5_UIN");
        m_owner2 = GetEnv("CPP_SDK_V5_OTHER_UIN");
        std::cout << "================SetUpTestCase End====================" << std::endl;   
    }

    
    static void TearDownTestCase() {
        std::cout << "================TearDownTestCase Begin====================" << std::endl;
        // 清空Object,删除Bucket
        std::vector<std::string> bucket_list = {m_bucket_name, m_bucket_name2};
        for (auto &bucketname : bucket_list) {
            CosAPI* client;;
            if (bucketname == m_bucket_name) {
                client = m_client;
            } else {
                client = m_client2;
            }
            GetBucketObjectVersionsReq req(bucketname);
            GetBucketObjectVersionsResp resp;
            CosResult result = client->GetBucketObjectVersions(req, &resp);
            EXPECT_TRUE(result.IsSucc());
            const std::vector<COSVersionSummary>& summs = resp.GetVersionSummary();
            for (std::vector<COSVersionSummary>::const_iterator c_itr = summs.begin();
                 c_itr != summs.end(); ++c_itr) {
                const COSVersionSummary& summ = *c_itr;
                DeleteObjectReq del_req(bucketname, summ.m_key);
                DeleteObjectResp del_resp;
                if (!summ.m_version_id.empty()) {
                    del_req.SetXCosVersionId(summ.m_version_id);
                }
                CosResult del_result = client->DeleteObject(del_req, &del_resp);
                EXPECT_TRUE(del_result.IsSucc());
                if (!del_result.IsSucc()) {
                    std::cout << "DeleteObject Failed, check object=" << summ.m_key << std::endl;
                }
            }

            {
                DeleteBucketReq req(bucketname);
                DeleteBucketResp resp;
                CosResult result = client->DeleteBucket(req, &resp);
                EXPECT_TRUE(result.IsSucc());
            }
        }
        
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
    static std::string m_bucket_name_nil;
    static std::string m_bucket_name_wrong;
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
std::string BucketOpTest::m_bucket_name = "";
std::string BucketOpTest::m_bucket_name2 = "";
std::string BucketOpTest::m_bucket_name_nil = "";
std::string BucketOpTest::m_bucket_name_wrong = "";
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

#if 0

TEST_F(BucketOpTest, HeadBucketTest) {
    // normal 200
    {
        HeadBucketReq req(m_bucket_name2);
        HeadBucketResp resp;
        //CosResult result = m_client->HeadBucket(req, &resp);
        CosResult result = m_client2->HeadBucket(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }

    // wrong bucket 404
    {
        HeadBucketReq req(m_bucket_name_nil);
        HeadBucketResp resp;
        CosResult result = m_client->HeadBucket(req, &resp);
        EXPECT_TRUE(!result.IsSucc());
        EXPECT_EQ(result.GetHttpStatus(), 404);
    }

    // wrong appid 403
    {
        //HeadBucketReq req(m_bucket_name);
        HeadBucketReq req(m_bucket_name_wrong);
        HeadBucketResp resp;
        CosResult result = m_client->HeadBucket(req, &resp);
        EXPECT_TRUE(!result.IsSucc());
        EXPECT_EQ(result.GetHttpStatus(), 403);
    }
}

TEST_F(BucketOpTest, ListMultipartUpload) {
    {
        std::string object_name = "testlistuploadpartsdiffer";
        InitMultiUploadReq init_req(m_bucket_name, object_name);
        InitMultiUploadResp init_resp;
        CosResult init_result = m_client->InitMultiUpload(init_req, &init_resp);
        EXPECT_TRUE(init_result.IsSucc());
        std::string upload_id = init_resp.GetUploadId();

        ListMultipartUploadReq req(m_bucket_name);
        req.SetPrefix(object_name);
        ListMultipartUploadResp resp;
        CosResult list_result = m_client->ListMultipartUpload(req, &resp);
        EXPECT_TRUE(list_result.IsSucc());

        EXPECT_EQ(m_bucket_name, resp.GetName());
        EXPECT_EQ("", resp.GetEncodingType());
        EXPECT_EQ("", resp.GetMarker());
        EXPECT_EQ("", resp.GetUploadIdMarker());
        std::vector<Upload> rst = resp.GetUpload();

        for (std::vector<qcloud_cos::Upload>::const_iterator itr = rst.begin(); itr != rst.end(); ++itr) {
            const qcloud_cos::Upload& upload = *itr;
            EXPECT_EQ(object_name, upload.m_key);
            EXPECT_EQ(upload_id, upload.m_uploadid);
            EXPECT_EQ("STANDARD", upload.m_storage_class);
        }

        AbortMultiUploadReq abort_req(m_bucket_name, object_name, upload_id);
        AbortMultiUploadResp abort_resp;
        CosResult abort_result = m_client->AbortMultiUpload(abort_req, &abort_resp);
        EXPECT_TRUE(abort_result.IsSucc());
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

    // GetBucket, maxkey=1
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

    // GetBucket, truncated
    {
        GetBucketReq req(m_bucket_name);
        GetBucketResp resp;
        req.SetPrefix("prefixA_");
        req.SetMaxKeys(3);
        CosResult result = m_client->GetBucket(req, &resp);
        EXPECT_TRUE(result.IsSucc());
        EXPECT_EQ(m_bucket_name, resp.GetName());
        EXPECT_EQ("prefixA_", resp.GetPrefix());
        EXPECT_EQ("prefixA_2", resp.GetNextMarker());
        EXPECT_TRUE(resp.IsTruncated());
        const std::vector<Content>& contents = resp.GetContents();
        EXPECT_EQ(3, contents.size());
    }

    // GetBucket, not truncated
    {
        GetBucketReq req(m_bucket_name);
        GetBucketResp resp;
        req.SetPrefix("prefixA_");
        req.SetMaxKeys(5);
        CosResult result = m_client->GetBucket(req, &resp);
        EXPECT_TRUE(result.IsSucc());
        EXPECT_EQ(m_bucket_name, resp.GetName());
        EXPECT_EQ("prefixA_", resp.GetPrefix());
        EXPECT_EQ("", resp.GetNextMarker());
        EXPECT_FALSE(resp.IsTruncated());
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
    sleep(1);
}

TEST_F(BucketOpTest, PutBucketVersioningTest) {
    PutBucketVersioningReq req(m_bucket_name);
    PutBucketVersioningResp resp;
    req.SetStatus(true);

    CosResult result = m_client->PutBucketVersioning(req, &resp);
    EXPECT_TRUE(result.IsSucc());
    sleep(1);
}

TEST_F(BucketOpTest, GetBucketVersioningTest) {
    sleep(5);
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

    sleep(5);

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

        sleep(5);

        {
            GetBucketVersioningReq req(m_bucket_name);
            GetBucketVersioningResp resp;

            CosResult result = m_client->GetBucketVersioning(req, &resp);
            EXPECT_TRUE(result.IsSucc());
            EXPECT_EQ(1, resp.GetStatus());
        }
    }
}
#endif

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
    //添加跨区复制规则时，会检查园区可达性，比如金融园区只能和金融园区互通、普通园区只能和普通园区互通
    //需要保证两个园区类型相同
    {
        PutBucketReplicationReq req(m_bucket_name);
        PutBucketReplicationResp resp;
        #if 0
        std::string appid = StringUtil::Uint64ToString(m_config2->GetAppId());
        std::string bucket_name = m_bucket_name2;
        if (appid == "0") {
            ssize_t pos = bucket_name.find("-");
            if (pos != std::string::npos) {
                appid = m_bucket_name2.substr(pos + 1);
                bucket_name = m_bucket_name2.substr(0, pos);
            }
        }
        #endif
        //std::string dest = "qcs:id/0:cos:" + StringUtil::StringRemovePrefix(m_config2->GetRegion(), "cos.")
        //    + ":appid/" + appid + ":" + bucket_name;
        std::string dest = "qcs::cos:" + StringUtil::StringRemovePrefix(m_config2->GetRegion(), "cos.")
            + "::" + m_bucket_name2;
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

#if 0
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

        // Days in the Expiration action for filter must be greater than Days in the Transition action
        LifecycleExpiration expiration;
        expiration.SetDays(31);
        rule.SetExpiration(expiration);

        LifecycleTransition transition;
        transition.SetDays(30);
        transition.SetStorageClass("STANDARD_IA");
        rule.AddTransition(transition);

        LifecycleNonCurrTransition non_cur_transition;
        non_cur_transition.SetDays(5);
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
        transition.SetStorageClass("STANDARD");
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
    sleep(1);
}

TEST_F(BucketOpTest, GetBucketLifecycleTest) {
    sleep(1);
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
        EXPECT_EQ(31, expiration.GetDays());

        const std::vector<LifecycleTransition>& transitions = rule.GetTransitions();
        EXPECT_EQ(30, transitions[0].GetDays());
        EXPECT_EQ("STANDARD_IA", transitions[0].GetStorageClass());

        const LifecycleNonCurrExpiration& non_cur_expiration = rule.GetNonCurrExpiration();
        EXPECT_EQ(8, non_cur_expiration.GetDays());

        const LifecycleNonCurrTransition& non_cur_transition = rule.GetNonCurrTransition();
        EXPECT_EQ(5, non_cur_transition.GetDays());
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
    sleep(1);
    // 1. Put
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
        sleep(5);
    }
}

TEST_F(BucketOpTest, GetBucketACLTest) {
    GetBucketACLReq req(m_bucket_name);
    GetBucketACLResp resp;

    CosResult result = m_client->GetBucketACL(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    const std::vector<qcloud_cos::Grant>& grants = resp.GetAccessControlList();
    EXPECT_EQ(1, grants.size());
    sleep(1);
}

TEST_F(BucketOpTest, PutBucketCORSTest) {
    PutBucketCORSReq req(m_bucket_name);
    PutBucketCORSResp resp;

    {
        CORSRule rule;
        rule.m_id = "cors_rule_00";
        rule.m_max_age_secs = "600";
        rule.m_allowed_headers.push_back("x-cos-meta-test");
        rule.m_allowed_origins.push_back("http://www.qq.com");
        rule.m_allowed_origins.push_back("http://www.qcloud.com");
        rule.m_allowed_methods.push_back("PUT");
        rule.m_allowed_methods.push_back("GET");
        rule.m_expose_headers.push_back("x-cos-expose");
        req.AddRule(rule);
    }

    {
        CORSRule rule;
        rule.m_id = "cors_rule_01";
        rule.m_max_age_secs = "30";
        rule.m_allowed_origins.push_back("http://www.qcloud100.com");
        rule.m_allowed_methods.push_back("HEAD");
        req.AddRule(rule);
    }

    CosResult result = m_client->PutBucketCORS(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    sleep(1);
}

TEST_F(BucketOpTest, GetBucketCORSTest) {
    sleep(5);
    GetBucketCORSReq req(m_bucket_name);
    GetBucketCORSResp resp;

    CosResult result = m_client->GetBucketCORS(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    std::vector<CORSRule> rules = resp.GetCORSRules();
    ASSERT_EQ(2, rules.size());

    EXPECT_EQ("cors_rule_00", rules[0].m_id);
    EXPECT_EQ("600", rules[0].m_max_age_secs);
    ASSERT_EQ(2, rules[0].m_allowed_origins.size());
    EXPECT_EQ("http://www.qq.com", rules[0].m_allowed_origins[0]);
    EXPECT_EQ("http://www.qcloud.com", rules[0].m_allowed_origins[1]);
    EXPECT_EQ("x-cos-meta-test", rules[0].m_allowed_headers[0]);
    EXPECT_EQ("PUT", rules[0].m_allowed_methods[0]);
    EXPECT_EQ("GET", rules[0].m_allowed_methods[1]);
    EXPECT_EQ("x-cos-expose", rules[0].m_expose_headers[0]);


    EXPECT_EQ("cors_rule_01", rules[1].m_id);
    EXPECT_EQ("30", rules[1].m_max_age_secs);
    ASSERT_EQ(1, rules[1].m_allowed_origins.size());
    EXPECT_EQ("http://www.qcloud100.com", rules[1].m_allowed_origins[0]);
    EXPECT_EQ("HEAD", rules[1].m_allowed_methods[0]);
    sleep(1);
}

TEST_F(BucketOpTest, DeleteBucketCORSTest) {
    {
        DeleteBucketCORSReq req(m_bucket_name);
        DeleteBucketCORSResp resp;

        CosResult result = m_client->DeleteBucketCORS(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }

    {
        GetBucketCORSReq req(m_bucket_name);
        GetBucketCORSResp resp;

        CosResult result = m_client->GetBucketCORS(req, &resp);
        ASSERT_FALSE(result.IsSucc());
    }
    sleep(1);
}

TEST_F(BucketOpTest, GetBucketObjectVersions) {
    // 1. 开启版本管理
    {
        PutBucketVersioningReq req(m_bucket_name);
        PutBucketVersioningResp resp;
        req.SetStatus(true);

        CosResult result = m_client->PutBucketVersioning(req, &resp);
        ASSERT_TRUE(result.IsSucc());
    }

    // 2. 上传文件
    {
        {
            std::istringstream iss("put object1");
            PutObjectByStreamReq req(m_bucket_name, "object_test", iss);
            req.SetXCosStorageClass(kStorageClassStandardIA);
            PutObjectByStreamResp resp;
            CosResult result = m_client->PutObject(req, &resp);
            ASSERT_TRUE(result.IsSucc());
        }

        {
            std::istringstream iss("put object2");
            PutObjectByStreamReq req(m_bucket_name, "object_test", iss);
            req.SetXCosStorageClass(kStorageClassStandardIA);
            PutObjectByStreamResp resp;
            CosResult result = m_client->PutObject(req, &resp);
            ASSERT_TRUE(result.IsSucc());
        }

        {
            std::istringstream iss("put object3");
            PutObjectByStreamReq req(m_bucket_name, "object_test2", iss);
            PutObjectByStreamResp resp;
            CosResult result = m_client->PutObject(req, &resp);
            ASSERT_TRUE(result.IsSucc());
        }

        {
            std::istringstream iss("put object4");
            PutObjectByStreamReq req(m_bucket_name, "bad_prefix_object_test", iss);
            PutObjectByStreamResp resp;
            CosResult result = m_client->PutObject(req, &resp);
            ASSERT_TRUE(result.IsSucc());
        }
        sleep(1);
    }

    // 3. 删除文件
    {
        DeleteObjectReq del_req(m_bucket_name, "object_test2");
        DeleteObjectResp del_resp;
        CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
        ASSERT_TRUE(del_result.IsSucc());
    }

    // 4. 获取Object版本
    // object_test 有两个版本，object_test2有两个把呢不能（旧版本，最新版本标记删除）
    {
        GetBucketObjectVersionsReq req(m_bucket_name);
        req.SetPrefix("object_test");
        GetBucketObjectVersionsResp resp;
        CosResult result = m_client->GetBucketObjectVersions(req, &resp);
        ASSERT_TRUE(result.IsSucc());
        EXPECT_EQ(m_bucket_name, resp.GetBucketName());
        EXPECT_EQ("object_test", resp.GetPrefix());
        EXPECT_FALSE(resp.IsTruncated());

        const std::vector<COSVersionSummary>& summs = resp.GetVersionSummary();
        EXPECT_EQ(4, summs.size());
        sleep(1);
    }
}

TEST_F(BucketOpTest, DeleteBucketTest) {
    //TODO
    // 未删除Object，删除Bucket返回false
    // 有BUG，如果此时删除bucket，则后一步get versions只返回一个版本
    {
    //    DeleteBucketReq req(m_bucket_name);
    //    DeleteBucketResp resp;
    //    CosResult result = m_client->DeleteBucket(req, &resp);
    //    EXPECT_FALSE(result.IsSucc());
    //    sleep(1);
    }

    // 清空Object
    {
        {
            GetBucketObjectVersionsReq req(m_bucket_name);
            GetBucketObjectVersionsResp resp;
            CosResult result = m_client->GetBucketObjectVersions(req, &resp);
            EXPECT_TRUE(result.IsSucc());

            const std::vector<COSVersionSummary>& summs = resp.GetVersionSummary();
            for (std::vector<COSVersionSummary>::const_iterator c_itr = summs.begin();
                 c_itr != summs.end(); ++c_itr) {
                const COSVersionSummary& summ = *c_itr;
                DeleteObjectReq del_req(m_bucket_name, summ.m_key);
                DeleteObjectResp del_resp;
                if (!summ.m_version_id.empty()) {
                    del_req.SetXCosVersionId(summ.m_version_id);
                }

                CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
                EXPECT_TRUE(del_result.IsSucc());
                if (!del_result.IsSucc()) {
                    std::cout << "DeleteObject Failed, check object=" << summ.m_key << std::endl;
                }
            }
            sleep(1);
        }
        {
            GetBucketObjectVersionsReq req(m_bucket_name2);
            GetBucketObjectVersionsResp resp;
            CosResult result = m_client2->GetBucketObjectVersions(req, &resp);
            EXPECT_TRUE(result.IsSucc());

            const std::vector<COSVersionSummary>& summs = resp.GetVersionSummary();
            for (std::vector<COSVersionSummary>::const_iterator c_itr = summs.begin();
                 c_itr != summs.end(); ++c_itr) {
                const COSVersionSummary& summ = *c_itr;
                DeleteObjectReq del_req(m_bucket_name2, summ.m_key);
                DeleteObjectResp del_resp;
                if (!summ.m_version_id.empty()) {
                    del_req.SetXCosVersionId(summ.m_version_id);
                }

                CosResult del_result = m_client2->DeleteObject(del_req, &del_resp);
                EXPECT_TRUE(del_result.IsSucc());
                if (!del_result.IsSucc()) {
                    std::cout << "DeleteObject Failed, check object=" << summ.m_key << std::endl;
                }
            }
            sleep(1);
        }
    }
    // Tear down的时候再删除bucket
    // 删除Bucket
    //{
    //    {
    //        DeleteBucketReq req(m_bucket_name);
    //        DeleteBucketResp resp;
    //        CosResult result = m_client->DeleteBucket(req, &resp);
    //        EXPECT_TRUE(result.IsSucc());
    //    }
    //    {
    //        DeleteBucketReq req(m_bucket_name2);
    //        DeleteBucketResp resp;
    //        CosResult result = m_client2->DeleteBucket(req, &resp);
    //        EXPECT_TRUE(result.IsSucc());
    //    }
    //    sleep(1);
    //}
}


TEST_F(BucketOpTest, PutBucketLogging) {
    // bucket日志配置,成功返回true， 失败返回false
    {
        PutBucketLoggingReq req(m_bucket_name);
        PutBucketLoggingResp resp;

        LoggingEnabled rules;
        rules.SetTargetBucket(m_bucket_name);
        rules.SetTargetPrefix("/cos-access-log/");
        req.SetLoggingEnabled(rules);
        CosResult result = m_client->PutBucketLogging(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }

    {
        GetBucketLoggingReq req(m_bucket_name);
        GetBucketLoggingResp resp;
        CosResult result = m_client->GetBucketLogging(req, &resp);
        EXPECT_TRUE(result.IsSucc());

        LoggingEnabled loggingenabled =resp.GetLoggingEnabled();
        EXPECT_EQ(m_bucket_name, loggingenabled.GetTargetBucket());
        EXPECT_EQ("/cos-access-log/", loggingenabled.GetTargetPrefix());
    }
    sleep(1);
}

TEST_F(BucketOpTest, PutBucketWebsite) {
    // 为存储桶配置静态网站,成功返回true,失败返回false
    // 为存储桶配置静态网站
    { 
        PutBucketWebsiteReq req(m_bucket_name);
        PutBucketWebsiteResp resp;
    
        req.SetSuffix("index.xml"); //必选项
        req.SetProtocol("https");
        req.SetKey("Error.html");
    
        //设置重定向规则，最多设置100条 
    
        //设置第一条规则
        RoutingRule routerule1;
        Condition temp_condtion1;
        temp_condtion1.SetHttpErrorCodeReturnedEquals(404);//需要设置，默认404
        routerule1.SetCondition(temp_condtion1);
        Redirect temp_redirect1;
        temp_redirect1.SetProtocol("https");
        temp_redirect1.SetReplaceKeyWith("404.htmp");
        routerule1.SetRedirect(temp_redirect1);
    
        //设置第二条规则
        RoutingRule routerule2;
        Condition temp_condtion2;
        temp_condtion2.SetHttpErrorCodeReturnedEquals(403);//需要设置，默认404
        routerule2.SetCondition(temp_condtion2);
        Redirect temp_redirect2;
        temp_redirect2.SetProtocol("https");
        temp_redirect2.SetReplaceKeyWith("403.htmp");
        routerule2.SetRedirect(temp_redirect2);

        req.AddRoutingRule(routerule1);
        req.AddRoutingRule(routerule2);
       
    
        CosResult result = m_client->PutBucketWebsite(req, &resp);
        EXPECT_TRUE(result.IsSucc());
        sleep(1);
    }

    {
        // 获取与存储桶关联的静态网站配置信息
        GetBucketWebsiteReq req(m_bucket_name);
        GetBucketWebsiteResp resp;
    
        CosResult result = m_client->GetBucketWebsite(req, &resp);
        EXPECT_TRUE(result.IsSucc());

        EXPECT_EQ("https", resp.GetProtocol());
        EXPECT_EQ("Error.html", resp.GetKey());
        EXPECT_EQ("index.xml", resp.GetSuffix());
 
    
        std::vector<RoutingRule> routingrules = resp.GetRoutingRules();
        std::vector<RoutingRule>::iterator it = routingrules.begin();
        
        const Condition& condition = it->GetCondition();
        EXPECT_EQ(404,  condition.GetHttpErrorCodeReturnedEquals());
        // EXPECT_EQ("https",  condition.GetKeyPrefixEquals());
        
        const Redirect& redirect = it->GetRedirect();
        EXPECT_EQ("https", redirect.GetProtocol());
        EXPECT_EQ("404.htmp", redirect.GetReplaceKeyWith());
      
        ++it;
        const Condition& condition2 = it->GetCondition();
        EXPECT_EQ(404,  condition.GetHttpErrorCodeReturnedEquals());;
        EXPECT_EQ("https", redirect.GetProtocol());
        EXPECT_EQ("404.htmp", redirect.GetReplaceKeyWith());
    } 

    {
        DeleteBucketWebsiteReq req(m_bucket_name);
        DeleteBucketWebsiteResp resp;
        CosResult result = m_client->DeleteBucketWebsite(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    } 
    sleep(1);
}

TEST_F(BucketOpTest, PutBucketTagging) {
    // 为存储桶配置Tag,成功返回true,失败返回false
    {
        std::vector<Tag> tagset;
        Tag tag1;
        tag1.SetKey("age");
        tag1.SetValue("19");
     
        Tag tag2;
        tag2.SetKey("name");
        tag2.SetValue("xiaoming");
        tagset.push_back(tag1);
        tagset.push_back(tag2);
        PutBucketTaggingReq req(m_bucket_name);
        req.SetTagSet(tagset);
        PutBucketTaggingResp resp;

        CosResult result = m_client->PutBucketTagging(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }

    {
        GetBucketTaggingReq req(m_bucket_name);
        GetBucketTaggingResp resp;
        CosResult result = m_client->GetBucketTagging(req, &resp);
       
        EXPECT_TRUE(result.IsSucc());

        std::vector<Tag> tagSet = resp.GetTagSet();
        std::vector<Tag>::iterator it = tagSet.begin();

        EXPECT_EQ("age", it->GetKey());
        EXPECT_EQ("19", it->GetValue());
        ++it;
        EXPECT_EQ("name", it->GetKey());
        EXPECT_EQ("xiaoming", it->GetValue());
    }

    {   
        DeleteBucketTaggingReq req(m_bucket_name);
        DeleteBucketTaggingResp resp;

        CosResult result = m_client->DeleteBucketTagging(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }
    sleep(1);
}


TEST_F(BucketOpTest, PutBucketInventory) {
    // 为存储桶配置清单,成功返回true,失败返回false
    {
        PutBucketInventoryReq req(m_bucket_name);

        req.SetId("list3");
        COSBucketDestination destination;
        destination.SetFormat("CSV"); 
        //destination.SetAccountId("100010974959");
        destination.SetAccountId(m_owner);
        std::string target_bucket = "qcs::cos:" + m_config->GetRegion() + "::" + m_bucket_name;
        destination.SetBucket(target_bucket);
        destination.SetPrefix("/");
        destination.SetEncryption(true);
    
        OptionalFields fields;
        fields.SetIsSize(true);
        fields.SetIsLastModified(true);
        fields.SetIsStorageClass(true);
        fields.SetIsMultipartUploaded(true);
        fields.SetIsReplicationStatus(true);
        fields.SetIsEtag(true);
   
        Inventory inventory;
        inventory.SetIsEnable(true);
        inventory.SetIncludedObjectVersions("All");
        inventory.SetFilter("/");
        inventory.SetId(req.GetId());
        inventory.SetFrequency("Daily");
        inventory.SetCOSBucketDestination(destination);
        inventory.SetOptionalFields(fields);
   
        req.SetInventory(inventory);

        PutBucketInventoryResp resp;
        CosResult result = m_client->PutBucketInventory(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }

    {
        PutBucketInventoryReq req(m_bucket_name);

        req.SetId("list2");
        COSBucketDestination destination;
        destination.SetFormat("CSV");
        //destination.SetAccountId("10001223951");
        //destination.SetBucket("qcs::cos:ap-guangzhou::loggtest-1259743198");
        destination.SetAccountId(m_owner);
        std::string target_bucket = "qcs::cos:" + m_config->GetRegion() + "::" + m_bucket_name;
        destination.SetBucket(target_bucket);
        destination.SetPrefix("/");
        destination.SetEncryption(true);
    
        OptionalFields fields;
        fields.SetIsSize(true);
        fields.SetIsLastModified(false);
        fields.SetIsStorageClass(true);
        fields.SetIsMultipartUploaded(true);
        fields.SetIsReplicationStatus(false);
        fields.SetIsEtag(false);
   
        Inventory inventory;
        inventory.SetIsEnable(true);
        inventory.SetIncludedObjectVersions("All");
        inventory.SetFilter("/");
        inventory.SetId(req.GetId());
        inventory.SetFrequency("Daily");
        inventory.SetCOSBucketDestination(destination);
        inventory.SetOptionalFields(fields);
   
        req.SetInventory(inventory);

        PutBucketInventoryResp resp;
        CosResult result = m_client->PutBucketInventory(req, &resp);
        EXPECT_TRUE(result.IsSucc());
        sleep(1);
    }

    {
        GetBucketInventoryReq req(m_bucket_name);
        GetBucketInventoryResp resp;
        req.SetId("list3");

        CosResult result = m_client->GetBucketInventory(req, &resp);
        
        EXPECT_TRUE(result.IsSucc());

        const Inventory inventory = resp.GetInventory();

        EXPECT_TRUE(inventory.GetIsEnable());
        EXPECT_EQ("All", inventory.GetIncludedObjectVersions());
        EXPECT_EQ("/", inventory.GetFilter());
        EXPECT_EQ("list3", inventory.GetId());
        EXPECT_EQ("Daily", inventory.GetFrequency());
        
        COSBucketDestination destination = inventory.GetCOSBucketDestination();

        EXPECT_EQ("CSV", destination.GetFormat());
        //EXPECT_EQ("100010974951", destination.GetAccountId());
        EXPECT_EQ(m_owner, destination.GetAccountId());
        std::string target_bucket = "qcs::cos:" + m_config->GetRegion() + "::" + m_bucket_name;
        EXPECT_EQ(target_bucket, destination.GetBucket());
        EXPECT_TRUE(destination.GetEncryption());

        OptionalFields fields = inventory.GetOptionalFields();
        EXPECT_TRUE(fields.GetIsSize());
        EXPECT_TRUE(fields.GetIsLastModified());
        EXPECT_TRUE(fields.GetIsStorageClass());
        EXPECT_TRUE(fields.GetIsMultipartUploaded());
        EXPECT_TRUE(fields.GetIsReplicationStatus());
        EXPECT_TRUE(fields.GetIsETag());
    }
    
    {
        ListBucketInventoryConfigurationsReq req(m_bucket_name);
        ListBucketInventoryConfigurationsResp resp;
    
        CosResult result = m_client->ListBucketInventoryConfigurations(req, &resp);
        std::vector<Inventory> inventory_vec = resp.GetInventory();
        std::vector<Inventory>::iterator itr = inventory_vec.begin();

        EXPECT_TRUE(result.IsSucc());
        COSBucketDestination  destination;
        OptionalFields  fields;
        for(; itr != inventory_vec.end(); ++itr) {
            std::string target_bucket = "qcs::cos:" + m_config->GetRegion() + "::" + m_bucket_name;
            if(itr->GetId() == "list2") {

                EXPECT_TRUE(itr->GetIsEnable());
                EXPECT_EQ("All", itr->GetIncludedObjectVersions());
                EXPECT_EQ("/", itr->GetFilter());
                EXPECT_EQ("list2", itr->GetId());
                EXPECT_EQ("Daily", itr->GetFrequency());
        
                destination = itr->GetCOSBucketDestination();

                EXPECT_EQ("CSV", destination.GetFormat());
                //EXPECT_EQ("100010974951", destination.GetAccountId());
                //EXPECT_EQ("qcs::cos:ap-guangzhou::loggtest-1259743191", destination.GetBucket());
                EXPECT_EQ(m_owner, destination.GetAccountId());
                EXPECT_EQ(target_bucket, destination.GetBucket());
                EXPECT_TRUE(destination.GetEncryption());

                fields = itr->GetOptionalFields();
                EXPECT_TRUE(fields.GetIsSize());
                EXPECT_TRUE(!fields.GetIsLastModified());
                EXPECT_TRUE(fields.GetIsStorageClass());
                EXPECT_TRUE(fields.GetIsMultipartUploaded());
                EXPECT_TRUE(!fields.GetIsReplicationStatus());
                EXPECT_TRUE(!fields.GetIsETag());
            }
            else {
                EXPECT_TRUE(itr->GetIsEnable());
                EXPECT_EQ("All", itr->GetIncludedObjectVersions());
                EXPECT_EQ("/", itr->GetFilter());
                EXPECT_EQ("list3", itr->GetId());
                EXPECT_EQ("Daily", itr->GetFrequency());
        
                destination = itr->GetCOSBucketDestination();

                EXPECT_EQ("CSV", destination.GetFormat());
                //EXPECT_EQ("100010974951", destination.GetAccountId());
                //EXPECT_EQ("qcs::cos:ap-guangzhou::test3-1259743191", destination.GetBucket());
                EXPECT_EQ(m_owner, destination.GetAccountId());
                EXPECT_EQ(target_bucket, destination.GetBucket());
                EXPECT_TRUE(destination.GetEncryption());

                fields = itr->GetOptionalFields();
                EXPECT_TRUE(fields.GetIsSize());
                EXPECT_TRUE(fields.GetIsLastModified());
                EXPECT_TRUE(fields.GetIsStorageClass());
                EXPECT_TRUE(fields.GetIsMultipartUploaded());
                EXPECT_TRUE(fields.GetIsReplicationStatus());
                EXPECT_TRUE(fields.GetIsETag());
            }
        }
    }

    {   
        DeleteBucketInventoryReq req(m_bucket_name);
        DeleteBucketInventoryResp resp;
        req.SetId("list3");
        CosResult result = m_client->DeleteBucketInventory(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }
   
    {   
        DeleteBucketInventoryReq req(m_bucket_name);
        DeleteBucketInventoryResp resp;
        req.SetId("list2");
        CosResult result = m_client->DeleteBucketInventory(req, &resp);
        EXPECT_TRUE(result.IsSucc());
    }
}
#endif
} // namespace qcloud_cos
