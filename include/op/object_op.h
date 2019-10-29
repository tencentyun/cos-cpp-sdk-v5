// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#ifndef OBJECT_OP_H
#define OBJECT_OP_H
#pragma once

#include "op/base_op.h"

#include "op/cos_result.h"
#include "request/object_req.h"
#include "response/object_resp.h"

#include "Poco/SharedPtr.h"
#include "trsf/transfer_handler.h"

namespace qcloud_cos {

class FileUploadTask;
class FileCopyTask;

/// \brief 封装了Object相关的操作
class ObjectOp : public BaseOp {
public:
    /// \brief BucketOp构造函数
    ///
    /// \param cos_conf Cos配置
    explicit ObjectOp(Poco::SharedPtr<CosConfig> config) : BaseOp(config) {}
    ObjectOp() {}

    /// \brief ObjectOP析构函数
    virtual ~ObjectOp() {}

    /// \brief 判断object是否存在
    bool IsObjectExist(const std::string& bucket_name, const std::string& object_name);

    std::string GetResumableUploadID(const std::string& bucket_name, const std::string& object_name) ;

    bool CheckUploadPart(const MultiUploadObjectReq& req, const std::string& bucket_name,
                         const std::string& object_name, const std::string& uploadid,
                         const std::string& localpath, std::vector<std::string>& already_exist); 

    bool check_single_part(const std::string& local_file_path, uint64_t offset, uint64_t local_part_size,
                           uint64_t size, const std::string& etag);
    /// \brief 获取对应Object的meta信息数据
    ///
    /// \param request   HeadObject请求
    /// \param response  HeadObject返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult HeadObject(const HeadObjectReq& req, HeadObjectResp* resp);

    /// \brief 下载Bucket中的一个文件至流中
    ///
    /// \param request   GetObjectByStream请求
    /// \param response  GetObjectByStream返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult GetObject(const GetObjectByStreamReq& req, GetObjectByStreamResp* resp);

    /// \brief 下载Bucket中的一个文件到本地
    ///
    /// \param request   GetObjectByFile请求
    /// \param response  GetObjectByFile返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult GetObject(const GetObjectByFileReq& req, GetObjectByFileResp* resp);

    /// \brief 多线程下载Bucket中的一个文件到本地
    ///
    /// \param request   MultiGetObject请求
    /// \param response  MultiGetObject返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult GetObject(const MultiGetObjectReq& req, MultiGetObjectResp* resp);

    /// \brief 将本地的文件上传至指定Bucket中
    ///
    /// \param request   PutObjectByFile请求
    /// \param response  PutObjectByFile返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult PutObject(const PutObjectByFileReq& req, PutObjectByFileResp* resp);

    /// \brief 将指定流上传至指定Bucket中
    ///
    /// \param request   PutObjectByStream请求
    /// \param response  PutObjectByStream返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult PutObject(const PutObjectByStreamReq& req, PutObjectByStreamResp* resp);

    /// \brief 删除Object
    ///
    /// \param req  DeleteObject请求
    /// \param resp DeleteObject返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteObject(const DeleteObjectReq& req, DeleteObjectResp* resp);

    /// \brief 批量删除Object
    ///
    /// \param req  DeleteObjects请求
    /// \param resp DeleteObjects返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteObjects(const DeleteObjectsReq& req, DeleteObjectsResp* resp);

    /// \brief 请求实现初始化分片上传,成功执行此请求以后会返回UploadId用于后续的Upload Part请求
    ///
    /// \param request   InitMultiUpload请求
    /// \param response  InitMultiUpload返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult InitMultiUpload(const InitMultiUploadReq& req, InitMultiUploadResp* resp);

    /// \brief 初始化以后的分块上传,支持的块的数量为1到10000,块的大小为1MB到5GB
    ///
    /// \param request   UploadPartData请求
    /// \param response  UploadPartData返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult UploadPartData(const UploadPartDataReq& req, UploadPartDataResp* resp);

    /// \brief 初始化以后的分块复制，实现将一个文件的分块内容从源路径复制到目标路径。
    ///        通过指定 x-cos-copy-source 来指定源文件，x-cos-copy-source-range 指定字节范围。
    ///        允许分块的大小为 5 MB - 5 GB。
    ///
    /// \param request   UploadPartCopyData请求
    /// \param response  UploadPartCopyData返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult UploadPartCopyData(const UploadPartCopyDataReq& req, UploadPartCopyDataResp* resp);

    /// \brief 完成整个分块上传。当使用 Upload Parts 上传完所有块以后，
    ///        必须调用该 API 来完成整个文件的分块上传
    ///
    /// \param request   CompleteMultiUpload请求
    /// \param response  CompleteMultiUpload返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult CompleteMultiUpload(const CompleteMultiUploadReq& req, CompleteMultiUploadResp* resp);

