// Copyright (c) 2026, Tencent Inc.
// All rights reserved.
//
// Description: 长连接（KeepAlive + 连接池）专项单测 —— 桶操作。
//   覆盖: PutBucket/HeadBucket/GetBucket/GetBucketLocation/DeleteBucket,
//         GetService/IsBucketExist, BucketVersioning, BucketACL, BucketCORS,
//         BucketTagging, BucketLifecycle, BucketPolicy, BucketLogging,
//         BucketWebsite, BucketReferer, BucketInventory
//
//   每个用例验证两点:
//     1. 接口在长连接模式下功能正常（返回成功且数据正确）;
//     2. 长连接生效（ConnectionPool 的 Acquire/Hit 计数均增加）。

#include "keepalive_test_common.h"

namespace qcloud_cos {

// KA-BKT-001: 建桶/Head/Get/Location/删桶完整流程（使用独立临时桶）
TEST_F(KeepAliveOpTest, KA_BKT_001_BucketLifecycleFlow) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // 1. HeadBucket 两次
  // 注意: 桶刚创建时元数据同步可能有延迟(读最终一致), 做有限重试
  for (int i = 0; i < 2; ++i) {
    HeadBucketReq req(m_bucket_name_tmp);
    HeadBucketResp resp;
    CosResult result;
    int retry = 0;
    do {
      result = m_client->HeadBucket(req, &resp);
      if (result.IsSucc()) break;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    } while (++retry < 10);
    if (!ExpectSucc(result, "HeadBucket#" + std::to_string(i))) return;
  }

  // 2. GetBucket 两次（列对象）
  for (int i = 0; i < 2; ++i) {
    GetBucketReq req(m_bucket_name_tmp);
    GetBucketResp resp;
    CosResult result = m_client->GetBucket(req, &resp);
    if (!ExpectSucc(result, "GetBucket#" + std::to_string(i))) return;
    EXPECT_TRUE(resp.GetContents().empty());
  }

  // 3. GetBucketLocation
  {
    std::string location = m_client->GetBucketLocation(m_bucket_name_tmp);
    EXPECT_EQ(m_config->GetRegion(), location);
  }

  // 4. 删桶后再建桶
  {
    DeleteBucketReq del_req(m_bucket_name_tmp);
    DeleteBucketResp del_resp;
    CosResult del_result = m_client->DeleteBucket(del_req, &del_resp);
    if (!ExpectSucc(del_result, "DeleteBucket")) return;

    PutBucketReq put_req(m_bucket_name_tmp);
    PutBucketResp put_resp;
    CosResult put_result = m_client->PutBucket(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutBucket")) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_001")) return;
}

// KA-BKT-002: GetService / IsBucketExist
TEST_F(KeepAliveOpTest, KA_BKT_002_GetServiceAndIsBucketExist) {
  // GetService 响应解析会触发 SDK 对每个未知字段打印全量 body(WARN),
  // 用例内临时压低日志级别避免刷屏(不影响其他用例的 DBG 输出)
  ScopedQuietLog quiet;

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // GetService 走 service.cos.myqcloud.com 端点, 调用两次验证该端点连接复用
  for (int i = 0; i < 2; ++i) {
    GetServiceReq req;
    GetServiceResp resp;
    CosResult result = m_client->GetService(req, &resp);
    if (!ExpectSucc(result, "GetService#" + std::to_string(i))) return;
    EXPECT_FALSE(resp.GetBuckets().empty());
  }

  EXPECT_TRUE(m_client->IsBucketExist(m_bucket_name));
  EXPECT_TRUE(m_client->IsBucketExist(m_bucket_name));

  if (!ExpectKeepAliveHit(snap, "KA_BKT_002")) return;
}

// KA-BKT-003: BucketVersioning
TEST_F(KeepAliveOpTest, KA_BKT_003_BucketVersioning) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // 开启版本控制
  {
    PutBucketVersioningReq req(m_bucket_name_cfg);
    PutBucketVersioningResp resp;
    req.SetStatus(true);
    CosResult result = m_client->PutBucketVersioning(req, &resp);
    if (!ExpectSucc(result, "PutBucketVersioning#enable")) return;
  }

  // 查询两次（服务端配置生效可能有延迟, 只验证请求成功）
  for (int i = 0; i < 2; ++i) {
    GetBucketVersioningReq req(m_bucket_name_cfg);
    GetBucketVersioningResp resp;
    CosResult result = m_client->GetBucketVersioning(req, &resp);
    if (!ExpectSucc(result, "GetBucketVersioning#" + std::to_string(i))) return;
  }

  // 关闭版本控制
  {
    PutBucketVersioningReq req(m_bucket_name_cfg);
    PutBucketVersioningResp resp;
    req.SetStatus(false);
    CosResult result = m_client->PutBucketVersioning(req, &resp);
    if (!ExpectSucc(result, "PutBucketVersioning#suspend")) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_003")) return;
}

