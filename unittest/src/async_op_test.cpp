#include "cos_api.h"
#include "test_utils.h"
#include "util/file_util.h"
#include "util/simple_dns_cache.h"
#include "gtest/gtest.h"
#include <condition_variable>
#include <mutex>
#include <unordered_map>
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

TEST_F(AsyncOpTest, AsyncOpWithoutCallback) {
  std::vector<int> base_size_v = {
      1, 5, 35, 356, 1111, 2545, 25678, 1024 * 1024, 5 * 1024 * 1024};
  for (auto& size : base_size_v) {
    for (int i = 1; i < 3; i++) {
      std::cout << "base_size: " << size << ", test_time: " << i << std::endl;

      size_t file_size = ((rand() % 100) + 1) * size;
      std::string object_name = "test_async_" + std::to_string(file_size);
      std::string local_file = "./" + object_name;

      std::cout << "generate file: " << local_file << std::endl;
      TestUtils::WriteRandomDatatoFile(local_file, file_size);

      uint64_t file_crc64_origin = FileUtil::GetFileCrc64(local_file);
      std::string file_md5_origin = FileUtil::GetFileMd5(local_file);

      std::cout << "file_crc64_origin: " << file_crc64_origin << std::endl;
      std::cout << "file_md5_origin: " << file_md5_origin << std::endl;

      // 异步上传
      qcloud_cos::MultiPutObjectReq put_req(m_bucket_name, object_name,
                                            local_file);
      std::cout << "async upload object: " << object_name
                << ", size: " << file_size << std::endl;
      // 开始上传
      SharedAsyncContext context = m_client->PutObjectAsync(put_req);
      // 等待上传结束
      context->WaitUntilFinish();
      ASSERT_TRUE(context->GetResult().IsSucc());
      ASSERT_TRUE(!context->GetResult().GetXCosRequestId().empty());
      ASSERT_EQ(context->GetMultiPutObjectResp().GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));
      ASSERT_EQ(context->GetMultiPutObjectResp().GetServer(), "tencent-cos");
      ASSERT_TRUE(!context->GetMultiPutObjectResp().GetDate().empty());
      ASSERT_TRUE(!context->GetMultiPutObjectResp().GetContentType().empty());

      // 下载
      std::string local_file_download = local_file + "_download";

      qcloud_cos::MultiGetObjectReq get_req(m_bucket_name, object_name,
                                            local_file_download);
      std::cout << "async download object: " << object_name << std::endl;

      // 开始下载
      context = m_client->GetObjectAsync(get_req);

      // 等待下载结束
      context->WaitUntilFinish();
      ASSERT_TRUE(context->GetResult().IsSucc());
      ASSERT_EQ(context->GetMultiGetObjectResp().GetXCosHashCrc64Ecma(),
                std::to_string(file_crc64_origin));
      ASSERT_EQ(context->GetMultiGetObjectResp().GetServer(), "tencent-cos");
      ASSERT_TRUE(context->GetMultiGetObjectResp().GetContentLength() > 0);
      ASSERT_TRUE(!context->GetMultiGetObjectResp().GetDate().empty());
      ASSERT_TRUE(!context->GetMultiGetObjectResp().GetLastModified().empty());
      ASSERT_TRUE(!context->GetMultiGetObjectResp().GetXCosRequestId().empty());
      ASSERT_TRUE(!context->GetMultiGetObjectResp().GetEtag().empty());
      ASSERT_TRUE(!context->GetMultiGetObjectResp().GetContentRange().empty());

      // 校验下载下来的文件
      std::string file_md5_download =
          TestUtils::CalcFileMd5(local_file_download);
      ASSERT_EQ(file_md5_download, file_md5_origin);

      // 删除对象
      CosResult del_result;
      qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
      qcloud_cos::DeleteObjectResp del_resp;
      del_result = m_client->DeleteObject(del_req, &del_resp);
      ASSERT_TRUE(del_result.IsSucc());
      ASSERT_TRUE(!del_result.GetXCosRequestId().empty());
      ASSERT_EQ(del_resp.GetServer(), "tencent-cos");
      ASSERT_TRUE(!del_resp.GetDate().empty());
      // 删除本地文件
      TestUtils::RemoveFile(local_file);
      TestUtils::RemoveFile(local_file_download);
    }
  }
}

