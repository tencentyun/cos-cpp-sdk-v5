// Copyright (c) 2017, Tencent Inc.
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
#include "util/http_sender.h"
#include "util/codec_util.h"

namespace qcloud_cos{

CosConfig BaseOp::GetCosConfig() const {
    return *m_config;
}

uint64_t BaseOp::GetAppId() const {
    return m_config->GetAppId();
}

std::string BaseOp::GetAccessKey() const {
    return m_config->GetAccessKey();
}

std::string BaseOp::GetSecretKey() const {
    return m_config->GetSecretKey();
}

std::string BaseOp::GetTmpToken() const {
    return m_config->GetTmpToken();
}

CosResult BaseOp::NormalAction(const std::string& host,
                               const std::string& path,
                               const BaseReq& req,
                               const std::string& req_body,
                               bool check_body,
                               BaseResp* resp) {
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    return NormalAction(host, path, req, additional_headers, additional_params,
                        req_body, check_body, resp);
}

CosResult BaseOp::NormalAction(const std::string& host,
                               const std::string& path,
                               const BaseReq& req,
                               const std::map<std::string, std::string>& additional_headers,
                               const std::map<std::string, std::string>& additional_params,
                               const std::string& req_body,
                               bool check_body,
                               BaseResp* resp) {
    CosResult result;
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
        req_headers["Host"] = CosSysConfig::GetDestDomain();
    }

    // 2. 计算签名
    std::string auth_str = AuthTool::Sign(GetAccessKey(), GetSecretKey(),
                                          req.GetMethod(), req.GetPath(),
                                          req_headers, req_params);
    if (auth_str.empty()) {
        result.SetErrorInfo("Generate auth str fail, check your access_key/secret_key.");
        return result;
    }
    req_headers["Authorization"] = auth_str;

    // 3. 发送请求
    std::map<std::string, std::string> resp_headers;
    std::string resp_body;

    std::string dest_url = GetRealUrl(host, path, req.IsHttps());
    std::string err_msg = "";
    int http_code = HttpSender::SendRequest(req.GetMethod(), dest_url, req_params, req_headers,
                                    req_body, req.GetConnTimeoutInms(), req.GetRecvTimeoutInms(),
                                    &resp_headers, &resp_body, &err_msg);
    if (http_code == -1) {
        result.SetErrorInfo(err_msg);
        return result;
    }

    // 4. 解析返回的xml字符串
    result.SetHttpStatus(http_code);
    if (http_code > 299 || http_code < 200) {
        // 无法解析的错误, 填充到cos_result的error_info中
        if (!result.ParseFromHttpResponse(resp_headers, resp_body)) {
            result.SetErrorInfo(resp_body);
        }
    } else {
        // 某些请求，如PutObjectCopy/Complete请求需要进一步检查Body
        if (check_body && result.ParseFromHttpResponse(resp_headers, resp_body)) {
            result.SetErrorInfo(resp_body);
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
                                 const std::string& path,
                                 const BaseReq& req,
                                 BaseResp* resp,
                                 std::ostream& os) {
    CosResult result;
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
        req_headers["Host"] = CosSysConfig::GetDestDomain();
    }

    // 2. 计算签名
    std::string auth_str = AuthTool::Sign(GetAccessKey(), GetSecretKey(),
                                          req.GetMethod(), req.GetPath(),
                                          req_headers, req_params);
    if (auth_str.empty()) {
        result.SetErrorInfo("Generate auth str fail, check your access_key/secret_key.");
        return result;
    }
    req_headers["Authorization"] = auth_str;

    // 3. 发送请求
    std::map<std::string, std::string> resp_headers;
    std::string xml_err_str; // 发送失败返回的xml写入该字符串，避免直接输出到流中

    std::string dest_url = GetRealUrl(host, path, req.IsHttps());
    std::string err_msg = "";
    uint64_t real_byte;
    int http_code = HttpSender::SendRequest(req.GetMethod(), dest_url, req_params, req_headers,
                                            "", req.GetConnTimeoutInms(), req.GetRecvTimeoutInms(),
                                            &resp_headers, &xml_err_str, os, &err_msg,
                                            &real_byte, req.CheckMD5());
    if (http_code == -1) {
        result.SetErrorInfo(err_msg);
        return result;
    }

    result.SetRealByte(real_byte);
    // 4. 解析返回的xml字符串
    result.SetHttpStatus(http_code);
    if (http_code > 299 || http_code < 200) {
        // 无法解析的错误, 填充到cos_result的error_info中
        if (!result.ParseFromHttpResponse(resp_headers, xml_err_str)) {
            result.SetErrorInfo(xml_err_str);
        }
    } else {
        result.SetSucc();
        resp->ParseFromHeaders(resp_headers);
        // resp requestid to result
        result.SetXCosRequestId(resp->GetXCosRequestId());
    }

    // Check the resp content length header, when return 200, but size not match case.
    if (result.IsSucc() && resp->GetContentLength() != real_byte) {
        result.SetFail();
        result.SetErrorInfo("Download failed with incomplete file");
        SDK_LOG_ERR("Response content length [%ld] is not same to real recv byte [%ld]",
                    resp->GetContentLength(), real_byte);
    }

    return result;
}

// TODO(sevenyou) 冗余代码
CosResult BaseOp::UploadAction(const std::string& host,
                               const std::string& path,
                               const BaseReq& req,
                               const std::map<std::string, std::string>& additional_headers,
                               const std::map<std::string, std::string>& additional_params,
                               std::istream& is,
                               BaseResp* resp) {
    CosResult result;
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
        req_headers["Host"] = CosSysConfig::GetDestDomain();
    }

