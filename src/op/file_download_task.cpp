#include "op/file_download_task.h"
#include <string.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "cos_sys_config.h"
#include "util/http_sender.h"
#include "util/base_op_util.h"

namespace qcloud_cos {

FileDownTask::FileDownTask(const std::string& host,
                           const std::string& path,
                           const bool is_https,
                           const BaseOpUtil& op_util,
                           const std::map<std::string, std::string>& headers,
                           const std::map<std::string, std::string>& params,
                           uint64_t conn_timeout_in_ms,
                           uint64_t recv_timeout_in_ms,
                           const SharedTransferHandler& handler,
                           uint64_t offset, unsigned char* pbuf,
                           const size_t data_len,
                           bool verify_cert,
                           const std::string& ca_lication,
                           SSLCtxCallback ssl_ctx_cb,
                           void *user_data,
                           Semaphore* semaphore)
    : m_host(host),
      m_path(path),
      m_is_https(is_https),
      m_op_util(op_util),
      m_headers(headers),
      m_params(params),
      m_conn_timeout_in_ms(conn_timeout_in_ms),
      m_recv_timeout_in_ms(recv_timeout_in_ms),
      m_handler(handler),
      m_offset(offset),
      m_data_buf_ptr(pbuf),
      m_data_len(data_len),
      m_semaphore(semaphore),
      m_resp(""),
      m_is_task_success(false),
      m_task_info(),
      m_real_down_len(0),
      m_verify_cert(verify_cert),
      m_ca_location(ca_lication),
      m_ssl_ctx_cb(ssl_ctx_cb),
      m_user_data(user_data),
      m_http_status(0) {}

void FileDownTask::run() {
  m_resp = "";
  m_is_task_success = false;
  m_task_info.status = TaskStatus::TASK_RUNNING;
  DownTask();
  // 任务完成后标记状态, 最后自动通知信号量，释放资源槽位
  m_task_info.status = TaskStatus::TASK_COMPLETED;
  if (m_semaphore != nullptr) {
    m_semaphore->release();
  }
}

void FileDownTask::SetDownParams(unsigned char* pbuf, size_t data_len,
                                 uint64_t offset) {
  m_data_buf_ptr = pbuf;
  m_data_len = data_len;
  m_offset = offset;
}

void FileDownTask::SetVerifyCert(bool verify_cert) {
  m_verify_cert = verify_cert;
}

void FileDownTask::SetCaLocation(const std::string& ca_location) {
  m_ca_location = ca_location;
}

void FileDownTask::SetSslCtxCb(SSLCtxCallback cb, void *data) {
  m_ssl_ctx_cb = cb;
  m_user_data = data;
}

void FileDownTask::DownTask() {
    char range_head[128];
    memset(range_head, 0, sizeof(range_head));
    snprintf(range_head, sizeof(range_head), "bytes=%" PRIu64 "-%" PRIu64, m_offset, (m_offset + m_data_len - 1));

    // 增加Range头域，避免大文件时将整个文件下载
    m_headers["Range"] = range_head;

    std::string domain = m_host;
    for (int i = 0;; i++) {
      SendRequestOnce(domain);
      if (m_is_task_success) {
          break;
      }
      CosResult result;
      result.SetHttpStatus(m_http_status);
      result.ParseFromHttpResponse(m_resp_headers, m_resp);
      SDK_LOG_ERR("FileDownload: host(%s) path(%s) fail, httpcode:%d, resp: %s, try_times: %d", domain.c_str(),
          m_path.c_str(), m_http_status, m_resp.c_str(), i);
      if (i >= m_op_util.GetMaxRetryTimes() || m_op_util.NoNeedRetry(result)) {
          break;
      }
      if (m_op_util.ShouldChangeBackupDomain(result, i)) {
          domain = m_op_util.ChangeHostSuffix(domain);
      }
      m_op_util.SleepBeforeRetry(i);
    }
}

void FileDownTask::SendRequestOnce(std::string domain) {
  m_resp_headers.clear();
  m_resp = "";

  std::string full_url = m_op_util.GetRealUrl(domain, m_path, m_is_https);
  m_http_status = HttpSender::SendRequest(m_handler, "GET", full_url, m_params, m_headers, "", m_conn_timeout_in_ms,
      m_recv_timeout_in_ms, &m_resp_headers, &m_resp, &m_err_msg, false, m_verify_cert, m_ca_location, m_ssl_ctx_cb,
      m_user_data);

  // 当实际长度小于请求的数据长度时httpcode为206
  if (m_http_status != 200 && m_http_status != 206) {
      m_is_task_success = false;
      m_real_down_len = 0;
      return;
  }

  size_t buf_max_size = m_data_len;
  size_t len = std::min(m_resp.length(), buf_max_size);
  memcpy(m_data_buf_ptr, m_resp.c_str(), len);
  m_real_down_len = len;
  m_is_task_success = true;
  m_resp = "";
}

}  // namespace qcloud_cos
