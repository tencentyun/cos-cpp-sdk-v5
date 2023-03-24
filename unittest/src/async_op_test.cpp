#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "cos_api.h"
#include "gtest/gtest.h"
#include "util/test_utils.h"
#include "util/file_util.h"
#include "util/simple_dns_cache.h"

/*
export CPP_SDK_V5_ACCESS_KEY=xxx
export CPP_SDK_V5_SECRET_KEY=xxx
export CPP_SDK_V5_REGION=ap-guangzhou
export CPP_SDK_V5_UIN=xxx
export CPP_SDK_V5_APPID=xxx
export COS_CPP_V5_TAG=""
export COS_CPP_V5_USE_DNS_CACHE="true"
*/

namespace qcloud_cos {

class AsyncOpTest : public testing::Test {
 protected:
  static void SetUpTestCase() {
    std::cout << "================SetUpTestCase Begin===================="
              << std::endl;
    m_config = new CosConfig("./config.json");
    m_config->SetAccessKey(GetEnvVar("CPP_SDK_V5_ACCESS_KEY"));
    m_config->SetSecretKey(GetEnvVar("CPP_SDK_V5_SECRET_KEY"));
    m_config->SetRegion(GetEnvVar("CPP_SDK_V5_REGION"));
    if (GetEnvVar("COS_CPP_V5_USE_DNS_CACHE") == "true") {
      std::cout << "================USE DNS CACHE===================="
                << std::endl;
      CosSysConfig::SetUseDnsCache(true);
    }
    m_client = new CosAPI(*m_config);

    m_bucket_name = "coscppsdkv5ut" + GetEnvVar("COS_CPP_V5_TAG") + "-" +
                    GetEnvVar("CPP_SDK_V5_APPID");
    {
      PutBucketReq req(m_bucket_name);
      PutBucketResp resp;
      CosResult result = m_client->PutBucket(req, &resp);
      ASSERT_TRUE(result.IsSucc());
    }

    std::cout << "================SetUpTestCase End===================="
              << std::endl;
  }

  static void TearDownTestCase() {
    std::cout << "================TearDownTestCase Begin===================="
              << std::endl;

    // 1. 删除所有Object
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
          std::cout << "DeleteObject Failed, check object=" << content.m_key
                    << std::endl;
        }
      }
    }

    {
      // 删除所有碎片
      std::vector<std::string> bucket_v = {m_bucket_name};
      for (auto& bucket : bucket_v) {
        qcloud_cos::ListMultipartUploadReq list_mp_req(bucket);
        qcloud_cos::ListMultipartUploadResp list_mp_resp;
        qcloud_cos::CosResult list_mp_result =
            m_client->ListMultipartUpload(list_mp_req, &list_mp_resp);
        ASSERT_TRUE(list_mp_result.IsSucc());
        std::vector<Upload> rst = list_mp_resp.GetUpload();
        for (std::vector<qcloud_cos::Upload>::const_iterator itr = rst.begin();
             itr != rst.end(); ++itr) {
          AbortMultiUploadReq abort_mp_req(bucket, itr->m_key, itr->m_uploadid);
          AbortMultiUploadResp abort_mp_resp;
          CosResult abort_mp_result =
              m_client->AbortMultiUpload(abort_mp_req, &abort_mp_resp);
          EXPECT_TRUE(abort_mp_result.IsSucc());
          if (!abort_mp_result.IsSucc()) {
            std::cout << "AbortMultiUpload Failed, object=" << itr->m_key
                      << ", upload_id=" << itr->m_uploadid << std::endl;
          }
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
    }

    delete m_client;
    delete m_config;
    std::cout << "================TearDownTestCase End===================="
              << std::endl;
  }  // namespace qcloud_cos

 protected:
  static CosConfig* m_config;
  static CosAPI* m_client;
  static std::string m_bucket_name;
};

std::string AsyncOpTest::m_bucket_name = "";
CosConfig* AsyncOpTest::m_config = NULL;
CosAPI* AsyncOpTest::m_client = NULL;

enum OpType { ASYNC_OP = 1, MULTI_ASYNC_OP };

#define GENERATE_LOCAL_FILE(size)                                       \
  std::string object_name = "test_async_" + std::to_string(size);       \
  std::string local_file = "./" + object_name;                          \
  std::cout << "generate local file: " << local_file << std::endl;      \
  TestUtils::WriteRandomDatatoFile(local_file, file_size);              \
  uint64_t file_crc64_origin = FileUtil::GetFileCrc64(local_file);      \
  std::string file_md5_origin = FileUtil::GetFileMd5(local_file);       \
  std::cout << "file_crc64_origin: " << file_crc64_origin << std::endl; \
  std::cout << "file_md5_origin: " << file_md5_origin << std::endl;

#define REQ_SET_META(req)                                  \
  put_req.SetXCosMeta("test-meta-key1", "test-meta-val1"); \
  put_req.SetXCosMeta("test-meta-key2", "test-meta-val2"); \
  put_req.SetXCosStorageClass(kStorageClassStandardIA);

#define CHECK_COMMON_RESULT(result)                \
  ASSERT_TRUE(result.IsSucc());                    \
  ASSERT_TRUE(!result.GetXCosRequestId().empty()); \
  ASSERT_TRUE(result.GetHttpStatus() >= 200 && result.GetHttpStatus() < 300);

#define CHECK_COMMON_RESP(resp)               \
  ASSERT_EQ(resp.GetServer(), "tencent-cos"); \
  ASSERT_TRUE(!resp.GetDate().empty());       \
  ASSERT_TRUE(!resp.GetConnection().empty()); \
  ASSERT_TRUE(!resp.GetXCosRequestId().empty());

#define CHECK_PUT_RESP(resp, op_type)                                        \
  ASSERT_EQ(resp.GetXCosHashCrc64Ecma(), std::to_string(file_crc64_origin)); \
  if (op_type == MULTI_ASYNC_OP) {                                           \
    ASSERT_TRUE(!resp.GetContentType().empty());                             \
    ASSERT_NE(resp.GetEtag(), file_md5_origin);                              \
  } else {                                                                   \
    ASSERT_EQ(resp.GetEtag(), file_md5_origin);                              \
  }

#define CHECK_HEAD_RESP(resp, op_type)                                       \
  ASSERT_EQ(resp.GetXCosHashCrc64Ecma(), std::to_string(file_crc64_origin)); \
  ASSERT_EQ(resp.GetContentLength(), file_size);                             \
  ASSERT_TRUE(!resp.GetLastModified().empty());                              \
  if (op_type == MULTI_ASYNC_OP) {                                           \
    ASSERT_NE(resp.GetEtag(), file_md5_origin);                              \
  } else {                                                                   \
    ASSERT_EQ(resp.GetEtag(), file_md5_origin);                              \
  }

