﻿// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "op/base_op.h"

#include <iostream>

#include "cos_sys_config.h"
#include "request/base_req.h"
#include "response/base_resp.h"
#include "util/auth_tool.h"
#include "util/codec_util.h"
#include "util/http_sender.h"
#include "util/simple_dns_cache.h"
#include "trsf/transfer_handler.h"

namespace qcloud_cos {

SimpleDnsCache& GetGlobalDnsCacheInstance() {
  static SimpleDnsCache dns_cache(CosSysConfig::GetDnsCacheSize(),
                                  CosSysConfig::GetDnsCacheExpireSeconds());
  return dns_cache;
}

CosConfig BaseOp::GetCosConfig() const { return *m_config; }

uint64_t BaseOp::GetAppId() const { return m_config->GetAppId(); }

std::string BaseOp::GetRegion() const { return m_config->GetRegion(); }

std::string BaseOp::GetAccessKey() const { return m_config->GetAccessKey(); }

std::string BaseOp::GetSecretKey() const { return m_config->GetSecretKey(); }

std::string BaseOp::GetTmpToken() const { return m_config->GetTmpToken(); }

std::string BaseOp::GetDestDomain() const {
  return m_config->GetDestDomain().empty() ? 
         CosSysConfig::GetDestDomain() : m_config->GetDestDomain();
}

CosResult BaseOp::NormalAction(const std::string& host, const std::string& path,
                               const BaseReq& req, const std::string& req_body,
                               bool check_body, BaseResp* resp) {
  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, check_body, resp);
}

CosResult BaseOp::NormalAction(
    const std::string& host, const std::string& path, const BaseReq& req,
    const std::map<std::string, std::string>& additional_headers,
    const std::map<std::string, std::string>& additional_params,
    const std::string& req_body, bool check_body, BaseResp* resp) {
  CosResult result;
  if (!CheckConfigValidation()) {
    std::string err_msg =
        "Invalid access_key secret_key or region, please check your "
        "configuration";
    SDK_LOG_ERR("%s", err_msg.c_str());
    result.SetErrorMsg(err_msg);
    result.SetFail();
    return result;
  }

  std::map<std::string, std::string> req_headers = req.GetHeaders();
  std::map<std::string, std::string> req_params = req.GetParams();
  req_headers.insert(additional_headers.begin(), additional_headers.end());
  req_params.insert(additional_params.begin(), additional_params.end());
  const std::string& tmp_token = m_config->GetTmpToken();
  if (!tmp_token.empty()) {
    req_headers["x-cos-security-token"] = tmp_token;
  }

  // 1. 获取host
  if (!CosSysConfig::IsDomainSameToHost()) {
    req_headers["Host"] = host;
  } else {
    req_headers["Host"] = GetDestDomain();
  }

  // 2. 计算签名
  std::string auth_str =
      AuthTool::Sign(GetAccessKey(), GetSecretKey(), req.GetMethod(),
                     req.GetPath(), req_headers, req_params);
  if (auth_str.empty()) {
    result.SetErrorMsg(
        "Generate auth str fail, check your access_key/secret_key.");
    return result;
  }
  req_headers["Authorization"] = auth_str;

  // 3. 发送请求
  std::map<std::string, std::string> resp_headers;
  std::string resp_body;

  std::string dest_url = GetRealUrl(host, path, req.IsHttps());
  std::string err_msg = "";
  int http_code = HttpSender::SendRequest(nullptr,
      req.GetMethod(), dest_url, req_params, req_headers, req_body,
      req.GetConnTimeoutInms(), req.GetRecvTimeoutInms(), &resp_headers,
      &resp_body, &err_msg, false, req.GetVerifyCert(), req.GetCaLocation());
  if (http_code == -1) {
    result.SetErrorMsg(err_msg);
    return result;
  }

  // 4. 解析返回的xml字符串
  result.SetHttpStatus(http_code);
  if (http_code > 299 || http_code < 200) {
    // 无法解析的错误, 填充到cos_result的error_info中
    if (!result.ParseFromHttpResponse(resp_headers, resp_body)) {
      result.SetErrorMsg(resp_body);
    }
  } else {
    // 某些请求，如PutObjectCopy/Complete请求需要进一步检查Body
    if (check_body && result.ParseFromHttpResponse(resp_headers, resp_body)) {
      result.SetErrorMsg(resp_body);
      return result;
    }

    result.SetSucc();
    resp->ParseFromXmlString(resp_body);
    resp->ParseFromHeaders(resp_headers);
    resp->SetBody(resp_body);
    // resp requestid to result
    result.SetXCosRequestId(resp->GetXCosRequestId());
  }

  return result;
}

