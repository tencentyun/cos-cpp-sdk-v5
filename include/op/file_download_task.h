// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:
#pragma once

#include <stdint.h>

#include <map>
#include <string>

#include "Poco/Foundation.h"
#include "Poco/Runnable.h"
#include "cos_config.h"
#include "trsf/transfer_handler.h"

namespace qcloud_cos {

class FileDownTask : public Poco::Runnable {
 public:
  FileDownTask(const std::string& full_url,
               const std::map<std::string, std::string>& headers,
               const std::map<std::string, std::string>& params,
               uint64_t conn_timeout_in_ms, uint64_t recv_timeout_in_ms,
               const SharedTransferHandler& handler = nullptr,
               uint64_t offset = 0, unsigned char* pbuf = NULL,
               const size_t data_len = 0, 
               bool verify_cert = true,
               const std::string& ca_lication = "");

  ~FileDownTask() {}

  void run();

  void DownTask();

  void SetDownParams(unsigned char* pdatabuf, size_t datalen, uint64_t offset);

  void SetVerifyCert(bool verify_cert);
  void SetCaLocation(const std::string& ca_location);

  std::string GetTaskResp();

  size_t GetDownLoadLen();

  bool IsTaskSuccess();

  int GetHttpStatus();

  std::map<std::string, std::string> GetRespHeaders();

  std::string GetErrMsg() const { return m_err_msg; }

 private:
  std::string m_full_url;
  std::map<std::string, std::string> m_headers;
  std::map<std::string, std::string> m_params;
  uint64_t m_conn_timeout_in_ms;
  uint64_t m_recv_timeout_in_ms;
  SharedTransferHandler m_handler;
  uint64_t m_offset;
  unsigned char* m_data_buf_ptr;
  size_t m_data_len;
  std::string m_resp;
  bool m_is_task_success;
  size_t m_real_down_len;
  int m_http_status;
  std::map<std::string, std::string> m_resp_headers;
  std::string m_err_msg;

  bool m_verify_cert;
  std::string m_ca_location;

  SharedConfig m_config;
};

}  // namespace qcloud_cos