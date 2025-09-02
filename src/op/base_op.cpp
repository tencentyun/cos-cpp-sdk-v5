// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "op/base_op.h"

#include <iostream>
#include <unordered_set>
#include <regex>
#include "cos_sys_config.h"
#include "request/base_req.h"
#include "response/base_resp.h"
#include "util/auth_tool.h"
#include "util/http_sender.h"
#include "util/simple_dns_cache.h"
#include "trsf/transfer_handler.h"

namespace qcloud_cos {
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

bool BaseOp::IsDomainSameToHost() const {
  return m_config->IsDomainSameToHostEnable() ?
         m_config->IsDomainSameToHost() : CosSysConfig::IsDomainSameToHost();
}

bool BaseOp::IsDefaultHost(const std::string &host) const {
    size_t dot_pos = host.find('.');

    if (dot_pos == std::string::npos) {
      return false;
    }
    const char* str = host.substr(dot_pos + 1).c_str();
    if (str == NULL) {
        return false;
    }

    int len = strlen(str);
    int i = 0;

    // 匹配 \cos\.
    if (i >= len || strncmp(str + i, "cos.", 4) != 0) {
        return false;
    }
    i += 4;

    // 匹配 ([\w-]+)-([\w-]+)
    int flag = 0;
    while (i < len && (isalnum(str[i]) || str[i] == '-')) {
        if(str[i] == '-') flag = 1;
        i++;
    }

    if (i >= len || str[i] != '.' || !flag) {
        return false;
    }

    if (i >= len || strncmp(str + i, ".myqcloud.com", 13) != 0) {
        return false;
    }
    i += 13;

    return i == len;
}

bool BaseOp::NoNeedRetry(const CosResult &result) {
  if (result.IsSucc()) {
    // 请求成功, 不重试
    return true;
  }
  int statusCode = result.GetHttpStatus();
  return statusCode >= 400 && statusCode < 500;
}

CosResult BaseOp::NormalAction(const std::string& host, const std::string& path,
                               const BaseReq& req, const std::string& req_body,
                               bool check_body, BaseResp* resp, bool is_ci_req) {
  std::map<std::string, std::string> additional_headers;
  std::map<std::string, std::string> additional_params;
  return NormalAction(host, path, req, additional_headers, additional_params,
                      req_body, check_body, resp, is_ci_req);
}

CosResult BaseOp::NormalAction(
    const std::string& host, const std::string& path, const BaseReq& req,
    const std::map<std::string, std::string>& additional_headers,
    const std::map<std::string, std::string>& additional_params,
    const std::string& req_body, bool check_body, BaseResp* resp,
    bool is_ci_req) {
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

  std::string domain = host;
  for (uint32_t i = 0; ; i++) {
    result = NormalRequest(domain, path, req, additional_headers, additional_params, req_body, check_body, resp, i, is_ci_req);
    if (i >= m_op_util.GetMaxRetryTimes() || NoNeedRetry(result)) {
      return result;
    }
    if (m_op_util.ShouldChangeBackupDomain(result, i, is_ci_req)) {
      domain = BaseOpUtil::ChangeHostSuffix(domain);
    }
    m_op_util.SleepBeforeRetry(i);
  }
}

CosResult BaseOp::NormalRequest(const std::string& host, const std::string& path, const BaseReq& req,
    const std::map<std::string, std::string>& additional_headers,
    const std::map<std::string, std::string>& additional_params,
    const std::string& req_body, bool check_body, BaseResp* resp,
    const uint32_t &request_retry_num, bool is_ci_req) {
  CosResult result;
  std::map<std::string, std::string> req_headers = req.GetHeaders();
  std::map<std::string, std::string> req_params = req.GetParams();
  req_headers.insert(additional_headers.begin(), additional_headers.end());
  req_params.insert(additional_params.begin(), additional_params.end());
  const std::string& tmp_token = m_config->GetTmpToken();
  if (!tmp_token.empty()) {
    if (is_ci_req) {
      req_headers["x-ci-security-token"] = tmp_token;
    } else {
      req_headers["x-cos-security-token"] = tmp_token;
    }
  }

  // 1. 获取host
  if (!IsDomainSameToHost()) {
    req_headers["Host"] = host;
  } else {
    req_headers["Host"] = GetDestDomain();
  }

  if (request_retry_num > 0) {
    req_headers[kReqHeaderXCosSdkRetry] = "true";
  }

  std::unordered_set<std::string> not_sign_headers;
  if (!req.SignHeaderHost()) {
    not_sign_headers.insert("Host");
  }
  req_headers[kHttpHeaderContentLength] = std::to_string(req_body.length());

  // 2. 计算签名
  std::string auth_str = AuthTool::Sign(GetAccessKey(), GetSecretKey(), req.GetMethod(),
                     req.GetPath(), req_headers, req_params, not_sign_headers);
  if (auth_str.empty()) {
    result.SetErrorMsg("Generate auth str fail, check your access_key/secret_key.");
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
      &resp_body, &err_msg, false, req.GetVerifyCert(), req.GetCaLocation(),
      req.GetSSLCtxCallback(), req.GetSSLCtxCbData());
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
    return result;
  }
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

