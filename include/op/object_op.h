// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/21/17
// Description:

#ifndef OBJECT_OP_H
#define OBJECT_OP_H

#include "op/base_op.h"
#include "op/cos_result.h"
#include "request/data_process_req.h"
#include "request/object_req.h"
#include "request/auditing_req.h"
#include "response/data_process_resp.h"
#include "response/object_resp.h"
#include "response/auditing_resp.h"


namespace qcloud_cos {

class FileUploadTask;
class FileCopyTask;

/// \brief 封装了Object相关的操作
class ObjectOp : public BaseOp {
 public:
  /// \brief BucketOp构造函数
  ///
  /// \param cos_conf Cos配置
  explicit ObjectOp(const SharedConfig& config) : BaseOp(config) {}
  ObjectOp() {}

  /// \brief ObjectOP析构函数
  virtual ~ObjectOp() {}

  /// \brief 判断object是否存在
  bool IsObjectExist(const std::string& bucket_name,
                     const std::string& object_name);

  std::string GetResumableUploadID(const std::string& bucket_name,
                                   const std::string& object_name);

  bool CheckUploadPart(const PutObjectByFileReq& req,
                       const std::string& bucket_name,
                       const std::string& object_name,
                       const std::string& uploadid,
                       std::vector<std::string>& already_exist);

  bool CheckSinglePart(const PutObjectByFileReq& req, uint64_t offset,
                       uint64_t local_part_size, uint64_t size,
                       const std::string& etag);

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
  CosResult GetObject(const GetObjectByStreamReq& req,
                      GetObjectByStreamResp* resp);

  /// \brief 下载Bucket中的一个文件到本地
  ///
  /// \param request   GetObjectByFile请求
  /// \param response  GetObjectByFile返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult GetObject(const GetObjectByFileReq& req, GetObjectByFileResp* resp,
                      const SharedTransferHandler& handler = nullptr);

