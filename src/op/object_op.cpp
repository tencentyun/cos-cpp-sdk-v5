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
#include <map>

#include "threadpool/boost/threadpool.hpp"
#include <boost/bind.hpp>

#include "cos_sys_config.h"
#include "op/file_copy_task.h"
#include "op/file_download_task.h"
#include "op/file_upload_task.h"
#include "util/auth_tool.h"
#include "util/file_util.h"
#include "util/http_sender.h"
#include "util/string_util.h"

#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/StreamCopier.h"

namespace qcloud_cos {

bool ObjectOp::IsObjectExist(const std::string& bucket_name, const std::string& object_name) {
    HeadObjectReq req(bucket_name, object_name);
    HeadObjectResp resp;
    CosResult result = HeadObject(req, &resp);
    if (result.IsSucc()) {
        return true;
    }

    return false;
}

CosResult ObjectOp::HeadObject(const HeadObjectReq& req, HeadObjectResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult ObjectOp::GetObject(const GetObjectByStreamReq& req,
                              GetObjectByStreamResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    std::ostream& os = req.GetStream();
    return DownloadAction(host, path, req, resp, os);
}

CosResult ObjectOp::GetObject(const GetObjectByFileReq& req,
                              GetObjectByFileResp* resp) {
    CosResult result;
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
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
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;

    std::istream& is = req.GetStream();

    // 如果传递的header中没有Content-MD5则进行SDK进行MD5校验
    bool is_check_md5 = false;
    std::string md5_str = "";
    if (req.GetHeader("Content-MD5").empty()) {
        Poco::MD5Engine md5;
        Poco::DigestOutputStream dos(md5);
        std::streampos pos = is.tellg();
        Poco::StreamCopier::copyStream(is, dos);
        is.clear();
        is.seekg(pos);
        dos.close();
        md5_str = Poco::DigestEngine::digestToHex(md5.digest());
        is_check_md5 = true;
        // 默认开启MD5校验
        if (req.ShouldComputeContentMd5()) {
            std::string bin_str = CodecUtil::HexToBin(md5_str);
            std::string encode_str = CodecUtil::Base64Encode(bin_str);
            additional_headers.insert(std::make_pair("Content-MD5",encode_str));
        }
    }

    result = UploadAction(host, path, req, additional_headers,
                          additional_params, is, resp);

    if (result.IsSucc() && is_check_md5 && md5_str != resp->GetEtag()) {
        result.SetFail();
        result.SetErrorInfo("Response etag is not correct, Please try again.");
        SDK_LOG_ERR("Response etag is not correct, Please try again. Expect md5 is%s, "
                    "but return etag is %s. RequestId=%s",
                    md5_str.c_str(), resp->GetEtag().c_str(), resp->GetXCosRequestId().c_str());
    }

    return result;
}

CosResult ObjectOp::PutObject(const PutObjectByFileReq& req, PutObjectByFileResp* resp) {
    CosResult result;
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;

    std::ifstream ifs(req.GetLocalFilePath().c_str(), std::ios::in | std::ios::binary);
    if (!ifs.is_open()) {
        result.SetErrorInfo("Open local file fail, local file=" + req.GetLocalFilePath());
        return result;
    }

    // 如果传递的header中没有Content-MD5则进行SDK进行MD5校验
    bool is_check_md5 = false;
    std::string md5_str = "";
    if (req.GetHeader("Content-MD5").empty()) {
        Poco::MD5Engine md5;
        Poco::DigestOutputStream dos(md5);
        std::streampos pos = ifs.tellg();
        Poco::StreamCopier::copyStream(ifs, dos);
        ifs.clear();
        ifs.seekg(pos);
        dos.close();
        md5_str = Poco::DigestEngine::digestToHex(md5.digest());
        is_check_md5 = true;
        // 默认开启MD5校验
        if (req.ShouldComputeContentMd5()) {
            std::string bin_str = CodecUtil::HexToBin(md5_str);
            std::string encode_str = CodecUtil::Base64Encode(bin_str);
            additional_headers.insert(std::make_pair("Content-MD5",encode_str));
        }
    }

    result = UploadAction(host, path, req, additional_headers,
                          additional_params, ifs, resp);
    if (result.IsSucc() && is_check_md5 && md5_str != resp->GetEtag()) {
        result.SetFail();
        result.SetErrorInfo("Response etag is not correct, Please try again.");
        SDK_LOG_ERR("Response etag is not correct, Please try again. Expect md5 is %s,"
                    "but return etag is %s. RequestId=%s",
                    md5_str.c_str(), resp->GetEtag().c_str(), resp->GetXCosRequestId().c_str());
    }

    ifs.close();
    return result;
}

CosResult ObjectOp::DeleteObject(const DeleteObjectReq& req, DeleteObjectResp* resp) {
    CosResult result;
    std::string object_name = req.GetObjectName();
    if (object_name.empty()) {
        result.SetErrorInfo("Delete object's name is empty.");
        return result;
    }

    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult ObjectOp::DeleteObjects(const DeleteObjectsReq& req, DeleteObjectsResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());

    CosResult result;
    std::string req_body = "";
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    if (!req.GenerateRequestBody(&req_body)) {
        result.SetErrorInfo("Generate DeleteObjects Request Body fail.");
        return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

    return NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, false, resp);
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
    const std::string& server_side_encryption = req.GetHeader("x-cos-server-side-encryption");
    if (!server_side_encryption.empty()) {
        init_req.SetXCosServerSideEncryption(server_side_encryption);
    }
    const std::string& storage_class = req.GetHeader("x-cos-storage-class");
    if (!storage_class.empty()) {
        init_req.SetXCosStorageClass(storage_class);
    }
    const std::string& acl = req.GetHeader("x-cos-acl");
    if (!acl.empty()) {
        init_req.SetXCosAcl(acl);
    }

    if (req.IsSetXCosMeta()) {
        const std::map<std::string, std::string> xcos_meta = req.GetXCosMeta();
        std::map<std::string, std::string>::const_iterator iter = xcos_meta.begin();  
        for(; iter != xcos_meta.end(); iter++) {
            init_req.SetXCosMeta(iter->first, iter->second);
        }
    }

    init_req.AddHeaders(req.GetHeaders());

    InitMultiUploadResp init_resp;
    init_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
    init_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms());
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
        // Copy失败则需要Abort
        AbortMultiUploadReq abort_req(req.GetBucketName(),
                req.GetObjectName(), upload_id);
        AbortMultiUploadResp abort_resp;

        CosResult abort_result = AbortMultiUpload(abort_req, &abort_resp);
        if (!abort_result.IsSucc()) {
            SDK_LOG_ERR("Upload failed, and abort muliti upload also failed"
                    ", upload_id=%s", upload_id.c_str());
            return abort_result;
        }
        return result;
    }