#define CHECK_HEAD_META(resp)                                           \
  ASSERT_EQ(head_resp.GetXCosMeta("test-meta-key1"), "test-meta-val1"); \
  ASSERT_EQ(head_resp.GetXCosMeta("test-meta-key2"), "test-meta-val2"); \
  ASSERT_EQ(head_resp.GetXCosStorageClass(), kStorageClassStandardIA);

#define CHECK_GET_RESP(resp, op_type)                                          \
  ASSERT_EQ(resp.GetXCosHashCrc64Ecma(), std::to_string(file_crc64_origin));   \
  if (op_type == ASYNC_OP) {                                                   \
    ASSERT_EQ(resp.GetContentLength(), file_size);                             \
  }                                                                            \
  ASSERT_TRUE(!resp.GetLastModified().empty());                                \
  if (op_type == MULTI_ASYNC_OP) {                                             \
    ASSERT_NE(resp.GetEtag(), file_md5_origin);                                \
    ASSERT_TRUE(!resp.GetContentRange().empty());                              \
  } else {                                                                     \
    ASSERT_EQ(resp.GetEtag(), file_md5_origin);                                \
  }                                                                            \
  std::string file_md5_download = TestUtils::CalcFileMd5(local_file_download); \
  ASSERT_EQ(file_md5_download, file_md5_origin);

#define CHECK_DEL_RESP(resp) ASSERT_EQ(resp.GetContentLength(), 0);


TEST_F(AsyncOpTest, AsyncOpWithoutCallback) {
  std::vector<int> base_file_size_v = {
      1, 5, 35, 356, 1111, 2545, 25678, 1024 * 1024, 5 * 1024 * 1024};
  std::vector<int> op_type_v = {ASYNC_OP, MULTI_ASYNC_OP};
  for (auto& size : base_file_size_v) {
    for (auto& op_type : op_type_v) {
      std::cout << "base_size: " << size << ", op_type: " << op_type
                << std::endl;
      size_t file_size = ((rand() % 100) + 1) * size;
      GENERATE_LOCAL_FILE(file_size)

      // 开始上传
      SharedAsyncContext context;
      if (op_type == ASYNC_OP) {
        std::cout << "async upload object: " << object_name << std::endl;
        qcloud_cos::AsyncPutObjectReq put_req(m_bucket_name, object_name,
                                              local_file);
        REQ_SET_META(put_req)
        context = m_client->AsyncPutObject(put_req);
      } else {
        std::cout << "multi async upload object: " << object_name << std::endl;
        qcloud_cos::AsyncMultiPutObjectReq put_req(m_bucket_name, object_name,
                                                   local_file);
        REQ_SET_META(put_req)
        context = m_client->AsyncMultiPutObject(put_req);
      }

      // 等待上传结束
      context->WaitUntilFinish();
      // 检查结果及响应
      CHECK_COMMON_RESULT(context->GetResult())
      AsyncResp put_resp = context->GetAsyncResp();
      CHECK_COMMON_RESP(put_resp)
      CHECK_PUT_RESP(put_resp, op_type)
      ASSERT_EQ(StringUtil::StringToUpper(put_resp.GetXCosStorageClass()),
                kStorageClassStandardIA);

      // head检查meta
      HeadObjectReq head_req(m_bucket_name, object_name);
      HeadObjectResp head_resp;
      CosResult result = m_client->HeadObject(head_req, &head_resp);
      CHECK_COMMON_RESULT(result)
      CHECK_COMMON_RESP(head_resp)
      CHECK_HEAD_RESP(head_resp, op_type)
      CHECK_HEAD_META(head_resp)

      // 下载对象
      std::string local_file_download = local_file + "_download";
      if (op_type == ASYNC_OP) {
        std::cout << "async download object: " << object_name << std::endl;
        qcloud_cos::AsyncGetObjectReq get_req(m_bucket_name, object_name,
                                              local_file_download);
        context = m_client->AsyncGetObject(get_req);
      } else {
        std::cout << "multi async download object: " << object_name
                  << std::endl;
        qcloud_cos::AsyncMultiGetObjectReq get_req(m_bucket_name, object_name,
                                                   local_file_download);
        context = m_client->AsyncMultiGetObject(get_req);
      }

      // 等待下载结束
      context->WaitUntilFinish();
      CHECK_COMMON_RESULT(result)
      AsyncResp get_resp = context->GetAsyncResp();
      CHECK_COMMON_RESP(get_resp)
      CHECK_GET_RESP(get_resp, op_type)
      ASSERT_EQ(get_resp.GetXCosStorageClass(), "STANDARD_IA");

      // 删除对象
      CosResult del_result;
      qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
      qcloud_cos::DeleteObjectResp del_resp;
      del_result = m_client->DeleteObject(del_req, &del_resp);
      CHECK_COMMON_RESULT(result)
      CHECK_COMMON_RESP(del_resp)
      CHECK_DEL_RESP(del_resp)

      // 删除本地文件
      TestUtils::RemoveFile(local_file);
      TestUtils::RemoveFile(local_file_download);
    }
  }
}

