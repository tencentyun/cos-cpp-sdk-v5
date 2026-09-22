// Copyright (c) 2026, Tencent Inc.
// All rights reserved.
//
// Description: 长连接（KeepAlive + 连接池）专项单测 —— 分块上传/分块复制/并发下载。
//   覆盖: InitMultiUpload/UploadPartData/ListParts/CompleteMultiUpload,
//         MultiUploadObject, UploadPartCopyData,
//         AbortMultiUpload/ListMultipartUpload,
//         ResumableGetObject, MultiThreadDownload
//
//   每个用例验证两点:
//     1. 接口在长连接模式下功能正常（返回成功且数据正确）;
//     2. 长连接生效（ConnectionPool 的 Acquire/Hit 计数均增加）。

#include "keepalive_test_common.h"

namespace qcloud_cos {

// KA-MP-001: 分块上传完整流程 Init -> UploadPart x2 -> ListParts -> Complete
TEST_F(KeepAliveOpTest, KA_MP_001_MultipartUploadFlow) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  const std::string object_name = "ka_mp_001";
  // COS 要求除最后一个分片外, 每个分片不小于 1MB
  const std::string part1(1024 * 1024, 'a');
  const std::string part2(512 * 1024, 'b');

  // 1. Init
  std::string upload_id;
  {
    InitMultiUploadReq req(m_bucket_name, object_name);
    InitMultiUploadResp resp;
    CosResult result = m_client->InitMultiUpload(req, &resp);
    if (!ExpectSucc(result, "InitMultiUpload")) return;
    upload_id = resp.GetUploadId();
  }

  // 2. UploadPart x2
  std::vector<std::string> etags;
  std::vector<uint64_t> part_numbers;
  {
    std::istringstream iss1(part1);
    UploadPartDataReq req1(m_bucket_name, object_name, upload_id, iss1);
    req1.SetPartNumber(1);
    UploadPartDataResp resp1;
    CosResult result1 = m_client->UploadPartData(req1, &resp1);
    if (!ExpectSucc(result1, "UploadPartData#1")) return;
    etags.push_back(resp1.GetEtag());
    part_numbers.push_back(1);

    std::istringstream iss2(part2);
    UploadPartDataReq req2(m_bucket_name, object_name, upload_id, iss2);
    req2.SetPartNumber(2);
    UploadPartDataResp resp2;
    CosResult result2 = m_client->UploadPartData(req2, &resp2);
    if (!ExpectSucc(result2, "UploadPartData#2")) return;
    etags.push_back(resp2.GetEtag());
    part_numbers.push_back(2);
  }

  // 3. ListParts
  {
    ListPartsReq req(m_bucket_name, object_name, upload_id);
    ListPartsResp resp;
    CosResult result = m_client->ListParts(req, &resp);
    if (!ExpectSucc(result, "ListParts")) return;
    const std::vector<Part>& parts = resp.GetParts();
    ASSERT_EQ(2u, parts.size());
    EXPECT_EQ(1u, parts[0].m_part_num);
    EXPECT_EQ(2u, parts[1].m_part_num);
  }

  // 4. Complete
  {
    CompleteMultiUploadReq req(m_bucket_name, object_name, upload_id);
    req.SetEtags(etags);
    req.SetPartNumbers(part_numbers);
    CompleteMultiUploadResp resp;
    CosResult result = m_client->CompleteMultiUpload(req, &resp);
    if (!ExpectSucc(result, "CompleteMultiUpload")) return;
  }

  // 5. 下载验证
  {
    std::ostringstream oss;
    GetObjectByStreamReq get_req(m_bucket_name, object_name, oss);
    GetObjectByStreamResp get_resp;
    CosResult get_result = m_client->GetObject(get_req, &get_resp);
    if (!ExpectSucc(get_result, "GetObject")) return;
    EXPECT_EQ(part1 + part2, oss.str());
  }

  if (!ExpectKeepAliveHit(snap, "KA_MP_001")) return;
}

