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

#include "Poco/Runnable.h"
#include "cos_config.h"
#include "trsf/transfer_handler.h"
#include "util/base_op_util.h"
#include "util/semaphore.h"
#include "util/task.h"

namespace qcloud_cos {

class FileDownTask : public Poco::Runnable {
 public:
  FileDownTask(const std::string& host, 
               const std::string& path, 
               const bool is_https, 
               const BaseOpUtil& op_util,
               const std::map<std::string, std::string>& headers,
               const std::map<std::string, std::string>& params,
               uint64_t conn_timeout_in_ms, uint64_t recv_timeout_in_ms,
               const SharedTransferHandler& handler = nullptr,
               uint64_t offset = 0, unsigned char* pbuf = NULL,
               const size_t data_len = 0,
               bool verify_cert = true,
               const std::string& ca_lication = "",
               SSLCtxCallback ssl_ctx_cb = nullptr,
               void *user_data = nullptr,
               Semaphore* semaphore = nullptr);

  ~FileDownTask() {}

  void run();

  void DownTask();

  void SetDownParams(unsigned char* pdatabuf, size_t datalen, uint64_t offset);

  void SetVerifyCert(bool verify_cert);
  void SetCaLocation(const std::string& ca_location);
  void SetSslCtxCb(SSLCtxCallback cb, void *data);

  // 设置信号量，用于任务完成时自动释放资源
  void SetSemaphore(Semaphore* semaphore) { m_semaphore = semaphore; }

  // 设置当前任务在下载序列中的顺序号
  void SetSequence(uint64_t sequence) { m_task_info.sequence = sequence;}

  // 重置任务状态为IDLE，供主线程在处理完TASK_COMPLETED后调用以复用任务槽
  void ResetTaskStatus() { m_task_info.status = TASK_IDLE; }

  void SetTaskRunning() { m_task_info.status = TASK_RUNNING; }

  TaskStatus GetTaskStatus() const { return m_task_info.status; }

  std::string GetTaskResp() const { return m_resp; }

  size_t GetDownLoadLen() const { return m_real_down_len; }

  bool IsTaskSuccess() const { return m_is_task_success; }

  uint64_t GetSequence() const { return m_task_info.sequence; }

  int GetHttpStatus() const { return m_http_status; }

  std::map<std::string, std::string> GetRespHeaders() const { return m_resp_headers; }

  std::string GetErrMsg() const { return m_err_msg; }

 private:
  std::string m_host;
  std::string m_path;
  bool m_is_https;
  BaseOpUtil m_op_util;
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
  SSLCtxCallback m_ssl_ctx_cb;
  void *m_user_data;

  // 信号量指针，用于任务完成时自动通知
  Semaphore* m_semaphore;

  TaskInfo m_task_info;

  SharedConfig m_config;

  void SendRequestOnce(std::string domain);
};

}  // namespace qcloud_cos