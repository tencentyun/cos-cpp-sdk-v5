#pragma once
#include "response/object_resp.h"
#include "trsf/transfer_handler.h"
#include <memory>

namespace qcloud_cos {

class AsyncContext {
 public:
  AsyncContext(const SharedTransferHandler& handler) : m_handler(handler) {}

  /// @brief 取消操作
  void Cancel() { return m_handler->Cancel(); }

  /// @brief 等待操作结束
  void WaitUntilFinish() { return m_handler->WaitUntilFinish(); }

  std::string GetBucketName() const { return m_handler->GetBucketName(); }

  std::string GetObjectName() const { return m_handler->GetObjectName(); }

  std::string GetLocalFilePath() const { return m_handler->GetLocalFilePath(); }

  /// @brief 获取操作结果
  CosResult GetResult() const { return m_handler->GetResult(); }

  /// @brief 获取多线程上传响应
  AsyncResp GetAsyncResp() const {
    return m_handler->GetAsyncResp();
  }

 private:
  SharedTransferHandler m_handler;
};

}  // namespace qcloud_cos