// KA-BKT-004: BucketACL
TEST_F(KeepAliveOpTest, KA_BKT_004_BucketACL) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  const std::string uin(GetEnvVar("CPP_SDK_V5_UIN"));

  // Get 两次
  for (int i = 0; i < 2; ++i) {
    GetBucketACLReq req(m_bucket_name_cfg);
    GetBucketACLResp resp;
    CosResult result = m_client->GetBucketACL(req, &resp);
    if (!ExpectSucc(result, "GetBucketACL#" + std::to_string(i))) return;
  }

  // Put
  {
    PutBucketACLReq req(m_bucket_name_cfg);
    PutBucketACLResp resp;
    Owner owner = {"qcs::cam::uin/" + uin + ":uin/" + uin,
                   "qcs::cam::uin/" + uin + ":uin/" + uin};
    req.SetOwner(owner);
    Grant grant;
    grant.m_perm = "READ";
    grant.m_grantee.m_type = "RootAccount";
    grant.m_grantee.m_uri =
        "http://cam.qcloud.com/groups/global/AllUsers";
    grant.m_grantee.m_id = "qcs::cam::uin/" + uin + ":uin/" + uin;
    grant.m_grantee.m_display_name =
        "qcs::cam::uin/" + uin + ":uin/" + uin;
    req.AddAccessControlList(grant);

    CosResult result = m_client->PutBucketACL(req, &resp);
    if (!ExpectSucc(result, "PutBucketACL")) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_004")) return;
}

// KA-BKT-005: BucketCORS
TEST_F(KeepAliveOpTest, KA_BKT_005_BucketCORS) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // Put CORS
  {
    PutBucketCORSReq req(m_bucket_name_cfg);
    PutBucketCORSResp resp;
    CORSRule rule;
    rule.m_id = "ka_cors_rule_00";
    rule.m_max_age_secs = "600";
    rule.m_allowed_headers.push_back("x-cos-meta-test");
    rule.m_allowed_origins.push_back("http://www.qq.com");
    rule.m_allowed_methods.push_back("PUT");
    rule.m_allowed_methods.push_back("GET");
    rule.m_expose_headers.push_back("x-cos-expose");
    req.AddRule(rule);
    CosResult result = m_client->PutBucketCORS(req, &resp);
    if (!ExpectSucc(result, "PutBucketCORS")) return;
  }

  // Get CORS 两次
  for (int i = 0; i < 2; ++i) {
    GetBucketCORSReq req(m_bucket_name_cfg);
    GetBucketCORSResp resp;
    CosResult result = m_client->GetBucketCORS(req, &resp);
    if (!ExpectSucc(result, "GetBucketCORS#" + std::to_string(i))) return;
    EXPECT_EQ(1u, resp.GetCORSRules().size());
  }

  // Delete CORS
  {
    DeleteBucketCORSReq req(m_bucket_name_cfg);
    DeleteBucketCORSResp resp;
    CosResult result = m_client->DeleteBucketCORS(req, &resp);
    if (!ExpectSucc(result, "DeleteBucketCORS")) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_005")) return;
}

// KA-BKT-006: BucketTagging
TEST_F(KeepAliveOpTest, KA_BKT_006_BucketTagging) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // Put
  {
    std::vector<Tag> tagset;
    Tag tag1;
    tag1.SetKey("ka_age");
    tag1.SetValue("19");
    Tag tag2;
    tag2.SetKey("ka_name");
    tag2.SetValue("keepalive");
    tagset.push_back(tag1);
    tagset.push_back(tag2);

    PutBucketTaggingReq req(m_bucket_name_cfg);
    req.SetTagSet(tagset);
    PutBucketTaggingResp resp;
    CosResult result = m_client->PutBucketTagging(req, &resp);
    if (!ExpectSucc(result, "PutBucketTagging")) return;
  }

  // Get 两次
  for (int i = 0; i < 2; ++i) {
    GetBucketTaggingReq req(m_bucket_name_cfg);
    GetBucketTaggingResp resp;
    CosResult result = m_client->GetBucketTagging(req, &resp);
    if (!ExpectSucc(result, "GetBucketTagging#" + std::to_string(i))) return;
    EXPECT_EQ(2u, resp.GetTagSet().size());
  }

  // Delete
  {
    DeleteBucketTaggingReq req(m_bucket_name_cfg);
    DeleteBucketTaggingResp resp;
    CosResult result = m_client->DeleteBucketTagging(req, &resp);
    if (!ExpectSucc(result, "DeleteBucketTagging")) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_006")) return;
}