TEST_F(AsyncOpTest, AsyncOpWithProgressCallback) {
  std::vector<int> base_size_v = {
      1, 5, 35, 356, 1024, 2545, 25678, 1024 * 1024, 5 * 1024 * 1024};
  for (auto& size : base_size_v) {
    for (int i = 1; i < 3; i++) {
      std::cout << "base_size: " << size << ", test_time: " << i << std::endl;

      size_t file_size = ((rand() % 100) + 1) * size;
      std::string object_name = "test_async_" + std::to_string(file_size);
      std::string local_file = "./" + object_name;

      std::cout << "generate file: " << local_file << std::endl;
      TestUtils::WriteRandomDatatoFile(local_file, file_size);

      uint64_t file_crc64_origin = FileUtil::GetFileCrc64(local_file);
      std::string file_md5_origin = FileUtil::GetFileMd5(local_file);

      std::cout << "file_crc64_origin: " << file_crc64_origin << std::endl;
      std::cout << "file_md5_origin: " << file_md5_origin << std::endl;

      FileInfo file_info = {
          object_name, local_file,        local_file + "_download",
          file_size,   file_crc64_origin, file_md5_origin};

      // 异步上传
      qcloud_cos::MultiPutObjectReq put_req(m_bucket_name, object_name,
                                            local_file);
      put_req.SetRecvTimeoutInms(1000 * 200);

      auto process_cb = [](uint64_t transferred_size, uint64_t total_size,
                           void* user_data) {
        FileInfo* file_info = reinterpret_cast<FileInfo*>(user_data);
        ASSERT_TRUE(transferred_size <= total_size);
        ASSERT_EQ(total_size, file_info->m_file_size);
      };

      std::cout << "async upload object: " << object_name
                << ", size: " << file_size << std::endl;
      // 设置上传进度回调
      put_req.SetTransferProgressCallback(process_cb);
      // 设置私有数据
      put_req.SetUserData(reinterpret_cast<void*>(&file_info));

      // 开始上传
      SharedAsyncContext context = m_client->PutObjectAsync(put_req);
      // 等待上传结束
      context->WaitUntilFinish();
      ASSERT_TRUE(context->GetResult().IsSucc());
      ASSERT_EQ(context->GetMultiPutObjectResp().GetXCosHashCrc64Ecma(),
                std::to_string(file_info.m_file_crc64_origin));

      std::string local_file_download = local_file + "_download";

      qcloud_cos::MultiGetObjectReq get_req(m_bucket_name, object_name,
                                            local_file_download);
      // 设置进度回调
      get_req.SetTransferProgressCallback(process_cb);
      // 设置私有数据
      get_req.SetUserData(reinterpret_cast<void*>(&file_info));
      std::cout << "async download object: " << object_name << std::endl;

      // 开始下载
      context = m_client->GetObjectAsync(get_req);

      // 等待下载结束
      context->WaitUntilFinish();
      ASSERT_TRUE(context->GetResult().IsSucc());

      // 校验下载下来的文件
      std::string file_md5_download =
          TestUtils::CalcFileMd5(local_file_download);
      ASSERT_EQ(file_md5_download, file_md5_origin);

      // 删除对象
      CosResult del_result;
      qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
      qcloud_cos::DeleteObjectResp del_resp;
      del_result = m_client->DeleteObject(del_req, &del_resp);
      ASSERT_TRUE(del_result.IsSucc());

      // 删除本地文件
      TestUtils::RemoveFile(local_file);
      TestUtils::RemoveFile(local_file_download);
    }
  }
}

