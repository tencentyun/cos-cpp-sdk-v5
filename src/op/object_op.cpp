// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#include "op/object_op.h"
#include "op/bucket_op.h"

#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>

#include "threadpool/boost/threadpool.hpp"
#include <boost/bind.hpp>
#if defined(WIN32)
#include <io.h>
#endif

#include "cos_sys_config.h"
#include "op/file_copy_task.h"
#include "op/file_download_task.h"
#include "op/file_upload_task.h"
#include "util/auth_tool.h"
#include "util/file_util.h"
#include "util/http_sender.h"
#include "util/string_util.h"

#include "request/bucket_req.h"
#include "response/bucket_resp.h"

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

std::string ObjectOp::GetResumableUploadID(const std::string& bucket_name, const std::string& object_name) {
    ListMultipartUploadReq req(bucket_name);
    req.SetPrefix(object_name);
    ListMultipartUploadResp resp;

    std::string host = CosSysConfig::GetHost(GetAppId(), m_config->GetRegion(),
                                             req.GetBucketName());
    std::string path = req.GetPath();
    CosResult result = NormalAction(host, path, req, "", false, &resp);

    std::vector<Upload> rst = resp.GetUpload();
	// Notice the index type, if size_t might over
    int index = rst.size() - 1;
    while (index >= 0) {
        if (rst[index].m_key == object_name) {
            return rst[index].m_uploadid;
        }
        index--;
    }
    return "";
}

bool ObjectOp::check_single_part(const std::string& local_file_path, uint64_t offset, uint64_t local_part_size,
                       uint64_t size, const std::string& etag) {
    if (local_part_size != size) {
        return false;
    }

    std::ifstream fin(local_file_path.c_str(), std::ios::in | std::ios::binary);
    if (!fin.is_open()) {
        SDK_LOG_ERR("CheckUploadPart: file open fail, %s", local_file_path.c_str());
        return false;
    }

    fin.seekg (offset);

    // Allocate memory:
    char *data = new char[local_part_size];

    // Read data as a block:
    fin.read (data,local_part_size);

    fin.seekg (0, fin.beg);
    fin.close();

    // Print content:
    std::istringstream stringStream(std::string(data, local_part_size));

    Poco::MD5Engine md5;
    Poco::DigestOutputStream dos(md5);
    Poco::StreamCopier::copyStream(stringStream, dos);

    dos.flush();

    std::string md5_str = Poco::DigestEngine::digestToHex(md5.digest());

    delete []data;
    dos.close();

    if (md5_str != etag) {
        return false;
    }
    return true;
}