    // 3. Complete
    CompleteMultiUploadReq comp_req(bucket_name, object_name, upload_id);
    CompleteMultiUploadResp comp_resp;
    comp_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
    comp_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms() * 2); // Complete的超时翻倍
    comp_req.SetEtags(etags);
    comp_req.SetPartNumbers(part_numbers);

    result = CompleteMultiUpload(comp_req, &comp_resp);
    resp->CopyFrom(comp_resp);

    return result;
}

CosResult ObjectOp::InitMultiUpload(const InitMultiUploadReq& req, InitMultiUploadResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_params.insert(std::make_pair("uploads", ""));
    return NormalAction(host, path, req, additional_headers,
                        additional_params, "", false, resp);
}

CosResult ObjectOp::UploadPartData(const UploadPartDataReq& req, UploadPartDataResp* resp) {
    CosResult result;
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_params.insert(std::make_pair("uploadId", req.GetUploadId()));
    additional_params.insert(std::make_pair("partNumber",
                             StringUtil::Uint64ToString(req.GetPartNumber())));

    std::istream& is = req.GetStream();
    if (is.peek() == EOF) {
        result.SetErrorInfo("Input Stream is empty.");
        return result;
    }

    // 如果传递的header中没有Content-MD5则SDK进行MD5校验
    bool is_check_md5 = false;
    std::string md5_str = "";
    if (req.GetHeader("Content-MD5").empty()) {
        Poco::MD5Engine md5;
        Poco::DigestOutputStream dos(md5);
        std::streampos pos = is.tellg();
        Poco::StreamCopier::copyStream(is, dos);
        is.clear();
        is.seekg(pos);
        dos.close();
        md5_str = Poco::DigestEngine::digestToHex(md5.digest());
        is_check_md5 = true;
        // 默认开启MD5校验
        if (req.ShouldComputeContentMd5()) {
            std::string bin_str = CodecUtil::HexToBin(md5_str);
            std::string encode_str = CodecUtil::Base64Encode(bin_str);
            additional_headers.insert(std::make_pair("Content-MD5",encode_str));
        }
    }

    result = UploadAction(host, path, req, additional_headers,
                          additional_params, is, resp);

    if (result.IsSucc() && is_check_md5 && md5_str != resp->GetEtag()) {
        result.SetFail();
        result.SetErrorInfo("Response etag is not correct, Please try again.");
        SDK_LOG_ERR("Response etag is not correct, Please try again. Expect md5 is%s, "
                    "but return etag is %s. RequestId=%s",
                    md5_str.c_str(), resp->GetEtag().c_str(), resp->GetXCosRequestId().c_str());
    }

    return result;
}

CosResult ObjectOp::UploadPartCopyData(const UploadPartCopyDataReq& req, UploadPartCopyDataResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_params.insert(std::make_pair("uploadId", req.GetUploadId()));
    additional_params.insert(std::make_pair("partNumber",
                             StringUtil::Uint64ToString(req.GetPartNumber())));

    return NormalAction(host, path, req, additional_headers,
                        additional_params, "", false, resp);
}

CosResult ObjectOp::CompleteMultiUpload(const CompleteMultiUploadReq& req,
                                        CompleteMultiUploadResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
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
                        additional_params, req_body, true, resp);
}

CosResult ObjectOp::AbortMultiUpload(const AbortMultiUploadReq& req, AbortMultiUploadResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_params.insert(std::make_pair("uploadId", req.GetUploadId()));
    return NormalAction(host, path, req, additional_headers,
                        additional_params, "", false, resp);
}