TEST_F(AsyncOpTest, AsyncOpWithProgressCallback) {
  std::vector<int> base_file_size_v = {
      1, 5, 35, 356, 1024, 2545, 25678, 1024 * 1024, 5 * 1024 * 1024};
  std::vector<int> op_type_v = {ASYNC_OP, MULTI_ASYNC_OP};
  for (auto& size : base_file_size_v) {
    for (auto& op_type : op_type_v) {
      std::cout << "base_size: " << size << ", op_type: " << op_type
                << std::endl;
      size_t file_size = ((rand() % 100) + 1) * size;
      GENERATE_LOCAL_FILE(file_size)

      FileInfo file_info = {
          object_name, local_file,        local_file + "_download",
          file_size,   file_crc64_origin, file_md5_origin,
          op_type};
      bool done = false;
      // 进度回调函数
      auto process_cb = [&done](uint64_t transferred_size, uint64_t total_size,
                                void* user_data) {
        FileInfo* file_info = reinterpret_cast<FileInfo*>(user_data);
        ASSERT_TRUE(transferred_size <= total_size);
        ASSERT_EQ(total_size, file_info->m_file_size);
        // 每1M输出一次
        if (0 == transferred_size % (1024 * 1024)) {
        std::cout << "transferred_size: " << transferred_size
                  << ", total_size: " << total_size << std::endl;
        }
        if (transferred_size == total_size) {
          done = true;
        }
      };

      // 开始上传
      SharedAsyncContext context;
      if (op_type == ASYNC_OP) {
        std::cout << "async upload object: " << object_name << std::endl;
        // 异步上传
        qcloud_cos::AsyncPutObjectReq put_req(m_bucket_name, object_name,
                                              local_file);
        // 设置上传进度回调
        put_req.SetTransferProgressCallback(process_cb);
        // 设置私有数据
        put_req.SetUserData(reinterpret_cast<void*>(&file_info));
        context = m_client->AsyncPutObject(put_req);
      } else {
        std::cout << "multi async upload object: " << object_name << std::endl;
        // 异步上传
        qcloud_cos::AsyncMultiPutObjectReq put_req(m_bucket_name, object_name,
                                                   local_file);
        // 设置上传进度回调
        put_req.SetTransferProgressCallback(process_cb);
        // 设置私有数据
        put_req.SetUserData(reinterpret_cast<void*>(&file_info));
        context = m_client->AsyncMultiPutObject(put_req);
      }

      // 等待上传结束
      context->WaitUntilFinish();
      // 校验响应
      ASSERT_TRUE(done);
      CHECK_COMMON_RESULT(context->GetResult())
      AsyncResp put_resp = context->GetAsyncResp();
      CHECK_COMMON_RESP(put_resp)
      CHECK_PUT_RESP(put_resp, op_type)

      // 下载文件
      done = false;
      std::string local_file_download = local_file + "_download";

      if (op_type == ASYNC_OP) {
        std::cout << "async download object: " << object_name << std::endl;
        qcloud_cos::AsyncGetObjectReq get_req(m_bucket_name, object_name,
                                              local_file_download);
        // 设置进度回调
        get_req.SetTransferProgressCallback(process_cb);
        // 设置私有数据
        get_req.SetUserData(reinterpret_cast<void*>(&file_info));
        context = m_client->AsyncGetObject(get_req);
      } else {
        std::cout << "multi async download object: " << object_name
                  << std::endl;
        qcloud_cos::AsyncMultiGetObjectReq get_req(m_bucket_name, object_name,
                                                   local_file_download);
        // 设置进度回调
        get_req.SetTransferProgressCallback(process_cb);
        // 设置私有数据
        get_req.SetUserData(reinterpret_cast<void*>(&file_info));
        context = m_client->AsyncMultiGetObject(get_req);
      }

      // 等待下载结束
      context->WaitUntilFinish();
      // 校验响应
      ASSERT_TRUE(done);
      CHECK_COMMON_RESULT(context->GetResult())
      AsyncResp get_resp = context->GetAsyncResp();
      CHECK_COMMON_RESP(get_resp)
      CHECK_GET_RESP(get_resp, op_type)

      // 删除对象
      CosResult del_result;
      qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
      qcloud_cos::DeleteObjectResp del_resp;
      del_result = m_client->DeleteObject(del_req, &del_resp);
      CHECK_COMMON_RESULT(del_result)
      CHECK_COMMON_RESP(del_resp)
      CHECK_DEL_RESP(del_resp)

      // 删除本地文件
      TestUtils::RemoveFile(local_file);
      TestUtils::RemoveFile(local_file_download);
    }
  }
}

TEST_F(AsyncOpTest, AsyncOpWithWithDoneCallback) {
  std::vector<int> base_file_size_v = {
      1, 5, 35, 356, 1024, 2545, 25678, 1024 * 1024, 5 * 1024 * 1024};
  std::vector<int> op_type_v = {ASYNC_OP, MULTI_ASYNC_OP};
  for (auto& size : base_file_size_v) {
    for (auto& op_type : op_type_v) {
      std::cout << "base_size: " << size << ", op_type: " << op_type
                << std::endl;
      size_t file_size = ((rand() % 100) + 1) * size;
      GENERATE_LOCAL_FILE(file_size)

      FileInfo file_info = {
          object_name, local_file,        local_file + "_download",
          file_size,   file_crc64_origin, file_md5_origin,
          op_type};

      // 完成回调
      auto multi_put_done_cb = [](const SharedAsyncContext& context,
                                  void* user_data) {
        CHECK_COMMON_RESULT(context->GetResult())
        FileInfo* file_info = reinterpret_cast<FileInfo*>(user_data);
        std::string file_md5_origin = file_info->m_file_md5_origin;
        uint64_t file_crc64_origin = file_info->m_file_crc64_origin;
        AsyncResp resp = context->GetAsyncResp();
        CHECK_COMMON_RESP(resp)
        CHECK_PUT_RESP(resp, file_info->m_op_type)
      };

      // 开始上传
      SharedAsyncContext context;
      if (op_type == ASYNC_OP) {
        std::cout << "async upload object: " << object_name << std::endl;
        // 异步上传
        qcloud_cos::AsyncPutObjectReq put_req(m_bucket_name, object_name,
                                              local_file);
        // 设置上传完成回调
        put_req.SetDoneCallback(multi_put_done_cb);
        // 设置私有数据
        put_req.SetUserData(reinterpret_cast<void*>(&file_info));

        context = m_client->AsyncPutObject(put_req);
      } else {
        std::cout << "multi async upload object: " << object_name << std::endl;
        // 异步上传
        qcloud_cos::AsyncMultiPutObjectReq put_req(m_bucket_name, object_name,
                                                   local_file);
        // 设置上传完成回调
        put_req.SetDoneCallback(multi_put_done_cb);
        // 设置私有数据
        put_req.SetUserData(reinterpret_cast<void*>(&file_info));
        context = m_client->AsyncMultiPutObject(put_req);
      }

      // 等待上传结束
      context->WaitUntilFinish();
      CHECK_COMMON_RESULT(context->GetResult())
      AsyncResp put_resp = context->GetAsyncResp();
      CHECK_COMMON_RESP(put_resp)
      CHECK_PUT_RESP(put_resp, op_type)

      // 下载
      auto multi_get_done_cb = [](const SharedAsyncContext& context,
                                  void* user_data) {
        CHECK_COMMON_RESULT(context->GetResult())
        FileInfo* file_info = (reinterpret_cast<FileInfo*>(user_data));
        std::string file_md5_origin = file_info->m_file_md5_origin;
        std::string local_file_download = file_info->m_local_file_download;
        uint64_t file_crc64_origin = file_info->m_file_crc64_origin;
        uint64_t file_size = file_info->m_file_size;
        AsyncResp resp = context->GetAsyncResp();
        CHECK_COMMON_RESP(resp)
        CHECK_GET_RESP(resp, file_info->m_op_type)
      };

      std::string local_file_download = local_file + "_download";
      if (op_type == ASYNC_OP) {
        std::cout << "async download object: " << object_name << std::endl;
        qcloud_cos::AsyncGetObjectReq get_req(m_bucket_name, object_name,
                                              local_file_download);
        // 设置完成回调
        get_req.SetDoneCallback(multi_get_done_cb);
        // 设置私有数据
        get_req.SetUserData(reinterpret_cast<void*>(&file_info));
        context = m_client->AsyncGetObject(get_req);
      } else {
        std::cout << "multi async download object: " << object_name
                  << std::endl;
        qcloud_cos::AsyncMultiGetObjectReq get_req(m_bucket_name, object_name,
                                                   local_file_download);
        // 设置完成回调
        get_req.SetDoneCallback(multi_get_done_cb);
        // 设置私有数据
        get_req.SetUserData(reinterpret_cast<void*>(&file_info));
        context = m_client->AsyncMultiGetObject(get_req);
      }

      // 等待下载结束
      context->WaitUntilFinish();
      CHECK_COMMON_RESULT(context->GetResult())
      AsyncResp get_resp = context->GetAsyncResp();
      CHECK_COMMON_RESP(get_resp)
      CHECK_GET_RESP(get_resp, op_type)

      // 删除对象
      CosResult del_result;
      qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
      qcloud_cos::DeleteObjectResp del_resp;
      del_result = m_client->DeleteObject(del_req, &del_resp);
      CHECK_COMMON_RESULT(del_result)
      CHECK_COMMON_RESP(del_resp)
      CHECK_DEL_RESP(del_resp)

      // 删除本地文件
      TestUtils::RemoveFile(local_file);
      TestUtils::RemoveFile(local_file_download);
    }
  }
}