CosResult BaseOp::DownloadAction(const std::string& host,
                                 const std::string& path, const BaseReq& req,
                                 BaseResp* resp, std::ostream& os,
                                 const SharedTransferHandler& handler) {
  CosResult result;
  if (!CheckConfigValidation()) {
    std::string err_msg =
        "Invalid access_key secret_key or region, please check your "
        "configuration";
    SDK_LOG_ERR("%s", err_msg.c_str());
    result.SetErrorMsg(err_msg);
    result.SetFail();
    return result;
  }

  std::map<std::string, std::string> req_headers = req.GetHeaders();
  std::map<std::string, std::string> req_params = req.GetParams();
  const std::string& tmp_token = m_config->GetTmpToken();
  if (!tmp_token.empty()) {
    req_headers["x-cos-security-token"] = tmp_token;
  }

  // 1. 获取host
  if (!CosSysConfig::IsDomainSameToHost()) {
    req_headers["Host"] = host;
  } else {
    req_headers["Host"] = GetDestDomain();
  }

  // 2. 计算签名
  std::string auth_str =
      AuthTool::Sign(GetAccessKey(), GetSecretKey(), req.GetMethod(),
                     req.GetPath(), req_headers, req_params);
  if (auth_str.empty()) {
    result.SetErrorMsg(
        "Generate auth str fail, check your access_key/secret_key.");
    return result;
  }
  req_headers["Authorization"] = auth_str;

  // 3. 发送请求
  std::map<std::string, std::string> resp_headers;
  std::string xml_err_str;  // 发送失败返回的xml写入该字符串，避免直接输出到流中

  std::string dest_url = GetRealUrl(host, path, req.IsHttps());
  std::string err_msg = "";
  uint64_t real_byte;
  int http_code = HttpSender::SendRequest(
      handler, req.GetMethod(), dest_url, req_params, req_headers, "",
      req.GetConnTimeoutInms(), req.GetRecvTimeoutInms(), &resp_headers,
      &xml_err_str, os, &err_msg, &real_byte, req.CheckMD5(), 
      req.GetVerifyCert(), req.GetCaLocation());
  if (http_code == -1) {
    result.SetErrorMsg(err_msg);
    resp->ParseFromHeaders(resp_headers);
    result.SetXCosRequestId(resp->GetXCosRequestId());
    return result;
  }

  result.SetRealByte(real_byte);
  // 4. 解析返回的xml字符串
  result.SetHttpStatus(http_code);
  if (http_code > 299 || http_code < 200) {
    // 无法解析的错误, 填充到cos_result的error_info中
    if (!result.ParseFromHttpResponse(resp_headers, xml_err_str)) {
      result.SetErrorMsg(xml_err_str);
    }
  } else {
    result.SetSucc();
    resp->ParseFromHeaders(resp_headers);
    // resp requestid to result
    result.SetXCosRequestId(resp->GetXCosRequestId());
  }

  // Check the resp content length header, when return 200, but size not match
  // case.
  if (result.IsSucc() && resp->GetContentLength() != real_byte) {
    result.SetFail();
    result.SetErrorMsg("Download failed with incomplete file");
    SDK_LOG_ERR("Response content length %" PRIu64
                "is not same to real recv byte %" PRIu64,
                resp->GetContentLength(), real_byte);
  }

  return result;
}