  /// \brief 多线程下载Bucket中的一个文件到本地
  ///
  /// \param request   MultiGetObject请求
  /// \param response  MultiGetObject返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult MultiGetObject(const GetObjectByFileReq& req,
                          GetObjectByFileResp* resp);

  /// \brief 将本地的文件上传至指定Bucket中
  ///
  /// \param request   PutObjectByFile请求
  /// \param response  PutObjectByFile返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult PutObject(const PutObjectByFileReq& req, PutObjectByFileResp* resp,
                      const SharedTransferHandler& handler = nullptr);

  /// \brief 将指定流上传至指定Bucket中
  ///
  /// \param request   PutObjectByStream请求
  /// \param response  PutObjectByStream返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult PutObject(const PutObjectByStreamReq& req,
                      PutObjectByStreamResp* resp, const SharedTransferHandler& handler=nullptr);

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

  /// \brief
  /// 请求实现初始化分片上传,成功执行此请求以后会返回UploadId用于后续的Upload
  /// Part请求
  ///
  /// \param request   InitMultiUpload请求
  /// \param response  InitMultiUpload返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult InitMultiUpload(const InitMultiUploadReq& req,
                            InitMultiUploadResp* resp);

  /// \brief 初始化以后的分块上传,支持的块的数量为1到10000,块的大小为1MB到5GB
  ///
  /// \param request   UploadPartData请求
  /// \param response  UploadPartData返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult UploadPartData(const UploadPartDataReq& req,
                           UploadPartDataResp* resp);

  /// \brief
  /// 初始化以后的分块复制，实现将一个文件的分块内容从源路径复制到目标路径。
  ///        通过指定 x-cos-copy-source 来指定源文件，x-cos-copy-source-range
  ///        指定字节范围。 允许分块的大小为 5 MB - 5 GB。
  ///
  /// \param request   UploadPartCopyData请求
  /// \param response  UploadPartCopyData返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult UploadPartCopyData(const UploadPartCopyDataReq& req,
                               UploadPartCopyDataResp* resp);

  /// \brief 完成整个分块上传。当使用 Upload Parts 上传完所有块以后，
  ///        必须调用该 API 来完成整个文件的分块上传
  ///
  /// \param request   CompleteMultiUpload请求
  /// \param response  CompleteMultiUpload返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult CompleteMultiUpload(const CompleteMultiUploadReq& req,
                                CompleteMultiUploadResp* resp);

  /// \brief 异步多线程上传
  /// \param request   MultiUploadObject请求
  /// \param response  MultiUploadObject返回
  /// \param handler   TransferHandler
  ///
  /// \return result
  CosResult MultiUploadObject(const PutObjectByFileReq& req,
                              MultiPutObjectResp* resp,
                              const SharedTransferHandler& handler = nullptr);

  /// \brief 舍弃一个分块上传并删除已上传的块
  ///
  /// \param req  AbortMultiUpload请求
  /// \param resp AbortMultiUpload返回
  ///
  /// \return
  CosResult AbortMultiUpload(const AbortMultiUploadReq& req,
                             AbortMultiUploadResp* resp);

  /// \brief 查询特定分块上传中的已上传的块
  ///
  /// \param req  ListParts请求
  /// \param resp ListParts返回
  ///
  /// \return result
  CosResult ListParts(const ListPartsReq& req, ListPartsResp* resp);

  /// \brief 列出Object下的ACL
  ///
  /// \param req  GetObjectACL请求
  /// \param resp GetObjectACL返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetObjectACL(const GetObjectACLReq& req, GetObjectACLResp* resp);

  /// \brief 增加/替换Object下的ACL, 可以通过Header或者Body传入ACL信息
  ///        注意Header 和 Body 只能选择其中一种，否则响应返回会冲突
  ///
  /// \param req  PutObjectACL请求
  /// \param resp PutObjectACL返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutObjectACL(const PutObjectACLReq& req, PutObjectACLResp* resp);

  /// \brief 复制Object
  ///
  /// \param req  PutObjectCopy请求
  /// \param resp PutObjectCopy返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutObjectCopy(const PutObjectCopyReq& req, PutObjectCopyResp* resp);

  /// \brief 复制文件，实现将一个文件的分块内容从源路径复制到目标路径。
  ///        通过指定 x-cos-copy-source 来指定源文件，x-cos-copy-source-range
  ///        指定字节范围。
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
  CosResult PostObjectRestore(const PostObjectRestoreReq& req,
                              PostObjectRestoreResp* resp);

  std::string GeneratePresignedUrl(const GeneratePresignedUrlReq& req);

  CosResult OptionsObject(const OptionsObjectReq& req, OptionsObjectResp* resp);

  CosResult SelectObjectContent(const SelectObjectContentReq& req,
                                SelectObjectContentResp* resp);

  CosResult AppendObject(const AppendObjectReq& req, AppendObjectResp* resp);

  /// \brief 创建推流通道
  ///
  /// \param request   PutLiveChannelReq请求
  /// \param response  PutLiveChannelResp返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult PutLiveChannel(const PutLiveChannelReq& req,
                           PutLiveChannelResp* resp);

  /// \brief 启用或禁用通道
  ///
  /// \param request   PutLiveChannelSwitchReq请求
  /// \param response  PutLiveChannelSwitchResp返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult PutLiveChannelSwitch(const PutLiveChannelSwitchReq& req,
                                 PutLiveChannelSwitchResp* resp);

  /// \brief 获取直播通道配置
  ///
  /// \param request   GetLiveChannelReq请求
  /// \param response  GetLiveChannelResp返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult GetLiveChannel(const GetLiveChannelReq& req,
                           GetLiveChannelResp* resp);

  /// \brief 获取直播通道推流历史
  ///
  /// \param request   GetLiveChannelHistoryReq请求
  /// \param response  GetLiveChannelHistoryResp返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult GetLiveChannelHistory(const GetLiveChannelHistoryReq& req,
                                  GetLiveChannelHistoryResp* resp);

  /// \brief 获取直播通道推流状态
  ///
  /// \param request   GetLiveChannelStatusReq请求
  /// \param response  GetLiveChannelStatusResp返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult GetLiveChannelStatus(const GetLiveChannelStatusReq& req,
                                 GetLiveChannelStatusResp* resp);

  /// \brief 删除直播通
  ///
  /// \param request   DeleteLiveChannelReq请求
  /// \param response  DeleteLiveChannelResp返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult DeleteLiveChannel(const DeleteLiveChannelReq& req,
                              DeleteLiveChannelResp* resp);

  /// \brief 查询指定通道在指定时间段推流生成的播放列表
  ///
  /// \param request   GetLiveChannelVodPlaylistReq请求
  /// \param response  GetLiveChannelVodPlaylistResp返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult GetLiveChannelVodPlaylist(const GetLiveChannelVodPlaylistReq& req,
                                      GetLiveChannelVodPlaylistResp* resp);

  /// \brief 指定通道生成一个可供点播例用的播放列表
  ///
  /// \param request  PostLiveChannelVodPlaylistReq请求
  /// \param response  GetLiveChannelVodPlaylistResp返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult PostLiveChannelVodPlaylist(const PostLiveChannelVodPlaylistReq& req,
                                       PostLiveChannelVodPlaylistResp* resp);

  /// \brief 异步多线程下载,handler处理回调
  CosResult MultiThreadDownload(const GetObjectByFileReq& req,
                                GetObjectByFileResp* resp,
                                const SharedTransferHandler& handler = nullptr);

  /* Resumable接口 */

  /// \brief 支持断点下载
  CosResult ResumableGetObject(const GetObjectByFileReq& req,
                               GetObjectByFileResp* resp,
                               const SharedTransferHandler& handler = nullptr);

  /*批量及目录操作接口*/
  CosResult PutObjects(const PutObjectsByDirectoryReq& req,
                       PutObjectsByDirectoryResp* resp);

  CosResult PutDirectory(const PutDirectoryReq& req, PutDirectoryResp* resp);

  CosResult MoveObject(const MoveObjectReq& req);

  /*数据处理接口*/

  /**基础图片处理**/

  /**图片持久化处理**/

  /***上传时处理***/
  CosResult PutImage(const PutImageByFileReq& req, PutImageByFileResp* resp);

  /***云上数据处理***/
  CosResult CloudImageProcess(const CloudImageProcessReq& req,
                              CloudImageProcessResp* resp);

  /***下载图片时识别二维码***/
  CosResult GetQRcode(const GetQRcodeReq& req, GetQRcodeResp* resp);

  /*文档处理接口*/

  /***查询已经开通文档预览功能的 Bucket***/
  CosResult DescribeDocProcessBuckets(const DescribeDocProcessBucketsReq& req,
                                      DescribeDocProcessBucketsResp* resp);

  /***预览文档***/
  CosResult DocPreview(const DocPreviewReq& req, DocPreviewResp* resp);

  /***获取媒体文件的信息***/
  CosResult GetMediaInfo(const GetMediaInfoReq& req, GetMediaInfoResp* resp);

  CosResult GetImageAuditing(const GetImageAuditingReq& req, GetImageAuditingResp* resp);

 private:
  // 生成request body所需的xml字符串
  bool GenerateCompleteMultiUploadReqBody(const CompleteMultiUploadReq& req,
                                          std::string* req_body);

  /// \brief 多线程上传,handler处理回调
  CosResult
  MultiThreadUpload(const PutObjectByFileReq& req, const std::string& upload_id,
                    const std::vector<std::string>& already_exist_parts,
                    bool resume_flag, std::vector<std::string>* etags_ptr,
                    std::vector<uint64_t>* part_numbers_ptr,
                    const SharedTransferHandler& handler = nullptr);

  /// \brief 读取文件内容, 并返回读取的长度
  // uint64_t GetContent(const std::string& src, std::string* file_content) const;

  void FillUploadTask(const std::string& upload_id, const std::string& host,
                      const std::string& path, unsigned char* file_content_buf,
                      uint64_t len, uint64_t part_number,
                      FileUploadTask* task_ptr, bool sign_header_host);

  void FillCopyTask(const std::string& upload_id, const std::string& host,
                    const std::string& path, uint64_t part_number,
                    const std::string& range,
                    const std::map<std::string, std::string>& headers,
                    const std::map<std::string, std::string>& params,
                    bool verify_cert,const std::string& ca_location, 
                    FileCopyTask* task, bool sign_header_host);

  /// \brief 检查是否可以走断点下载
  /// \param req  PutObjectByFile请求
  /// \param head_resp  HeadObjectResp响应结果
  /// \param last_offset 返回的上一次下载偏移量
  /// \return true可以走断点下载,false表示不可以
  bool CheckResumableDownloadTask(
      const std::string& json_file,
      const std::map<std::string, std::string>& element_map,
      uint64_t* resume_offset);
  /// \brief 更新断点下载json文件
  /// \param json_file  json文件名
  /// \param element_map  检查的元素映射
  /// \param last_offset 返回的上一次下载偏移量
  /// \return true文件检查成功, 否则失败
  void UpdateResumableDownloadTaskFile(
      const std::string& json_file,
      const std::map<std::string, std::string>& element_map,
      uint64_t resume_offset);
  void SetResultAndLogError(CosResult& result, const std::string& err_msg);
};

}  // namespace qcloud_cos
#endif  // OBJECT_OP_H