TEST_F(AsyncOpTest, AsyncPutByStreamWithDoneCallback) {
  {
    size_t file_size = ((rand() % 100) + 1) * 1024;
    std::string object_name = "test_async_by_stream_" + std::to_string(file_size); 
    std::istringstream iss(TestUtils::GetRandomString(file_size));
    // 完成回调
    auto multi_put_done_cb = [](const SharedAsyncContext& context,
                                void* user_data) {
      CHECK_COMMON_RESULT(context->GetResult())
      AsyncResp resp = context->GetAsyncResp();
      CHECK_COMMON_RESP(resp)
    };

    // 开始上传
    SharedAsyncContext context;
    std::cout << "async upload object: " << object_name << std::endl;

    // 异步上传
    qcloud_cos::AsyncPutObjectByStreamReq put_req(m_bucket_name, object_name,
                                          iss);
    // 设置上传完成回调
    put_req.SetDoneCallback(multi_put_done_cb);
    // 设置私有数据
    put_req.SetUserData(&put_req);
    context = m_client->AsyncPutObject(put_req);
    // 等待上传结束
    context->WaitUntilFinish();
    CHECK_COMMON_RESULT(context->GetResult())
    AsyncResp put_resp = context->GetAsyncResp();
    CHECK_COMMON_RESP(put_resp)

    // 删除对象
    CosResult del_result;
    qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
    qcloud_cos::DeleteObjectResp del_resp;
    del_result = m_client->DeleteObject(del_req, &del_resp);
    CHECK_COMMON_RESULT(del_result)
    CHECK_COMMON_RESP(del_resp)
    CHECK_DEL_RESP(del_resp)
  }
}

TEST_F(AsyncOpTest, AsyncPutByStreamWithDoneCallbackWithOutputTaskManager) {
  {
    size_t file_size = ((rand() % 100) + 1) * 1024;
    std::string object_name = "test_async_by_stream_" + std::to_string(file_size); 
    std::istringstream iss(TestUtils::GetRandomString(file_size));
    // 完成回调
    auto multi_put_done_cb = [](const SharedAsyncContext& context,
                                void* user_data) {
      CHECK_COMMON_RESULT(context->GetResult())
      AsyncResp resp = context->GetAsyncResp();
      CHECK_COMMON_RESP(resp)
    };

    // 开始上传
    SharedAsyncContext context;
    Poco::TaskManager* taskManager;
    std::cout << "async upload object: " << object_name << std::endl;

    // 异步上传
    qcloud_cos::AsyncPutObjectByStreamReq put_req(m_bucket_name, object_name,
                                          iss);
    // 设置上传完成回调
    put_req.SetDoneCallback(multi_put_done_cb);
    // 设置私有数据
    put_req.SetUserData(&put_req);
    context = m_client->AsyncPutObject(put_req, taskManager);
    // 等待上传结束
    context->WaitUntilFinish();
    taskManager->joinAll();
    CHECK_COMMON_RESULT(context->GetResult())
    AsyncResp put_resp = context->GetAsyncResp();
    CHECK_COMMON_RESP(put_resp)

    // 删除对象
    CosResult del_result;
    qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
    qcloud_cos::DeleteObjectResp del_resp;
    del_result = m_client->DeleteObject(del_req, &del_resp);
    CHECK_COMMON_RESULT(del_result)
    CHECK_COMMON_RESP(del_resp)
    CHECK_DEL_RESP(del_resp)
  }
}



