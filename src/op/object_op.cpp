// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "op/object_op.h"

#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "threadpool/boost/threadpool.hpp"
#include <boost/bind.hpp>

#include "cos_sys_config.h"
#include "op/file_download_task.h"
#include "op/file_upload_task.h"
#include "util/auth_tool.h"
#include "util/file_util.h"
#include "util/http_sender.h"
#include "util/string_util.h"

namespace qcloud_cos {

CosResult ObjectOp::HeadObject(const HeadObjectReq& req, HeadObjectResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", resp);
}

CosResult ObjectOp::GetObject(const GetObjectByStreamReq& req,
                              GetObjectByStreamResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    std::ostream& os = req.GetStream();
    return DownloadAction(host, path, req, resp, os);
}

CosResult ObjectOp::GetObject(const GetObjectByFileReq& req,
                              GetObjectByFileResp* resp) {
    CosResult result;
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    std::ofstream ofs(req.GetLocalFilePath().c_str(),
                     std::ios::out | std::ios::binary | std::ios::trunc);
    if (!ofs.is_open()) {
        result.SetErrorInfo("Open local file fail, local file=" + req.GetLocalFilePath());
        return result;
    }
    result = DownloadAction(host, path, req, resp, ofs);
    ofs.close();
    return result;
}

CosResult ObjectOp::GetObject(const MultiGetObjectReq& req, MultiGetObjectResp* resp) {
    return MultiThreadDownload(req, resp);
}

CosResult ObjectOp::PutObject(const PutObjectByStreamReq& req, PutObjectByStreamResp* resp) {
    CosResult result;
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;

    std::istream& is = req.GetStream();
    result = UploadAction(host, path, req, additional_headers,
                          additional_params, is, resp);
    return result;
}

CosResult ObjectOp::PutObject(const PutObjectByFileReq& req, PutObjectByFileResp* resp) {
    CosResult result;
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;

    std::ifstream ifs(req.GetLocalFilePath().c_str(), std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        result.SetErrorInfo("Open local file fail, local file=" + req.GetLocalFilePath());
        return result;
    }
    result = UploadAction(host, path, req, additional_headers,
                          additional_params, ifs, resp);
    ifs.close();
    return result;
}

CosResult ObjectOp::MultiUploadObject(const MultiUploadObjectReq& req,
                                      MultiUploadObjectResp* resp) {
    CosResult result;
    uint64_t app_id = GetAppId();
    std::string bucket_name = req.GetBucketName();
    std::string object_name = req.GetObjectName();
    std::string local_file_path = req.GetLocalFilePath();

    std::ifstream fin(local_file_path.c_str() , std::ios::in);
    if (!fin) {
        result.SetErrorInfo("Open local file fail, local file=" + local_file_path);
        return result;
    }

    // 1. Init
    InitMultiUploadReq init_req(bucket_name, object_name);
    InitMultiUploadResp init_resp;
    result = InitMultiUpload(init_req, &init_resp);
    if (!result.IsSucc()) {
        SDK_LOG_ERR("Multi upload object fail, check init mutli result.");
        resp->CopyFrom(init_resp);
        return result;
    }
    std::string upload_id = init_resp.GetUploadId();
    if (upload_id.empty()) {
        SDK_LOG_ERR("Multi upload object fail, upload id is empty.");
        resp->CopyFrom(init_resp);
        return result;
    }

    // 2. Multi Upload
    std::vector<std::string> etags;
    std::vector<uint64_t> part_numbers;
    // TODO(返回值判断)
    result = MultiThreadUpload(req, upload_id, &etags, &part_numbers);
    if (!result.IsSucc()) {
        SDK_LOG_ERR("Multi upload object fail, check upload mutli result.");
        return result;
    }

    // 3. Complete
    CompleteMultiUploadReq comp_req(bucket_name, object_name, upload_id);
    CompleteMultiUploadResp comp_resp;
    comp_req.SetEtags(etags);
    comp_req.SetPartNumbers(part_numbers);

    result = CompleteMultiUpload(comp_req, &comp_resp);
    resp->CopyFrom(comp_resp);

    return result;
}

CosResult ObjectOp::InitMultiUpload(const InitMultiUploadReq& req, InitMultiUploadResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_params.insert(std::make_pair("uploads", ""));
    return NormalAction(host, path, req, additional_headers,
                        additional_params, "", resp);
}

CosResult ObjectOp::UploadPartData(const UploadPartDataReq& req, UploadPartDataResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_params.insert(std::make_pair("uploadId", req.GetUploadId()));
    additional_params.insert(std::make_pair("partNumber",
                             StringUtil::Uint64ToString(req.GetPartNumber())));

    if (req.GetStream().peek() == EOF) {
        CosResult result;
        result.SetErrorInfo("Input Stream is empty.");
        return result;
    }

    return UploadAction(host, path, req, additional_headers,
                        additional_params, req.GetStream(), resp);
}

CosResult ObjectOp::CompleteMultiUpload(const CompleteMultiUploadReq& req,
                                        CompleteMultiUploadResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    std::string req_body;
    if (!req.GenerateRequestBody(&req_body)) {
        CosResult result;
        result.SetErrorInfo("GenerateCompleteMultiUploadReqBody fail, "
                            "check your part_numbers and etags.");
        return result;
    }

    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_params.insert(std::make_pair("uploadId", req.GetUploadId()));

    return NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, resp);
}

