// Copyright (c) 2026, Tencent Inc.
// All rights reserved.
//
// Description: 长连接（KeepAlive + 连接池）专项单测 —— 异步操作。
//   覆盖: AsyncPutObject(文件/流), AsyncGetObject, AsyncMultiPutObject,
//         AsyncMultiGetObject, AsyncResumableGetObject
//
//   每个用例验证两点:
//     1. 接口在长连接模式下功能正常（返回成功且数据正确）;
//     2. 长连接生效（ConnectionPool 的 Acquire/Hit 计数均增加）。

#include "keepalive_test_common.h"

namespace qcloud_cos {

// KA-ASYNC-001: AsyncPutObject 异步简单上传（文件 + 流）
TEST_F(KeepAliveOpTest, KA_ASYNC_001_AsyncPutObject) {
  const std::string local_file = "./ka_async_001_src";
  TestUtils::WriteRandomDatatoFile(local_file, 1024);
  const std::string file_md5_origin = TestUtils::CalcFileMd5(local_file);

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  // 1. 文件异步上传
  {
    AsyncPutObjectReq put_req(m_bucket_name, "ka_async_001_file", local_file);
    SharedAsyncContext context = m_client->AsyncPutObject(put_req);
    context->WaitUntilFinish();
    if (!ExpectSucc(context->GetResult(), "AsyncPutObject#file")) return;
    EXPECT_TRUE(m_client->IsObjectExist(m_bucket_name, "ka_async_001_file"));
  }

  // 2. 流式异步上传
  {
    std::istringstream iss("ka_async_001_stream_content");
    AsyncPutObjectByStreamReq put_req(m_bucket_name, "ka_async_001_stream",
                                      iss);
    SharedAsyncContext context = m_client->AsyncPutObject(put_req);
    context->WaitUntilFinish();
    if (!ExpectSucc(context->GetResult(), "AsyncPutObject#stream")) return;
    EXPECT_TRUE(m_client->IsObjectExist(m_bucket_name, "ka_async_001_stream"));
  }

  TestUtils::RemoveFile(local_file);

  if (!ExpectKeepAliveHit(snap, "KA_ASYNC_001")) return;
}

// KA-ASYNC-002: AsyncGetObject 异步简单下载
TEST_F(KeepAliveOpTest, KA_ASYNC_002_AsyncGetObject) {
  const std::string object_name = "ka_async_002";
  const std::string content = "ka_async_002_content";
  {
    std::istringstream iss(content);
    PutObjectByStreamReq put_req(m_bucket_name, object_name, iss);
    PutObjectByStreamResp put_resp;
    CosResult put_result = m_client->PutObject(put_req, &put_resp);
    if (!ExpectSucc(put_result, "PutObject")) return;
  }

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  for (int i = 0; i < 2; ++i) {
    const std::string file_download =
        "./ka_async_002_dst_" + std::to_string(i);
    CosResult get_result = CallWithRetry([&]() {
      AsyncGetObjectReq get_req(m_bucket_name, object_name, file_download);
      SharedAsyncContext context = m_client->AsyncGetObject(get_req);
      context->WaitUntilFinish();
      return context->GetResult();
    });
    if (!ExpectSucc(get_result, "AsyncGetObject#" + std::to_string(i))) return;
    EXPECT_EQ(TestUtils::CalcStringMd5(content),
              TestUtils::CalcFileMd5(file_download));
    TestUtils::RemoveFile(file_download);
  }

  if (!ExpectKeepAliveHit(snap, "KA_ASYNC_002")) return;
}

// KA-ASYNC-003: AsyncMultiPutObject 异步分块上传（多线程并发分片）
TEST_F(KeepAliveOpTest, KA_ASYNC_003_AsyncMultiPutObject) {
  // 分片大小已在 SetUpTestCase 设为 1MB, 生成 2.5MB 文件触发 3 个分片
  const std::string local_file = "./ka_async_003_src";
  const std::string file_download = "./ka_async_003_dst";
  const uint64_t file_size = 2 * 1024 * 1024 + 512 * 1024;
  TestUtils::WriteRandomDatatoFile(local_file, file_size);
  const std::string file_md5_origin = TestUtils::CalcFileMd5(local_file);

  KeepAlivePoolSnapshot snap = GetPoolSnapshot();

  for (int i = 0; i < 2; ++i) {
    const std::string object_name = "ka_async_003_" + std::to_string(i);
    AsyncMultiPutObjectReq put_req(m_bucket_name, object_name, local_file);
    put_req.SetRecvTimeoutInms(1000 * 60);
    SharedAsyncContext context = m_client->AsyncMultiPutObject(put_req);
    context->WaitUntilFinish();
    if (!ExpectSucc(context->GetResult(),
                    "AsyncMultiPutObject#" + std::to_string(i))) {
      return;
    }

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

  if (!ExpectKeepAliveHit(snap, "KA_ASYNC_003")) return;
}

// KA-ASYNC-004: AsyncMultiGetObject 异步多线程下载
TEST_F(KeepAliveOpTest, KA_ASYNC_004_AsyncMultiGetObject) {
  const std::string object_name = "ka_async_004";
  const std::string local_file = "./ka_async_004_src";
  const std::string file_download = "./ka_async_004_dst";
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
      AsyncMultiGetObjectReq get_req(m_bucket_name, object_name,
                                     file_download);
      SharedAsyncContext context = m_client->AsyncMultiGetObject(get_req);
      context->WaitUntilFinish();
      return context->GetResult();
    });
    if (!ExpectSucc(get_result, "AsyncMultiGetObject#" + std::to_string(i))) {
      return;
    }
    EXPECT_EQ(file_md5_origin, TestUtils::CalcFileMd5(file_download));
    TestUtils::RemoveFile(file_download);
  }

  TestUtils::RemoveFile(local_file);

  if (!ExpectKeepAliveHit(snap, "KA_ASYNC_004")) return;
}

// KA-ASYNC-005: AsyncResumableGetObject 异步断点（分片）下载
TEST_F(KeepAliveOpTest, KA_ASYNC_005_AsyncResumableGetObject) {
  const std::string object_name = "ka_async_005";
  const std::string local_file = "./ka_async_005_src";
  const std::string file_download = "./ka_async_005_dst";
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
      AsyncGetObjectReq get_req(m_bucket_name, object_name, file_download);
      SharedAsyncContext context = m_client->AsyncResumableGetObject(get_req);
      context->WaitUntilFinish();
      return context->GetResult();
    });
    if (!ExpectSucc(get_result,
                    "AsyncResumableGetObject#" + std::to_string(i))) {
      return;
    }
    EXPECT_EQ(file_md5_origin, TestUtils::CalcFileMd5(file_download));
    TestUtils::RemoveFile(file_download);
  }

  TestUtils::RemoveFile(local_file);

  if (!ExpectKeepAliveHit(snap, "KA_ASYNC_005")) return;
}

}  // namespace qcloud_cos