TEST_F(AsyncOpTest, AsyncOpWithWithDoneCallbackWithOutputTaskManager) {
  std::vector<int> base_file_size_v = {
      1, 5, 35, 356, 1024, 2545, 25678, 1024 * 1024, 5 * 1024 * 1024};
  std::vector<int> op_type_v = {ASYNC_OP, MULTI_ASYNC_OP};
  for (auto& size : base_file_size_v) {
    for (auto& op_type : op_type_v) {
      std::cout << "base_size: " << size << ", op_type: " << op_type
                << std::endl;
      size_t file_size = ((rand() % 100) + 1) * size;
      GENERATE_LOCAL_FILE(file_size)

      FileInfo file_info = {
          object_name, local_file,        local_file + "_download",
          file_size,   file_crc64_origin, file_md5_origin,
          op_type};

      // 完成回调
      auto multi_put_done_cb = [](const SharedAsyncContext& context,
                                  void* user_data) {
        CHECK_COMMON_RESULT(context->GetResult())
        FileInfo* file_info = reinterpret_cast<FileInfo*>(user_data);
        std::string file_md5_origin = file_info->m_file_md5_origin;
        uint64_t file_crc64_origin = file_info->m_file_crc64_origin;
        AsyncResp resp = context->GetAsyncResp();
        CHECK_COMMON_RESP(resp)
        CHECK_PUT_RESP(resp, file_info->m_op_type)
      };

      // 开始上传
      SharedAsyncContext context;
      Poco::TaskManager* taskManager;
      if (op_type == ASYNC_OP) {
        std::cout << "async upload object: " << object_name << std::endl;
        // 异步上传
        qcloud_cos::AsyncPutObjectReq put_req(m_bucket_name, object_name,
                                              local_file);
        // 设置上传完成回调
        put_req.SetDoneCallback(multi_put_done_cb);
        // 设置私有数据
        put_req.SetUserData(reinterpret_cast<void*>(&file_info));

        context = m_client->AsyncPutObject(put_req, taskManager);
      } else {
        std::cout << "multi async upload object: " << object_name << std::endl;
        // 异步上传
        qcloud_cos::AsyncMultiPutObjectReq put_req(m_bucket_name, object_name,
                                                   local_file);
        // 设置上传完成回调
        put_req.SetDoneCallback(multi_put_done_cb);
        // 设置私有数据
        put_req.SetUserData(reinterpret_cast<void*>(&file_info));
        context = m_client->AsyncMultiPutObject(put_req, taskManager);
      }

      // 等待上传结束
      context->WaitUntilFinish();
      taskManager->joinAll();
      CHECK_COMMON_RESULT(context->GetResult())
      AsyncResp put_resp = context->GetAsyncResp();
      CHECK_COMMON_RESP(put_resp)
      CHECK_PUT_RESP(put_resp, op_type)

      // 下载
      auto multi_get_done_cb = [](const SharedAsyncContext& context,
                                  void* user_data) {
        CHECK_COMMON_RESULT(context->GetResult())
        FileInfo* file_info = (reinterpret_cast<FileInfo*>(user_data));
        std::string file_md5_origin = file_info->m_file_md5_origin;
        std::string local_file_download = file_info->m_local_file_download;
        uint64_t file_crc64_origin = file_info->m_file_crc64_origin;
        uint64_t file_size = file_info->m_file_size;
        AsyncResp resp = context->GetAsyncResp();
        CHECK_COMMON_RESP(resp)
        CHECK_GET_RESP(resp, file_info->m_op_type)
      };
      std::string local_file_download = local_file + "_download";
      if (op_type == ASYNC_OP) {
        std::cout << "async download object: " << object_name << std::endl;
        qcloud_cos::AsyncGetObjectReq get_req(m_bucket_name, object_name,
                                              local_file_download);
        // 设置完成回调
        get_req.SetDoneCallback(multi_get_done_cb);
        // 设置私有数据
        get_req.SetUserData(reinterpret_cast<void*>(&file_info));
        context = m_client->AsyncGetObject(get_req, taskManager);
      } else {
        std::cout << "multi async download object: " << object_name
                  << std::endl;
        qcloud_cos::AsyncMultiGetObjectReq get_req(m_bucket_name, object_name,
                                                   local_file_download);
        // 设置完成回调
        get_req.SetDoneCallback(multi_get_done_cb);
        // 设置私有数据
        get_req.SetUserData(reinterpret_cast<void*>(&file_info));
        context = m_client->AsyncMultiGetObject(get_req, taskManager);
      }

      // 等待下载结束
      context->WaitUntilFinish();
      taskManager->joinAll();
      CHECK_COMMON_RESULT(context->GetResult())
      AsyncResp get_resp = context->GetAsyncResp();
      CHECK_COMMON_RESP(get_resp)
      CHECK_GET_RESP(get_resp, op_type)

      // 删除对象
      CosResult del_result;
      qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
      qcloud_cos::DeleteObjectResp del_resp;
      del_result = m_client->DeleteObject(del_req, &del_resp);
      CHECK_COMMON_RESULT(del_result)
      CHECK_COMMON_RESP(del_resp)
      CHECK_DEL_RESP(del_resp)

      // 删除本地文件
      TestUtils::RemoveFile(local_file);
      TestUtils::RemoveFile(local_file_download);
    }
  }
}

