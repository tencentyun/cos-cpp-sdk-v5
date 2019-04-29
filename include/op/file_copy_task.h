#ifndef FILE_COPY_TASK_H
#define FILE_COPY_TASK_H
#pragma once

#include <string>

#include "cos_config.h"
#include "cos_defines.h"
#include "cos_params.h"
#include "cos_sys_config.h"
#include "op/object_op.h"
#include "util/codec_util.h"
#include "util/file_util.h"
#include "util/http_sender.h"
#include "util/string_util.h"

namespace qcloud_cos {

class FileCopyTask {
public:
    FileCopyTask(const std::string& full_url,
                 uint64_t conn_timeout_in_ms,
                 uint64_t recv_timeout_in_ms);

    ~FileCopyTask() {}

    void Run();

    void CopyTask();

    bool IsTaskSuccess() const;

    int GetHttpStatus() const ;

    std::string GetTaskResp() const;

    std::map<std::string, std::string> GetRespHeaders() const;

    void SetParams(const std::map<std::string, std::string>& params);

    void SetHeaders(const std::map<std::string, std::string>& headers);

    std::string GetErrMsg() const { return m_err_msg; }

    std::string GetEtag() const { return m_etag; }

    std::string GetLastModified() const { return m_last_modified; }

private:
    std::string m_full_url;
    std::map<std::string, std::string> m_headers;
    std::map<std::string, std::string> m_params;
    uint64_t m_conn_timeout_in_ms;
    uint64_t m_recv_timeout_in_ms;
    std::string m_resp;
    bool m_is_task_success;
    int m_http_status;
    std::map<std::string, std::string> m_resp_headers;
    std::string m_err_msg;
    std::string m_etag;
    std::string m_last_modified;
};

}
#endif