CosResult ObjectOp::ListParts(const ListPartsReq& req, ListPartsResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult ObjectOp::GetObjectACL(const GetObjectACLReq& req,
                                 GetObjectACLResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult ObjectOp::PutObjectACL(const PutObjectACLReq& req,
                                 PutObjectACLResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();

    CosResult result;
    std::string req_body;
    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    const std::map<std::string, std::string>& headers = req.GetHeaders();

    // 头部中不包含任何授权信息,则通过 Body 以 XML 格式传入 ACL 信息
    if (headers.find("x-cos-acl") == headers.end()
        && headers.find("x-cos-grant-read") == headers.end()
        && headers.find("x-cos-grant-write") == headers.end()
        && headers.find("x-cos-grant-full-control") == headers.end()) {
        if (!req.GenerateRequestBody(&req_body)) {
            result.SetErrorInfo("Generate PutObjectACL Request Body fail.");
            return result;
        }
        std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));
        additional_headers.insert(std::make_pair("Content-MD5", raw_md5));
    }

    return NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, false, resp);
}

CosResult ObjectOp::PutObjectCopy(const PutObjectCopyReq& req,
                                  PutObjectCopyResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", true, resp);
}

CosResult ObjectOp::Copy(const CopyReq& req, CopyResp* resp) {
    SDK_LOG_DBG("Copy request=%s", req.DebugString().c_str());
    CosResult result;

    // 获取源bucket/object/region
    std::string src_bucket_appid = "", src_obj = "", src_region = "";
    std::string copy_source = req.GetHeader("x-cos-copy-source");
    if (copy_source.empty()) {
        SDK_LOG_ERR("You must SetXCosCopySource before call Copy.");
        result.SetErrorInfo("You must SetXCosCopySource before call Copy.");
        return result;
    }
    std::vector<std::string> v;
    StringUtil::SplitString(copy_source, '.', &v);
    // 正确的x-cos-copy-source起码有5段
    if (v.size() < 5) {
        SDK_LOG_ERR("x-cos-copy-source is illegal, source = %s", copy_source.c_str());
        result.SetErrorInfo("Check XCosCopySource.");
        return result;
    }
    src_bucket_appid = v[0];
    src_region = v[2];

    std::vector<std::string>().swap(v);
    StringUtil::SplitString(copy_source, '/', &v);
    if (v.size() < 2) {
        SDK_LOG_ERR("x-cos-copy-source is illegal, source = %s", copy_source.c_str());
        result.SetErrorInfo("Copy fail, please check XCosCopySource.");
        return result;
    }
    src_obj = copy_source.substr(v[0].size());

    // 同一区域直接使用PutObjectCopy
    if (src_region == m_config->GetRegion()) {
        SDK_LOG_INFO("Same region[%s], use put object copy.", src_region.c_str());
        PutObjectCopyReq put_copy_req(req.GetBucketName(), req.GetObjectName());
        put_copy_req.AddHeaders(req.GetHeaders());
        PutObjectCopyResp put_copy_resp;
        put_copy_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
        put_copy_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms());

        result = PutObjectCopy(put_copy_req, &put_copy_resp);
        if (result.IsSucc()) {
            resp->CopyFrom(put_copy_resp);
        }
        return result;
    }

    // 以"/"分割的copy_source第一段就是host
    HeadObjectReq head_req(src_bucket_appid, src_obj);
    head_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
    head_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms());
    HeadObjectResp head_resp;
    std::string host = v[0];
    std::string path = head_req.GetPath();
    result = NormalAction(host, path, head_req, "", false, &head_resp);
    if (!result.IsSucc()) {
        SDK_LOG_ERR("Get object length before download object fail, req=[%s]", req.DebugString().c_str());
        result.SetErrorInfo("Copy fail, can't get source object length.");
        return result;
    }

    uint64_t file_size = head_resp.GetContentLength();

    // 源文件小于5G则采用PutObjectCopy进行复制
    if (file_size < kPartSize5G || src_region == m_config->GetRegion()) {
        SDK_LOG_INFO("File Size=%ld less than 5G, use put object copy.", file_size);
        PutObjectCopyReq put_copy_req(req.GetBucketName(), req.GetObjectName());
        put_copy_req.AddHeaders(req.GetHeaders());
        PutObjectCopyResp put_copy_resp;
        put_copy_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
        put_copy_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms());

        result = PutObjectCopy(put_copy_req, &put_copy_resp);
        if (result.IsSucc()) {
            resp->CopyFrom(put_copy_resp);
        }
        return result;
    } else if (file_size < req.GetPartSize() * 10000) {
        SDK_LOG_INFO("File Size=%ld bigger than 5G, use put object copy.", file_size);
        // 1. InitMultiUploadReq
        InitMultiUploadReq init_req(req.GetBucketName(), req.GetObjectName());
        InitMultiUploadResp init_resp;
        init_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
        init_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms());
        init_req.AddHeaders(req.GetInitHeader());

        result = InitMultiUpload(init_req, &init_resp);
        if (!result.IsSucc()) {
            SDK_LOG_ERR("InitMultiUpload in Copy fail, req=[%s], result=[%s]",
                          init_req.DebugString().c_str(), result.DebugString().c_str());
            return result;
        }

        // 2. UploadPartCopyData
        std::string upload_id = init_resp.GetUploadId();
        uint64_t offset = 0;
        uint64_t part_number = 1;
        std::vector<std::string> etags;
        std::vector<uint64_t> part_numbers;
        const std::map<std::string, std::string>& part_copy_headers = req.GetPartCopyHeader();

        unsigned pool_size = req.GetThreadPoolSize();
        unsigned part_size = req.GetPartSize();
        unsigned max_task_num = file_size / part_size + 1;
        if (max_task_num < pool_size) {
            pool_size = max_task_num;
        }

        boost::threadpool::pool tp(pool_size);
        std::string path = "/" + req.GetObjectName();
        std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
        std::string dest_url = GetRealUrl(host, path, req.IsHttps());
        FileCopyTask** pptaskArr = new FileCopyTask*[pool_size];
        for (int i = 0; i < pool_size; ++i) {
            pptaskArr[i] = new FileCopyTask(dest_url, req.GetConnTimeoutInms(), req.GetRecvTimeoutInms());
        }

        while (offset < file_size) {
            unsigned task_index = 0;
            for (; task_index < pool_size && offset < file_size; ++task_index) {
                uint64_t end = offset + part_size;
                if (end >= file_size) {
                    end = file_size - 1;
                }
                SDK_LOG_DBG("copy data, task_index=%d, file_size=%lu, offset=%lu, end=%lu",
                        task_index, file_size, offset, end);

                std::string range = "bytes=" + StringUtil::Uint64ToString(offset) + "-" + StringUtil::Uint64ToString(end);

                FileCopyTask* ptask = pptaskArr[task_index];
                FillCopyTask(upload_id, host, path, part_number, range,
                             part_copy_headers, req.GetParams(), ptask);

                tp.schedule(boost::bind(&FileCopyTask::Run, ptask));
                part_numbers.push_back(part_number);
                ++part_number;
                offset = end + 1;
            }

            unsigned task_num = task_index;
            tp.wait();

            for (task_index = 0; task_index < task_num; ++task_index) {
                FileCopyTask* ptask = pptaskArr[task_index];
                if (!ptask->IsTaskSuccess()) {
                    const std::string& task_resp = ptask->GetTaskResp();
                    const std::map<std::string, std::string>& task_resp_headers = ptask->GetRespHeaders();
                    SDK_LOG_ERR("Copy failed , upload_id=%s, task_resp=%s", upload_id.c_str(), task_resp.c_str());
                    // 释放相关资源
                    for (int i = 0; i < pool_size; ++i) {
                        delete pptaskArr[i];
                    }
                    delete [] pptaskArr;

                    // Copy失败则需要Abort
                    AbortMultiUploadReq abort_req(req.GetBucketName(),
                                                  req.GetObjectName(), upload_id);
                    AbortMultiUploadResp abort_resp;

                    CosResult abort_result = AbortMultiUpload(abort_req, &abort_resp);
                    if (!abort_result.IsSucc()) {
                        SDK_LOG_ERR("Copy failed, and abort muliti upload also failed"
                                ", upload_id=%s", upload_id.c_str());
                        return abort_result;
                    } else {
                        SDK_LOG_ERR("Copy failed, abort upload part copy, upload_id=%s", upload_id.c_str());
                        CosResult ret;

                        ret.SetHttpStatus(ptask->GetHttpStatus());
                        if (ptask->GetHttpStatus() == -1) {
                            ret.SetErrorInfo(ptask->GetErrMsg());
                        } else if (!ret.ParseFromHttpResponse(task_resp_headers, task_resp)) {
                            result.SetErrorInfo(task_resp);
                        }
                        return ret;
                    }
                } else {
                    SDK_LOG_DBG("Copy succ");
                    etags.push_back(ptask->GetEtag());
                }
            }
        }

        // 3. Complete
        CompleteMultiUploadReq comp_req(req.GetBucketName(), req.GetObjectName(), upload_id);
        comp_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
        comp_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms() * 2); // Complete的超时翻倍
        CompleteMultiUploadResp comp_resp;

        comp_req.SetEtags(etags);
        comp_req.SetPartNumbers(part_numbers);

        result = CompleteMultiUpload(comp_req, &comp_resp);
        if (result.IsSucc()) {
            resp->CopyFrom(comp_resp);
        }

        return result;
    } else {
        SDK_LOG_ERR("Source Object is too large or your upload copy part size in config"
                    "is too small, src obj size=%ld, copy_part_size=%ld",
                    file_size, CosSysConfig::GetUploadCopyPartSize());
        result.SetErrorInfo("Could not copy object, because of object size is too large "
                            "or part size is too small.");
        return result;
    }
}