bool ObjectOp::CheckUploadPart(const MultiUploadObjectReq& req, const std::string& bucket_name,
                     const std::string& object_name, const std::string& uploadid,
                     const std::string& localpath, std::vector<std::string>& already_exist) {
    // Count the size info 
    std::string local_file_path = req.GetLocalFilePath();
    std::ifstream fin(local_file_path.c_str(), std::ios::in | std::ios::binary);
    if (!fin.is_open()){
        SDK_LOG_ERR("CheckUploadPart: file open fail, %s", local_file_path.c_str());
        return false;
    }
    uint64_t file_size = FileUtil::GetFileLen(local_file_path);
    uint64_t part_size = req.GetPartSize();
    uint64_t part_num  = file_size / part_size;
    uint64_t last_part_size = file_size % part_size;

    if (0 != last_part_size) {
        part_num += 1;
    } else {
        last_part_size = part_size;
    }
    if (part_num > kMaxPartNumbers) {
        return false;
    }

    ListPartsReq list_req(bucket_name, object_name, uploadid);
    ListPartsResp resp;
    int part_num_marker = 0;
    bool list_over_flag = false;

    std::vector<Part> parts_info;

    while (!list_over_flag) {
        std::string marker = StringUtil::IntToString(part_num_marker);
        list_req.SetPartNumberMarker(marker);
        CosResult result = ListParts(list_req, &resp);
        // Add to the parts_info;
        std::vector<Part> rst = resp.GetParts();
        for (std::vector<Part>::const_iterator itr = rst.begin(); itr != rst.end(); ++itr) {
            parts_info.push_back(*itr);
        }

        if (!resp.IsTruncated()) {
            list_over_flag = true;
        }else {
            part_num_marker = int(resp.GetNextPartNumberMarker());
        }
    }

    for (std::vector<Part>::const_iterator itr = parts_info.begin(); itr != parts_info.end(); ++itr) {
        uint64_t sev_part_num = itr->m_part_num;
        if (sev_part_num > part_num) {
            return false;
        }
        uint64_t offset = (sev_part_num - 1) * part_size;
        uint64_t local_part_size = part_size;
        if (sev_part_num == part_num) {
            local_part_size = last_part_size;
        }

        // Check single upload part each md5
        std::string etag = itr->m_etag;
        if (!check_single_part(local_file_path, offset, local_part_size, itr->m_size, etag)) {
            return false;
        }

        // Add the part_num with etags in already exist
        already_exist[sev_part_num] = itr->m_etag;
    }

    return true;
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


// Origin call
CosResult ObjectOp::MultiUploadObject(const MultiUploadObjectReq& req,
                                      MultiUploadObjectResp* resp) {
    CosResult result;
    uint64_t app_id = GetAppId();
    std::string bucket_name = req.GetBucketName();
    std::string object_name = req.GetObjectName();
    std::string local_file_path = req.GetLocalFilePath();

    bool resume_flag = false;
    // There is mem or cpu problem, if use the red-black tree might be slow
    std::vector<std::string> already_exist_parts(kMaxPartNumbers);
    // check the breakpoint 
    std::string resume_uploadid = GetResumableUploadID(bucket_name, object_name);
    if (!resume_uploadid.empty()) {
        resume_flag = CheckUploadPart(req, bucket_name, object_name, resume_uploadid,
                                      local_file_path, already_exist_parts);
    }

    if (!resume_flag) {
        // 1. Init
        InitMultiUploadReq init_req(bucket_name, object_name);
        const std::string& server_side_encryption = req.GetHeader("x-cos-server-side-encryption");
        if (!server_side_encryption.empty()) {
            init_req.SetXCosServerSideEncryption(server_side_encryption);
        }

        if (req.IsSetXCosMeta()) {
            const std::map<std::string, std::string> xcos_meta = req.GetXCosMeta();
            std::map<std::string, std::string>::const_iterator iter = xcos_meta.begin();  
            for(; iter != xcos_meta.end(); iter++) {
                init_req.SetXCosMeta(iter->first, iter->second);
            }
        }

        InitMultiUploadResp init_resp;
        init_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
        init_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms());
        result = InitMultiUpload(init_req, &init_resp);
        if (!result.IsSucc()) {
            SDK_LOG_ERR("Multi upload object fail, check init mutli result.");
            resp->CopyFrom(init_resp);
            return result;
        }
        resume_uploadid = init_resp.GetUploadId();
        if (resume_uploadid.empty()) {
            SDK_LOG_ERR("Multi upload object fail, upload id is empty.");
            resp->CopyFrom(init_resp);
            return result;
        }
    }

    // 2. Multi Upload
    std::vector<std::string> etags;
    std::vector<uint64_t> part_numbers;
    // TODO(返回值判断), add the already exist parts
    result = MultiThreadUpload(req, resume_uploadid, already_exist_parts,
                               resume_flag, &etags, &part_numbers);
    if (!result.IsSucc()) {
        SDK_LOG_ERR("Multi upload object fail, check upload mutli result.");
        // Copy失败则需要Abort
        AbortMultiUploadReq abort_req(req.GetBucketName(),
                req.GetObjectName(), resume_uploadid);
        AbortMultiUploadResp abort_resp;

        CosResult abort_result = AbortMultiUpload(abort_req, &abort_resp);
        if (!abort_result.IsSucc()) {
            SDK_LOG_ERR("Upload failed, and abort muliti upload also failed"
                    ", resume_uploadid=%s", resume_uploadid.c_str());
            return abort_result;
        }
        return result;
    }

    // 3. Complete
    CompleteMultiUploadReq comp_req(bucket_name, object_name, resume_uploadid);
    CompleteMultiUploadResp comp_resp;
    comp_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
    comp_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms() * 2); // Complete的超时翻倍
    comp_req.SetEtags(etags);
    comp_req.SetPartNumbers(part_numbers);

    result = CompleteMultiUpload(comp_req, &comp_resp);
    resp->CopyFrom(comp_resp);

    return result;
}
    
// Create the handler
Poco::SharedPtr<TransferHandler> ObjectOp::CreateUploadHandler(const std::string& bucket_name, const std::string& object_name,
                                                               const std::string& local_path) {
    TransferHandler *p = new TransferHandler(bucket_name, object_name, 0, local_path);
    Poco::SharedPtr<TransferHandler> handler(p);

    uint64_t file_size = FileUtil::GetFileLen(local_path);

    handler->SetTotalSize(file_size);
    return handler;
}

