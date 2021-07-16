#pragma once

#include <string>
#include <map>
#include "request/object_req.h"
#include "trsf/transfer_handler.h"

#include "Poco/Foundation.h"
#include "Poco/Runnable.h"

namespace qcloud_cos{

class FileUploadTask : public Poco::Runnable {
    public:
        FileUploadTask(const std::string& full_url,
                       uint64_t conn_timeout_in_ms,
                       uint64_t recv_timeout_in_ms,
                       unsigned char* pbuf = NULL,
                       const size_t data_len = 0);

        FileUploadTask(const std::string& full_url,
                       uint64_t conn_timeout_in_ms,
                       uint64_t recv_timeout_in_ms,
                       const SharedTransferHandler& handler);

        FileUploadTask(const std::string& full_url,
                       const std::map<std::string, std::string>& headers,
                       const std::map<std::string, std::string>& params,
                       uint64_t conn_timeout_in_ms,
                       uint64_t recv_timeout_in_ms,
                       unsigned char* pbuf = NULL,
                       const size_t data_len = 0);

        ~FileUploadTask() {}

        void run();

        void UploadTask();

        void SetUploadBuf(unsigned char* pdatabuf, size_t data_len);

        std::string GetTaskResp() const;

        bool IsTaskSuccess() const;

        void SetTaskSuccess() { m_is_task_success = true; }

        int GetHttpStatus() const ;

        std::map<std::string, std::string> GetRespHeaders() const;

        void SetParams(const std::map<std::string, std::string>& params);

        void SetHeaders(const std::map<std::string, std::string>& headers);

        std::string GetErrMsg() const { return m_err_msg; }

        void SetResume(const bool is_resume) { m_is_resume = is_resume; }

        bool IsResume() const { return m_is_resume; }

        void SetResumeEtag(const std::string& etag) { m_resume_etag = etag; }

        std::string GetResumeEtag() const { return m_resume_etag; }

        void SetPartNumber(uint64_t part_number);

        uint64_t GetPartNumber() const { return m_part_number;}

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
        std::string m_err_msg;
        bool m_is_resume;
        std::string m_resume_etag;
        uint64_t m_part_number;
        SharedTransferHandler m_handler;
    };

}
