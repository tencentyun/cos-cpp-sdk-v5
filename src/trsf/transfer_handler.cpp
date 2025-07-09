#include "trsf/transfer_handler.h"
#include <iostream>
#include "Poco/Buffer.h"
#include "Poco/StreamCopier.h"
#include "response/object_resp.h"
#include "request/object_req.h"
#include "trsf/async_context.h"

namespace qcloud_cos {
PartState::PartState()
    : m_part_num(0), m_etag(""), m_size_inbytes(0), m_lastpart(false) {}

PartState::PartState(int part_num, std::string& etag, size_t size,
                     bool last_part)
    : m_part_num(part_num), m_etag(etag), m_size_inbytes(size),
      m_lastpart(last_part) {}

TransferHandler::TransferHandler()
    : m_total_size(0), m_current_progress(0),
      m_status(TransferStatus::NOT_START), m_uploadid(""), m_cancel(false),
      m_progress_cb(nullptr), m_done_cb(nullptr), m_user_data(nullptr) {}

# if 0
TransferHandler::TransferHandler(const ObjectReq* req)
    : m_bucket_name(req->GetBucketName()), m_object_name(req->GetObjectName()),
      m_local_file_path(req->GetLocalFilePath()),
      m_total_size(req->GetLocalFileSize()), m_current_progress(0),
      m_status(TransferStatus::NOT_START), m_uploadid(""), m_cancel(false),
      m_progress_cb(req->GetTransferProgressCallback()),
      m_done_cb(req->GetDoneCallback()), m_user_data(req->GetUserData()) {}
#endif
static std::string GetNameForStatus(TransferStatus status) {
  switch (status) {
    case TransferStatus::NOT_START:
      return "NOT_START";
    case TransferStatus::IN_PROGRESS:
      return "IN_PROGRESS";
    case TransferStatus::CANCELED:
      return "CANCELED";
    case TransferStatus::FAILED:
      return "FAILED";
    case TransferStatus::COMPLETED:
      return "COMPLETED";
    case TransferStatus::ABORTED:
      return "ABORTED";
    default:
      return "UNKNOWN";
  }
}

void TransferHandler::UpdateProgress(uint64_t update_prog) {
  {
    std::lock_guard<std::mutex> locker(m_lock_prog);

    m_current_progress += update_prog;

    // Notice the progress there can not backwards, but the each parts has retry
    // counts, should limit the progress no bigger than the total size. s3 has
    // two invariants:(1) Never lock; (2) Never go backwards. Complete me.
    if (m_current_progress > m_total_size) {
      m_current_progress = m_total_size;
    }
  }

  // trigger progress callback
  if (m_progress_cb) {
    m_progress_cb(m_current_progress, m_total_size, m_user_data);
  }
}

uint64_t TransferHandler::GetProgress() const {
  std::lock_guard<std::mutex> locker(m_lock_prog);
  return m_current_progress;
}

bool TransferHandler::IsFinishStatus(TransferStatus status) const {
  switch (status) {
    case TransferStatus::ABORTED:
    case TransferStatus::COMPLETED:
    case TransferStatus::FAILED:
    case TransferStatus::CANCELED:
      return true;
    default:
      return false;
  }
}

bool TransferHandler::IsAllowTransition(TransferStatus org,
                                        TransferStatus dst) const {
  if (org == dst) {
    return true;
  }

  if (IsFinishStatus(org) && IsFinishStatus(dst)) {
    return org == TransferStatus::CANCELED && dst == TransferStatus::ABORTED;
  }

  return true;
}

void TransferHandler::UpdateStatus(const TransferStatus& status) {
  // 必须先调done回调
  if (m_done_cb && IsFinishStatus(status)) {
    SharedAsyncContext context(new AsyncContext(shared_from_this()));
    m_done_cb(context, m_user_data);
  }

  {
    std::unique_lock<std::mutex> locker(m_lock_stat);
    if (IsAllowTransition(m_status, status)) {
      m_status = status;

      if (IsFinishStatus(status)) {
        if (status == TransferStatus::COMPLETED) {
          // Some other logic
        }
        // locker.unlock();
        m_cond.notify_all();
      }
    }
  }
}

void TransferHandler::UpdateStatus(const TransferStatus& status,
                                   const CosResult& result) {
  m_result = result;
  UpdateStatus(status);
}

void TransferHandler::UpdateStatus(
    const TransferStatus& status, const CosResult& result,
    const std::map<std::string, std::string> headers, const std::string& body) {
  // 先更新result，header和body
  m_result = result;
  m_resp_headers = headers;
  m_resp_body = body;
  UpdateStatus(status);
}

void TransferHandler::SetRequest(const void* req) {
  const ObjectReq* object_req = reinterpret_cast<const ObjectReq*>(req);
  m_bucket_name = object_req->GetBucketName();
  m_object_name = object_req->GetObjectName();
  m_local_file_path = object_req->GetLocalFilePath();
  //m_total_size = object_req->GetLocalFileSize();
  m_progress_cb = object_req->GetTransferProgressCallback();
  m_done_cb = object_req->GetDoneCallback();
  m_user_data = object_req->GetUserData();
}

AsyncResp TransferHandler::GetAsyncResp() const {
  AsyncResp resp;
  resp.SetHeaders(m_resp_headers);
  resp.SetBody(m_resp_body);
  return resp;
}

TransferStatus TransferHandler::GetStatus() const {
  std::lock_guard<std::mutex> locker(m_lock_stat);
  return m_status;
}

void TransferHandler::Cancel() {
  std::lock_guard<std::mutex> locker(m_lock_stat);
  m_cancel = true;
}

bool TransferHandler::ShouldContinue() const {
  std::lock_guard<std::mutex> locker(m_lock_stat);
  return !m_cancel;
}

void TransferHandler::WaitUntilFinish() {
  std::unique_lock<std::mutex> locker(m_lock_stat);
  while (!IsFinishStatus(m_status)) {
    m_cond.wait(locker);
  }
}

std::string TransferHandler::GetStatusString() const {
  return GetNameForStatus(m_status);
}

std::streamsize
HandleStreamCopier::handleCopyStream(const SharedTransferHandler& handler,
                                     const char *buf, size_t buf_len, std::ostream& ostr,
                                     std::size_t bufferSize) {
  poco_assert(bufferSize > 0);
  poco_assert(buf != nullptr);

  std::streamsize len = 0;
  std::streamsize n = static_cast<std::streamsize>(buf_len);
  std::streamsize w_len = 0;
  std::streamsize part_size = static_cast<std::streamsize>(bufferSize);
  while (n > 0) {
    // 用户取消操作
    if (handler && !handler->ShouldContinue()) {
      throw UserCancelException();
    }

    w_len = n > part_size ? part_size : n;
    ostr.write(buf + len, w_len);
    n -= w_len;
    len += w_len;

    // update progress
    if (handler) {
      handler->UpdateProgress(w_len);
    }
  }
  return len;
}

std::streamsize
HandleStreamCopier::handleCopyStream(const SharedTransferHandler& handler,
                                     std::istream& istr, std::ostream& ostr,
                                     std::size_t bufferSize) {
  poco_assert(bufferSize > 0);

  Poco::Buffer<char> buffer(bufferSize);
  std::streamsize len = 0;
  istr.read(buffer.begin(), bufferSize);
  std::streamsize n = istr.gcount();
  while (n > 0) {
    // 用户取消操作
    if (handler && !handler->ShouldContinue()) {
      throw UserCancelException();
    }

    len += n;
    ostr.write(buffer.begin(), n);
    // update progress
    if (handler) {
      handler->UpdateProgress(n);
    }
    if (istr && ostr) {
      istr.read(buffer.begin(), bufferSize);
      n = istr.gcount();
    } else {
      n = 0;
    }
  }
  return len;
}
}  // namespace qcloud_cos
