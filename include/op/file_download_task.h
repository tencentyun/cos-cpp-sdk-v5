// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:

#ifndef FILE_DOWN_TASK_H
#define FILE_DOWN_TASK_H
#pragma once

#include <string>

#include "cos_config.h"
#include "cos_defines.h"
#include "cos_params.h"
#include "cos_sys_config.h"
#include "op/base_op.h"
#include "util/codec_util.h"
#include "util/file_util.h"
#include "util/http_sender.h"
#include "util/string_util.h"

namespace qcloud_cos {

class FileDownTask {
public:
    FileDownTask(const std::string& full_url,
                 const std::map<std::string, std::string>& headers,
                 const std::map<std::string, std::string>& params,
                 uint64_t conn_timeout_in_ms,
                 uint64_t recv_timeout_in_ms,
                 uint64_t offset = 0,
                 unsigned char* pbuf = NULL,
                 const size_t data_len = 0);

    ~FileDownTask() {}

    void Run();

    void DownTask();

    void SetDownParams(unsigned char* pdatabuf, size_t datalen, uint64_t offset, uint64_t target_size);

    std::string GetTaskResp();

    size_t GetDownLoadLen();

    bool IsTaskSuccess();

    int GetHttpStatus();

    std::map<std::string, std::string> GetRespHeaders();

    std::string GetErrMsg() const { return m_err_msg; }

private:
    std::string m_full_url;
    std::map<std::string, std::string> m_headers;
    std::map<std::string, std::string> m_params;
    uint64_t m_conn_timeout_in_ms;
    uint64_t m_recv_timeout_in_ms;
    uint64_t m_offset;
    unsigned char* m_data_buf_ptr;
    size_t m_data_len;
    std::string m_resp;
    bool m_is_task_success;
    size_t m_real_down_len;
    int m_http_status;
    std::map<std::string, std::string> m_resp_headers;
    std::string m_err_msg;
    uint64_t m_target_size;
};

} // namespace qcloud_cos
#endif