// KA-MP-002: MultiPutObject 一步式多分片上传（多线程并发）
TEST_F(KeepAliveOpTest, KA_MP_002_MultiPutObject) {
  // 分片大小已在 SetUpTestCase 设为 1MB, 生成 2.5MB 文件触发 3 个分片
  const std::string local_file = "./ka_mp_002_src";
  const std::string file_download = "./ka_mp_002_dst";
  const uint64_t file_size = 2 * 1024 * 1024 + 512 * 1024;
  TestUtils::WriteRandomDatatoFile(local_file, file_size);
  const std::string file_md5_origin = TestUtils::CalcFileMd5(local_file);

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  for (int i = 0; i < 2; ++i) {
    const std::string object_name = "ka_mp_002_" + std::to_string(i);
    MultiPutObjectReq put_req(m_bucket_name, object_name, local_file);
    MultiPutObjectResp put_resp;
    CosResult put_result = m_client->MultiPutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "MultiPutObject#" + std::to_string(i))) return;

    // 下载验证
    CosResult get_result = CallWithRetry([&]() {
      GetObjectByFileReq get_req(m_bucket_name, object_name, file_download);
      GetObjectByFileResp get_resp;
      return m_client->GetObject(get_req, &get_resp);
    });
    if (!ExpectSucc(get_result, "GetObject#" + std::to_string(i))) return;
    EXPECT_EQ(file_md5_origin, TestUtils::CalcFileMd5(file_download));
  }

  TestUtils::RemoveFile(local_file);
  TestUtils::RemoveFile(file_download);

  if (!ExpectKeepAliveHit(snap, "KA_MP_002")) return;
}

// KA-MP-003: UploadPartCopyData 分块复制
TEST_F(KeepAliveOpTest, KA_MP_003_UploadPartCopyData) {
  const std::string src_object = "ka_mp_003_src";
  const std::string local_file = "./ka_mp_003_src_file";
  // COS 要求除最后一个分片外, 每个分片不小于 1MB, 源文件取 2.5MB
  const uint64_t file_size = 2 * 1024 * 1024 + 512 * 1024;
  TestUtils::WriteRandomDatatoFile(local_file, file_size);
  const std::string file_md5_origin = TestUtils::CalcFileMd5(local_file);
  {
    PutObjectByFileReq put_req(m_bucket_name, src_object, local_file);
    PutObjectByFileResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject")) return;
  }

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  const std::string object_name = "ka_mp_003_copy";
  const std::string host = CosSysConfig::GetHost(m_config->GetAppId(),
                                                 m_config->GetRegion(),
                                                 m_bucket_name);

  // 1. Init
  std::string upload_id;
  {
    InitMultiUploadReq req(m_bucket_name, object_name);
    InitMultiUploadResp resp;
    CosResult result = m_client->InitMultiUpload(req, &resp);
    if (!ExpectSucc(result, "InitMultiUpload")) return;
    upload_id = resp.GetUploadId();
  }

  // 2. 分两块复制（1MB + 1.5MB）
  std::vector<std::string> etags;
  std::vector<uint64_t> part_numbers;
  {
    const uint64_t first_part = 1024 * 1024;
    for (int part = 1; part <= 2; ++part) {
      UploadPartCopyDataReq req(m_bucket_name, object_name, upload_id, part);
      req.SetXCosCopySource(host + "/" + src_object);
      std::string range =
          "bytes=" + std::to_string((part - 1) * first_part) + "-" +
          std::to_string(part == 1 ? first_part - 1 : file_size - 1);
      req.SetXCosCopySourceRange(range);
      UploadPartCopyDataResp resp;
      CosResult result = m_client->UploadPartCopyData(req, &resp);
      if (!ExpectSucc(result, "UploadPartCopyData#" + std::to_string(part))) {
        return;
      }
      etags.push_back(resp.GetEtag());
      part_numbers.push_back(part);
    }
  }

  // 3. Complete
  {
    CompleteMultiUploadReq req(m_bucket_name, object_name, upload_id);
    req.SetEtags(etags);
    req.SetPartNumbers(part_numbers);
    CompleteMultiUploadResp resp;
    CosResult result = m_client->CompleteMultiUpload(req, &resp);
    if (!ExpectSucc(result, "CompleteMultiUpload")) return;
  }

  // 4. 下载验证
  {
    const std::string file_download = "./ka_mp_003_dst";
    GetObjectByFileReq get_req(m_bucket_name, object_name, file_download);
    GetObjectByFileResp get_resp;
    CosResult get_result = m_client->GetObject(get_req, &get_resp);
    if (!ExpectSucc(get_result, "GetObject")) return;
    EXPECT_EQ(file_md5_origin, TestUtils::CalcFileMd5(file_download));
    TestUtils::RemoveFile(file_download);
  }

  TestUtils::RemoveFile(local_file);

  if (!ExpectKeepAliveHit(snap, "KA_MP_003")) return;
}

