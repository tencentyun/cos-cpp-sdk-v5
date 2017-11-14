#include "op/file_upload_task.h"

#include <stdint.h>
#include <string.h>

#include <map>

namespace qcloud_cos{

FileUploadTask::FileUploadTask(const std::string& full_url,
                               uint64_t conn_timeout_in_ms,
                               uint64_t recv_timeout_in_ms,
                               unsigned char* pbuf,
                               const size_t data_len)
    : m_full_url(full_url), m_data_buf_ptr(pbuf), m_data_len(data_len),
      m_conn_timeout_in_ms(conn_timeout_in_ms), m_recv_timeout_in_ms(recv_timeout_in_ms),
      m_resp(""), m_is_task_success(false) {
}

FileUploadTask::FileUploadTask(const std::string& full_url,
                               const std::map<std::string, std::string>& headers,
                               const std::map<std::string, std::string>& params,
                               uint64_t conn_timeout_in_ms,
                               uint64_t recv_timeout_in_ms,
                               unsigned char* pbuf,
                               const size_t data_len)
    : m_full_url(full_url), m_headers(headers), m_params(params),
      m_conn_timeout_in_ms(conn_timeout_in_ms), m_recv_timeout_in_ms(recv_timeout_in_ms),
      m_data_buf_ptr(pbuf), m_data_len(data_len), m_resp(""), m_is_task_success(false) {
}

void FileUploadTask::Run() {
    m_resp = "";
    m_is_task_success = false;
    UploadTask();
}

void FileUploadTask::SetUploadBuf(unsigned char* pbuf, size_t data_len) {
    m_data_buf_ptr = pbuf;
    m_data_len = data_len;
}

bool FileUploadTask::IsTaskSuccess() const {
    return m_is_task_success;
}

std::string FileUploadTask::GetTaskResp() const {
    return m_resp;
}

int FileUploadTask::GetHttpStatus() const {
    return m_http_status;
}

std::map<std::string, std::string> FileUploadTask::GetRespHeaders() const {
    return m_resp_headers;
}

void FileUploadTask::SetParams(const std::map<std::string, std::string>& params) {
    m_params.clear();
    m_params.insert(params.begin(), params.end());
}

void FileUploadTask::SetHeaders(const std::map<std::string, std::string>& headers) {
    m_headers.clear();
    m_headers.insert(headers.begin(), headers.end());
}

void FileUploadTask::UploadTask() {
    int loop = 0;
    do {
        loop++;
        std::string body((const char *)m_data_buf_ptr, m_data_len);
        m_resp_headers.clear();
        m_resp = "";
        m_http_status = HttpSender::SendRequest("PUT", m_full_url, m_params, m_headers,
                                        body, m_conn_timeout_in_ms, m_recv_timeout_in_ms,
                                        &m_resp_headers, &m_resp, &m_err_msg);
        if (m_http_status != 200) {
            SDK_LOG_ERR("FileUpload: url(%s) fail, httpcode:%d, resp: %s",
                        m_full_url.c_str(), m_http_status, m_resp.c_str());
            m_is_task_success = false;
            continue;
        }
        m_is_task_success = true;
    } while (!m_is_task_success && loop <= kMaxRetryTimes);

    return;
}

}

