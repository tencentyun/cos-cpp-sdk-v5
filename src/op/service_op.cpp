// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 11/14/17
// Description:

#include "op/service_op.h"
#include "util/codec_util.h"

namespace qcloud_cos {

CosResult ServiceOp::GetService(const GetServiceReq& req, GetServiceResp* resp) {
    // Get Service的Host是固定的
    std::string host = "service.cos.myqcloud.com";
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

} // qcloud_cos
