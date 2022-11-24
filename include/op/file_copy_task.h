#pragma once

#include <stdint.h>

#include <map>
#include <string>

#include "Poco/Foundation.h"
#include "Poco/Runnable.h"

namespace qcloud_cos {

class FileCopyTask : public Poco::Runnable {
 public:
  FileCopyTask(const std::string& full_url, uint64_t conn_timeout_in_ms,
               uint64_t recv_timeout_in_ms);

  ~FileCopyTask() {}

  void run();

  void CopyTask();

  bool IsTaskSuccess() const;

  int GetHttpStatus() const;

  std::string GetTaskResp() const;

  std::map<std::string, std::string> GetRespHeaders() const;

  void SetParams(const std::map<std::string, std::string>& params);

  void SetHeaders(const std::map<std::string, std::string>& headers);

  void SetVerifyCert(bool verify_cert);
  void SetCaLocation(const std::string& ca_location);

  std::string GetErrMsg() const { return m_err_msg; }

  std::string GetEtag() const { return m_etag; }

  std::string GetLastModified() const { return m_last_modified; }

 private:
  std::string m_full_url;
  std::map<std::string, std::string> m_headers;
  std::map<std::string, std::string> m_params;
  uint64_t m_conn_timeout_in_ms;
  uint64_t m_recv_timeout_in_ms;
  std::string m_resp;
  bool m_is_task_success;
  int m_http_status;
  std::map<std::string, std::string> m_resp_headers;
  std::string m_err_msg;
  std::string m_etag;
  std::string m_last_modified;

  bool m_verify_cert;
  std::string m_ca_location;
};

}  // namespace qcloud_cos