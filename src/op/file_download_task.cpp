#include "op/file_download_task.h"

#include "cos_sys_config.h"
#include "util/http_sender.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string.h>

namespace qcloud_cos {

FileDownTask::FileDownTask(const std::string& full_url,
                           const std::map<std::string, std::string>& headers,
                           const std::map<std::string, std::string>& params,
                           uint64_t conn_timeout_in_ms,
                           uint64_t recv_timeout_in_ms,
                           const SharedTransferHandler& handler,
                           uint64_t offset, unsigned char* pbuf,
                           const size_t data_len,
                           const std::string& ca_lication)
    : m_full_url(full_url),
      m_headers(headers),
      m_params(params),
      m_conn_timeout_in_ms(conn_timeout_in_ms),
      m_recv_timeout_in_ms(recv_timeout_in_ms),
      m_handler(handler),
      m_offset(offset),
      m_data_buf_ptr(pbuf),
      m_data_len(data_len),
      m_resp(""),
      m_is_task_success(false),
      m_real_down_len(0),
      m_ca_location(ca_lication) {}

void FileDownTask::run() {
  m_resp = "";
  m_is_task_success = false;
  DownTask();
}

void FileDownTask::SetDownParams(unsigned char* pbuf, size_t data_len,
                                 uint64_t offset) {
  m_data_buf_ptr = pbuf;
  m_data_len = data_len;
  m_offset = offset;
}

void FileDownTask::SetCaLocation(const std::string& ca_location) {
  m_ca_location = ca_location;
}

size_t FileDownTask::GetDownLoadLen() { return m_real_down_len; }

bool FileDownTask::IsTaskSuccess() { return m_is_task_success; }

std::string FileDownTask::GetTaskResp() { return m_resp; }

int FileDownTask::GetHttpStatus() { return m_http_status; }

std::map<std::string, std::string> FileDownTask::GetRespHeaders() {
  return m_resp_headers;
}

void FileDownTask::DownTask() {
  char range_head[128];
  memset(range_head, 0, sizeof(range_head));
  snprintf(range_head, sizeof(range_head), "bytes=%" PRIu64 "-%" PRIu64, m_offset,
           (m_offset + m_data_len - 1));

  // 增加Range头域，避免大文件时将整个文件下载
  m_headers["Range"] = range_head;

  int try_times = 0;
  do {
    try_times++;
    //if (m_handler) {
    //  SDK_LOG_INFO("transfer send GET request");
    //  std::istringstream iss("");
    //  std::ostringstream oss;
    //  m_http_status = HttpSender::TransferSendRequest(
    //      m_handler, "GET", m_full_url, m_params, m_headers, iss,
    //     m_conn_timeout_in_ms, m_recv_timeout_in_ms, &m_resp_headers, oss,
    //      &m_err_msg, false);
    //  m_resp = oss.str();
    //} else {
    m_http_status = HttpSender::SendRequest(
          m_handler, "GET", m_full_url, m_params, m_headers, "",
          m_conn_timeout_in_ms, m_recv_timeout_in_ms, &m_resp_headers, &m_resp,
          &m_err_msg, false, m_ca_location);
    //}
    //当实际长度小于请求的数据长度时httpcode为206
    if (m_http_status != 200 && m_http_status != 206) {
      SDK_LOG_ERR("FileDownload: url(%s) fail, httpcode:%d, resp: %s, try_times:%d",
                  m_full_url.c_str(), m_http_status, m_resp.c_str(), try_times);
      m_is_task_success = false;
      m_real_down_len = 0;
      continue;
    }

    size_t buf_max_size = m_data_len;
    size_t len = std::min(m_resp.length(), buf_max_size);
    memcpy(m_data_buf_ptr, m_resp.c_str(), len);
    m_real_down_len = len;
    m_is_task_success = true;
    m_resp = "";
    return;
  } while (!m_is_task_success && try_times <= kMaxRetryTimes);

  return;
}

}  // namespace qcloud_cos
