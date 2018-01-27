// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:

#include "gtest/gtest.h"

#include "response/object_resp.h"

namespace qcloud_cos {

TEST(ObjectRespTest, NormalTest) {
    {
        GetObjectResp resp;
        std::map<std::string, std::string> input_headers;
        input_headers[kReqHeaderContentLen] = "123";
        kReqHeaderContentType
        kReqHeaderXCosTraceId
        kReqHeaderXCosReqId
        kReqHeaderServer
        kReqHeaderDate
        kReqHeaderConnection
        kReqHeaderEtag
        resp.ParseFromHeaders(input_headers);
    }

}

} // namespace qcloud_cos