CosResult ObjectOp::AbortMultiUpload(const AbortMultiUploadReq& req, AbortMultiUploadResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_params.insert(std::make_pair("uploadId", req.GetUploadId()));
    return NormalAction(host, path, req, additional_headers,
                        additional_params, "", resp);
}

// TODO(sevenyou) 多线程下载, 返回的resp内容需要再斟酌下. 另外函数体太长了
CosResult ObjectOp::MultiThreadDownload(const MultiGetObjectReq& req, MultiGetObjectResp* resp) {
    CosResult result;
    // 1. 调用HeadObject获取文件长度
    HeadObjectReq head_req(req.GetBucketName(), req.GetObjectName());;
    HeadObjectResp head_resp;
    result = HeadObject(head_req, &head_resp);
    // TODO(sevenyou): 下载请求返回head失败的信息, 略奇怪, 后面考虑优化下
    if (!result.IsSucc()) {
        SDK_LOG_ERR("Get object length before download object fail.");
        return result;
    }

    // 2. 填充header
    std::map<std::string, std::string> headers = req.GetHeaders();
    std::map<std::string, std::string> params = req.GetParams();
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());
    std::string path = req.GetPath();
    headers["Host"] = host;
    std::string auth_str = AuthTool::Sign(GetAccessKey(), GetSecretKey(),
                                          req.GetMethod(), path, headers, params);
    if (auth_str.empty()) {
        result.SetErrorInfo("Generate auth str fail, check your access_key/secret_key.");
        return result;
    }
    headers["Authorization"] = auth_str;

    uint64_t file_size = head_resp.GetContentLength();

    // 3. 打开本地文件
    std::string local_path = req.GetLocalFilePath();
    int fd = open(local_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (-1 == fd) {
        std::string err_info = "open file(" + local_path + ") fail, errno="
            + StringUtil::IntToString(errno);
        SDK_LOG_ERR("%s", err_info.c_str());
        result.SetErrorInfo(err_info);
        return result;
    }

    // 4. 多线程下载
    unsigned pool_size = req.GetThreadPoolSize();
    unsigned slice_size = req.GetSliceSize();
    unsigned max_task_num = file_size / slice_size + 1;
    if (max_task_num < pool_size) {
        pool_size = max_task_num;
    }

    unsigned char** file_content_buf = new unsigned char* [pool_size];
    for(unsigned i = 0; i < pool_size; ++i) {
        file_content_buf[i] = new unsigned char[slice_size];
    }

    std::string dest_url = GetRealUrl(host, path);
    FileDownTask** pptaskArr = new FileDownTask*[pool_size];
    for (unsigned i = 0; i < pool_size; ++i) {
        pptaskArr[i] = new FileDownTask(dest_url, headers, params,
                                req.GetConnTimeoutInms(), req.GetRecvTimeoutInms());
    }

    SDK_LOG_DBG("download data,url=%s, poolsize=%u,slice_size=%u,file_size=%lu",
                dest_url.c_str(), pool_size, slice_size, file_size);

    std::vector<uint64_t> vec_offset;
    vec_offset.resize(pool_size);
    boost::threadpool::pool tp(pool_size);
    uint64_t offset =0;
    bool task_fail_flag = false;
    unsigned down_times = 0;
    bool is_header_set = false;
    while(offset < file_size) {
        SDK_LOG_DBG("down data, offset=%lu, file_size=%lu", offset, file_size);
        unsigned task_index = 0;
        vec_offset.clear();
        for (; task_index < pool_size && (offset < file_size); ++task_index) {
            SDK_LOG_DBG("down data, task_index=%d, file_size=%lu, offset=%lu",
                        task_index, file_size, offset);
            FileDownTask* ptask = pptaskArr[task_index];

            ptask->SetDownParams(file_content_buf[task_index], slice_size, offset);
            tp.schedule(boost::bind(&FileDownTask::Run, ptask));
            vec_offset[task_index] = offset;
            offset += slice_size;
            ++down_times;
        }

        unsigned task_num = task_index;

        tp.wait();

        for (task_index = 0; task_index < task_num; ++task_index) {
            FileDownTask *ptask = pptaskArr[task_index];
            if (!ptask->IsTaskSuccess()) {
                const std::string& task_resp = ptask->GetTaskResp();
                const std::map<std::string, std::string>& task_resp_headers
                    = ptask->GetRespHeaders();
                SDK_LOG_ERR("down data, down task fail, rsp:%s", task_resp.c_str());
                result.SetHttpStatus(ptask->GetHttpStatus());
                if (!result.ParseFromHttpResponse(task_resp_headers, task_resp)) {
                    result.SetErrorInfo(task_resp);
                }
                resp->ParseFromHeaders(ptask->GetRespHeaders());

                task_fail_flag = true;
                break;
            } else {
                if (-1 == lseek(fd, vec_offset[task_index], SEEK_SET)) {
                    std::string err_info = "down data, lseek ret="
                        + StringUtil::IntToString(errno) + ", offset="
                        + StringUtil::Uint64ToString(vec_offset[task_index]);
                    SDK_LOG_ERR("%s", err_info.c_str());
                    result.SetErrorInfo(err_info);
                    task_fail_flag = true;
                    break;
                }

                if (-1 == write(fd, file_content_buf[task_index], ptask->GetDownLoadLen())) {
                    std::string err_info = "down data, write ret="
                        + StringUtil::IntToString(errno) + ", len="
                        + StringUtil::Uint64ToString(ptask->GetDownLoadLen());
                    SDK_LOG_ERR("%s", err_info.c_str());
                    result.SetErrorInfo(err_info);
                    task_fail_flag = true;
                    break;
                }

                if (!is_header_set) {
                    resp->ParseFromHeaders(ptask->GetRespHeaders());
                    is_header_set = true;
                }
                SDK_LOG_DBG("down data, down_times=%u,task_index=%d, file_size=%lu, "
                            "offset=%lu, downlen:%lu ",
                            down_times,task_index, file_size,
                            vec_offset[task_index], ptask->GetDownLoadLen());
            }
        }

        if (task_fail_flag) {
            break;
        }
    }

    if (!task_fail_flag) {
        result.SetSucc();
        // 下载成功则用head得到的content_length和etag设置get response
        resp->SetContentLength(file_size);
        resp->SetEtag(head_resp.GetEtag());
    }

    // 4. 释放所有资源
    close(fd);
    for(unsigned i = 0; i < pool_size; i++){
        delete [] file_content_buf[i];
        delete pptaskArr[i];
    }
    delete [] pptaskArr;
    delete [] file_content_buf;

    uint64_t file_len = offset > file_size ? file_size : offset;

    return result;
}

