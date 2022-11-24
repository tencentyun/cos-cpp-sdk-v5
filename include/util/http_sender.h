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
                         const std::string& ca_location = "");

  static int SendRequest(const SharedTransferHandler& handler,
                         const std::string& http_method,
                         const std::string& url_str,
                         const std::map<std::string, std::string>& req_params,
                         const std::map<std::string, std::string>& req_headers,
                         const std::string& req_body,
                         uint64_t conn_timeout_in_ms,
                         uint64_t recv_timeout_in_ms,
                         std::map<std::string, std::string>* resp_headers,
                         std::ostream& resp_stream, std::string* err_msg,
                         bool is_check_md5 = false,
                         bool is_verify_cert = true,
                         const std::string& ca_location = "");

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
                         const std::string& ca_location = "");

  static int SendRequest(const SharedTransferHandler& handler,
                         const std::string& http_method,
                         const std::string& url_str,
                         const std::map<std::string, std::string>& req_params,
                         const std::map<std::string, std::string>& req_headers,
                         std::istream& is, uint64_t conn_timeout_in_ms,
                         uint64_t recv_timeout_in_ms,
                         std::map<std::string, std::string>* resp_headers,
                         std::ostream& resp_stream, std::string* err_msg,
                         bool is_check_md5 = false,
                         bool is_verify_cert = true,
                         const std::string& ca_location = "");

  static int SendRequest(const SharedTransferHandler& handler,
                         const std::string& http_method,
                         const std::string& url_str,
                         const std::map<std::string, std::string>& req_params,
                         const std::map<std::string, std::string>& req_headers,
                         const std::string& req_body,
                         uint64_t conn_timeout_in_ms,
                         uint64_t recv_timeout_in_ms,
                         std::map<std::string, std::string>* resp_headers,
                         std::string* xml_err_str, std::ostream& resp_stream,
                         std::string* err_msg, uint64_t* real_byte,
                         bool is_check_md5 = false,
                         bool is_verify_cert = true, 
                         const std::string& ca_location = "");
};

}  // namespace qcloud_cos
#endif  // HTTP_SENDER_H