TEST_F(AsyncOpTest, AsyncOpWithWithDoneCallback) {
  std::vector<int> base_size_v = {
      1, 5, 35, 356, 1024, 2545, 25678, 1024 * 1024, 5 * 1024 * 1024};
  for (auto& size : base_size_v) {
    for (int i = 1; i < 3; i++) {
      std::cout << "base_size: " << size << ", test_time: " << i << std::endl;

      size_t file_size = ((rand() % 100) + 1) * size;
      std::string object_name = "test_async_" + std::to_string(file_size);
      std::string local_file = "./" + object_name;

      std::cout << "generate file: " << local_file << std::endl;
      TestUtils::WriteRandomDatatoFile(local_file, file_size);

      uint64_t file_crc64_origin = FileUtil::GetFileCrc64(local_file);
      std::string file_md5_origin = FileUtil::GetFileMd5(local_file);

      std::cout << "file_crc64_origin: " << file_crc64_origin << std::endl;
      std::cout << "file_md5_origin: " << file_md5_origin << std::endl;

      FileInfo file_info = {
          object_name, local_file,        local_file + "_download",
          file_size,   file_crc64_origin, file_md5_origin};

      // 异步上传
      qcloud_cos::MultiPutObjectReq put_req(m_bucket_name, object_name,
                                            local_file);
      // 完成回调
      auto multi_put_done_cb = [](const SharedAsyncContext& context,
                                  void* user_data) {
        ASSERT_TRUE(context->GetResult().IsSucc());
        FileInfo* file_info = reinterpret_cast<FileInfo*>(user_data);
        ASSERT_EQ(context->GetMultiGetObjectResp().GetXCosHashCrc64Ecma(),
                  std::to_string(file_info->m_file_crc64_origin));
        ASSERT_EQ(context->GetMultiPutObjectResp().GetServer(), "tencent-cos");
        ASSERT_TRUE(!context->GetMultiPutObjectResp().GetDate().empty());
        ASSERT_TRUE(!context->GetMultiPutObjectResp().GetContentType().empty());
        ASSERT_TRUE(!context->GetMultiPutObjectResp().GetEtag().empty());
      };

      std::cout << "async upload object: " << object_name
                << ", size: " << file_size << std::endl;
      // 设置上传完成回调
      put_req.SetDoneCallback(multi_put_done_cb);
      // 设置私有数据
      put_req.SetUserData(reinterpret_cast<void*>(&file_info));

      // 开始上传
      SharedAsyncContext context = m_client->PutObjectAsync(put_req);

      // 等待上传结束
      context->WaitUntilFinish();
      ASSERT_TRUE(context->GetResult().IsSucc());

      // 下载
      auto multi_get_done_cb = [](const SharedAsyncContext& context,
                                  void* user_data) {
        ASSERT_TRUE(context->GetResult().IsSucc());
        FileInfo* file_info = (reinterpret_cast<FileInfo*>(user_data));
        MultiGetObjectResp resp = context->GetMultiGetObjectResp();
        ASSERT_EQ(resp.GetXCosHashCrc64Ecma(),
                  std::to_string(file_info->m_file_crc64_origin));
        // ASSERT_EQ(resp.GetEtag(), file_info->m_file_md5_origin);
        ASSERT_EQ(context->GetMultiGetObjectResp().GetServer(), "tencent-cos");
        ASSERT_TRUE(context->GetMultiGetObjectResp().GetContentLength() > 0);
        ASSERT_TRUE(!context->GetMultiGetObjectResp().GetDate().empty());
        ASSERT_TRUE(
            !context->GetMultiGetObjectResp().GetLastModified().empty());
        ASSERT_TRUE(
            !context->GetMultiGetObjectResp().GetXCosRequestId().empty());
        ASSERT_TRUE(!context->GetMultiGetObjectResp().GetEtag().empty());
        ASSERT_TRUE(
            !context->GetMultiGetObjectResp().GetContentRange().empty());
      };
      std::string local_file_download = local_file + "_download";

      qcloud_cos::MultiGetObjectReq get_req(m_bucket_name, object_name,
                                            local_file_download);
      // 设置完成回调
      get_req.SetDoneCallback(multi_get_done_cb);
      // 设置私有数据
      get_req.SetUserData(reinterpret_cast<void*>(&file_info));
      std::cout << "async download object: " << object_name << std::endl;
      // 开始下载
      context = m_client->GetObjectAsync(get_req);

      // 等待下载结束
      context->WaitUntilFinish();
      ASSERT_TRUE(context->GetResult().IsSucc());

      // 校验下载下来的文件
      std::string file_md5_download =
          TestUtils::CalcFileMd5(local_file_download);
      ASSERT_EQ(file_md5_download, file_md5_origin);

      // 删除对象
      CosResult del_result;
      qcloud_cos::DeleteObjectReq del_req(m_bucket_name, object_name);
      qcloud_cos::DeleteObjectResp del_resp;
      del_result = m_client->DeleteObject(del_req, &del_resp);
      ASSERT_TRUE(del_result.IsSucc());

      // 删除本地文件
      TestUtils::RemoveFile(local_file);
      TestUtils::RemoveFile(local_file_download);
    }
  }
}

