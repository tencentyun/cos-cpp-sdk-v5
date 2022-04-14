// Copyright (c) 2022, Tencent Inc.
// All rights reserved.

#include <stdlib.h>
#include <sys/stat.h>

#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>

#include "cos_api.h"
#include "cos_sys_config.h"
#include "util/auth_tool.h"
#include "util/test_utils.h"

using namespace qcloud_cos;

static void PrintResult(const qcloud_cos::CosResult& result) {
  std::stringstream oss;
  oss << "ErrorMsg=" << result.GetErrorMsg() << std::endl;
  oss << "HttpStatus=" << result.GetHttpStatus() << std::endl;
  oss << "ErrorCode=" << result.GetErrorCode() << std::endl;
  oss << "ErrorMsg=" << result.GetErrorMsg() << std::endl;
  oss << "ResourceAddr=" << result.GetResourceAddr() << std::endl;
  oss << "XCosRequestId=" << result.GetXCosRequestId() << std::endl;
  oss << "XCosTraceId=" << result.GetXCosTraceId() << std::endl;
  SDK_LOG_INFO("%s", oss.str().c_str());
}

enum PutOpType { SIMPLE_PUT = 1, MULTI_PUT, ASYNC_PUT, ASYNC_MULTI_PUT };
enum GetOpType { SIMPLE_GET = 6, MULTI_GET, ASYNC_GET, ASYNC_MULTI_GET };

#define GENERATE_LOCAL_FILE(size)                                       \
  std::string object_name = "stable_test_size_" + std::to_string(size); \
  std::string local_file = "./" + object_name;                          \
  SDK_LOG_INFO("generate local file: %s", local_file.c_str());          \
  TestUtils::WriteRandomDatatoFile(local_file, file_size);              \
  uint64_t file_crc64_origin = FileUtil::GetFileCrc64(local_file);      \
  std::string file_md5_origin = FileUtil::GetFileMd5(local_file);       \
  SDK_LOG_INFO("file_crc64_origin: %lu", file_crc64_origin);            \
  SDK_LOG_INFO("file_md5_origin: %s", file_md5_origin.c_str());

#define CHECK_COMMON_RESULT(result)           \
  assert(result.IsSucc());                    \
  assert(!result.GetXCosRequestId().empty()); \
  assert(result.GetHttpStatus() >= 200 && result.GetHttpStatus() < 300);

#define CHECK_COMMON_RESP(resp)              \
  assert(resp.GetServer() == "tencent-cos"); \
  assert(!resp.GetDate().empty());           \
  assert(!resp.GetConnection().empty());     \
  assert(!resp.GetXCosRequestId().empty());

#define CHECK_PUT_RESP(resp, op_type)                                       \
  assert(resp.GetXCosHashCrc64Ecma() == std::to_string(file_crc64_origin)); \
  if (op_type == MULTI_PUT || op_type == ASYNC_MULTI_PUT) {                 \
    assert(!resp.GetContentType().empty());                                 \
    assert(resp.GetEtag() != file_md5_origin);                              \
  } else {                                                                  \
    assert(resp.GetEtag() == file_md5_origin);                              \
  }

#define CHECK_HEAD_RESP(resp)                                               \
  assert(resp.GetXCosHashCrc64Ecma() == std::to_string(file_crc64_origin)); \
  assert(resp.GetContentLength() == file_size);                             \
  assert(!resp.GetLastModified().empty());

#define CHECK_GET_RESP(resp, op_type)                                          \
  assert(resp.GetXCosHashCrc64Ecma() == std::to_string(file_crc64_origin));    \
  assert(!resp.GetLastModified().empty());                                     \
  std::string file_md5_download = TestUtils::CalcFileMd5(local_file_download); \
  assert(file_md5_download == file_md5_origin);                                \
  if (op_type == SIMPLE_GET || op_type == ASYNC_GET) {                         \
    assert(resp.GetContentLength() == file_size);                              \
  }