CosResult ObjectOp::PostObjectRestore(const PostObjectRestoreReq& req,
                                      PostObjectRestoreResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();

    CosResult result;
    std::string req_body;
    if (!req.GenerateRequestBody(&req_body)) {
        result.SetErrorInfo("Generate PostObjectRestore Request Body fail.");
        return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

    return NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, false, resp);
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
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();

    if (!CosSysConfig::IsDomainSameToHost()) {
        headers["Host"] = host;
    } else {
        headers["Host"] = CosSysConfig::GetDestDomain();
    }

    const std::string& tmp_token = m_config->GetTmpToken();
    if (!tmp_token.empty()) {
        headers["x-cos-security-token"] = tmp_token;
    }

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

    std::string dest_url = GetRealUrl(host, path, req.IsHttps());
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
                if (ptask->GetHttpStatus() == -1) {
                    result.SetErrorInfo(ptask->GetErrMsg());
                } else if (!result.ParseFromHttpResponse(task_resp_headers, task_resp)) {
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
    // TODO(jackyding) 是否要执行fsync
    // fsync(fd);
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
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());

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

    // get headers and params
    std::map<std::string, std::string> headers = req.GetHeaders();
    std::map<std::string, std::string> params = req.GetParams();

    std::string dest_url = GetRealUrl(host, path, req.IsHttps());
    FileUploadTask** pptaskArr = new FileUploadTask*[pool_size];
    for (int i = 0; i < pool_size; ++i) {
        pptaskArr[i] = new FileUploadTask(dest_url, headers, params, 
                           req.GetConnTimeoutInms(), req.GetRecvTimeoutInms());
    }

    SDK_LOG_DBG("upload data,url=%s, poolsize=%u, part_size=%lu, file_size=%lu",
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

                SDK_LOG_DBG("upload data, part_number=%lu, task_index=%d, file_size=%lu, offset=%lu, len=%lu",
                            part_number, task_index, file_size, offset, read_len);

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
                    if (ptask->GetHttpStatus() == -1) {
                        result.SetErrorInfo(ptask->GetErrMsg());
                    } else if (!result.ParseFromHttpResponse(task_resp_headers, task_resp)) {
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
    req_params.insert(std::make_pair("partNumber", StringUtil::Uint64ToString(part_number)));
    std::map<std::string, std::string> req_headers;

    if (!CosSysConfig::IsDomainSameToHost()) {
        req_headers["Host"] = host;
    } else {
        req_headers["Host"] = CosSysConfig::GetDestDomain();
    }
    std::string auth_str = AuthTool::Sign(GetAccessKey(), GetSecretKey(), "PUT",
                                          path, req_headers, req_params);
    req_headers["Authorization"] = auth_str;

    const std::string& tmp_token = m_config->GetTmpToken();
    if (!tmp_token.empty()) {
        req_headers["x-cos-security-token"] = tmp_token;
    }

    task_ptr->SetParams(req_params);
    task_ptr->SetHeaders(req_headers);
    task_ptr->SetUploadBuf(file_content_buf, len);
    task_ptr->SetPartNumber(part_number);
}

void ObjectOp::FillCopyTask(const std::string& upload_id,
                            const std::string& host,
                            const std::string& path,
                            uint64_t part_number,
                            const std::string& range,
                            const std::map<std::string, std::string>& headers,
                            const std::map<std::string, std::string>& params,
                            FileCopyTask* task_ptr) {
    std::map<std::string, std::string> req_params = params;
    req_params.insert(std::make_pair("uploadId", upload_id));
    req_params.insert(std::make_pair("partNumber",
                                     StringUtil::Uint64ToString(part_number)));
    std::map<std::string, std::string> req_headers = headers;
    if (!CosSysConfig::IsDomainSameToHost()) {
        req_headers["Host"] = host;
    } else {
        req_headers["Host"] = CosSysConfig::GetDestDomain();
    }

    req_headers["x-cos-copy-source-range"] = range;
    std::string auth_str = AuthTool::Sign(GetAccessKey(), GetSecretKey(), "PUT",
                                          path, req_headers, req_params);
    req_headers["Authorization"] = auth_str;

    const std::string& tmp_token = m_config->GetTmpToken();
    if (!tmp_token.empty()) {
        req_headers["x-cos-security-token"] = tmp_token;
    }

    task_ptr->SetParams(req_params);
    task_ptr->SetHeaders(req_headers);
}

std::string ObjectOp::GeneratePresignedUrl(const GeneratePresignedUrlReq& req) {
    std::string auth_str = "";
    if (req.GetStartTimeInSec() == 0 || req.GetExpiredTimeInSec() == 0) {
        auth_str = AuthTool::Sign(GetAccessKey(), GetSecretKey(), req.GetMethod(),
                req.GetPath(), req.GetHeaders(), req.GetParams());
    } else {
        auth_str = AuthTool::Sign(GetAccessKey(), GetSecretKey(), req.GetMethod(),
                req.GetPath(), req.GetHeaders(), req.GetParams(),
                req.GetStartTimeInSec(), req.GetStartTimeInSec() + req.GetExpiredTimeInSec());
    }

    if (auth_str.empty()) {
        return "";
    }

    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string signed_url = GetRealUrl(host, req.GetPath(), false);
    signed_url += "?sign=" + CodecUtil::EncodeKey(auth_str);

    const std::map<std::string, std::string>& req_params = req.GetParams();
    std::string query_str = "";
    for (std::map<std::string, std::string>::const_iterator c_itr = req_params.begin();
            c_itr != req_params.end(); ++c_itr) {
        std::string part = "";
        if (c_itr->second.empty()) {
            part = c_itr->first + "&";
        } else {
            part = c_itr->first + "=" + c_itr->second + "&";
        }
        query_str += part;
    }

    signed_url += query_str;
    return signed_url;
}

CosResult ObjectOp::OptionsObject(const OptionsObjectReq& req, OptionsObjectResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, "", false, resp);
}

CosResult ObjectOp::SelectObjectContent(const SelectObjectContentReq& req, SelectObjectContentResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    CosResult result;

    std::string req_body;
    if (!req.GenerateRequestBody(&req_body)) {
        result.SetErrorInfo("Generate PostObjectRestore Request Body fail.");
        return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

    return NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, false, resp);
}

CosResult ObjectOp::PutLiveChannel(const PutLiveChannelReq& req, PutLiveChannelResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    CosResult result;

    std::string req_body;
    if (!req.GenerateRequestBody(&req_body)) {
        result.SetErrorInfo("Generate PutLiveChannel Request Body fail.");
        return result;
    }
    std::string raw_md5 = CodecUtil::Base64Encode(CodecUtil::RawMd5(req_body));

    std::map<std::string, std::string> additional_headers;
    std::map<std::string, std::string> additional_params;
    additional_headers.insert(std::make_pair("Content-MD5", raw_md5));

    result =  NormalAction(host, path, req, additional_headers,
                        additional_params, req_body, false, resp);
    if (result.IsSucc() && result.GetHttpStatus()) {
        std::string sign_info = AuthTool::RtmpSign(GetAccessKey(), GetSecretKey(), GetTmpToken(),
                                                   req.GetBucketName(), req.GetObjectName(),
                                                   req.GetUrlParams(), req.GetExpire());
        std::string& publish_url = resp->GetPublishUrl();
        publish_url.append("?" + sign_info);
    }
    return result;
}

CosResult ObjectOp::PutLiveChannelSwitch(const PutLiveChannelSwitchReq& req, PutLiveChannelSwitchResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, std::map<std::string, std::string>(),
                        std::map<std::string, std::string>(), "", false, resp);
}