TEST_F(AsyncOpTest, AsyncOpWithConcurrent) {
  std::vector<int> base_size_v = {
      1, 5, 35, 356, 1024, 2545, 25678, 1024 * 1024, 5 * 1024 * 1024};
  for (auto& size : base_size_v) {
    const int concurrent_size = 5;
    std::unordered_map<std::string, FileInfo> concurrent_objects;
    std::list<std::string> waiting_list;
    std::mutex mutex;
    std::condition_variable cond;

    // 生成文件
    for (int i = 0; i < concurrent_size; i++) {
      size_t file_size = ((rand() % 100) + 1) * size;
      std::string object_name = "test_async_" + std::to_string(i);
      std::string local_file = "./" + object_name;
      std::cout << "generate file: " << local_file << std::endl;
      TestUtils::WriteRandomDatatoFile(local_file, file_size);
      uint64_t file_crc64_origin = FileUtil::GetFileCrc64(local_file);
      std::string file_md5_origin = FileUtil::GetFileMd5(local_file);
      FileInfo file_info = {
          object_name, local_file,        local_file + "_downlaod",
          file_size,   file_crc64_origin, file_md5_origin};
      concurrent_objects[object_name] = file_info;
      waiting_list.push_back(object_name);
    }

    // 多个文件异步上传
    DoneCallback multi_put_done_cb;
    for (auto& o : concurrent_objects) {
      qcloud_cos::MultiPutObjectReq put_req(
          m_bucket_name, o.second.m_object_name, o.second.m_local_file);
      multi_put_done_cb = [multi_put_done_cb, &mutex, &cond,
                           &waiting_list](const SharedAsyncContext& context,
                                          void* user_data) {
        EXPECT_TRUE(context->GetResult().IsSucc());
        std::unordered_map<std::string, FileInfo>* concurrent_objects =
            reinterpret_cast<std::unordered_map<std::string, FileInfo>*>(
                user_data);
        ASSERT_TRUE(concurrent_objects->count(context->GetObjectName()) == 1);
        if (!context->GetResult().IsSucc()) {
          std::cout << "failed to uplaod " << context->GetObjectName()
                    << ", retry..." << std::endl;
          // 上传失败，重试
          qcloud_cos::MultiPutObjectReq put_req(context->GetBucketName(),
                                                context->GetObjectName(),
                                                context->GetLocalFilePath());
          // 设置上传完成回调
          put_req.SetDoneCallback(multi_put_done_cb);
          // 设置私有数据
          put_req.SetUserData(user_data);
          // 开始上传
          m_client->PutObjectAsync(put_req);
        } else {
          // 上传成功
          std::cout << "succeed to uplaod " << context->GetObjectName()
                    << std::endl;
          MultiPutObjectResp resp = context->GetMultiPutObjectResp();
          ASSERT_EQ(
              resp.GetXCosHashCrc64Ecma(),
              std::to_string((*concurrent_objects)[context->GetObjectName()]
                                 .m_file_crc64_origin));
          std::unique_lock<std::mutex> lock(mutex);
          waiting_list.remove(context->GetObjectName());
          cond.notify_one();
        }
      };
      // 设置上传完成回调
      put_req.SetDoneCallback(multi_put_done_cb);
      // 设置私有数据
      put_req.SetUserData(reinterpret_cast<void*>(&concurrent_objects));
      std::cout << "start to uplaod " << o.second.m_object_name << std::endl;
      m_client->PutObjectAsync(put_req);
    }

    // TODO timeout
    while (!waiting_list.empty()) {
      std::string waiting_objects;
      for (auto& o : waiting_list) {
        waiting_objects.append(o).append(" ");
      }
      std::cout << "waiting for " << waiting_objects << " to complete uploading"
                << std::endl;
      std::unique_lock<std::mutex> lock(mutex);
      cond.wait(lock);
    }

    std::cout << "succeed to upload all objects" << std::endl;

    // 多个文件异步下载
    for (auto& o : concurrent_objects) {
      waiting_list.push_back(o.first);
    }

    DoneCallback multi_get_done_cb;
    for (auto& o : concurrent_objects) {
      multi_get_done_cb = [&multi_get_done_cb, &mutex, &cond,
                           &waiting_list](const SharedAsyncContext& context,
                                          void* user_data) {
        EXPECT_TRUE(context->GetResult().IsSucc());
        std::unordered_map<std::string, FileInfo>* concurrent_objects =
            reinterpret_cast<std::unordered_map<std::string, FileInfo>*>(
                user_data);
        ASSERT_TRUE(concurrent_objects->count(context->GetObjectName()) == 1);
        if (!context->GetResult().IsSucc()) {
          std::cout << "failed to download " << context->GetObjectName()
                    << ", retry..." << std::endl;
          // 下载失败，重试
          qcloud_cos::MultiGetObjectReq get_req(context->GetBucketName(),
                                                context->GetObjectName(),
                                                context->GetLocalFilePath());
          get_req.SetDoneCallback(multi_get_done_cb);
          get_req.SetUserData(user_data);
          m_client->GetObjectAsync(get_req);
        } else {
          // 下载成功
          std::cout << "succeed to download " << context->GetObjectName()
                    << std::endl;
          MultiGetObjectResp resp = context->GetMultiGetObjectResp();
          ASSERT_EQ(
              resp.GetXCosHashCrc64Ecma(),
              std::to_string((*concurrent_objects)[context->GetObjectName()]
                                 .m_file_crc64_origin));
          std::unique_lock<std::mutex> lock(mutex);
          waiting_list.remove(context->GetObjectName());
          cond.notify_one();
        }
      };
      qcloud_cos::MultiGetObjectReq get_req(m_bucket_name,
                                            o.second.m_object_name,
                                            o.second.m_local_file_download);
      // 设置完成回调
      get_req.SetDoneCallback(multi_get_done_cb);
      // 设置私有数据
      get_req.SetUserData(reinterpret_cast<void*>(&concurrent_objects));
      std::cout << "start to download object: " << o.second.m_object_name
                << std::endl;
      // 开始下载
      m_client->GetObjectAsync(get_req);
    }

    // TODO timeout
    while (!waiting_list.empty()) {
      std::string waiting_objects;
      for (auto& o : waiting_list) {
        waiting_objects.append(o).append(" ");
      }
      std::cout << "waiting for " << waiting_objects
                << " to complete downloading" << std::endl;
      std::unique_lock<std::mutex> lock(mutex);
      cond.wait(lock);
    }

    std::cout << "succeed to download all objects" << std::endl;

    // 校验下载下来的文件
    for (auto& o : concurrent_objects) {
      std::string file_md5_download =
          TestUtils::CalcFileMd5(o.second.m_local_file_download);
      ASSERT_EQ(file_md5_download, o.second.m_file_md5_origin);
    }

    for (auto& o : concurrent_objects) {
      // 删除对象
      CosResult del_result;
      qcloud_cos::DeleteObjectReq del_req(m_bucket_name,
                                          o.second.m_object_name);
      qcloud_cos::DeleteObjectResp del_resp;
      del_result = m_client->DeleteObject(del_req, &del_resp);
      ASSERT_TRUE(del_result.IsSucc());
      // 删除本地文件
      TestUtils::RemoveFile(o.second.m_local_file);
      TestUtils::RemoveFile(o.second.m_local_file_download);
    }
  }
}

TEST_F(AsyncOpTest, AsyncOpWithException) {
  // TODO
}

}  // namespace qcloud_cos