class TestStatistics {
 public:
  TestStatistics()
      : put_object_succ(0),
        put_object_fail(0),
        multi_put_object_succ(0),
        multi_put_object_fail(0),
        async_put_object_succ(0),
        async_put_object_fail(0),
        async_multi_put_object_succ(0),
        async_multi_put_object_fail(0),
        get_object_succ(0),
        get_object_fail(0),
        multi_get_object_succ(0),
        multi_get_object_fail(0),
        async_get_object_succ(0),
        async_get_object_fail(0),
        async_multi_get_object_succ(0),
        async_multi_get_object_fail(0),
        head_object_succ(0),
        head_object_fail(0),
        delete_object_succ(0),
        delete_object_fail(0) {}

  int put_object_succ;
  int put_object_fail;
  int multi_put_object_succ;
  int multi_put_object_fail;
  int async_put_object_succ;
  int async_put_object_fail;
  int async_multi_put_object_succ;
  int async_multi_put_object_fail;
  int get_object_succ;
  int get_object_fail;
  int multi_get_object_succ;
  int multi_get_object_fail;
  int async_get_object_succ;
  int async_get_object_fail;
  int async_multi_get_object_succ;
  int async_multi_get_object_fail;
  int head_object_succ;
  int head_object_fail;
  int delete_object_succ;
  int delete_object_fail;
  void UpdataStat(bool succ, int op_type) {
    switch (op_type) {
      case SIMPLE_PUT:
        succ ? put_object_succ++ : put_object_fail++;
      case MULTI_PUT:
        succ ? multi_put_object_succ++ : multi_put_object_fail++;
      case ASYNC_PUT:
        succ ? async_put_object_succ++ : async_put_object_fail++;
      case ASYNC_MULTI_PUT:
        succ ? async_multi_put_object_succ++ : async_multi_put_object_fail++;
      case SIMPLE_GET:
        succ ? get_object_succ++ : get_object_fail++;
      case MULTI_GET:
        succ ? multi_get_object_succ++ : multi_get_object_fail++;
      case ASYNC_GET:
        succ ? async_get_object_succ++ : async_get_object_fail++;
      case ASYNC_MULTI_GET:
        succ ? async_multi_get_object_succ++ : async_multi_get_object_fail++;
    }
  }
  void PrintStat() {
    std::stringstream oss;
    oss << std::endl;
    oss << "put_object_succ:" << put_object_succ << std::endl;
    oss << "put_object_fail:" << put_object_fail << std::endl;
    oss << "multi_put_object_succ:" << put_object_succ << std::endl;
    oss << "multi_put_object_fail:" << put_object_fail << std::endl;
    oss << "async_put_object_succ:" << put_object_succ << std::endl;
    oss << "async_put_object_fail:" << put_object_fail << std::endl;
    oss << "async_multi_put_object_succ:" << put_object_succ << std::endl;
    oss << "async_multi_put_object_fail:" << put_object_fail << std::endl;
    oss << "get_object_succ:" << put_object_succ << std::endl;
    oss << "get_object_fail:" << put_object_fail << std::endl;
    oss << "multi_get_object_succ:" << put_object_succ << std::endl;
    oss << "multi_get_object_fail:" << put_object_fail << std::endl;
    oss << "async_get_object_succ:" << put_object_succ << std::endl;
    oss << "async_get_object_fail:" << put_object_fail << std::endl;
    oss << "async_multi_get_object_succ:" << put_object_succ << std::endl;
    oss << "async_multi_get_object_fail:" << put_object_fail << std::endl;
    oss << "head_object_succ:" << put_object_succ << std::endl;
    oss << "head_object_fail:" << put_object_fail << std::endl;
    oss << "delete_object_succ:" << put_object_succ << std::endl;
    oss << "delete_object_fail:" << put_object_fail << std::endl;
    SDK_LOG_INFO("%s", oss.str().c_str());
  }
};

void TestLogCallback(const std::string& log) {
  std::ofstream ofs;
  ofs.open("./stable_test.log", std::ios_base::app);
  ofs << log;
  ofs.close();
}