CosResult ObjectOp::GetLiveChannel(const GetLiveChannelReq& req, GetLiveChannelResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, std::map<std::string, std::string>(),
                        std::map<std::string, std::string>(), "", false, resp);
}

CosResult ObjectOp::GetLiveChannelHistory(const GetLiveChannelHistoryReq& req, GetLiveChannelHistoryResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, std::map<std::string, std::string>(),
                        std::map<std::string, std::string>(), "", false, resp);
}

CosResult ObjectOp::GetLiveChannelStatus(const GetLiveChannelStatusReq& req, GetLiveChannelStatusResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, std::map<std::string, std::string>(),
                        std::map<std::string, std::string>(), "", false, resp);
}

CosResult ObjectOp::DeleteLiveChannel(const DeleteLiveChannelReq& req, DeleteLiveChannelResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, std::map<std::string, std::string>(),
                        std::map<std::string, std::string>(), "", false, resp);
}

CosResult ObjectOp::GetLiveChannelVodPlaylist(const GetLiveChannelVodPlaylistReq& req,
                                                GetLiveChannelVodPlaylistResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, std::map<std::string, std::string>(),
                        std::map<std::string, std::string>(), "", false, resp);
}

CosResult ObjectOp::PostLiveChannelVodPlaylist(const PostLiveChannelVodPlaylistReq& req,
                                                PostLiveChannelVodPlaylistResp* resp) {
    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(), req.GetBucketName());
    std::string path = req.GetPath();
    return NormalAction(host, path, req, std::map<std::string, std::string>(),
                        std::map<std::string, std::string>(), "", false, resp);
}

