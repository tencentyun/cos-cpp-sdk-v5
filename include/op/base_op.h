﻿#pragma once
#ifndef COS_CPP_SDK_V5_INCLUDE_OP_BASE_OP_H_
#define COS_CPP_SDK_V5_INCLUDE_OP_BASE_OP_H_

#include <inttypes.h>
#include <stdint.h>

#include <map>
#include <string>

#include "cos_config.h"
#include "op/cos_result.h"
#include "trsf/transfer_handler.h"

namespace qcloud_cos {

class BaseReq;
class BaseResp;

class BaseOp {
 public:
  /// \brief BaseOp构造函数
  ///
  /// \param cos_conf Cos配置
  explicit BaseOp(const SharedConfig& cos_conf) : m_config(cos_conf) {}

  BaseOp() {}

  /// \brief BaseOp析构函数
  ~BaseOp() {}

  /// \brief 获取Cos配置
  CosConfig GetCosConfig() const;

  /// \brief 获取AppID
  uint64_t GetAppId() const;

  /// \brief 获取Region
  std::string GetRegion() const;

  /// \brief 获取AccessKey
  std::string GetAccessKey() const;

  /// \brief 获取SecretKey
  std::string GetSecretKey() const;

  /// \brief 获取Token
  std::string GetTmpToken() const;

  std::string GetDestDomain() const;

  bool IsDomainSameToHost() const;

  bool UseDefaultDomain() const;

  bool IsDefaultHost(const std::string &host) const;

  std::string ChangeHostSuffix(const std::string &host);

  /// \brief 封装了cos Service/Bucket/Object 相关接口的通用操作,
  ///        包括签名计算、请求发送、返回内容解析等
  ///
  /// \param host      目标主机, 以http://开头
  /// \param path      http path
  /// \param req       http请求
  /// \param req_body  http request的body
  /// \param resp      http返回
  /// \param is_ci_req 是否为万象域名请求
  ///
  /// \return http调用情况(状态码等)
  CosResult NormalAction(const std::string& host, const std::string& path,
                         const BaseReq& req, const std::string& req_body,
                         bool check_body, BaseResp* resp, bool is_ci_req = false);

  /// \brief 封装了cos Service/Bucket/Object相关接口的通用操作,
  ///        包括签名计算、请求发送、返回内容解析等
  ///
  /// \param host     目标主机, 以http://开头
  /// \param path     http path
  /// \param req      http请求
  /// \param additional_headers http请求需要所需的额外header
  /// \param additional_params  http请求需要所需的额外params
  /// \param req_body http request的body
  /// \param resp     http返回
  /// \param is_ci_req 是否为万象域名请求
  ///
  /// \return http调用情况(状态码等)
  CosResult NormalAction(
      const std::string& host, const std::string& path, const BaseReq& req,
      const std::map<std::string, std::string>& additional_headers,
      const std::map<std::string, std::string>& additional_params,
      const std::string& req_body, bool check_body, BaseResp* resp,
      bool is_ci_req = false);

  /// \brief 下载文件并输出到流中
  ///
  /// \param host     目标主机, 以http://开头
  /// \param path     http path
  /// \param req      http请求
  /// \param resp     http返回
  /// \param os       输出流
  ///
  /// \return http调用情况(状态码等)
  CosResult DownloadAction(const std::string& host, const std::string& path,
                           const BaseReq& req, BaseResp* resp, std::ostream& os,
                           const SharedTransferHandler& handler = nullptr);

  /// \brief 支持从stream中读入数据并上传
  ///
  /// \param host     目标主机, 以http://开头
  /// \param path     http path
  /// \param req      http请求
  /// \param additional_headers http请求需要所需的额外header
  /// \param additional_params  http请求需要所需的额外params
  /// \param is       http request的body
  /// \param resp     http返回
  ///
  /// \return http调用情况(状态码等)
  CosResult UploadAction(
      const std::string& host, const std::string& path, const BaseReq& req,
      const std::map<std::string, std::string>& additional_headers,
      const std::map<std::string, std::string>& additional_params,
      std::istream& is, BaseResp* resp, const SharedTransferHandler& handler = nullptr);

  std::string GetRealUrl(const std::string& host, const std::string& path,
                         bool is_https, bool is_generate_presigned_url = false);

 protected:
  bool CheckConfigValidation() const;
  SharedConfig m_config;
};

}  // namespace qcloud_cos

#endif //  COS_CPP_SDK_V5_INCLUDE_OP_BASE_OP_H_
