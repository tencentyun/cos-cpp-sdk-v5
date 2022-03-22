#ifndef __TRSF_HANDLER_H__
#define __TRSF_HANDLER_H__

#include "op/cos_result.h"
#include "response/object_resp.h"
#include <condition_variable>
#include <exception>
#include <functional>
#include <istream>
#include <map>
#include <memory>
#include <mutex>
#include <ostream>

namespace qcloud_cos {

class ObjectReq;
class TransferHandler;
class AsyncContext;

typedef std::shared_ptr<TransferHandler> SharedTransferHandler;

typedef std::shared_ptr<AsyncContext> SharedAsyncContext;

/// @brief 进度回调函数
using TransferProgressCallback = std::function<void(
    uint64_t transferred_size, uint64_t total_size, void* user_data)>;

/// @brief 完成回调函数
using DoneCallback =
    std::function<void(const SharedAsyncContext& context, void* user_data)>;

class PartState {
 public:
  PartState();
  PartState(int part_num, std::string& etag, size_t size,
            bool last_part = false);

  void SetPartNum(int number) { m_part_num = number; }
  int GetPartNum() const { return m_part_num; }

  void SetEtag(const std::string& etag) { m_etag = etag; }
  std::string GetEtag() const { return m_etag; }

  void SetSize(size_t size) { m_size_inbytes = size; }
  size_t GetSize() const { return m_size_inbytes; }

  void SetLastPart(bool lastpart) { m_lastpart = lastpart; }
  bool IsLastPart() { return m_lastpart; }

 private:
  int m_part_num;
  // current use the md5
  std::string m_etag;

  size_t m_size_inbytes;

  // TODO for now just care about the whole progress
  /* size_t m_current_progress_inbytes; */
  /* size_t m_range_begin; */

  bool m_lastpart;
};

typedef std::shared_ptr<PartState> PartPointer;
// Key is partnumber
typedef std::map<int, PartPointer> PartStateMap;

enum class TransferStatus {
  NOT_START,
  // Operation is now running
  IN_PROGRESS,
  // Operation was canceled.
  CANCELED,
  // Operation failed
  FAILED,
  // Operation was successful
  COMPLETED,
  // Operation either failed or was canceled and a user deleted the multi-part
  // upload .
  ABORTED
};

class TransferHandler : public std::enable_shared_from_this<TransferHandler> {
 public:
  TransferHandler();

  ~TransferHandler() {}

  void SetBucketName(const std::string& bucket_name) {
    m_bucket_name = bucket_name;
  }
  std::string GetBucketName() const { return m_bucket_name; }

  void SetObjectName(const std::string& object_name) {
    m_object_name = object_name;
  }
  std::string GetObjectName() const { return m_object_name; }

  void SetLocalFilePath(const std::string& local_file_path) {
    m_local_file_path = local_file_path;
  }
  std::string GetLocalFilePath() const { return m_local_file_path; }

  void SetTotalSize(uint64_t total_size) { m_total_size = total_size; }
  uint64_t GetTotalSize() const { return m_total_size; }

  // Notice there can not backwards
  void UpdateProgress(uint64_t update_prog);
  // Get the current upload size(B).
  uint64_t GetProgress() const;

  void UpdateStatus(const TransferStatus& status);

  void UpdateStatus(const TransferStatus& status, const CosResult& result);

  void UpdateStatus(const TransferStatus& status, const CosResult& result,
                    const std::map<std::string, std::string> headers,
                    const std::string& body = "");

  TransferStatus GetStatus() const;

  std::string GetStatusString() const;

  void SetUploadID(const std::string& uploadid) { m_uploadid = uploadid; }
  // Get the init or resumed uploadid.
  std::string GetUploadID() const { return m_uploadid; }

  // Cancel the process of interface the uploadid can reuse.
  void Cancel();

  bool ShouldContinue() const;

  bool IsFinishStatus(TransferStatus status) const;

  bool IsAllowTransition(TransferStatus org, TransferStatus dst) const;

  // Block until finish.
  void WaitUntilFinish();

  /// @brief  设置进度回调函数
  void SetTransferProgressCallback(const TransferProgressCallback& callback) {
    m_progress_cb = callback;
  }
  /// @brief  设置状态回调函数
  void SetDoneCallback(const DoneCallback& callback) { m_done_cb = callback; }

  /// @brief 设置回调私有数据
  void SetUserData(void* user_data) { m_user_data = user_data; }

  /// @brief 设置请求信息
  void SetRequest(const void* req);

  ///////////////////////////////////////////////////////////////////////////
  // 用户调用的函数
  /// @brief 获取操作结果
  CosResult GetResult() const { return m_result; }

  /// @brief 获取响应
  AsyncResp GetAsyncResp() const;

 private:
  CosResult m_result;
  std::map<std::string, std::string> m_resp_headers;
  std::string m_resp_body;

 private:
  std::string m_bucket_name;
  std::string m_object_name;
  std::string m_local_file_path;
  uint64_t m_total_size;
  // The m_current_progress best to use the atomic. but can not support c11 for
  // now, so use the mutex.
  uint64_t m_current_progress;
  TransferStatus m_status;
  std::string m_uploadid;
  // Is cancel
  bool m_cancel;

  PartStateMap m_part_map;

  // Mutex lock for the progress
  mutable std::mutex m_lock_prog;
  // Mutex lock for the status
  mutable std::mutex m_lock_stat;
  // Condition
  mutable std::condition_variable m_cond;

  // callback function
  TransferProgressCallback m_progress_cb;
  DoneCallback m_done_cb;
  void* m_user_data;

  // Mutex lock for the part map
  // mutable boost::mutex m_lock_parts;
};

class HandleStreamCopier {
 public:
  static std::streamsize handleCopyStream(const SharedTransferHandler& handler,
                                          std::istream& istr,
                                          std::ostream& ostr,
                                          std::size_t bufferSize = 8192);
};

class UserCancelException : public std::exception {
 public:
  UserCancelException() {}
  ~UserCancelException() throw() {}
};

}  // namespace qcloud_cos

#endif