    /// \brief 封装了初始化分块上传、分块上传、完成分块上传三步
    ///
    /// \param request   MultiUploadObject请求
    /// \param response  MultiUploadObject返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult MultiUploadObject(const MultiUploadObjectReq& req, Poco::SharedPtr<TransferHandler>& handler,
                                MultiUploadObjectResp* resp);

    CosResult MultiUploadObject(const MultiUploadObjectReq& req, MultiUploadObjectResp* resp);

    Poco::SharedPtr<TransferHandler> CreateUploadHandler(const std::string& bucket_name, const std::string& object_name,
                                                         const std::string& local_path);
    /// \brief 舍弃一个分块上传并删除已上传的块
    ///
    /// \param req  AbortMultiUpload请求
    /// \param resp AbortMultiUpload返回
    ///
    /// \return
    CosResult AbortMultiUpload(const AbortMultiUploadReq& req, AbortMultiUploadResp* resp);

    /// \brief 查询特定分块上传中的已上传的块
    ///
    /// \param req  ListParts请求
    /// \param resp ListParts返回
    ///
    /// \return
    CosResult ListParts(const ListPartsReq& req, ListPartsResp* resp);

    /// \brief 列出Object下的ACL
    ///
    /// \param req  GetObjectACL请求
    /// \param resp GetObjectACL返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetObjectACL(const GetObjectACLReq& req,
                           GetObjectACLResp* resp);

    /// \brief 增加/替换Object下的ACL, 可以通过Header或者Body传入ACL信息
    ///        注意Header 和 Body 只能选择其中一种，否则响应返回会冲突
    ///
    /// \param req  PutObjectACL请求
    /// \param resp PutObjectACL返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutObjectACL(const PutObjectACLReq& req,
                           PutObjectACLResp* resp);

    /// \brief 复制Object
    ///
    /// \param req  PutObjectCopy请求
    /// \param resp PutObjectCopy返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutObjectCopy(const PutObjectCopyReq& req,
                            PutObjectCopyResp* resp);

    /// \brief 复制文件，实现将一个文件的分块内容从源路径复制到目标路径。
    ///        通过指定 x-cos-copy-source 来指定源文件，x-cos-copy-source-range 指定字节范围。
    ///
    /// \param request   Copy请求
    /// \param response  Copy返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult Copy(const CopyReq& req, CopyResp* resp);

    /// \brief 对一个通过 COS 归档为 archive 类型的对象进行恢复
    ///
    /// \param request   PostObjectRestore请求
    /// \param response  PostObjectRestore返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult PostObjectRestore(const PostObjectRestoreReq& req, PostObjectRestoreResp* resp);

    std::string GeneratePresignedUrl(const GeneratePresignedUrlReq& req);

private:
    // 生成request body所需的xml字符串
    bool GenerateCompleteMultiUploadReqBody(const CompleteMultiUploadReq& req,
                                            std::string* req_body);

    // 下载文件, 内部使用多线程
    CosResult MultiThreadDownload(const MultiGetObjectReq& req, MultiGetObjectResp* resp);

    // 上传文件, 内部使用多线程
    CosResult MultiThreadUpload(const MultiUploadObjectReq& req,
                                const std::string& upload_id,
                                const std::vector<std::string>& already_exist_parts,
                                Poco::SharedPtr<TransferHandler>& handler,
                                bool resume_flag,
                                std::vector<std::string>* etags_ptr,
                                std::vector<uint64_t>* part_numbers_ptr); 
    
    CosResult MultiThreadUpload(const MultiUploadObjectReq& req,
                                const std::string& upload_id,
                                const std::vector<std::string>& already_exist_parts, 
                                bool resume_flag,
                                std::vector<std::string>* etags_ptr,
                                std::vector<uint64_t>* part_numbers_ptr);

    // 读取文件内容, 并返回读取的长度
    uint64_t GetContent(const std::string& src, std::string* file_content) const;

    void FillUploadTask(const std::string& upload_id, const std::string& host,
                        const std::string& path, unsigned char* file_content_buf,
                        uint64_t len, uint64_t part_number,
                        FileUploadTask* task_ptr);

    void FillCopyTask(const std::string& upload_id, const std::string& host,
                      const std::string& path, uint64_t part_number,
                      const std::string& range,
                      const std::map<std::string, std::string>& headers,
                      const std::map<std::string, std::string>& params,
                      FileCopyTask* task);
};

} // namespace qcloud_cos
#endif // OBJECT_OP_H
