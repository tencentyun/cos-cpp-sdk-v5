#include "op/file_upload_task.h"

#include <sstream>
#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/StreamCopier.h"
#include "util/string_util.h"
#include "util/http_sender.h"

namespace qcloud_cos {

    FileUploadTask::FileUploadTask(const std::string& full_url,
                                   uint64_t conn_timeout_in_ms,
                                   uint64_t recv_timeout_in_ms,
                                   unsigned char* pbuf,
                                   const size_t data_len)
            : m_full_url(full_url), m_data_buf_ptr(pbuf), m_data_len(data_len),
              m_conn_timeout_in_ms(conn_timeout_in_ms), m_recv_timeout_in_ms(recv_timeout_in_ms),
              m_resp(""), m_is_task_success(false), m_is_resume(false), m_handler(NULL) {
    }

    FileUploadTask::FileUploadTask(const std::string& full_url,
                                       uint64_t conn_timeout_in_ms,
                                       uint64_t recv_timeout_in_ms,
                                       const SharedTransferHandler& handler)
            : m_full_url(full_url), m_data_buf_ptr(NULL), m_data_len(0),
              m_conn_timeout_in_ms(conn_timeout_in_ms), m_recv_timeout_in_ms(recv_timeout_in_ms),
              m_resp(""), m_is_task_success(false), m_is_resume(false), m_handler(handler) {
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
              m_data_buf_ptr(pbuf), m_data_len(data_len), m_resp(""), m_is_task_success(false),
              m_is_resume(false), m_handler(NULL) {
    }

    void FileUploadTask::run() {
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

    void FileUploadTask::SetPartNumber(uint64_t part_number) {
        m_part_number = part_number;
    }

    void FileUploadTask::UploadTask() {
        std::string body((const char *)m_data_buf_ptr, m_data_len);
        // 计算上传的md5
        Poco::MD5Engine md5;
        std::istringstream istr(body);
        Poco::DigestOutputStream dos(md5);
        Poco::StreamCopier::copyStream(istr, dos);
        dos.close();
        const std::string& md5_str = Poco::DigestEngine::digestToHex(md5.digest());

        int loop = 0;
        do {
            loop++;
            m_resp_headers.clear();
            m_resp.clear();

            if (m_handler) {
                SDK_LOG_INFO("transfer send request");
                std::istringstream iss(body);
                std::ostringstream oss;
                m_http_status = HttpSender::TransferSendRequest(m_handler, "PUT", m_full_url, m_params, m_headers,
                                                        iss, m_conn_timeout_in_ms, m_recv_timeout_in_ms,
                                                        &m_resp_headers, oss, &m_err_msg, false);
                m_resp = oss.str();
            } else {
                m_http_status = HttpSender::SendRequest("PUT", m_full_url, m_params, m_headers,
                                                        body, m_conn_timeout_in_ms, m_recv_timeout_in_ms,
                                                        &m_resp_headers, &m_resp, &m_err_msg);
            }

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