#if 0
TEST_F(AsyncOpTest, AsyncOpWithConcurrent) {
  const int concurrent_size = 5;  // 并发文件个数为5
  std::vector<int> base_file_size_v = {
      1, 5, 35, 356, 1024, 2545, 25678, 1024 * 1024, 5 * 1024 * 1024};
  std::vector<int> op_type_v = {ASYNC_OP, MULTI_ASYNC_OP};
  for (auto& size : base_file_size_v) {
    for (auto& op_type : op_type_v) {
      std::unordered_map<std::string, FileInfo> concurrent_objects;
      std::list<std::string> waiting_object_list;
      std::mutex mutex;
      std::condition_variable cond;

      // 生成多个本地文件
      for (int i = 0; i < concurrent_size; i++) {
        size_t file_size = ((rand() % 100) + 1) * size;
        GENERATE_LOCAL_FILE(file_size)
        FileInfo file_info = {
            object_name, local_file,        local_file + "_downlaod",
            file_size,   file_crc64_origin, file_md5_origin,
            op_type};
        concurrent_objects[object_name] = file_info;
        waiting_object_list.push_back(object_name);
      }

      // 上传回调函数
      DoneCallback multi_put_done_cb;
      multi_put_done_cb = [multi_put_done_cb, &mutex, &cond,
                           &waiting_object_list](
                              const SharedAsyncContext& context,
                              void* user_data) {
        EXPECT_TRUE(context->GetResult().IsSucc());
        std::unordered_map<std::string, FileInfo>* concurrent_objects =
            reinterpret_cast<std::unordered_map<std::string, FileInfo>*>(
                user_data);
        ASSERT_TRUE(concurrent_objects->count(context->GetObjectName()) == 1);
        FileInfo file_info = (*concurrent_objects)[context->GetObjectName()];
        if (!context->GetResult().IsSucc()) {
          // 上传失败，重试
          std::cout << "failed to uplaod " << context->GetObjectName()
                    << ", retry..." << std::endl;
          qcloud_cos::AsyncPutObjectReq put_req(context->GetBucketName(),
                                                context->GetObjectName(),
                                                context->GetLocalFilePath());
          put_req.SetDoneCallback(multi_put_done_cb);
          put_req.SetUserData(user_data);
          m_client->AsyncPutObject(put_req);
        } else {
          // 上传成功，检查响应
          std::cout << "succeed to uplaod " << context->GetObjectName()
                    << std::endl;
          CHECK_COMMON_RESULT(context->GetResult())
          std::string file_md5_origin = file_info.m_file_md5_origin;
          uint64_t file_crc64_origin = file_info.m_file_crc64_origin;
          AsyncResp resp = context->GetAsyncResp();
          CHECK_COMMON_RESP(resp)
          CHECK_PUT_RESP(resp, file_info.m_op_type)
          std::unique_lock<std::mutex> lock(mutex);
          waiting_object_list.remove(context->GetObjectName());
          cond.notify_one();
        }
      };

      // 多个文件并发上传
      for (auto& o : concurrent_objects) {
        if (op_type == ASYNC_OP) {
          std::cout << "async upload object: " << o.second.m_object_name
                    << std::endl;
          qcloud_cos::AsyncPutObjectReq put_req(
              m_bucket_name, o.second.m_object_name, o.second.m_local_file);
          // 设置上传完成回调
          put_req.SetDoneCallback(multi_put_done_cb);
          // 设置私有数据
          put_req.SetUserData(reinterpret_cast<void*>(&concurrent_objects));
          m_client->AsyncPutObject(put_req);
        } else {
          std::cout << "multi async upload object: " << o.second.m_object_name
                    << std::endl;
          qcloud_cos::AsyncMultiPutObjectReq put_req(
              m_bucket_name, o.second.m_object_name, o.second.m_local_file);
          // 设置上传完成回调
          put_req.SetDoneCallback(multi_put_done_cb);
          // 设置私有数据
          put_req.SetUserData(reinterpret_cast<void*>(&concurrent_objects));
          m_client->AsyncMultiPutObject(put_req);
        }
      }

      // TODO timeout
      // 等待文件上传完成
      while (!waiting_object_list.empty()) {
        std::string waiting_objects_str;
        for (auto& o : waiting_object_list) {
          waiting_objects_str.append(o).append(" ");
        }
        std::cout << "waiting for " << waiting_objects_str
                  << " to complete uploading" << std::endl;
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock);
      }

      std::cout << "succeed to upload all objects" << std::endl;

      // 生成下载文件列表
      for (auto& o : concurrent_objects) {
        waiting_object_list.push_back(o.first);
      }

      // 下载回调
      DoneCallback multi_get_done_cb;
      multi_get_done_cb = [&multi_get_done_cb, &mutex, &cond,
                           &waiting_object_list](
                              const SharedAsyncContext& context,
                              void* user_data) {
        EXPECT_TRUE(context->GetResult().IsSucc());
        std::unordered_map<std::string, FileInfo>* concurrent_objects =
            reinterpret_cast<std::unordered_map<std::string, FileInfo>*>(
                user_data);
        ASSERT_TRUE(concurrent_objects->count(context->GetObjectName()) == 1);
        FileInfo file_info = (*concurrent_objects)[context->GetObjectName()];

        if (!context->GetResult().IsSucc()) {
          std::cout << "failed to download " << context->GetObjectName()
                    << ", retry..." << std::endl;
          // 下载失败，重试
          qcloud_cos::AsyncGetObjectReq get_req(context->GetBucketName(),
                                                context->GetObjectName(),
                                                context->GetLocalFilePath());
          get_req.SetDoneCallback(multi_get_done_cb);
          get_req.SetUserData(user_data);
          m_client->AsyncGetObject(get_req);
        } else {
          // 下载成功
          std::cout << "succeed to download " << context->GetObjectName()
                    << std::endl;
          CHECK_COMMON_RESULT(context->GetResult())
          std::string file_md5_origin = file_info.m_file_md5_origin;
          std::string local_file_download = file_info.m_local_file_download;
          uint64_t file_crc64_origin = file_info.m_file_crc64_origin;
          uint64_t file_size = file_info.m_file_size;
          AsyncResp resp = context->GetAsyncResp();
          CHECK_COMMON_RESP(resp)
          CHECK_GET_RESP(resp, file_info.m_op_type)
          std::unique_lock<std::mutex> lock(mutex);
          waiting_object_list.remove(context->GetObjectName());
          cond.notify_one();
        }
      };

      // 多个文件并发下载
      for (auto& o : concurrent_objects) {
        if (op_type == ASYNC_OP) {
          std::cout << "async download object: " << o.second.m_object_name
                    << std::endl;
          qcloud_cos::AsyncGetObjectReq get_req(m_bucket_name,
                                                o.second.m_object_name,
                                                o.second.m_local_file_download);
          get_req.SetDoneCallback(multi_get_done_cb);
          get_req.SetUserData(reinterpret_cast<void*>(&concurrent_objects));
          m_client->AsyncGetObject(get_req);
        } else {
          std::cout << "multi async download object: " << o.second.m_object_name
                    << std::endl;
          qcloud_cos::AsyncMultiGetObjectReq get_req(
              m_bucket_name, o.second.m_object_name,
              o.second.m_local_file_download);
          get_req.SetDoneCallback(multi_get_done_cb);
          get_req.SetUserData(reinterpret_cast<void*>(&concurrent_objects));
          m_client->AsyncMultiGetObject(get_req);
        }
      }

      // 等待文件下载完成
      // TODO timeout
      while (!waiting_object_list.empty()) {
        std::string waiting_objects_str;
        for (auto& o : waiting_object_list) {
          waiting_objects_str.append(o).append(" ");
        }
        std::cout << "waiting for " << waiting_objects_str
                  << " to complete downloading" << std::endl;
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock);
      }

      std::cout << "succeed to download all objects" << std::endl;

      for (auto& o : concurrent_objects) {
        // 删除对象
        CosResult del_result;
        qcloud_cos::DeleteObjectReq del_req(m_bucket_name,
                                            o.second.m_object_name);
        qcloud_cos::DeleteObjectResp del_resp;
        del_result = m_client->DeleteObject(del_req, &del_resp);
        CHECK_COMMON_RESULT(del_result)
        CHECK_COMMON_RESP(del_resp)
        CHECK_DEL_RESP(del_resp)
        // 删除本地文件
        TestUtils::RemoveFile(o.second.m_local_file);
        TestUtils::RemoveFile(o.second.m_local_file_download);
      }
    }
  }
}
#endif