// Transfer call
CosResult ObjectOp::MultiUploadObject(const MultiUploadObjectReq& req,
                                      Poco::SharedPtr<TransferHandler>& handler,
                                      MultiUploadObjectResp* resp) {
    CosResult result;
    uint64_t app_id = GetAppId();
    std::string bucket_name = req.GetBucketName();
    std::string object_name = req.GetObjectName();
    std::string local_file_path = req.GetLocalFilePath();

    bool resume_flag = false;
    // There is mem or cpu problem, if use the red-black tree might be slow
    std::vector<std::string> already_exist_parts(kMaxPartNumbers);
    // check the breakpoint 
    std::string resume_uploadid = GetResumableUploadID(bucket_name, object_name);
    if (!resume_uploadid.empty()) {
        resume_flag = CheckUploadPart(req, bucket_name, object_name, resume_uploadid,
                                      local_file_path, already_exist_parts);
    }

    if (!resume_flag) {
        // 1. Init
        InitMultiUploadReq init_req(bucket_name, object_name);
        const std::string& server_side_encryption = req.GetHeader("x-cos-server-side-encryption");
        if (!server_side_encryption.empty()) {
            init_req.SetXCosServerSideEncryption(server_side_encryption);
        }

        if (req.IsSetXCosMeta()) {
            const std::map<std::string, std::string> xcos_meta = req.GetXCosMeta();
            std::map<std::string, std::string>::const_iterator iter = xcos_meta.begin();  
            for(; iter != xcos_meta.end(); iter++) {
                init_req.SetXCosMeta(iter->first, iter->second);
            }
        }

        InitMultiUploadResp init_resp;
        init_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
        init_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms());
        result = InitMultiUpload(init_req, &init_resp);
        if (!result.IsSucc()) {
            SDK_LOG_ERR("Multi upload object fail, check init mutli result.");
            resp->CopyFrom(init_resp);
            handler->UpdateStatus(TransferStatus::FAILED);
            handler->m_result = result;
            req.TriggerTransferStatusUpdateCallback(handler);
            return result;
        }
        resume_uploadid = init_resp.GetUploadId();
        if (resume_uploadid.empty()) {
            SDK_LOG_ERR("Multi upload object fail, upload id is empty.");
            resp->CopyFrom(init_resp);
            handler->UpdateStatus(TransferStatus::FAILED);
            handler->m_result = result;
            req.TriggerTransferStatusUpdateCallback(handler);
            return result;
        }
    }
    SDK_LOG_INFO("Multi upload object handler way id:%s, resumed:%d", resume_uploadid.c_str(), resume_flag);
	
    // 2. Multi Upload
    std::vector<std::string> etags;
    std::vector<uint64_t> part_numbers;
    // Add the already exist parts
    handler->SetUploadID(resume_uploadid);
    handler->UpdateStatus(TransferStatus::IN_PROGRESS);

    result = MultiThreadUpload(req, resume_uploadid, already_exist_parts, handler,
                               resume_flag, &etags, &part_numbers);
	// Cancel way 
    if (!handler->ShouldContinue()) {
        SDK_LOG_INFO("Multi upload object, canceled");
        handler->UpdateStatus(TransferStatus::CANCELED);
        req.TriggerTransferStatusUpdateCallback(handler);
        return result;
    }

    // Notice the cancel way not need to abort the uploadid
    if (!result.IsSucc()) {
        SDK_LOG_ERR("Multi upload object fail, check upload mutli result.");
        // When copy failed need abort.
        AbortMultiUploadReq abort_req(req.GetBucketName(),
                req.GetObjectName(), resume_uploadid);
        AbortMultiUploadResp abort_resp;

        CosResult abort_result = AbortMultiUpload(abort_req, &abort_resp);
        if (!abort_result.IsSucc()) {
            SDK_LOG_ERR("Upload failed, and abort muliti upload also failed"
                    ", resume_uploadid=%s", resume_uploadid.c_str());
            handler->UpdateStatus(TransferStatus::FAILED);
            handler->m_result = abort_result;
            req.TriggerTransferStatusUpdateCallback(handler);
            return abort_result;
        }
        handler->UpdateStatus(TransferStatus::ABORTED);
        handler->m_result = result;
        req.TriggerTransferStatusUpdateCallback(handler);
        return result;
    }

    // 3. Complete
    CompleteMultiUploadReq comp_req(bucket_name, object_name, resume_uploadid);
    CompleteMultiUploadResp comp_resp;
    comp_req.SetConnTimeoutInms(req.GetConnTimeoutInms());
	// Double timeout time
    comp_req.SetRecvTimeoutInms(req.GetRecvTimeoutInms() * 2); 
    comp_req.SetEtags(etags);
    comp_req.SetPartNumbers(part_numbers);

    result = CompleteMultiUpload(comp_req, &comp_resp);
    if (!result.IsSucc()) {
        handler->UpdateStatus(TransferStatus::FAILED);
    }else {
        handler->UpdateStatus(TransferStatus::COMPLETED);
    }
    resp->CopyFrom(comp_resp);
    handler->m_result = result;
    req.TriggerTransferStatusUpdateCallback(handler);

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
        SDK_LOG_INFO("File Size=%lld less than 5G, use put object copy.", file_size);
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
        SDK_LOG_INFO("File Size=%lld bigger than 5G, use put object copy.", file_size);
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
                SDK_LOG_DBG("copy data, task_index=%d, file_size=%llu, offset=%llu, end=%llu",
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
                    "is too small, src obj size=%lld, copy_part_size=%lld",
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
    headers["Host"] = host;
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
#if defined(WIN32)
	int fd = _open(local_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC,
		_S_IREAD | _S_IWRITE);
#else
	int fd = open(local_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC,
		S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
#endif
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

    SDK_LOG_DBG("download data,url=%s, poolsize=%u,slice_size=%u,file_size=%llu",
                dest_url.c_str(), pool_size, slice_size, file_size);

    std::vector<uint64_t> vec_offset;
    vec_offset.resize(pool_size);
    boost::threadpool::pool tp(pool_size);
    uint64_t offset =0;
    bool task_fail_flag = false;
    unsigned down_times = 0;
    bool is_header_set = false;
    while(offset < file_size) {
        SDK_LOG_DBG("down data, offset=%llu, file_size=%llu", offset, file_size);
        unsigned task_index = 0;
        vec_offset.clear();
        for (; task_index < pool_size && (offset < file_size); ++task_index) {
            SDK_LOG_DBG("down data, task_index=%d, file_size=%llu, offset=%llu",
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
                // Notice the lseek when used in 32bit plat has the biggest offset limit 2G
                // It fs can not seek over it and return NO_VAL(errno = 22 in windows),
                // There use the _lseeki64() instead in windows and the * in linux.
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
                SDK_LOG_DBG("down data, down_times=%u,task_index=%d, file_size=%llu, "
                            "offset=%llu, downlen:%zu ",
                            down_times,task_index, file_size,
                            vec_offset[task_index], ptask->GetDownLoadLen());
            }
        }

        if (task_fail_flag) {
	    // The result reused need to set status last
            result.SetFail();
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

// Origin way
CosResult ObjectOp::MultiThreadUpload(const MultiUploadObjectReq& req,
                                      const std::string& upload_id,
                                      const std::vector<std::string>& already_exist_parts, 
                                      bool resume_flag,
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

    // Check the part number
    uint64_t part_number = file_size / part_size;
    uint64_t last_part_size = file_size % part_size;
    if (0 != last_part_size) {
        part_number += 1;
    }else {
        last_part_size = part_size; // for now not use this.
    }

    if (part_number > kMaxPartNumbers) {
        SDK_LOG_ERR("FileUploadSliceData: part number bigger than 10000, %lld", part_number);
        result.SetErrorInfo("part number bigger than 10000");
        return result;
    }


    unsigned char** file_content_buf = new unsigned char*[pool_size];
    for(int i = 0; i < pool_size; ++i) {
        file_content_buf[i] = new unsigned char[part_size];
    }

    std::string dest_url = GetRealUrl(host, path, req.IsHttps());
    FileUploadTask** pptaskArr = new FileUploadTask*[pool_size];
    for (int i = 0; i < pool_size; ++i) {
        pptaskArr[i] = new FileUploadTask(dest_url, req.GetConnTimeoutInms(), req.GetRecvTimeoutInms());
    }

    SDK_LOG_DBG("upload data,url=%s, poolsize=%u, part_size=%llu, file_size=%llu",
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

                SDK_LOG_DBG("upload data, task_index=%d, file_size=%llu, offset=%llu, len=%zu",
                            task_index, file_size, offset, read_len);

                // Check the resume
                FileUploadTask* ptask = pptaskArr[task_index];
                if (resume_flag && !already_exist_parts[part_number].empty()) {
                    // Already has this part
                    ptask->SetResume(resume_flag);
                    ptask->SetResumeEtag(already_exist_parts[part_number]);
                    std::cout << "task etag is" << already_exist_parts[part_number] << std::endl;
                    ptask->SetTaskSuccess();
                    SDK_LOG_INFO("upload data part:%lld has resumed", part_number);

                }else {
                    FillUploadTask(upload_id, host, path, file_content_buf[task_index], read_len,
                                   part_number, ptask);
                    tp.schedule(boost::bind(&FileUploadTask::Run, ptask));
                }
                
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
                } else if (ptask->IsResume() && !ptask->GetResumeEtag().empty()) {
                    etags_ptr->push_back(ptask->GetResumeEtag());
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
    
// Trsf way
CosResult ObjectOp::MultiThreadUpload(const MultiUploadObjectReq& req,
                                      const std::string& upload_id,
                                      const std::vector<std::string>& already_exist_parts,
                                      Poco::SharedPtr<TransferHandler>& handler,
                                      bool resume_flag,
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

    // Check the part number
    uint64_t part_number = file_size / part_size;
    uint64_t last_part_size = file_size % part_size;
    if (0 != last_part_size) {
        part_number += 1;
    }else {
        last_part_size = part_size; // for now not use this.
    }

    if (part_number > kMaxPartNumbers) {
        SDK_LOG_ERR("FileUploadSliceData: part number bigger than 10000, %lld", part_number);
        result.SetErrorInfo("part number bigger than 10000");
        return result;
    }


    unsigned char** file_content_buf = new unsigned char*[pool_size];
    for(int i = 0; i < pool_size; ++i) {
        file_content_buf[i] = new unsigned char[part_size];
    }

    std::string dest_url = GetRealUrl(host, path, req.IsHttps());
    FileUploadTask** pptaskArr = new FileUploadTask*[pool_size];
    for (int i = 0; i < pool_size; ++i) {
        pptaskArr[i] = new FileUploadTask(dest_url, req.GetConnTimeoutInms(), req.GetRecvTimeoutInms());
    }

    SDK_LOG_DBG("upload data,url=%s, poolsize=%u, part_size=%llu, file_size=%llu",
                dest_url.c_str(), pool_size, part_size, file_size);

    boost::threadpool::pool tp(pool_size);

    // 3. 多线程upload
    {
        uint64_t part_number = 1;
        while (offset < file_size) {
            int task_index = 0;
            if (!handler->ShouldContinue()) {
                task_fail_flag = true;
                result.SetErrorInfo("FileUpload handler canceled");
                break;
            }

            for (; task_index < pool_size; ++task_index) {
                fin.read((char *)file_content_buf[task_index], part_size);
                size_t read_len = fin.gcount();
                if (read_len == 0 && fin.eof()) {
                    SDK_LOG_DBG("read over, task_index: %d", task_index);
                    break;
                }

                SDK_LOG_DBG("upload data, task_index=%d, file_size=%llu, offset=%llu, len=%zu",
                            task_index, file_size, offset, read_len);

                // Check the resume
                FileUploadTask* ptask = pptaskArr[task_index];

                ptask->m_req = const_cast<MultiUploadObjectReq*>(&req);
                ptask->SetHandler(true);

                if (resume_flag && !already_exist_parts[part_number].empty()) {
                    // Already has this part
                    ptask->SetResume(resume_flag);
                    ptask->SetResumeEtag(already_exist_parts[part_number]);
                    std::cout << "task etag is" << already_exist_parts[part_number] << std::endl;
                    ptask->SetTaskSuccess();
                    SDK_LOG_INFO("upload data part:%lld has resumed", part_number);

                    handler->UpdateProgress(read_len);
                    req.TriggerUploadProgressCallback(handler);
                    
                }else {
                    ptask->m_handler = handler;
                    FillUploadTask(upload_id, host, path, file_content_buf[task_index], read_len,
                                   part_number, ptask);
                    tp.schedule(boost::bind(&FileUploadTask::Run, ptask));
                }
                
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
                } else if (ptask->IsResume() && !ptask->GetResumeEtag().empty()) {
                    etags_ptr->push_back(ptask->GetResumeEtag());
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

    const std::string& tmp_token = m_config->GetTmpToken();
    if (!tmp_token.empty()) {
        req_headers["x-cos-security-token"] = tmp_token;
    }

    task_ptr->SetParams(req_params);
    task_ptr->SetHeaders(req_headers);
    task_ptr->SetUploadBuf(file_content_buf, len);
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
    req_headers["Host"] = host;
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

} // namespace qcloud_cos
