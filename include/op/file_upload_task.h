#pragma once

#include <map>
#include <string>

#include "Poco/Runnable.h"
#include "request/object_req.h"
#include "trsf/transfer_handler.h"
#include "util/base_op_util.h"

namespace qcloud_cos {

class FileUploadTask : public Poco::Runnable {
 public:
  FileUploadTask(const std::string& host, 
                 const std::string& path, 
                 const bool is_https, 
                 const BaseOpUtil& op_util, 
                 uint64_t conn_timeout_in_ms,
                 uint64_t recv_timeout_in_ms, unsigned char* pbuf = NULL,
                 const size_t data_len = 0,
                 bool verify_cert = true,
                 const std::string& ca_location = "",
                 SSLCtxCallback ssl_ctx_cb = nullptr,
                 void *user_data = nullptr);

  FileUploadTask(const std::string& host,
                 const std::string& path, 
                 const bool is_https, 
                 const BaseOpUtil& op_util, 
                 const std::map<std::string, std::string>& headers,
                 const std::map<std::string, std::string>& params,
                 uint64_t conn_timeout_in_ms, uint64_t recv_timeout_in_ms,
                 const SharedTransferHandler& handler,
                 bool verify_cert = true,
                 const std::string& ca_location = "",
                 SSLCtxCallback ssl_ctx_cb = nullptr,
                 void *user_data = nullptr);

  FileUploadTask(const std::string& host,
                 const std::string& path, 
                 const bool is_https, 
                 const BaseOpUtil& op_util,
                 const std::map<std::string, std::string>& headers,
                 const std::map<std::string, std::string>& params,
                 uint64_t conn_timeout_in_ms, uint64_t recv_timeout_in_ms,
                 unsigned char* pbuf = NULL, const size_t data_len = 0,
                 bool verify_cert = true,
                 const std::string& ca_location = "",
                 SSLCtxCallback ssl_ctx_cb = nullptr,
                 void *user_data = nullptr);

  ~FileUploadTask() {}

  void run();

  void UploadTask();

  void SetUploadBuf(unsigned char* pdatabuf, size_t data_len);

  std::string GetTaskResp() const;

  bool IsTaskSuccess() const;

  void SetTaskSuccess() { m_is_task_success = true; }

  int GetHttpStatus() const;

  std::map<std::string, std::string> GetRespHeaders() const;

  void AddParams(const std::map<std::string, std::string>& params);
  void SetParams(const std::map<std::string, std::string>& params);

  void AddHeaders(const std::map<std::string, std::string>& headers);
  void SetHeaders(const std::map<std::string, std::string>& headers);

  std::string GetErrMsg() const { return m_err_msg; }

  void SetResume(const bool is_resume) { m_is_resume = is_resume; }

  bool IsResume() const { return m_is_resume; }

  void SetResumeEtag(const std::string& etag) { m_resume_etag = etag; }

  std::string GetResumeEtag() const { return m_resume_etag; }

  void SetPartNumber(uint64_t part_number);

  uint64_t GetPartNumber() const { return m_part_number; }

  void SetVerifyCert(bool verify_cert);
  void SetCaLocation(const std::string& ca_location);
  void SetSslCtxCb(SSLCtxCallback cb, void *data);

  void SetCheckCrc64(bool check_crc64) {
    mb_check_crc64 = check_crc64;
  }

  uint64_t GetCrc64Value() const {
    return m_crc64_value;
  }

 private:
  std::string m_host;
  std::string m_path;
  bool m_is_https;
  std::map<std::string, std::string> m_headers;
  std::map<std::string, std::string> m_params;
  uint64_t m_conn_timeout_in_ms;
  uint64_t m_recv_timeout_in_ms;
  unsigned char* m_data_buf_ptr;
  size_t m_data_len;
  std::string m_resp;
  bool m_is_task_success;
  int m_http_status;
  std::map<std::string, std::string> m_resp_headers;
  std::string m_err_msg;
  bool m_is_resume;
  std::string m_resume_etag;
  uint64_t m_part_number;
  SharedTransferHandler m_handler;

  bool m_verify_cert;
  std::string m_ca_location;
  SSLCtxCallback m_ssl_ctx_cb;
  void *m_user_data;

  bool mb_check_crc64;
  uint64_t m_crc64_value;

  BaseOpUtil m_op_util;

  void SendRequestOnce(std::string domain, std::string md5_str);
};

}  // namespace qcloud_cos