TEST_F(AsyncOpTest, AsyncPutWithException) {
  // cancel op
  std::vector<int> base_file_size_v = {1,    5,    35,    356,
                                       1024, 2545, 25678, 1024 * 1024};
  std::vector<int> op_type_v = {ASYNC_OP, MULTI_ASYNC_OP};
  for (auto& size : base_file_size_v) {
    for (auto& op_type : op_type_v) {
      size_t file_size = ((rand() % 100) + 1) * size;
      GENERATE_LOCAL_FILE(file_size)
      bool done = false;
      // 完成回调
      auto multi_put_done_cb = [&done](const SharedAsyncContext& context,
                                       void* user_data) {
        UNUSED_PARAM(user_data)
        ASSERT_TRUE(!context->GetResult().IsSucc());
        ASSERT_EQ(context->GetResult().GetErrorMsg(),
                  "Request canceled by user");
        done = true;
      };

      SharedAsyncContext context;
      if (op_type == ASYNC_OP) {
        std::cout << "async upload object: " << object_name << std::endl;
        qcloud_cos::AsyncPutObjectReq put_req(m_bucket_name, object_name,
                                              local_file);
        put_req.SetDoneCallback(multi_put_done_cb);
        context = m_client->AsyncPutObject(put_req);
      } else {
        std::cout << "multi async upload object: " << object_name << std::endl;
        qcloud_cos::AsyncMultiPutObjectReq put_req(m_bucket_name, object_name,
                                                   local_file);
        put_req.SetDoneCallback(multi_put_done_cb);
        context = m_client->AsyncMultiPutObject(put_req);
      }
      if (file_size >= 1024 * 1024) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
      }
      context->Cancel();
      context->WaitUntilFinish();
      ASSERT_TRUE(done);
      ASSERT_TRUE(!context->GetResult().IsSucc());
      ASSERT_EQ(context->GetResult().GetErrorMsg(), "Request canceled by user");

      TestUtils::RemoveFile(local_file);
    }
  }

  // put file not exists
  for (auto& op_type : op_type_v) {
    std::string object_name = "test";
    std::string local_file = "./local_file_not_exists";
    bool done = false;
    // 完成回调
    auto multi_put_done_cb = [&done](const SharedAsyncContext& context,
                                     void* user_data) {
      UNUSED_PARAM(user_data)
      ASSERT_TRUE(!context->GetResult().IsSucc());
      ASSERT_EQ(context->GetResult().GetErrorMsg(),
                "Failed to open file ./local_file_not_exists");
      done = true;
    };

    SharedAsyncContext context;
    if (op_type == ASYNC_OP) {
      std::cout << "async upload object: " << object_name << std::endl;
      qcloud_cos::AsyncPutObjectReq put_req(m_bucket_name, object_name,
                                            local_file);
      put_req.SetDoneCallback(multi_put_done_cb);
      context = m_client->AsyncPutObject(put_req);
    } else {
      std::cout << "multi async upload object: " << object_name << std::endl;
      qcloud_cos::AsyncMultiPutObjectReq put_req(m_bucket_name, object_name,
                                                 local_file);
      put_req.SetDoneCallback(multi_put_done_cb);
      context = m_client->AsyncMultiPutObject(put_req);
    }
    context->WaitUntilFinish();
    ASSERT_TRUE(done);
    ASSERT_TRUE(!context->GetResult().IsSucc());
    ASSERT_EQ(context->GetResult().GetErrorMsg(),
              "Failed to open file ./local_file_not_exists");
  }

  // no authorization to put file
  std::string ak = m_config->GetAccessKey();
  m_config->SetAccessKey("invalid ak");
  CosAPI* client_invalid = new CosAPI(*m_config);
  for (auto& op_type : op_type_v) {
    size_t file_size = 100;
    GENERATE_LOCAL_FILE(file_size)
    bool done = false;
    auto multi_put_done_cb = [&done](const SharedAsyncContext& context,
                                     void* user_data) {
      UNUSED_PARAM(user_data)
      ASSERT_TRUE(!context->GetResult().IsSucc());
      ASSERT_EQ(context->GetResult().GetErrorMsg(), "Access Denied.");
      done = true;
    };

    SharedAsyncContext context;
    if (op_type == ASYNC_OP) {
      qcloud_cos::AsyncPutObjectReq put_req(m_bucket_name, object_name,
                                            local_file);
      put_req.SetDoneCallback(multi_put_done_cb);
      context = client_invalid->AsyncPutObject(put_req);
    } else {
      qcloud_cos::AsyncMultiPutObjectReq put_req(m_bucket_name, object_name,
                                                 local_file);
      put_req.SetDoneCallback(multi_put_done_cb);
      context = client_invalid->AsyncMultiPutObject(put_req);
    }
    context->WaitUntilFinish();
    ASSERT_TRUE(done);
    ASSERT_TRUE(!context->GetResult().IsSucc());
    ASSERT_EQ(context->GetResult().GetErrorMsg(), "Access Denied.");

    TestUtils::RemoveFile(local_file);
  }
  m_config->SetAccessKey(ak);
  delete client_invalid;
}

TEST_F(AsyncOpTest, AsyncGetWithException) {
  // get file not exists
  std::vector<int> op_type_v = {ASYNC_OP, MULTI_ASYNC_OP};
  for (auto& op_type : op_type_v) {
    bool done = false;
    auto multi_get_done_cb = [&done](const SharedAsyncContext& context,
                                     void* user_data) {
      UNUSED_PARAM(user_data)
      ASSERT_TRUE(!context->GetResult().IsSucc());
      ASSERT_EQ(context->GetResult().GetHttpStatus(), 404);
      done = true;
    };

    std::string object_name = "async_get_file_not_exists";
    std::string local_file_download = object_name + "_download";
    SharedAsyncContext context;
    if (op_type == ASYNC_OP) {
      std::cout << "async get object: " << local_file_download << std::endl;
      qcloud_cos::AsyncGetObjectReq get_req(m_bucket_name, object_name,
                                            local_file_download);
      get_req.SetDoneCallback(multi_get_done_cb);
      context = m_client->AsyncGetObject(get_req);
    } else {
      std::cout << "multi async get object: " << object_name << std::endl;
      qcloud_cos::AsyncMultiGetObjectReq get_req(m_bucket_name, object_name,
                                                 local_file_download);
      get_req.SetDoneCallback(multi_get_done_cb);
      context = m_client->AsyncMultiGetObject(get_req);
    }

    context->WaitUntilFinish();
    ASSERT_TRUE(done);
    ASSERT_TRUE(!context->GetResult().IsSucc());
    ASSERT_EQ(context->GetResult().GetHttpStatus(), 404);

    TestUtils::RemoveFile(local_file_download);
  }

  // no authorization to get file
  std::string ak = m_config->GetAccessKey();
  m_config->SetAccessKey("invalid ak");
  CosAPI* client_invalid = new CosAPI(*m_config);
  for (auto& op_type : op_type_v) {
    std::string object_name = "test_no_authorization";
    std::string local_file_download = object_name + "_download";
    bool done = false;
    auto multi_get_done_cb = [&done](const SharedAsyncContext& context,
                                     void* user_data) {
      UNUSED_PARAM(user_data)
      ASSERT_TRUE(!context->GetResult().IsSucc());
      ASSERT_EQ(context->GetResult().GetErrorMsg(), "Access Denied.");
      done = true;
    };

    SharedAsyncContext context;
    if (op_type == ASYNC_OP) {
      qcloud_cos::AsyncGetObjectReq get_req(m_bucket_name, object_name,
                                            local_file_download);
      get_req.SetDoneCallback(multi_get_done_cb);
      context = client_invalid->AsyncGetObject(get_req);
    } else {
      qcloud_cos::AsyncMultiPutObjectReq get_req(m_bucket_name, object_name,
                                                 local_file_download);
      get_req.SetDoneCallback(multi_get_done_cb);
      context = client_invalid->AsyncMultiPutObject(get_req);
    }
    context->WaitUntilFinish();
    ASSERT_TRUE(done);
    ASSERT_TRUE(!context->GetResult().IsSucc());
    ASSERT_EQ(context->GetResult().GetErrorMsg(), "Access Denied.");

    TestUtils::RemoveFile(local_file_download);
  }
  m_config->SetAccessKey(ak);
  delete client_invalid;

  // get object, cancel op
  for (auto& op_type : op_type_v) {
    // 先上传文件
    size_t file_size = 1024 * 1024;
    GENERATE_LOCAL_FILE(file_size)
    SharedAsyncContext context;
    if (op_type == ASYNC_OP) {
      std::cout << "async upload object: " << object_name << std::endl;
      qcloud_cos::AsyncPutObjectReq put_req(m_bucket_name, object_name,
                                            local_file);
      context = m_client->AsyncPutObject(put_req);
    } else {
      std::cout << "multi async upload object: " << object_name << std::endl;
      qcloud_cos::AsyncMultiPutObjectReq put_req(m_bucket_name, object_name,
                                                 local_file);
      context = m_client->AsyncMultiPutObject(put_req);
    }
    context->WaitUntilFinish();
    CHECK_COMMON_RESULT(context->GetResult())
    AsyncResp put_resp = context->GetAsyncResp();
    CHECK_COMMON_RESP(put_resp)
    CHECK_PUT_RESP(put_resp, op_type)

    // 下载文件，执行cancel
    bool done = false;
    auto multi_get_done_cb = [&done](const SharedAsyncContext& context,
                                     void* user_data) {
      UNUSED_PARAM(user_data)
      ASSERT_TRUE(!context->GetResult().IsSucc());
      ASSERT_EQ(context->GetResult().GetErrorMsg(), "Request canceled by user");
      done = true;
    };

    std::string local_file_download = object_name + "_download";
    if (op_type == ASYNC_OP) {
      std::cout << "async get object: " << object_name << std::endl;
      qcloud_cos::AsyncGetObjectReq get_req(m_bucket_name, object_name,
                                            local_file_download);
      get_req.SetDoneCallback(multi_get_done_cb);
      context = m_client->AsyncGetObject(get_req);
    } else {
      std::cout << "multi async put object: " << object_name << std::endl;
      qcloud_cos::AsyncMultiGetObjectReq get_req(m_bucket_name, object_name,
                                                 local_file_download);
      get_req.SetDoneCallback(multi_get_done_cb);
      context = m_client->AsyncMultiGetObject(get_req);
    }
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    context->Cancel();
    context->WaitUntilFinish();
    ASSERT_TRUE(done);
    ASSERT_TRUE(!context->GetResult().IsSucc());
    ASSERT_EQ(context->GetResult().GetErrorMsg(), "Request canceled by user");

    TestUtils::RemoveFile(local_file);
    TestUtils::RemoveFile(local_file_download);
  }
}