// KA-MP-004: AbortMultiUpload / ListMultipartUpload
TEST_F(KeepAliveOpTest, KA_MP_004_AbortAndListMultipartUpload) {
  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // 1. 发起两个未完成的分块上传
  for (int i = 0; i < 2; ++i) {
    InitMultiUploadReq req(m_bucket_name, "ka_mp_004_" + std::to_string(i));
    InitMultiUploadResp resp;
    CosResult result = m_client->InitMultiUpload(req, &resp);
    if (!ExpectSucc(result, "InitMultiUpload#" + std::to_string(i))) return;
  }

  // 2. ListMultipartUpload 两次, 应能看到未完成的分块
  for (int i = 0; i < 2; ++i) {
    ListMultipartUploadReq req(m_bucket_name);
    ListMultipartUploadResp resp;
    CosResult result = m_client->ListMultipartUpload(req, &resp);
    if (!ExpectSucc(result, "ListMultipartUpload#" + std::to_string(i))) return;
    EXPECT_GE(resp.GetUpload().size(), 2u);
  }

  // 3. Abort 所有未完成的分块
  {
    ListMultipartUploadReq list_req(m_bucket_name);
    ListMultipartUploadResp list_resp;
    CosResult list_result = m_client->ListMultipartUpload(list_req, &list_resp);
    if (!ExpectSucc(list_result, "ListMultipartUpload")) return;
    std::vector<Upload> uploads = list_resp.GetUpload();
    for (const auto& upload : uploads) {
      AbortMultiUploadReq abort_req(m_bucket_name, upload.m_key,
                                    upload.m_uploadid);
      AbortMultiUploadResp abort_resp;
      CosResult abort_result = m_client->AbortMultiUpload(abort_req, &abort_resp);
      if (!ExpectSucc(abort_result, "AbortMultiUpload")) return;
    }
  }

  if (!ExpectKeepAliveHit(snap, "KA_MP_004")) return;
}

// KA-MP-005: ResumableGetObject 断点（分片）下载
TEST_F(KeepAliveOpTest, KA_MP_005_ResumableGetObject) {
  const std::string object_name = "ka_mp_005";
  const std::string local_file = "./ka_mp_005_src";
  const std::string file_download = "./ka_mp_005_dst";
  const uint64_t file_size = 2 * 1024 * 1024 + 512 * 1024;

  // 上传测试文件
  TestUtils::WriteRandomDatatoFile(local_file, file_size);
  const std::string file_md5_origin = TestUtils::CalcFileMd5(local_file);
  {
    PutObjectByFileReq put_req(m_bucket_name, object_name, local_file);
    PutObjectByFileResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject")) return;
  }

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // 两次断点下载（DownSliceSize 已设为 1MB, 2.5MB 文件分 3 片下载）
  for (int i = 0; i < 2; ++i) {
    CosResult get_result = CallWithRetry([&]() {
      GetObjectByFileReq get_req(m_bucket_name, object_name, file_download);
      GetObjectByFileResp get_resp;
      return m_client->ResumableGetObject(get_req, &get_resp);
    });
    if (!ExpectSucc(get_result, "ResumableGetObject#" + std::to_string(i))) {
      return;
    }
    EXPECT_EQ(file_md5_origin, TestUtils::CalcFileMd5(file_download));
    TestUtils::RemoveFile(file_download);
  }

  TestUtils::RemoveFile(local_file);

  if (!ExpectKeepAliveHit(snap, "KA_MP_005")) return;
}

// KA-MP-006: MultiGetObject 多线程下载
TEST_F(KeepAliveOpTest, KA_MP_006_MultiGetObject) {
  const std::string object_name = "ka_mp_006";
  const std::string local_file = "./ka_mp_006_src";
  const std::string file_download = "./ka_mp_006_dst";
  const uint64_t file_size = 2 * 1024 * 1024 + 512 * 1024;

  TestUtils::WriteRandomDatatoFile(local_file, file_size);
  const std::string file_md5_origin = TestUtils::CalcFileMd5(local_file);
  {
    PutObjectByFileReq put_req(m_bucket_name, object_name, local_file);
    PutObjectByFileResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject")) return;
  }

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  for (int i = 0; i < 2; ++i) {
    CosResult get_result = CallWithRetry([&]() {
      MultiGetObjectReq get_req(m_bucket_name, object_name, file_download);
      MultiGetObjectResp get_resp;
      return m_client->MultiGetObject(get_req, &get_resp);
    });
    if (!ExpectSucc(get_result, "MultiGetObject#" + std::to_string(i))) return;
    EXPECT_EQ(file_md5_origin, TestUtils::CalcFileMd5(file_download));
    TestUtils::RemoveFile(file_download);
  }

  TestUtils::RemoveFile(local_file);

  if (!ExpectKeepAliveHit(snap, "KA_MP_006")) return;
}

}  // namespace qcloud_cos