/// \brief check resume upload
bool ObjectOp::CheckResumeUpload(const std::string& upload_id, const std::string& bucket,
                            const std::string& object, const std::string& local_file,
                            std::map<uint32_t, std::string> *existing_part_map, uint64_t *existing_part_size) {
    CosResult result;
    std::list<Part> existing_part_list;
    bool truncated = false;
    uint64_t part_number_marker = 0;
    uint64_t part_size = 0;

    // list all parts
    do {
        ListPartsReq list_part_req(bucket, object, upload_id);
        if (part_number_marker > 0) {
            list_part_req.SetPartNumberMarker(StringUtil::Uint64ToString(part_number_marker));
        }
        ListPartsResp list_part_resp;
        result = ListParts(list_part_req, &list_part_resp);
        if (!result.IsSucc()) {
            SDK_LOG_ERR("failed to list parts, result:%s", result.DebugString().c_str());
            return false;
        }
        part_number_marker = list_part_resp.GetNextPartNumberMarker();
        truncated = list_part_resp.IsTruncated();
        if (truncated && part_number_marker == 0) {
            SDK_LOG_ERR("invalid list part response");
            return false;
        }
        const std::vector<Part> &parts = list_part_resp.GetParts();
        for (const auto& it: parts) {
            existing_part_list.push_back(it);
        }
    } while(truncated);

    if (0 == existing_part_list.size()) {
        SDK_LOG_DBG("no existing part");
        return false;
    }

    if (existing_part_list.size() > 10000) {
        SDK_LOG_ERR("existing part number larger than 10000");
        return false;
    }

    // get first part size
    part_size = existing_part_list.front().m_size;
    if (part_size > kPartSize5G) {
        SDK_LOG_ERR("invalid part_size:%d", part_size);
        return false;
    }

    // open local file
    std::ifstream fin(local_file.c_str(), std::ios::in | std::ios::binary);
    if (!fin.is_open()){
        SDK_LOG_ERR("failed to open local_file:%s", local_file.c_str());
        return false;
    }

    char * part_buffer = new char [part_size];
    if (NULL == part_buffer) {
        SDK_LOG_ERR("failed to alloc part buffer");
        fin.close();
        return false;
    }

    // check all existing part
    for (const auto &part : existing_part_list) {
        // check part size, all part size should be equal to part_size, except for the last part
        if (part.m_size != part_size && part.m_part_num != existing_part_list.back().m_part_num) {
            SDK_LOG_ERR("part number:%d, part_size:%d != %d", part.m_part_num, part.m_size, part_size);
            break;
        }

        // last part size shold not larger than part_size
        if (part.m_part_num == existing_part_list.back().m_part_num && part.m_size > part_size) {
            SDK_LOG_ERR("last part number:%d, part_size:%d > %d", part.m_part_num, part.m_size, part_size);
            break;
        }
        
        if (!fin.good()) {
            SDK_LOG_ERR("inlivad stream state");
            break;
        }

        if (fin.eof()) {
            SDK_LOG_ERR("get eof");
            break;
        }

        fin.seekg((part.m_part_num - 1) * part_size, fin.beg);
        fin.read(part_buffer, part.m_size);
        if (fin.gcount() != part.m_size) {
            SDK_LOG_ERR("part number:%d, gcount:%d not equal to part size:%d",
                        part.m_part_num, fin.gcount(), part.m_size);
            break;
        }
        
        // calc part etag
        std::string part_str((const char *)part_buffer, part.m_size);
        Poco::MD5Engine md5;
        std::istringstream istr(part_str);
        Poco::DigestOutputStream dos(md5);
        Poco::StreamCopier::copyStream(istr, dos);
        dos.close();
        const std::string& local_part_md5 = Poco::DigestEngine::digestToHex(md5.digest());
        if (part.m_etag != local_part_md5) {
            SDK_LOG_ERR("part number:%d, etag:%s does not matches with local part md5:%s",
                        part.m_part_num, part.m_etag.c_str(), local_part_md5.c_str());
            break;
        }
        SDK_LOG_DBG("part number:%d, etag:%s matches with local part md5:%s",
                    part.m_part_num, part.m_etag.c_str(), local_part_md5.c_str());

        if ((*existing_part_map).count(part.m_part_num)) {
            SDK_LOG_ERR("invalid list part response, duplicate part number:%d", part.m_part_num);
            break;
        }
        (*existing_part_map)[part.m_part_num] = part.m_etag;
    }

    if (fin.is_open()) {
        fin.close();
    }

    // check size
    if (existing_part_list.size() != (*existing_part_map).size()) {
        SDK_LOG_ERR("existing_part_list size:%d != existing_part_map size:%d",
                existing_part_list.size(), (*existing_part_map).size());
        return false;
    }

    *existing_part_size = part_size;
    return true;
}