    // 2. 计算签名
    std::string auth_str = AuthTool::Sign(GetAccessKey(), GetSecretKey(),
                                          req.GetMethod(), req.GetPath(),
                                          req_headers, req_params);
    if (auth_str.empty()) {
        result.SetErrorInfo("Generate auth str fail, check your access_key/secret_key.");
        return result;
    }
    req_headers["Authorization"] = auth_str;

    // 3. 发送请求
    std::map<std::string, std::string> resp_headers;
    std::string resp_body;

    std::string dest_url = GetRealUrl(host, path, req.IsHttps());
    std::string err_msg = "";
    int http_code = HttpSender::SendRequest(req.GetMethod(), dest_url, req_params, req_headers,
                                            is, req.GetConnTimeoutInms(), req.GetRecvTimeoutInms(),
                                            &resp_headers, &resp_body, &err_msg);
    if (http_code == -1) {
        result.SetErrorInfo(err_msg);
        return result;
    }

    // 4. 解析返回的xml字符串
    result.SetHttpStatus(http_code);
    if (http_code > 299 || http_code < 200) {
        // 无法解析的错误, 填充到cos_result的error_info中
        if (!result.ParseFromHttpResponse(resp_headers, resp_body)) {
            result.SetErrorInfo(resp_body);
        }
    } else {
        result.SetSucc();
        resp->ParseFromXmlString(resp_body);
        resp->ParseFromHeaders(resp_headers);
        resp->SetBody(resp_body);
        // resp requestid to result
        result.SetXCosRequestId(resp->GetXCosRequestId());
    }

    return result;
}

// 如果设置了目的url, 那么就用设置的, 否则使用appid和bucket拼接的泛域名
std::string BaseOp::GetRealUrl(const std::string& host,
                               const std::string& path,
                               bool is_https) {
    std::string protocal = "http://";
    if (is_https) {
        protocal = "https://";
    }

    std::string temp = path;
    if (temp.empty() || '/' != temp[0]) {
        temp = "/" + temp;
    }

    if(CosSysConfig::IsUseIntranet() && !CosSysConfig::GetIntranetAddr().empty()) {
        return protocal + CosSysConfig::GetIntranetAddr() + CodecUtil::EncodeKey(temp);
    }

    if (!CosSysConfig::GetDestDomain().empty()) {
        return protocal + CosSysConfig::GetDestDomain() + CodecUtil::EncodeKey(temp);
    }

    return protocal + host + CodecUtil::EncodeKey(temp);
}

} // namespace qcloud_cos