#if 0
TEST_F(AsyncOpTest, AsyncConcurrentWithException) {
  const int concurrent_size = 5;  // 并发数为5
  std::vector<int> op_type_v = {ASYNC_OP, MULTI_ASYNC_OP};
  for (auto& op_type : op_type_v) {
    std::list<std::string> waiting_object_list;
    std::mutex mutex;
    std::condition_variable cond;

    // put local file not exists
    DoneCallback multi_put_done_cb = [&mutex, &cond, &waiting_object_list](
                            const SharedAsyncContext& context,
                            void* user_data) {
      UNUSED_PARAM(user_data)
      std::cout << "done " << context->GetObjectName() << std::endl;
      EXPECT_TRUE(!context->GetResult().IsSucc());
      ASSERT_EQ(context->GetResult().GetErrorMsg(),
                "Failed to open file ./local_file_not_exists");
      waiting_object_list.remove(context->GetObjectName());
      cond.notify_one();
    };

    std::string local_file = "./local_file_not_exists";
    for (int i = 0; i < concurrent_size; i++) {
      std::string object_name = "test-async-put-not-exists" + std::to_string(i);
      waiting_object_list.push_back(object_name);
    }

    for (auto & o: waiting_object_list) {
      if (op_type == ASYNC_OP) {
        std::cout << "async upload object: " << o << std::endl;
        qcloud_cos::AsyncPutObjectReq put_req(m_bucket_name, o,
                                              local_file);
        put_req.SetDoneCallback(multi_put_done_cb);
        m_client->AsyncPutObject(put_req);
      } else {
        std::cout << "multi async upload object: " << o << std::endl;
        qcloud_cos::AsyncMultiPutObjectReq put_req(m_bucket_name, o,
                                                   local_file);
        put_req.SetDoneCallback(multi_put_done_cb);
        m_client->AsyncMultiPutObject(put_req);
      }
    }

    while (!waiting_object_list.empty()) {
      std::string waiting_objects_str;
      for (auto& o : waiting_object_list) {
        waiting_objects_str.append(o).append(" ");
      }
      std::cout << "waiting for " << waiting_objects_str
                << " to complete uploading" << std::endl;
      auto now = std::chrono::system_clock::now();
      std::unique_lock<std::mutex> lock(mutex);
      cond.wait_until(lock, now +  std::chrono::seconds(1));
    }

    // get object not exists
    DoneCallback multi_get_done_cb = [&mutex, &cond, &waiting_object_list](
                                         const SharedAsyncContext& context,
                                         void* user_data) {
      UNUSED_PARAM(user_data)
      std::cout << "done " << context->GetObjectName() << std::endl;
      EXPECT_TRUE(!context->GetResult().IsSucc());
      ASSERT_EQ(context->GetResult().GetHttpStatus(), 404);
      waiting_object_list.remove(context->GetObjectName());
      cond.notify_one();
    };

    std::string local_file_download = "async_get_file_not_exists_download";
    for (int i = 0; i < concurrent_size; i++) {
      std::string object_name = "test-async-get-not-exists-" + std::to_string(i);
      waiting_object_list.push_back(object_name);
    }

    for (auto & o: waiting_object_list) {
      if (op_type == ASYNC_OP) {
        std::cout << "async get object: " << o << std::endl;
        qcloud_cos::AsyncGetObjectReq get_req(m_bucket_name, o,
                                              local_file_download);
        get_req.SetDoneCallback(multi_get_done_cb);
        m_client->AsyncGetObject(get_req);
      } else {
        std::cout << "multi async upload object: " << o << std::endl;
        qcloud_cos::AsyncMultiGetObjectReq get_req(m_bucket_name, o,
                                                   local_file_download);
        get_req.SetDoneCallback(multi_get_done_cb);
        m_client->AsyncMultiGetObject(get_req);
      }
    }

    while (!waiting_object_list.empty()) {
      std::string waiting_objects_str;
      for (auto& o : waiting_object_list) {
        waiting_objects_str.append(o).append(" ");
      }
      std::cout << "waiting for " << waiting_objects_str
                << " to complete downloading" << std::endl;
      std::unique_lock<std::mutex> lock(mutex);
      auto now = std::chrono::system_clock::now();
      cond.wait_until(lock, now +  std::chrono::seconds(1));
    }

    TestUtils::RemoveFile(local_file_download);
  }
}
#endif

}  // namespace qcloud_cos