CosResult ObjectOp::ResumeUploadObject(const MultiUploadObjectReq& req,
                                       MultiUploadObjectResp* resp,
                                       const std::string& upload_id,
                                       const std::map<uint32_t, std::string>& existing_part_map,
                                       uint64_t existing_part_size) {
    CosResult result;
    if (upload_id.empty() || 0 == existing_part_map.size() || 0 == existing_part_size) {
        SDK_LOG_ERR("invalid input parameters");
        result.SetErrorInfo("invalid input parameters");
        return result;
    }

    // 1. get file size
    std::string local_file_path = req.GetLocalFilePath();
    uint64_t file_size = FileUtil::GetFileLen(local_file_path);
    uint64_t part_size = existing_part_size;


    // 2. calc total part number
    uint32_t total_part_number = file_size / part_size;
    if ((file_size % part_size) > 0) {
        total_part_number++;
    }

    int unfinished_part_number = total_part_number - existing_part_map.size();
    // unfinished_part_number can be 0, we don't need to upload any part, just need to complete
    if (unfinished_part_number < 0) {
        SDK_LOG_ERR("invalid unfinished part number:%d", unfinished_part_number);
        result.SetErrorInfo("invalid unfinished part number");
        return result;
    }

    SDK_LOG_INFO("file_size:%ul, part_size:%u, total_part_number:%u, unfinished_part_number:%d",
                file_size, part_size, total_part_number, unfinished_part_number);

    // copy existing part
    std::map<uint32_t, std::string> complete_parts = existing_part_map;

    if (unfinished_part_number > 0) {
        std::string path = "/" + req.GetObjectName();
        std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                                 req.GetBucketName());
        // 3. open file
        std::ifstream fin(local_file_path, std::ios::in | std::ios::binary);
        if (!fin.is_open()) {
            SDK_LOG_ERR("failed to open file: %s", local_file_path.c_str());
            result.SetErrorInfo("local file not exists, local_file=" + local_file_path);
            return result;
        }

        // 4. init upload task
        int pool_size = req.GetThreadPoolSize();
        if (unfinished_part_number < pool_size) {
            // adjust pool size
            pool_size = unfinished_part_number;
        }
        unsigned char** file_content_buf = new unsigned char*[pool_size];
        for(int i = 0; i < pool_size; ++i) {
            file_content_buf[i] = new unsigned char[part_size];
        }

        // get headers and params
        std::map<std::string, std::string> headers = req.GetHeaders();
        std::map<std::string, std::string> params = req.GetParams();
        std::string dest_url = GetRealUrl(host, path, req.IsHttps());

        FileUploadTask** pptaskArr = new FileUploadTask*[pool_size];
        for (int i = 0; i < pool_size; ++i) {
            pptaskArr[i] = new FileUploadTask(dest_url, headers, params,
                                              req.GetConnTimeoutInms(), req.GetRecvTimeoutInms());
        }

        SDK_LOG_DBG("upload data,url=%s, poolsize=%u, part_size=%lu, file_size=%lu",
                    dest_url.c_str(), pool_size, part_size, file_size);

        boost::threadpool::pool tp(pool_size);
        bool task_fail_flag = false;

        // 5. upload part using thread pool
        uint32_t part_number = 1;
        while (part_number <= total_part_number) {
            int task_index = 0;
            for (; task_index < pool_size && part_number <= total_part_number;) {
                // skip existing part
                SDK_LOG_DBG("processing part_number=%u", part_number);
                if (existing_part_map.count(part_number)) {
                    SDK_LOG_DBG("skip uploading part, part_number=%u, etag:%s",
                                part_number, existing_part_map.at(part_number).c_str());
                } else {
                    // need to upload this part
                    if (!fin.good()) {
                        SDK_LOG_ERR("stream status abnormal");
                        task_fail_flag = true;
                        break;
                    }
                    if (fin.eof()) {
                        SDK_LOG_ERR("stream status abnormal");
                        task_fail_flag = true;
                        break;
                    }
                    uint64_t offset =  (part_number - 1) * part_size;
                    fin.seekg((std::streampos)offset, fin.beg);
                    fin.read((char *)file_content_buf[task_index], part_size);
                    size_t read_len = fin.gcount();
                    SDK_LOG_DBG("read_len:%lu", read_len);
                    if (read_len == 0) {
                        SDK_LOG_ERR("no data, read_len=0");
                        task_fail_flag = true;
                        break;;
                    }
                    // no enough data, and the part is not the last part,
                    if (read_len < part_size && part_number != total_part_number) {
                        SDK_LOG_ERR("no enough data, part number:%u, read_len:%ul, part_size:%ul",
                                    part_number, read_len, part_size);
                        task_fail_flag = true;
                        break;;
                    }
                    SDK_LOG_DBG("upload data, part_number=%lu, task_index=%d, part_size=%lu, read_len=%lu",
                                part_number, task_index, part_size, read_len);

                    FileUploadTask* ptask = pptaskArr[task_index];
                    FillUploadTask(upload_id, host, path, file_content_buf[task_index], read_len,
                                   part_number, ptask);
                    tp.schedule(boost::bind(&FileUploadTask::Run, ptask));
                    task_index++;
                }
                ++part_number;
            }

            int max_task_num = task_index;
            // wait this batch of tasks to finish
            tp.wait();
            for (task_index = 0; task_index < max_task_num; ++task_index) {
                FileUploadTask* ptask = pptaskArr[task_index];
                if (!ptask->IsTaskSuccess()) {
                    const std::string& task_resp = ptask->GetTaskResp();
                    const std::map<std::string, std::string>& task_resp_headers = ptask->GetRespHeaders();
                    SDK_LOG_ERR("upload data, upload task fail, rsp:%s", task_resp.c_str());
                    result.SetHttpStatus(ptask->GetHttpStatus());
                    if (ptask->GetHttpStatus() == -1) {
                        result.SetErrorInfo(ptask->GetErrMsg());
                    } else if (!result.ParseFromHttpResponse(task_resp_headers, task_resp)) {
                        result.SetErrorInfo(task_resp);
                    }

                    task_fail_flag = true;
                    break;
                }

                // find etag
                const std::map<std::string, std::string>& resp_header = ptask->GetRespHeaders();
                std::map<std::string, std::string>::const_iterator itr = resp_header.find(kReqHeaderEtag);
                if (itr != resp_header.end()) {
                    complete_parts[ptask->GetPartNumber()] = itr->second;
                } else {
                    itr = resp_header.find(kReqHeaderLowerCaseEtag);
                    if (itr != resp_header.end()) {
                        complete_parts[ptask->GetPartNumber()] = itr->second;
                    } else {
                        std::string err_info = "upload data, upload task succ, "
                                               "but response header missing etag field.";
                        SDK_LOG_ERR("%s", err_info.c_str());
                        result.SetHttpStatus(ptask->GetHttpStatus());
                        task_fail_flag = true;
                        break;
                    }
                }
            }

            if (task_fail_flag) {
                break;
            }
        }

        // 6. release resource
        fin.close();
        for (int i = 0; i< pool_size; ++i) {
            delete pptaskArr[i];
        }
        delete [] pptaskArr;

        for (int i = 0; i < pool_size; ++i) {
            delete [] file_content_buf[i];
        }
        delete [] file_content_buf;

        // if upload part failed, just return
        if (task_fail_flag) {
            return result;
        }
    } else {
        SDK_LOG_INFO("no part to upload, just complete");
    }
    
    // 7. complete uploads
    std::vector<uint64_t> v_part_numbers;
    std::vector<std::string> v_etags;
    v_part_numbers.reserve(unfinished_part_number);
    v_etags.reserve(unfinished_part_number);
    for (const auto &it : complete_parts) {
        v_part_numbers.push_back(it.first);
        v_etags.push_back(it.second);
    }

    CompleteMultiUploadReq comp_req(req.GetBucketName(), req.GetObjectName(), upload_id);
    CompleteMultiUploadResp comp_resp;
    comp_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
    comp_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms() * 2); // Complete的超时翻倍
    comp_req.SetPartNumbers(v_part_numbers);
    comp_req.SetEtags(v_etags);

    result = CompleteMultiUpload(comp_req, &comp_resp);
    resp->CopyFrom(comp_resp);

    return result;
}

} // namespace qcloud_cos