  std::string domain = host;
  for (uint32_t i = 0; ; i++) {
    result = DownloadRequest(domain, path, req, resp, os, i, handler);
    if (i >= m_op_util.GetMaxRetryTimes() || NoNeedRetry(result)) {
      return result;
    }
    os.rdbuf()->pubseekpos(0, std::ios_base::out);
    os.clear();
    if (m_op_util.ShouldChangeBackupDomain(result, i)) {
      domain = BaseOpUtil::ChangeHostSuffix(domain);
    }
    m_op_util.SleepBeforeRetry(i);
  }
}

CosResult BaseOp::DownloadRequest(const std::string &host, const std::string &path, const BaseReq &req,
    BaseResp *resp, std::ostream &os, const uint32_t &request_retry_num, const SharedTransferHandler &handler) {
  CosResult result;
  std::map<std::string, std::string> req_headers = req.GetHeaders();
  std::map<std::string, std::string> req_params = req.GetParams();
  const std::string& tmp_token = m_config->GetTmpToken();
  if (!tmp_token.empty()) {
    req_headers["x-cos-security-token"] = tmp_token;
  }

  // 1. 获取host
  if (!IsDomainSameToHost()) {
    req_headers["Host"] = host;
  } else {
    req_headers["Host"] = GetDestDomain();
  }

  if (request_retry_num > 0) {
    req_headers[kReqHeaderXCosSdkRetry] = "true";
  }

  std::unordered_set<std::string> not_sign_headers;
  if (!req.SignHeaderHost()) {
    not_sign_headers.insert("Host");
  }
  // 2. 计算签名
  std::string auth_str =
      AuthTool::Sign(GetAccessKey(), GetSecretKey(), req.GetMethod(),
                     req.GetPath(), req_headers, req_params, not_sign_headers);
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
      req.GetVerifyCert(), req.GetCaLocation(),
      req.GetSSLCtxCallback(), req.GetSSLCtxCbData());
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

  std::string domain = host;
  for (uint32_t i = 0; ; i++) {
    std::streampos initial_pos = is.tellg();
    result = UploadRequest(domain, path, req, additional_headers, additional_params, is, resp, i, handler);
    if (i >= m_op_util.GetMaxRetryTimes() || NoNeedRetry(result) ) {
      return result;
    }
    if (m_op_util.ShouldChangeBackupDomain(result, i)) {
      domain = BaseOpUtil::ChangeHostSuffix(domain);
    }
    is.clear();
    is.seekg(initial_pos);
    m_op_util.SleepBeforeRetry(i);
  }
}

CosResult BaseOp::UploadRequest(
    const std::string &host, const std::string &path, const BaseReq &req,
    const std::map<std::string, std::string> &additional_headers,
    const std::map<std::string, std::string> &additional_params,
    std::istream &is, BaseResp *resp, const uint32_t &request_retry_num, const SharedTransferHandler &handler) {
  CosResult result;
  std::map<std::string, std::string> req_headers = req.GetHeaders();
  std::map<std::string, std::string> req_params = req.GetParams();
  req_headers.insert(additional_headers.begin(), additional_headers.end());
  req_params.insert(additional_params.begin(), additional_params.end());
  const std::string &tmp_token = m_config->GetTmpToken();
  if (!tmp_token.empty()) {
    req_headers["x-cos-security-token"] = tmp_token;
  }

  // 1. 获取host
  if (!IsDomainSameToHost()) {
    req_headers["Host"] = host;
  } else {
    req_headers["Host"] = GetDestDomain();
  }

  if (request_retry_num > 0) {
    req_headers[kReqHeaderXCosSdkRetry] = "true";
  }

  std::unordered_set<std::string> not_sign_headers;
  if (!req.SignHeaderHost()) {
    not_sign_headers.insert("Host");
  }
  req_headers[kHttpHeaderContentLength] = std::to_string(StringUtil::GetLengthFromIStream(is));
  // 2. 计算签名
  std::string auth_str =
      AuthTool::Sign(GetAccessKey(), GetSecretKey(), req.GetMethod(),
                     req.GetPath(), req_headers, req_params, not_sign_headers);
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
      &resp_body, &err_msg, false, req.GetVerifyCert(), req.GetCaLocation(),
      req.GetSSLCtxCallback(), req.GetSSLCtxCbData());
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
                               bool is_https, bool is_generate_presigned_url) {
  return m_op_util.GetRealUrl(host, path, is_https, is_generate_presigned_url);
}

bool BaseOp::CheckConfigValidation() const {
  if (GetAccessKey().empty() || GetSecretKey().empty() || GetRegion().empty()) {
    return false;
  }
  return true;
}

}  // namespace qcloud_cos
