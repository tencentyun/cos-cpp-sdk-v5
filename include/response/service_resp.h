// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 11/14/17
// Description:

#ifndef SERVICE_RESP_H
#define SERVICE_RESP_H
#pragma once

#include <vector>

#include "cos_config.h"
#include "cos_defines.h"
#include "response/base_resp.h"

namespace qcloud_cos {

class GetServiceResp : public BaseResp {
 public:
  GetServiceResp() {}
  virtual ~GetServiceResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  /// \brief 获取 Bucket 持有者的信息
  ///
  /// \return Owner 持有者的信息
  Owner GetOwner() const { return m_owner; }

  /// \brief 获取所有 Bucket 列表信息
  std::vector<Bucket> GetBuckets() const { return m_buckets; }

 private:
  Owner m_owner;
  std::vector<Bucket> m_buckets;
};

}  // namespace qcloud_cos
#endif  // SERVICE_RESP_H
