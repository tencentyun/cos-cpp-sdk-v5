#include "op/file_copy_task.h"

#include "op/object_op.h"
#include "request/object_req.h"
#include "response/object_resp.h"
#include "util/http_sender.h"
#include "util/base_op_util.h"

namespace qcloud_cos {

FileCopyTask::FileCopyTask(const std::string& host,
                           const std::string& path,
                           const bool is_https,
                           const BaseOpUtil& op_util,
                           uint64_t conn_timeout_in_ms,
                           uint64_t recv_timeout_in_ms)
    : m_host(host),
      m_path(path),
      m_is_https(is_https),
      m_op_util(op_util),
      m_conn_timeout_in_ms(conn_timeout_in_ms),
      m_recv_timeout_in_ms(recv_timeout_in_ms),
      m_is_task_success(false),
      m_etag(""),
      m_verify_cert(true),
      m_ssl_ctx_cb(nullptr),
      m_user_data(nullptr) {}

bool FileCopyTask::IsTaskSuccess() const { return m_is_task_success; }

std::string FileCopyTask::GetTaskResp() const { return m_resp; }

int FileCopyTask::GetHttpStatus() const { return m_http_status; }

std::map<std::string, std::string> FileCopyTask::GetRespHeaders() const {
  return m_resp_headers;
}

void FileCopyTask::SetParams(const std::map<std::string, std::string>& params) {
  m_params.clear();
  m_params.insert(params.begin(), params.end());
}

void FileCopyTask::SetHeaders(
    const std::map<std::string, std::string>& headers) {
  m_headers.clear();
  m_headers.insert(headers.begin(), headers.end());
}

void FileCopyTask::SetVerifyCert(bool verify_cert) {
  m_verify_cert = verify_cert;
}

void FileCopyTask::SetCaLocation(const std::string& ca_location) {
  m_ca_location = ca_location;
}

void FileCopyTask::SetSslCtxCb(SSLCtxCallback cb, void *data) {
  m_ssl_ctx_cb = cb;
  m_user_data = data;
}

void FileCopyTask::run() {
  m_is_task_success = false;
  CopyTask();
}

void FileCopyTask::CopyTask() {
  std::string domain = m_host;
  for (int i = 0;; i++) {
    SendRequestOnce(domain);
    if (i >= m_op_util.GetMaxRetryTimes()) {
      break;
    }
    if (m_is_task_success) {
      break;
    }
    SDK_LOG_ERR("FileCopy: host(%s) path(%s) fail, retry num: %d, httpcode:%d, resp: %s",
            domain.c_str(), m_path.c_str(), i, m_http_status, m_resp.c_str());
    if (m_http_status >= 400 && m_http_status < 500) {
      break;
    }
    CosResult result;
    result.SetHttpStatus(m_http_status);
    result.ParseFromHttpResponse(m_resp_headers, m_resp);
    if (m_op_util.ShouldChangeBackupDomain(result, i)) {
      domain = m_op_util.ChangeHostSuffix(domain);
    }
    m_op_util.SleepBeforeRetry(i);
  }
}

void FileCopyTask::SendRequestOnce(std::string domain) {
    m_resp_headers.clear();
    m_resp = "";

    std::string full_url = m_op_util.GetRealUrl(domain, m_path, m_is_https);
    m_http_status = HttpSender::SendRequest(nullptr, "PUT", full_url, m_params, m_headers, "", m_conn_timeout_in_ms,
        m_recv_timeout_in_ms, &m_resp_headers, &m_resp, &m_err_msg, false, m_verify_cert, m_ca_location, m_ssl_ctx_cb,
        m_user_data);

    if (m_http_status != 200) {
        m_is_task_success = false;
        return;
    }

    UploadPartCopyDataResp resp;
    if (!resp.ParseFromXmlString(m_resp)) {
        m_is_task_success = false;
        return;
    }

    m_etag = resp.GetEtag();
    m_last_modified = resp.GetLastModified();
    m_is_task_success = true;
}

}  // namespace qcloud_cos
