#include "op/file_upload_task.h"
#include <sstream>
#include "Poco/DigestStream.h"
#include "Poco/MD5Engine.h"
#include "Poco/StreamCopier.h"
#include "util/http_sender.h"
#include "util/string_util.h"
#include "util/codec_util.h"
#include "util/crc64.h"
#ifdef USE_OPENSSL_MD5
#include <openssl/md5.h>
#endif

namespace qcloud_cos {

FileUploadTask::FileUploadTask(const std::string& full_url,
                               uint64_t conn_timeout_in_ms,
                               uint64_t recv_timeout_in_ms, unsigned char* pbuf,
                               const size_t data_len,
                               bool verify_cert,
                               const std::string& ca_location,
                               SSLCtxCallback ssl_ctx_cb,
                               void *user_data)
    : m_full_url(full_url),
      m_conn_timeout_in_ms(conn_timeout_in_ms),
      m_recv_timeout_in_ms(recv_timeout_in_ms),
      m_data_buf_ptr(pbuf),
      m_data_len(data_len),
      m_resp(""),
      m_is_task_success(false),
      m_is_resume(false),
      m_handler(NULL),
      m_verify_cert(verify_cert),
      m_ca_location(ca_location),
      m_ssl_ctx_cb(ssl_ctx_cb),
      m_user_data(user_data),
      mb_check_crc64(false),
      m_crc64_value(0) {}

FileUploadTask::FileUploadTask(
    const std::string& full_url,
    const std::map<std::string, std::string>& headers,
    const std::map<std::string, std::string>& params,
    uint64_t conn_timeout_in_ms, uint64_t recv_timeout_in_ms,
    const SharedTransferHandler& handler,
    bool verify_cert,
    const std::string& ca_location,
    SSLCtxCallback ssl_ctx_cb,
    void *user_data)
    : m_full_url(full_url),
      m_headers(headers),
      m_params(params),
      m_conn_timeout_in_ms(conn_timeout_in_ms),
      m_recv_timeout_in_ms(recv_timeout_in_ms),
      m_data_buf_ptr(NULL),
      m_data_len(0),
      m_resp(""),
      m_is_task_success(false),
      m_is_resume(false),
      m_handler(handler),
      m_verify_cert(verify_cert),
      m_ca_location(ca_location),
      m_ssl_ctx_cb(ssl_ctx_cb),
      m_user_data(user_data),
      mb_check_crc64(false),
      m_crc64_value(0) {}

FileUploadTask::FileUploadTask(
    const std::string& full_url,
    const std::map<std::string, std::string>& headers,
    const std::map<std::string, std::string>& params,
    uint64_t conn_timeout_in_ms, uint64_t recv_timeout_in_ms,
    unsigned char* pbuf, const size_t data_len,
    bool verify_cert,
    const std::string& ca_location,
    SSLCtxCallback ssl_ctx_cb,
    void *user_data)
    : m_full_url(full_url),
      m_headers(headers),
      m_params(params),
      m_conn_timeout_in_ms(conn_timeout_in_ms),
      m_recv_timeout_in_ms(recv_timeout_in_ms),
      m_data_buf_ptr(pbuf),
      m_data_len(data_len),
      m_resp(""),
      m_is_task_success(false),
      m_is_resume(false),
      m_handler(NULL),
      m_verify_cert(verify_cert),
      m_ca_location(ca_location),
      m_ssl_ctx_cb(ssl_ctx_cb),
      m_user_data(user_data),
      mb_check_crc64(false),
      m_crc64_value(0) {}

void FileUploadTask::run() {
  m_resp = "";
  m_is_task_success = false;
  UploadTask();
}

void FileUploadTask::SetUploadBuf(unsigned char* pbuf, size_t data_len) {
  m_data_buf_ptr = pbuf;
  m_data_len = data_len;
}

bool FileUploadTask::IsTaskSuccess() const { return m_is_task_success; }

std::string FileUploadTask::GetTaskResp() const { return m_resp; }

int FileUploadTask::GetHttpStatus() const { return m_http_status; }

std::map<std::string, std::string> FileUploadTask::GetRespHeaders() const {
  return m_resp_headers;
}

void FileUploadTask::SetParams(
    const std::map<std::string, std::string>& params) {
  m_params.clear();
  m_params.insert(params.begin(), params.end());
}

void FileUploadTask::AddParams(
    const std::map<std::string, std::string>& params) {
  std::map<std::string, std::string>::const_iterator itr = params.begin();
  for (; itr != params.end(); ++itr) {
    m_params[itr->first] = itr->second;
  }
}

void FileUploadTask::SetHeaders(
    const std::map<std::string, std::string>& headers) {
  m_headers.clear();
  m_headers.insert(headers.begin(), headers.end());
}

void FileUploadTask::AddHeaders(
    const std::map<std::string, std::string>& headers) {
  std::map<std::string, std::string>::const_iterator itr = headers.begin();
  for (; itr != headers.end(); ++itr) {
    m_headers[itr->first] = itr->second;
  }
}

void FileUploadTask::SetPartNumber(uint64_t part_number) {
  m_part_number = part_number;
}

void FileUploadTask::SetVerifyCert(bool verify_cert) {
  m_verify_cert = verify_cert;
}

void FileUploadTask::SetCaLocation(const std::string& ca_location) {
  m_ca_location = ca_location;
}

void FileUploadTask::SetSslCtxCb(SSLCtxCallback cb, void *data) {
  m_ssl_ctx_cb = cb;
  m_user_data = data;
}

void FileUploadTask::UploadTask() {
  std::string md5_str;
  // 数据一致性校验采用crc64
  if (mb_check_crc64) {
    m_crc64_value = 0;
    m_crc64_value = CRC64::CalcCRC(m_crc64_value, static_cast<void*>(m_data_buf_ptr), m_data_len);
    SDK_LOG_DBG("Part Crc64: %" PRIu64, m_crc64_value);
  }
  // 没有crc64则默认走md5校验
  else {
  #ifdef USE_OPENSSL_MD5
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)m_data_buf_ptr, m_data_len, digest);
    md5_str = CodecUtil::DigestToHex(digest, MD5_DIGEST_LENGTH);
  #else
    // 计算上传的md5
    Poco::MD5Engine md5;
    std::string body((const char*)m_data_buf_ptr, m_data_len);
    std::istringstream istr(body);
    Poco::DigestOutputStream dos(md5);
    Poco::StreamCopier::copyStream(istr, dos);
    dos.close();
    md5_str = Poco::DigestEngine::digestToHex(md5.digest());
  #endif
    SDK_LOG_DBG("Part Md5: %s", md5_str.c_str());
  }

  int loop = 0;
  do {
    loop++;
    m_resp_headers.clear();
    m_resp.clear();

    // if (m_handler) {
    //   SDK_LOG_INFO("transfer send request");
    //   std::istringstream iss(body);
    //   std::ostringstream oss;
    //   m_http_status = HttpSender::TransferSendRequest(
    //       m_handler, "PUT", m_full_url, m_params, m_headers, iss,
    //       m_conn_timeout_in_ms, m_recv_timeout_in_ms, &m_resp_headers, oss,
    //       &m_err_msg, false);
    //   m_resp = oss.str();
    // } else {
    std::istringstream is;
    std::ostringstream oss;
    m_http_status = HttpSender::SendRequest(
        m_handler, "PUT", m_full_url, m_params, m_headers, is,
        m_conn_timeout_in_ms, m_recv_timeout_in_ms, &m_resp_headers, oss,
        &m_err_msg, false, m_verify_cert, m_ca_location, m_ssl_ctx_cb, m_user_data,
        (const char*)m_data_buf_ptr, m_data_len);
    //}
    m_resp = oss.str();

    if (m_http_status != 200) {
      SDK_LOG_ERR("FileUpload: url(%s) fail, httpcode:%d, resp: %s",
                  m_full_url.c_str(), m_http_status, m_resp.c_str());
      m_is_task_success = false;
      continue;
    }

    // crc64一致性校验
    if (mb_check_crc64) {
      std::map<std::string, std::string>::const_iterator c_itr =
        m_resp_headers.find(kRespHeaderXCosHashCrc64Ecma);
      if (c_itr == m_resp_headers.end() || 
          StringUtil::StringToUint64(c_itr->second) != m_crc64_value) {
        SDK_LOG_ERR(
            "Response x-cos-hash-crc64ecma is not correct, try again. Expect crc64 is %" PRIu64 ", but "
            "return crc64 is %s",
            m_crc64_value, c_itr->second.c_str());
        m_is_task_success = false;
        continue;
      }
      SDK_LOG_DBG("Part Crc64 Check Success.");
    } else {
      std::map<std::string, std::string>::const_iterator c_itr =
          m_resp_headers.find("ETag");
      if (c_itr == m_resp_headers.end() ||
          StringUtil::Trim(c_itr->second, "\"") != md5_str) {
        SDK_LOG_ERR(
            "Response etag is not correct, try again. Expect md5 is %s, but "
            "return etag is %s.",
            md5_str.c_str(), StringUtil::Trim(c_itr->second, "\"").c_str());
        m_is_task_success = false;
        continue;
      }
      SDK_LOG_DBG("Part Md5 Check Success.");
    }

    m_is_task_success = true;
  } while (!m_is_task_success && loop <= kMaxRetryTimes);

  return;
}
}  // namespace qcloud_cos
