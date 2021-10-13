// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 11/14/17
// Description:

#ifndef SERVICEOP_H
#define SERVICEOP_H
#pragma once

#include "cos_sys_config.h"
#include "op/base_op.h"
#include "op/cos_result.h"
#include "request/service_req.h"
#include "response/service_resp.h"

namespace qcloud_cos {

/// \brief 封装了Service相关的操作
class ServiceOp : public BaseOp {
 public:
  /// \brief ServiceOp构造函数
  ///
  /// \param cos_conf Cos配置
  explicit ServiceOp(const SharedConfig& config) : BaseOp(config) {}

  /// \brief ServiceOp析构函数
  virtual ~ServiceOp() {}

  /// \brief 获取请求者名下的所有存储空间列表Bucket list
  ///        (详见:https://cloud.tencent.com/document/api/436/8291)
  ///
  /// \param req  GetService请求
  /// \param resp GetService返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetService(const GetServiceReq& req, GetServiceResp* resp);
};

}  // namespace qcloud_cos
#endif  // SERVICEOP_H
