#ifndef FILE_UPLOAD_TASK_H
#define FILE_UPLOAD_TASK_H
#pragma once

#include <pthread.h>

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

namespace qcloud_cos{

class FileUploadTask {
public:
    FileUploadTask(const std::string& full_url,
                   uint64_t conn_timeout_in_ms,
                   uint64_t recv_timeout_in_ms,
                   unsigned char* pbuf = NULL,
                   const size_t data_len = 0);

    FileUploadTask(const std::string& full_url,
                   const std::map<std::string, std::string>& headers,
                   const std::map<std::string, std::string>& params,
                   uint64_t conn_timeout_in_ms,
                   uint64_t recv_timeout_in_ms,
                   unsigned char* pbuf = NULL,
                   const size_t data_len = 0);

    ~FileUploadTask() {}

    void Run();

    void UploadTask();

    void SetUploadBuf(unsigned char* pdatabuf, size_t data_len);

    std::string GetTaskResp() const;

    bool IsTaskSuccess() const;

    int GetHttpStatus() const ;

    std::map<std::string, std::string> GetRespHeaders() const;

    void SetParams(const std::map<std::string, std::string>& params);

    void SetHeaders(const std::map<std::string, std::string>& headers);

private:
    std::string m_full_url;
    std::map<std::string, std::string> m_headers;
    std::map<std::string, std::string> m_params;
    uint64_t m_conn_timeout_in_ms;
    uint64_t m_recv_timeout_in_ms;
    unsigned char*  m_data_buf_ptr;
    size_t m_data_len;
    std::string m_resp;
    bool m_is_task_success;
    int m_http_status;
    std::map<std::string, std::string> m_resp_headers;
};

}
#endif
