#include "op/file_upload_task.h"

#include <stdint.h>
#include <string.h>

#include <map>
#include <sstream>

#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/StreamCopier.h"

#include "util/string_util.h"

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
    : m_full_url(full_url), m_base_headers(headers), m_base_params(params),
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
    m_final_params = params;
    if (!m_base_params.empty()) {
        m_final_params.insert(m_base_params.begin(), m_base_params.end());
    }
}

void FileUploadTask::SetHeaders(const std::map<std::string, std::string>& headers) {
    m_final_headers = headers;
    if (!m_base_headers.empty()) {
        m_final_headers.insert(m_base_headers.begin(), m_base_headers.end());
    }
}

void FileUploadTask::UploadTask() {
    int loop = 0;

    std::string body((const char *)m_data_buf_ptr, m_data_len);
    // 计算上传的md5
    Poco::MD5Engine md5;
    std::istringstream istr(body);
    Poco::DigestOutputStream dos(md5);
    Poco::StreamCopier::copyStream(istr, dos);
    dos.close();
    const std::string& md5_str = Poco::DigestEngine::digestToHex(md5.digest());

    do {
        loop++;
        m_resp_headers.clear();
        m_resp = "";
        
        m_http_status = HttpSender::SendRequest("PUT", m_full_url, m_final_params, m_final_headers,
                                        body, m_conn_timeout_in_ms, m_recv_timeout_in_ms,
                                        &m_resp_headers, &m_resp, &m_err_msg);

        if (m_http_status != 200) {
            SDK_LOG_ERR("FileUpload: url(%s) fail, httpcode:%d, resp: %s",
                        m_full_url.c_str(), m_http_status, m_resp.c_str());
            m_is_task_success = false;
            continue;
        }

        std::map<std::string, std::string>::const_iterator c_itr = m_resp_headers.find("ETag");
        if (c_itr == m_resp_headers.end() || StringUtil::Trim(c_itr->second, "\"") != md5_str) {
            SDK_LOG_ERR("Response etag is not correct, try again. Expect md5 is %s, but return etag is %s.",
                        md5_str.c_str(), StringUtil::Trim(c_itr->second, "\"").c_str());
            m_is_task_success = false;
            continue;
        }

        m_is_task_success = true;
    } while (!m_is_task_success && loop <= kMaxRetryTimes);

    return;
}

}