// TODO(sevenyou) 多线程上传, 返回的resp内容需要再斟酌下.
CosResult ObjectOp::MultiThreadUpload(const MultiUploadObjectReq& req,
                                      const std::string& upload_id,
                                      std::vector<std::string>* etags_ptr,
                                      std::vector<uint64_t>* part_numbers_ptr) {
    CosResult result;
    std::string path = "/" + req.GetObjectName();
    std::string host = CosSysConfig::GetHost(GetAppId(), req.GetBucketName());

    // 1. 获取文件大小
    std::string local_file_path = req.GetLocalFilePath();
    std::ifstream fin(local_file_path.c_str(), std::ios::in | std::ios::binary);
    if (!fin.is_open()){
        SDK_LOG_ERR("FileUploadSliceData: file open fail, %s", local_file_path.c_str());
        result.SetErrorInfo("local file not exist, local_file=" + local_file_path);
        return result;
    }
    uint64_t file_size = FileUtil::GetFileLen(local_file_path);

    // 2. 初始化upload task
    uint64_t offset = 0;
    bool task_fail_flag = false;

    uint64_t part_size = req.GetPartSize();
    int pool_size = req.GetThreadPoolSize();
    unsigned char** file_content_buf = new unsigned char*[pool_size];
    for(int i = 0; i < pool_size; ++i) {
        file_content_buf[i] = new unsigned char[part_size];
    }

    std::string dest_url = GetRealUrl(host, path);
    FileUploadTask** pptaskArr = new FileUploadTask*[pool_size];
    for (int i = 0; i < pool_size; ++i) {
        pptaskArr[i] = new FileUploadTask(dest_url, req.GetConnTimeoutInms(), req.GetRecvTimeoutInms());
    }

    SDK_LOG_DBG("upload data,url=%s, poolsize=%u, part_size=%u, file_size=%lu",
                dest_url.c_str(), pool_size, part_size, file_size);

    boost::threadpool::pool tp(pool_size);

    // 3. 多线程upload
    {
        uint64_t part_number = 1;
        while (offset < file_size) {
            int task_index = 0;
            for (; task_index < pool_size; ++task_index) {
                fin.read((char *)file_content_buf[task_index], part_size);
                size_t read_len = fin.gcount();
                if (read_len == 0 && fin.eof()) {
                    SDK_LOG_DBG("read over, task_index: %d", task_index);
                    break;
                }

                SDK_LOG_DBG("upload data, task_index=%d, file_size=%lu, offset=%lu, len=%lu",
                            task_index, file_size, offset, read_len);

                FileUploadTask* ptask = pptaskArr[task_index];
                FillUploadTask(upload_id, host, path, file_content_buf[task_index], read_len,
                               part_number, ptask);
                tp.schedule(boost::bind(&FileUploadTask::Run, ptask));
                offset += read_len;
                part_numbers_ptr->push_back(part_number);
                ++part_number;
            }

            int max_task_num = task_index;

            tp.wait();
            for (task_index = 0; task_index < max_task_num; ++task_index) {
                FileUploadTask* ptask = pptaskArr[task_index];
                if (!ptask->IsTaskSuccess()) {
                    const std::string& task_resp = ptask->GetTaskResp();
                    const std::map<std::string, std::string>& task_resp_headers = ptask->GetRespHeaders();
                    SDK_LOG_ERR("upload data, upload task fail, rsp:%s", task_resp.c_str());
                    result.SetHttpStatus(ptask->GetHttpStatus());
                    if (!result.ParseFromHttpResponse(task_resp_headers, task_resp)) {
                        result.SetErrorInfo(task_resp);
                    }

                    task_fail_flag = true;
                    break;
                }

                // 找不到etag也算失败
                const std::map<std::string, std::string>& resp_header = ptask->GetRespHeaders();
                std::map<std::string, std::string>::const_iterator itr = resp_header.find("ETag");
                if (itr != resp_header.end()) {
                    etags_ptr->push_back(itr->second);
                } else {
                    std::string err_info = "upload data, upload task succ, "
                        "but response header missing etag field.";
                    SDK_LOG_ERR("%s", err_info.c_str());
                    result.SetHttpStatus(ptask->GetHttpStatus());
                    task_fail_flag = true;
                    break;
                }
            }

            if (task_fail_flag) {
                break;
            }
        }
    }

    if (!task_fail_flag) {
        result.SetSucc();
    }

    // 释放相关资源
    fin.close();
    for (int i = 0; i< pool_size; ++i) {
        delete pptaskArr[i];
    }
    delete [] pptaskArr;

    for (int i = 0; i < pool_size; ++i) {
        delete [] file_content_buf[i];
    }
    delete [] file_content_buf;

    return result;
}

uint64_t ObjectOp::GetContent(const std::string& src, std::string* file_content) const {
    //读取文件内容
    const unsigned char * pbuf = NULL;
    *file_content = FileUtil::GetFileContent(src);
    pbuf = (unsigned char *)file_content->c_str();
    uint64_t len = file_content->length();
    return len;
}

void ObjectOp::FillUploadTask(const std::string& upload_id, const std::string& host,
                              const std::string& path, unsigned char* file_content_buf,
                              uint64_t len, uint64_t part_number,
                              FileUploadTask* task_ptr) {
    std::map<std::string, std::string> req_params;
    req_params.insert(std::make_pair("uploadId", upload_id));
    req_params.insert(std::make_pair("partNumber",
                                     StringUtil::Uint64ToString(part_number)));
    std::map<std::string, std::string> req_headers;
    req_headers["Host"] = host;
    std::string auth_str = AuthTool::Sign(GetAccessKey(), GetSecretKey(), "PUT",
                                          path, req_headers, req_params);
    req_headers["Authorization"] = auth_str;

    task_ptr->SetParams(req_params);
    task_ptr->SetHeaders(req_headers);
    task_ptr->SetUploadBuf(file_content_buf, len);
}

} // namespace qcloud_cos
