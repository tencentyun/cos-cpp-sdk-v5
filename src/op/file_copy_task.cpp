#include "op/file_copy_task.h"

#include "op/object_op.h"
#include "request/object_req.h"
#include "response/object_resp.h"
#include "util/http_sender.h"

namespace qcloud_cos {

FileCopyTask::FileCopyTask(const std::string& full_url,
                           uint64_t conn_timeout_in_ms,
                           uint64_t recv_timeout_in_ms)
    : m_full_url(full_url), m_conn_timeout_in_ms(conn_timeout_in_ms),
      m_recv_timeout_in_ms(recv_timeout_in_ms), m_is_task_success(false), m_etag("") {
}

bool FileCopyTask::IsTaskSuccess() const {
    return m_is_task_success;
}

std::string FileCopyTask::GetTaskResp() const {
    return m_resp;
}

int FileCopyTask::GetHttpStatus() const {
    return m_http_status;
}

std::map<std::string, std::string> FileCopyTask::GetRespHeaders() const {
    return m_resp_headers;
}

void FileCopyTask::SetParams(const std::map<std::string, std::string>& params) {
    m_params.clear();
    m_params.insert(params.begin(), params.end());
}

void FileCopyTask::SetHeaders(const std::map<std::string, std::string>& headers) {
    m_headers.clear();
    m_headers.insert(headers.begin(), headers.end());
}

void FileCopyTask::run() {
    m_is_task_success = false;
    CopyTask();
}

void FileCopyTask::CopyTask() {
    int loop = 0;
    do {
        loop++;
        m_resp_headers.clear();
        m_resp = "";

        m_http_status = HttpSender::SendRequest("PUT", m_full_url, m_params, m_headers,
                                        "", m_conn_timeout_in_ms, m_recv_timeout_in_ms,
                                        &m_resp_headers, &m_resp, &m_err_msg);

        if (m_http_status != 200) {
            SDK_LOG_ERR("FileUpload: url(%s) fail, httpcode:%d, resp: %s",
                        m_full_url.c_str(), m_http_status, m_resp.c_str());
            m_is_task_success = false;
            continue;
        }

        UploadPartCopyDataResp resp;
        if (!resp.ParseFromXmlString(m_resp)) {
            SDK_LOG_ERR("FileUpload response string is illegal. try again.");
            m_is_task_success = false;
            continue;
        }

        m_etag = resp.GetEtag();
        m_last_modified = resp.GetLastModified();
        m_is_task_success = true;
    } while (!m_is_task_success && loop <= kMaxRetryTimes);
}

} // namespace qcloud_cos