// KA-BKT-007: BucketLifecycle
TEST_F(KeepAliveOpTest, KA_BKT_007_BucketLifecycle) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // Put
  {
    PutBucketLifecycleReq req(m_bucket_name_cfg);
    PutBucketLifecycleResp resp;
    LifecycleRule rule;
    rule.SetIsEnable(true);
    rule.SetId("ka_lifecycle_rule00");
    LifecycleFilter filter;
    filter.SetPrefix("ka_prefix");
    rule.SetFilter(filter);
    LifecycleTransition transition;
    transition.SetDays(30);
    transition.SetStorageClass("STANDARD_IA");
    rule.AddTransition(transition);
    req.AddRule(rule);
    CosResult result = m_client->PutBucketLifecycle(req, &resp);
    if (!ExpectSucc(result, "PutBucketLifecycle")) return;
  }

  // Get 两次
  for (int i = 0; i < 2; ++i) {
    GetBucketLifecycleReq req(m_bucket_name_cfg);
    GetBucketLifecycleResp resp;
    CosResult result = m_client->GetBucketLifecycle(req, &resp);
    if (!ExpectSucc(result, "GetBucketLifecycle#" + std::to_string(i))) return;
    ASSERT_EQ(1u, resp.GetRules().size());
    EXPECT_EQ("ka_lifecycle_rule00", resp.GetRules()[0].GetId());
  }

  // Delete
  {
    DeleteBucketLifecycleReq req(m_bucket_name_cfg);
    DeleteBucketLifecycleResp resp;
    CosResult result = m_client->DeleteBucketLifecycle(req, &resp);
    if (!ExpectSucc(result, "DeleteBucketLifecycle")) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_007")) return;
}

// KA-BKT-008: BucketPolicy
TEST_F(KeepAliveOpTest, KA_BKT_008_BucketPolicy) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // Put
  {
    PutBucketPolicyReq req(m_bucket_name_cfg);
    PutBucketPolicyResp resp;
    std::string bucket_policy =
        "  {"
        "    \"Statement\": ["
        "      {"
        "        \"Principal\": {"
        "          \"qcs\": ["
        "            \"*\""
        "          ]"
        "        },"
        "        \"Effect\": \"allow\","
        "        \"Action\": ["
        "          \"cos:PutObject\""
        "        ],"
        "        \"Resource\": ["
        "          \"qcs::cos:";
    bucket_policy += GetEnvVar("CPP_SDK_V5_REGION");
    bucket_policy += ":uid/";
    bucket_policy += GetEnvVar("CPP_SDK_V5_APPID");
    bucket_policy += ":";
    bucket_policy += m_bucket_name_cfg;
    bucket_policy += "/*\""
                     "        ]"
                     "      }"
                     "    ],"
                     "    \"Version\": \"2.0\""
                     "  }";
    req.SetBody(bucket_policy);
    CosResult result = m_client->PutBucketPolicy(req, &resp);
    if (!ExpectSucc(result, "PutBucketPolicy")) return;
  }

  // Get 两次
  for (int i = 0; i < 2; ++i) {
    GetBucketPolicyReq req(m_bucket_name_cfg);
    GetBucketPolicyResp resp;
    CosResult result = m_client->GetBucketPolicy(req, &resp);
    if (!ExpectSucc(result, "GetBucketPolicy#" + std::to_string(i))) return;
  }

  // Delete
  {
    DeleteBucketPolicyReq req(m_bucket_name_cfg);
    DeleteBucketPolicyResp resp;
    CosResult result = m_client->DeleteBucketPolicy(req, &resp);
    if (!ExpectSucc(result, "DeleteBucketPolicy")) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_008")) return;
}

// KA-BKT-009: BucketLogging
TEST_F(KeepAliveOpTest, KA_BKT_009_BucketLogging) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // Put
  {
    PutBucketLoggingReq req(m_bucket_name_cfg);
    PutBucketLoggingResp resp;
    LoggingEnabled rules;
    rules.SetTargetBucket(m_bucket_name_cfg);
    rules.SetTargetPrefix("/ka-access-log/");
    req.SetLoggingEnabled(rules);
    CosResult result = m_client->PutBucketLogging(req, &resp);
    if (!ExpectSucc(result, "PutBucketLogging")) return;
  }

  // Get 两次
  for (int i = 0; i < 2; ++i) {
    GetBucketLoggingReq req(m_bucket_name_cfg);
    GetBucketLoggingResp resp;
    CosResult result = m_client->GetBucketLogging(req, &resp);
    if (!ExpectSucc(result, "GetBucketLogging#" + std::to_string(i))) return;
    LoggingEnabled enabled = resp.GetLoggingEnabled();
    EXPECT_EQ(m_bucket_name_cfg, enabled.GetTargetBucket());
    EXPECT_EQ("/ka-access-log/", enabled.GetTargetPrefix());
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_009")) return;
}

