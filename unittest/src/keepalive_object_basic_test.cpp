// Copyright (c) 2026, Tencent Inc.
// All rights reserved.
//
// Description: 长连接（KeepAlive + 连接池）专项单测 —— 基本对象操作。
//   覆盖: PutObjectByFile/GetObjectByFile, PutObjectByStream/GetObjectByStream,
//         HeadObject, IsObjectExist, AppendObject, PutObjectCopy/Copy,
//         ObjectACL, ObjectTagging, DeleteObject/DeleteObjects
//
//   每个用例验证两点:
//     1. 接口在长连接模式下功能正常（返回成功且数据正确）;
//     2. 长连接生效（ConnectionPool 的 Acquire/Hit 计数均增加）。

#include "keepalive_test_common.h"

namespace qcloud_cos {

// KA-OBJ-001: 文件上传/下载
TEST_F(KeepAliveOpTest, KA_OBJ_001_PutObjectByFileAndGetObjectByFile) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  const std::string local_file = "./ka_obj_001_src";
  const std::string file_download = "./ka_obj_001_dst";
  TestUtils::WriteRandomDatatoFile(local_file, 100 * 1024);
  const std::string file_md5_origin = TestUtils::CalcFileMd5(local_file);

  // 两轮上传+下载, 第二轮请求应复用第一轮归还的连接
  for (int i = 0; i < 2; ++i) {
    const std::string object_name = "ka_obj_001_" + std::to_string(i);

    PutObjectByFileReq put_req(m_bucket_name, object_name, local_file);
    PutObjectByFileResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject#" + std::to_string(i))) return;
    EXPECT_EQ(file_md5_origin, put_resp.GetEtag());

    CosResult get_result = CallWithRetry([&]() {
      GetObjectByFileReq req(m_bucket_name, object_name, file_download);
      GetObjectByFileResp resp;
      return m_client->GetObject(req, &resp);
    });
    if (!ExpectSucc(get_result, "GetObject#" + std::to_string(i))) return;
    EXPECT_EQ(file_md5_origin, TestUtils::CalcFileMd5(file_download));
  }

  TestUtils::RemoveFile(local_file);
  TestUtils::RemoveFile(file_download);

  if (!ExpectKeepAliveHit(snap, "KA_OBJ_001")) return;
}

// KA-OBJ-002: 流式上传/下载
TEST_F(KeepAliveOpTest, KA_OBJ_002_PutObjectByStreamAndGetObjectByStream) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  for (int i = 0; i < 2; ++i) {
    const std::string object_name = "ka_obj_002_" + std::to_string(i);
    const std::string content = "ka_obj_002_stream_content_" + std::to_string(i);

    std::istringstream iss(content);
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject#" + std::to_string(i))) return;

    std::ostringstream oss;
    GetObjectByStreamReq get_req(m_bucket_name, object_name, oss);
    GetObjectByStreamResp get_resp;
    CosResult get_result = m_client->GetObject(get_req, &get_resp);
    if (!ExpectSucc(get_result, "GetObject#" + std::to_string(i))) return;
    EXPECT_EQ(content, oss.str());
    EXPECT_EQ(TestUtils::CalcStringMd5(content), get_resp.GetEtag());
  }

  if (!ExpectKeepAliveHit(snap, "KA_OBJ_002")) return;
}