int main(int argc, char** argv) {
  UNUSED_PARAM(argc)
  UNUSED_PARAM(argv)
  qcloud_cos::CosConfig config("./config.json");
  config.SetLogCallback(&TestLogCallback);
  qcloud_cos::CosAPI cos(config);
  TestStatistics stat;
  std::string bucket_name = "test-12345678";
  size_t max_file_size = 100 * 1024 * 1024;
  while (true) {
    std::vector<int> put_op_type_v = {SIMPLE_PUT, MULTI_PUT, ASYNC_PUT,
                                      ASYNC_MULTI_PUT};
    for (auto& op_type : put_op_type_v) {
      size_t file_size = rand() % max_file_size;
      GENERATE_LOCAL_FILE(file_size)

      bool done = false;
      std::mutex mutex;
      std::condition_variable cond;

      auto put_done_cb = [file_crc64_origin, file_md5_origin, &done, &mutex,
                          &cond, &stat](const SharedAsyncContext& context,
                                        void* user_data) {
        int op_type = (*reinterpret_cast<int*>(user_data));
        CHECK_COMMON_RESULT(context->GetResult())
        if (context->GetResult().IsSucc()) {
          stat.UpdataStat(true, op_type);
        } else {
          stat.UpdataStat(false, op_type);
        }
        AsyncResp resp = context->GetAsyncResp();
        CHECK_COMMON_RESP(resp)
        CHECK_PUT_RESP(resp, op_type)
        done = true;
        std::unique_lock<std::mutex> lock(mutex);
        cond.notify_one();
      };

      // put object
      CosResult result;
      PutObjectByFileResp resp1;
      MultiPutObjectResp resp2;
      if (op_type == SIMPLE_PUT) {
        SDK_LOG_INFO("put object, object_name: %s", object_name.c_str());
        PutObjectByFileReq req(bucket_name, object_name, local_file);
        result = cos.PutObject(req, &resp1);
      } else if (op_type == MULTI_PUT) {
        SDK_LOG_INFO("multi put object, object_name: %s", object_name.c_str());
        MultiPutObjectReq req(bucket_name, object_name, local_file);
        result = cos.MultiPutObject(req, &resp2);
      } else if (op_type == ASYNC_PUT) {
        SDK_LOG_INFO("async put object, object_name: %s", object_name.c_str());
        AsyncPutObjectReq req(bucket_name, object_name, local_file);
        req.SetDoneCallback(put_done_cb);
        req.SetUserData(reinterpret_cast<void*>(&op_type));
        MultiPutObjectResp resp;
        cos.AsyncPutObject(req);
      } else if (op_type == ASYNC_MULTI_PUT) {
        SDK_LOG_INFO("async multi put object, object_name: %s",
                     object_name.c_str());
        AsyncMultiPutObjectReq req(bucket_name, object_name, local_file);
        req.SetUserData(reinterpret_cast<void*>(&op_type));
        req.SetDoneCallback(put_done_cb);
        cos.AsyncMultiPutObject(req);
      }
      if (op_type == SIMPLE_PUT || op_type == MULTI_PUT) {
        if (!result.IsSucc()) {
          SDK_LOG_ERR("failed to put object, object_name: %s",
                      object_name.c_str());
          PrintResult(result);
          stat.UpdataStat(false, op_type);
        } else {
          stat.UpdataStat(true, op_type);
          CHECK_COMMON_RESULT(result)
          if (op_type == SIMPLE_PUT) {
            CHECK_COMMON_RESP(resp1)
            CHECK_PUT_RESP(resp1, op_type)
          } else {
            CHECK_COMMON_RESP(resp2)
            CHECK_PUT_RESP(resp2, op_type)
          }
        }
      } else {
        while (!done) {
          SDK_LOG_INFO("wait for put completing, object_name: %s",
                       object_name.c_str());
          std::unique_lock<std::mutex> lock(mutex);
          cond.wait_until(
              lock, std::chrono::system_clock::now() + std::chrono::seconds(1));
        }
      }

      // head object
      HeadObjectReq head_req(bucket_name, object_name);
      HeadObjectResp headresp;
      SDK_LOG_INFO("head object, object_name: %s", object_name.c_str());
      result = cos.HeadObject(head_req, &headresp);
      if (!result.IsSucc()) {
        SDK_LOG_ERR("failed to head object, object_name: %s",
                    object_name.c_str());
        PrintResult(result);
        stat.head_object_fail++;
      } else {
        stat.head_object_succ++;
        CHECK_COMMON_RESULT(result)
        CHECK_COMMON_RESP(headresp)
        CHECK_HEAD_RESP(headresp)
      }

      // get object
      // 使用多个接口下载同一个对象
      done = false;
      std::string local_file_download = local_file + "_download";
      auto get_done_cb = [file_size, local_file_download, file_crc64_origin,
                          file_md5_origin, &done, &mutex, &cond,
                          &stat](const SharedAsyncContext& context,
                                 void* user_data) {
        int op_type = (*reinterpret_cast<int*>(user_data));
        CHECK_COMMON_RESULT(context->GetResult())
        if (context->GetResult().IsSucc()) {
          stat.UpdataStat(true, op_type);
        } else {
          stat.UpdataStat(false, op_type);
        }
        AsyncResp resp = context->GetAsyncResp();
        CHECK_COMMON_RESP(resp)
        CHECK_GET_RESP(resp, op_type)
        done = true;
        std::unique_lock<std::mutex> lock(mutex);
        cond.notify_one();
      };
      std::vector<int> get_op_type_v = {SIMPLE_GET, MULTI_GET, ASYNC_GET,
                                        ASYNC_MULTI_GET};
      for (auto& op_type : get_op_type_v) {
        GetObjectByFileResp resp1;
        MultiGetObjectResp resp2;
        if (op_type == SIMPLE_GET) {
          SDK_LOG_INFO("get object, object_name: %s", object_name.c_str());
          GetObjectByFileReq req(bucket_name, object_name, local_file_download);
          result = cos.GetObject(req, &resp1);
        } else if (op_type == MULTI_GET) {
          SDK_LOG_INFO("multi get object, object_name: %s",
                       object_name.c_str());
          MultiGetObjectReq req(bucket_name, object_name, local_file_download);
          result = cos.MultiGetObject(req, &resp2);
        } else if (op_type == ASYNC_GET) {
          SDK_LOG_INFO("async get object, object_name: %s",
                       object_name.c_str());
          AsyncGetObjectReq req(bucket_name, object_name, local_file_download);
          req.SetDoneCallback(get_done_cb);
          req.SetUserData(reinterpret_cast<void*>(&op_type));
          cos.AsyncGetObject(req);
        } else if (op_type == ASYNC_MULTI_GET) {
          SDK_LOG_INFO("async multi get object, object_name: %s",
                       object_name.c_str());
          AsyncMultiGetObjectReq req(bucket_name, object_name,
                                     local_file_download);
          req.SetUserData(reinterpret_cast<void*>(&op_type));
          req.SetDoneCallback(get_done_cb);
          cos.AsyncMultiGetObject(req);
        }
        if (op_type == SIMPLE_GET || op_type == MULTI_GET) {
          if (!result.IsSucc()) {
            SDK_LOG_ERR("failed to get object, object_name: %s",
                        object_name.c_str());
            PrintResult(result);
            stat.UpdataStat(false, op_type);
          } else {
            stat.UpdataStat(true, op_type);
            CHECK_COMMON_RESULT(result)
            if (op_type == SIMPLE_GET) {
              CHECK_COMMON_RESP(resp1)
              CHECK_GET_RESP(resp1, op_type)
            } else {
              CHECK_COMMON_RESP(resp2)
              CHECK_GET_RESP(resp2, op_type)
            }
          }
        } else {
          while (!done) {
            SDK_LOG_INFO("wait for get completing, object_name: %s",
                         object_name.c_str());
            std::unique_lock<std::mutex> lock(mutex);
            cond.wait_until(lock, std::chrono::system_clock::now() +
                                      std::chrono::seconds(1));
          }
          done = false;
        }
      }

      // delete object
      SDK_LOG_INFO("delete object, object_name: %s", object_name.c_str());
      qcloud_cos::DeleteObjectReq del_req(bucket_name, object_name);
      qcloud_cos::DeleteObjectResp del_resp;
      result = cos.DeleteObject(del_req, &del_resp);
      if (!result.IsSucc()) {
        SDK_LOG_ERR("failed to delete object, object_name: %s",
                    object_name.c_str());
        PrintResult(result);
        stat.delete_object_fail++;
      } else {
        stat.delete_object_succ++;
        CHECK_COMMON_RESULT(result)
        CHECK_COMMON_RESP(del_resp)
      }

      // delete local file
      TestUtils::RemoveFile(local_file);
      TestUtils::RemoveFile(local_file_download);
    }
    stat.PrintStat();
  }

#if defined(_WIN32)
  system("pause");
#endif
  return 0;
}