CosResult BaseOp::UploadAction(
    const std::string& host, const std::string& path, const BaseReq& req,
    const std::map<std::string, std::string>& additional_headers,
    const std::map<std::string, std::string>& additional_params,
    std::istream& is, BaseResp* resp, const SharedTransferHandler& handler) {
  CosResult result;
  if (!CheckConfigValidation()) {
    std::string err_msg =
        "Invalid access_key secret_key or region, please check your "
        "configuration";
    SDK_LOG_ERR("%s", err_msg.c_str());
    result.SetErrorMsg(err_msg);
    return result;
  }

  std::map<std::string, std::string> req_headers = req.GetHeaders();
  std::map<std::string, std::string> req_params = req.GetParams();
  req_headers.insert(additional_headers.begin(), additional_headers.end());
  req_params.insert(additional_params.begin(), additional_params.end());
  const std::string& tmp_token = m_config->GetTmpToken();
  if (!tmp_token.empty()) {
    req_headers["x-cos-security-token"] = tmp_token;
  }

  // 1. 获取host
  if (!CosSysConfig::IsDomainSameToHost()) {
    req_headers["Host"] = host;
  } else {
    req_headers["Host"] = GetDestDomain();
  }

  // 2. 计算签名
  std::string auth_str =
      AuthTool::Sign(GetAccessKey(), GetSecretKey(), req.GetMethod(),
                     req.GetPath(), req_headers, req_params);
  if (auth_str.empty()) {
    result.SetErrorMsg(
        "Generate auth str fail, check your access_key/secret_key.");
    return result;
  }
  req_headers["Authorization"] = auth_str;

  // 3. 发送请求
  std::map<std::string, std::string> resp_headers;
  std::string resp_body;

  std::string dest_url = GetRealUrl(host, path, req.IsHttps());
  std::string err_msg = "";
  int http_code = HttpSender::SendRequest(
      handler, req.GetMethod(), dest_url, req_params, req_headers, is,
      req.GetConnTimeoutInms(), req.GetRecvTimeoutInms(), &resp_headers,
      &resp_body, &err_msg, false, req.GetVerifyCert(), req.GetCaLocation());
  if (http_code == -1) {
    result.SetErrorMsg(err_msg);
    return result;
  }

  // 4. 解析返回的xml字符串
  result.SetHttpStatus(http_code);
  if (http_code > 299 || http_code < 200) {
    // 无法解析的错误, 填充到cos_result的error_info中
    if (!result.ParseFromHttpResponse(resp_headers, resp_body)) {
      result.SetErrorMsg(resp_body);
    }
  } else {
    result.SetSucc();
    resp->ParseFromXmlString(resp_body);
    resp->ParseFromHeaders(resp_headers);
    resp->SetBody(resp_body);
    //resp->SetHeaders(resp_headers);
    result.SetXCosRequestId(resp->GetXCosRequestId());
  }

  return result;
}

// 1. host优先级，私有ip > 自定义域名 > DNS cache > 默认域名
std::string BaseOp::GetRealUrl(const std::string& host, const std::string& path,
                               bool is_https) {
  std::string dest_uri;
  std::string dest_host = host;
  std::string dest_path = path;
  std::string dest_protocal = "http://";
  if (is_https) {
    dest_protocal = "https://";
  }

  if (dest_path.empty() || '/' != dest_path[0]) {
    dest_path = "/" + dest_path;
  }

  if (m_config->GetSetIntranetOnce() &&
      m_config->IsUseIntranet() &&
      !m_config->GetIntranetAddr().empty()) {
    dest_host = m_config->GetIntranetAddr();
  } else if (CosSysConfig::IsUseIntranet() &&
      !CosSysConfig::GetIntranetAddr().empty()) {
    dest_host = CosSysConfig::GetIntranetAddr();
  } else if (!m_config->GetDestDomain().empty()) {
    dest_host = m_config->GetDestDomain();
  } else if (!CosSysConfig::GetDestDomain().empty()) {
    dest_host = CosSysConfig::GetDestDomain();
  } else if (CosSysConfig::GetUseDnsCache()) {
    dest_host = GetGlobalDnsCacheInstance().Resolve(host);
  }

  dest_uri = dest_protocal + dest_host + CodecUtil::EncodeKey(dest_path);
  SDK_LOG_DBG("dest_uri: %s", dest_uri.c_str());
  return dest_uri;
}

bool BaseOp::CheckConfigValidation() const {
  if (GetAccessKey().empty() || GetSecretKey().empty() || GetRegion().empty()) {
    return false;
  }
  return true;
}

}  // namespace qcloud_cos