// KA-BKT-010: BucketWebsite
TEST_F(KeepAliveOpTest, KA_BKT_010_BucketWebsite) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // Put
  {
    PutBucketWebsiteReq req(m_bucket_name_cfg);
    PutBucketWebsiteResp resp;
    req.SetSuffix("index.xml");
    req.SetProtocol("https");
    req.SetKey("Error.html");
    CosResult result = m_client->PutBucketWebsite(req, &resp);
    if (!ExpectSucc(result, "PutBucketWebsite")) return;
  }

  // Get 两次
  for (int i = 0; i < 2; ++i) {
    GetBucketWebsiteReq req(m_bucket_name_cfg);
    GetBucketWebsiteResp resp;
    CosResult result = m_client->GetBucketWebsite(req, &resp);
    if (!ExpectSucc(result, "GetBucketWebsite#" + std::to_string(i))) return;
    EXPECT_EQ("https", resp.GetProtocol());
    EXPECT_EQ("index.xml", resp.GetSuffix());
  }

  // Delete
  {
    DeleteBucketWebsiteReq req(m_bucket_name_cfg);
    DeleteBucketWebsiteResp resp;
    CosResult result = m_client->DeleteBucketWebsite(req, &resp);
    if (!ExpectSucc(result, "DeleteBucketWebsite")) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_010")) return;
}

// KA-BKT-011: BucketReferer
TEST_F(KeepAliveOpTest, KA_BKT_011_BucketReferer) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // Put
  {
    PutBucketRefererReq req(m_bucket_name_cfg);
    PutBucketRefererResp resp;
    req.SetStatus("Enabled");
    req.SetRefererType("White-List");
    req.AddDomain("ka-test1.com");
    req.AddDomain("ka-test2.com");
    CosResult result = m_client->PutBucketReferer(req, &resp);
    if (!ExpectSucc(result, "PutBucketReferer")) return;
  }

  // Get 两次
  for (int i = 0; i < 2; ++i) {
    GetBucketRefererReq req(m_bucket_name_cfg);
    GetBucketRefererResp resp;
    CosResult result = m_client->GetBucketReferer(req, &resp);
    if (!ExpectSucc(result, "GetBucketReferer#" + std::to_string(i))) return;
    EXPECT_EQ("Enabled", resp.GetStatus());
    EXPECT_EQ("White-List", resp.GetRefererType());
    ASSERT_EQ(2u, resp.GetDomainList().size());
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_011")) return;
}

// KA-BKT-012: BucketInventory
TEST_F(KeepAliveOpTest, KA_BKT_012_BucketInventory) {
  const std::string owner_uin = GetEnvVar("CPP_SDK_V5_UIN");
  const std::string inventory_id = "ka_inventory_00";

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // Put
  {
    PutBucketInventoryReq req(m_bucket_name_cfg);
    req.SetId(inventory_id);
    COSBucketDestination destination;
    destination.SetFormat("CSV");
    destination.SetAccountId(owner_uin);
    destination.SetBucket("qcs::cos:" + m_config->GetRegion() + "::" +
                          m_bucket_name_cfg);
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
    inventory.SetId(inventory_id);
    inventory.SetFrequency("Daily");
    inventory.SetCOSBucketDestination(destination);
    inventory.SetOptionalFields(fields);
    req.SetInventory(inventory);

    PutBucketInventoryResp resp;
    CosResult result = m_client->PutBucketInventory(req, &resp);
    if (!ExpectSucc(result, "PutBucketInventory")) return;
  }

  // Get 两次
  for (int i = 0; i < 2; ++i) {
    GetBucketInventoryReq req(m_bucket_name_cfg);
    req.SetId(inventory_id);
    GetBucketInventoryResp resp;
    CosResult result = m_client->GetBucketInventory(req, &resp);
    if (!ExpectSucc(result, "GetBucketInventory#" + std::to_string(i))) return;
    EXPECT_EQ(inventory_id, resp.GetInventory().GetId());
  }

  // List
  {
    ListBucketInventoryConfigurationsReq req(m_bucket_name_cfg);
    ListBucketInventoryConfigurationsResp resp;
    CosResult result = m_client->ListBucketInventoryConfigurations(req, &resp);
    if (!ExpectSucc(result, "ListBucketInventoryConfigurations")) return;
    EXPECT_GE(resp.GetInventory().size(), 1u);
  }

  // Delete
  {
    DeleteBucketInventoryReq req(m_bucket_name_cfg);
    req.SetId(inventory_id);
    DeleteBucketInventoryResp resp;
    CosResult result = m_client->DeleteBucketInventory(req, &resp);
    if (!ExpectSucc(result, "DeleteBucketInventory")) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_BKT_012")) return;
}

}  // namespace qcloud_cos