// KA-OBJ-003: HeadObject
TEST_F(KeepAliveOpTest, KA_OBJ_003_HeadObject) {
  // 准备对象
  {
    std::istringstream iss("ka_obj_003_content");
    PutObjectByStreamReq put_req(m_bucket_name, "ka_obj_003", iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject")) return;
  }

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();
  for (int i = 0; i < 2; ++i) {
    HeadObjectReq head_req(m_bucket_name, "ka_obj_003");
    HeadObjectResp head_resp;
    CosResult head_result = m_client->HeadObject(head_req, &head_resp);
    if (!ExpectSucc(head_result, "HeadObject#" + std::to_string(i))) return;
    // "ka_obj_003_content" 长度为 18
    EXPECT_EQ(18u, head_resp.GetContentLength());
  }

  if (!ExpectKeepAliveHit(snap, "KA_OBJ_003")) return;
}

// KA-OBJ-004: IsObjectExist
TEST_F(KeepAliveOpTest, KA_OBJ_004_IsObjectExist) {
  {
    std::istringstream iss("ka_obj_004_content");
    PutObjectByStreamReq put_req(m_bucket_name, "ka_obj_004", iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject")) return;
  }

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();
  EXPECT_TRUE(m_client->IsObjectExist(m_bucket_name, "ka_obj_004"));
  EXPECT_TRUE(m_client->IsObjectExist(m_bucket_name, "ka_obj_004"));
  EXPECT_FALSE(m_client->IsObjectExist(m_bucket_name, "ka_obj_004_not_exist"));

  if (!ExpectKeepAliveHit(snap, "KA_OBJ_004")) return;
}

// KA-OBJ-005: AppendObject 追加
TEST_F(KeepAliveOpTest, KA_OBJ_005_AppendObject) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  const std::string object_name = "ka_obj_005_append";
  const std::string part1 = "ka_append_part1_";
  const std::string part2 = "ka_append_part2";

  // 第一次追加, position = 0
  uint64_t next_position = 0;
  {
    std::istringstream iss(part1);
    AppendObjectReq req(m_bucket_name, object_name, iss);
    req.SetPosition(std::to_string(next_position));
    AppendObjectResp resp;
    CosResult result = m_client->AppendObject(req, &resp);
    if (!ExpectSucc(result, "AppendObject#1")) return;
    next_position = std::stoull(resp.GetNextPosition());
    EXPECT_EQ(part1.size(), next_position);
  }

  // 第二次追加, position = 上次返回的 NextPosition
  {
    std::istringstream iss(part2);
    AppendObjectReq req(m_bucket_name, object_name, iss);
    req.SetPosition(std::to_string(next_position));
    AppendObjectResp resp;
    CosResult result = m_client->AppendObject(req, &resp);
    if (!ExpectSucc(result, "AppendObject#2")) return;
    EXPECT_EQ(part1.size() + part2.size(),
              std::stoull(resp.GetNextPosition()));
  }

  // 下载验证内容（Appendable 对象的 ETag 非 MD5, 用 HeadObject 验证长度）
  {
    HeadObjectReq head_req(m_bucket_name, object_name);
    HeadObjectResp head_resp;
    CosResult head_result = m_client->HeadObject(head_req, &head_resp);
    if (!ExpectSucc(head_result, "HeadObject")) return;
    EXPECT_EQ(part1.size() + part2.size(), head_resp.GetContentLength());
    EXPECT_EQ(kObjectTypeAppendable, head_resp.GetXCosObjectType());
  }

  if (!ExpectKeepAliveHit(snap, "KA_OBJ_005")) return;
}

// KA-OBJ-006: PutObjectCopy / Copy
TEST_F(KeepAliveOpTest, KA_OBJ_006_PutObjectCopyAndCopy) {
  const std::string src_object = "ka_obj_006_src";
  {
    std::istringstream iss("ka_obj_006_content");
    PutObjectByStreamReq put_req(m_bucket_name, src_object, iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject")) return;
  }

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  const std::string host = CosSysConfig::GetHost(m_config->GetAppId(),
                                                 m_config->GetRegion(),
                                                 m_bucket_name);
  {
    PutObjectCopyReq req(m_bucket_name, "ka_obj_006_copy");
    PutObjectCopyResp resp;
    req.SetXCosCopySource(host + "/" + src_object);
    CosResult result = m_client->PutObjectCopy(req, &resp);
    if (!ExpectSucc(result, "PutObjectCopy")) return;
  }

  {
    CopyReq req(m_bucket_name, "ka_obj_006_copy2");
    CopyResp resp;
    req.SetXCosCopySource(host + "/" + src_object);
    CosResult result = m_client->Copy(req, &resp);
    if (!ExpectSucc(result, "Copy")) return;
  }

  // 验证拷贝结果
  {
    EXPECT_TRUE(m_client->IsObjectExist(m_bucket_name, "ka_obj_006_copy"));
    EXPECT_TRUE(m_client->IsObjectExist(m_bucket_name, "ka_obj_006_copy2"));
  }

  if (!ExpectKeepAliveHit(snap, "KA_OBJ_006")) return;
}

// KA-OBJ-007: Object ACL
TEST_F(KeepAliveOpTest, KA_OBJ_007_ObjectACL) {
  const std::string object_name = "ka_obj_007";
  {
    std::istringstream iss("ka_obj_007_content");
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject")) return;
  }

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // Put ACL
  {
    const std::string uin(GetEnvVar("CPP_SDK_V5_UIN"));
    PutObjectACLReq req(m_bucket_name, object_name);
    PutObjectACLResp resp;
    Owner owner = {"qcs::cam::uin/" + uin + ":uin/" + uin,
                   "qcs::cam::uin/" + uin + ":uin/" + uin};
    Grant grant;
    req.SetOwner(owner);
    grant.m_perm = "READ";
    grant.m_grantee.m_type = "RootAccount";
    grant.m_grantee.m_uri =
        "http://cam.qcloud.com/groups/global/AllUsers";
    grant.m_grantee.m_id = "qcs::cam::uin/" + uin + ":uin/" + uin;
    grant.m_grantee.m_display_name =
        "qcs::cam::uin/" + uin + ":uin/" + uin;
    req.AddAccessControlList(grant);

    CosResult result = m_client->PutObjectACL(req, &resp);
    if (!ExpectSucc(result, "PutObjectACL")) return;
  }

  // Get ACL 两次
  for (int i = 0; i < 2; ++i) {
    GetObjectACLReq req(m_bucket_name, object_name);
    GetObjectACLResp resp;
    CosResult result = m_client->GetObjectACL(req, &resp);
    if (!ExpectSucc(result, "GetObjectACL#" + std::to_string(i))) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_OBJ_007")) return;
}

// KA-OBJ-008: Object Tagging
TEST_F(KeepAliveOpTest, KA_OBJ_008_ObjectTagging) {
  const std::string object_name = "ka_obj_008";
  {
    std::istringstream iss("ka_obj_008_content");
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject")) return;
  }

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  {
    PutObjectTaggingReq req(m_bucket_name, object_name);
    PutObjectTaggingResp resp;
    Tag tag1;
    tag1.SetKey("ka_key");
    tag1.SetValue("ka_value");
    req.AddTag(tag1);
    CosResult result = m_client->PutObjectTagging(req, &resp);
    if (!ExpectSucc(result, "PutObjectTagging")) return;
  }

  {
    GetObjectTaggingReq req(m_bucket_name, object_name);
    GetObjectTaggingResp resp;
    CosResult result = m_client->GetObjectTagging(req, &resp);
    if (!ExpectSucc(result, "GetObjectTagging")) return;
    const std::vector<Tag>& tagset = resp.GetTagSet();
    ASSERT_EQ(1u, tagset.size());
    EXPECT_EQ("ka_key", tagset[0].GetKey());
    EXPECT_EQ("ka_value", tagset[0].GetValue());
  }

  {
    DeleteObjectTaggingReq req(m_bucket_name, object_name);
    DeleteObjectTaggingResp resp;
    CosResult result = m_client->DeleteObjectTagging(req, &resp);
    if (!ExpectSucc(result, "DeleteObjectTagging")) return;
  }

  if (!ExpectKeepAliveHit(snap, "KA_OBJ_008")) return;
}

// KA-OBJ-009: DeleteObject / DeleteObjects
TEST_F(KeepAliveOpTest, KA_OBJ_009_DeleteObjects) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // 准备 3 个对象
  for (int i = 0; i < 3; ++i) {
    std::istringstream iss("ka_obj_009_content");
    PutObjectByStreamReq put_req(
        m_bucket_name, "ka_obj_009_" + std::to_string(i), iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject#" + std::to_string(i))) return;
  }

  // 批量删除
  {
    DeleteObjectsReq req(m_bucket_name);
    req.AddObject("ka_obj_009_0");
    req.AddObject("ka_obj_009_1");
    DeleteObjectsResp resp;
    CosResult result = m_client->DeleteObjects(req, &resp);
    if (!ExpectSucc(result, "DeleteObjects")) return;
  }

  // 单个删除两次
  for (int i = 0; i < 2; ++i) {
    DeleteObjectReq del_req(m_bucket_name, "ka_obj_009_2");
    DeleteObjectResp del_resp;
    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
    if (!ExpectSucc(del_result, "DeleteObject#" + std::to_string(i))) return;
  }

  EXPECT_FALSE(m_client->IsObjectExist(m_bucket_name, "ka_obj_009_0"));
  EXPECT_FALSE(m_client->IsObjectExist(m_bucket_name, "ka_obj_009_2"));

  if (!ExpectKeepAliveHit(snap, "KA_OBJ_009")) return;
}

}  // namespace qcloud_cos
