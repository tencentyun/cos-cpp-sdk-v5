// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 11/14/17
// Description:

#ifndef SERVICE_REQ_H
#define SERVICE_REQ_H
#pragma once

#include "cos_defines.h"
#include "request/base_req.h"
#include "util/string_util.h"

namespace qcloud_cos {

class GetServiceReq : public BaseReq {
 public:
  GetServiceReq() {
    SetMethod("GET");
    SetPath("/");
  }

  virtual ~GetServiceReq() {}
};

}  // namespace qcloud_cos

#endif  // SERVICE_REQ_H
