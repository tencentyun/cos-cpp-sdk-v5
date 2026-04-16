// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/18/17
// Description:

#ifndef HTTP_SENDER_H
#define HTTP_SENDER_H
#pragma once

#include <stdint.h>

#include <map>
#include <string>

#include "trsf/transfer_handler.h"

namespace qcloud_cos {

// SendRequest 返回的特殊状态码（非 HTTP 标准状态码）
static const int kHttpStatusNetError    = -1;  // 网络/超时/SSL/校验等异常
static const int kHttpStatusUserCancel  = -2;  // 用户主动取消

class HttpSender {
 public:
  static int SendRequest(const SharedTransferHandler& handler,
                         const std::string& http_method,
                         const std::string& url_str,
                         const std::map<std::string, std::string>& req_params,
                         const std::map<std::string, std::string>& req_headers,
                         const std::string& req_body,
                         uint64_t conn_timeout_in_ms,
                         uint64_t recv_timeout_in_ms,
                         std::map<std::string, std::string>* resp_headers,
                         std::string* resp_body, std::string* err_msg,
                         bool is_check_md5 = false,
                         bool is_verify_cert = true,
                         const std::string& ca_location = "",
                         const SSLCtxCallback& ssl_ctx_cb = nullptr,
                         void *user_data = nullptr);

  static int SendRequest(const SharedTransferHandler& handler,
                         const std::string& http_method,
                         const std::string& url_str,
                         const std::map<std::string, std::string>& req_params,
                         const std::map<std::string, std::string>& req_headers,
                         std::istream& is, uint64_t conn_timeout_in_ms,
                         uint64_t recv_timeout_in_ms,
                         std::map<std::string, std::string>* resp_headers,
                         std::string* resp_body, std::string* err_msg,
                         bool is_check_md5 = false,
                         bool is_verify_cert = true,
                         const std::string& ca_location = "",
                         const SSLCtxCallback& ssl_ctx_cb = nullptr,
                         void *user_data = nullptr);

  static int SendRequest(const SharedTransferHandler& handler,
                         const std::string& http_method,
                         const std::string& url_str,
                         const std::map<std::string, std::string>& req_params,
                         const std::map<std::string, std::string>& req_headers,
                         std::istream& is, // 流式输入，用于传输请求正文
                         uint64_t conn_timeout_in_ms,
                         uint64_t recv_timeout_in_ms,
                         std::map<std::string, std::string>* resp_headers,
                         std::ostream& resp_stream, // 流式输出，用于接收响应正文
                         std::string* err_msg,
                         bool is_check_md5 = false,
                         bool is_verify_cert = true,
                         const std::string& ca_location = "",
                         const SSLCtxCallback& ssl_ctx_cb = nullptr,
                         void *user_data = nullptr,
                         const char *req_body_buf = nullptr,  // 可选的缓冲区
                         size_t req_body_len = 0);

  static int SendRequest(const SharedTransferHandler& handler,
                         const std::string& http_method,
                         const std::string& url_str,
                         const std::map<std::string, std::string>& req_params,
                         const std::map<std::string, std::string>& req_headers,
                         const std::string& req_body, // 字符串输入，用于传输请求正文
                         uint64_t conn_timeout_in_ms,
                         uint64_t recv_timeout_in_ms,
                         std::map<std::string, std::string>* resp_headers,
                         std::string* xml_err_str,  // 额外的错误信息, 用于响应返回非 2xx 错误码时, 传输报错响应信息
                         std::ostream& resp_stream, // 流式输出, 用于传输响应正文
                         std::string* err_msg, 
                         uint64_t* real_byte, // 实际接收字节数
                         bool is_check_md5 = false,
                         bool is_verify_cert = true,
                         const std::string& ca_location = "",
                         const SSLCtxCallback& ssl_ctx_cb = nullptr,
                         void *user_data = nullptr);
};

}  // namespace qcloud_cos
#endif  // HTTP_SENDER_H
