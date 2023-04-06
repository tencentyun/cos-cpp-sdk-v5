#ifndef COS_API_H
#define COS_API_H

#include "op/bucket_op.h"
#include "op/cos_result.h"
#include "op/object_op.h"
#include "op/service_op.h"
#include "util/auth_tool.h"
#include "util/codec_util.h"
#include "Poco/TaskManager.h"

namespace qcloud_cos {

class CosAPI {
 public:
  /// \brief CosAPI构造函数
  ///
  /// \param config    cos配置
  explicit CosAPI(CosConfig& config);

  ~CosAPI();

  /// \brief 设置密钥
  void SetCredentail(const std::string& ak, const std::string& sk,
                     const std::string& token);

  /// \brief 获取 Bucket 所在的地域信息
  std::string GetBucketLocation(const std::string& bucket_name);

  /// \brief 生成一个预签名链接
  std::string GeneratePresignedUrl(const GeneratePresignedUrlReq& req);

  /// \brief 生成一个预签名链接
  std::string GeneratePresignedUrl(const std::string& bucket_name,
                                   const std::string& object_name,
                                   uint64_t start_time_in_s,
                                   uint64_t end_time_in_s,
                                   HTTP_METHOD http_method);

  /// \brief 生成一个预签名的GET下载链接
  std::string GeneratePresignedUrl(const std::string& bucket_name,
                                   const std::string& object_name,
                                   uint64_t start_time_in_s,
                                   uint64_t end_time_in_s);

  /// \brief 判断Bucket是否存在
  bool IsBucketExist(const std::string& bucket_name);

  /// \brief 判断Object是否存在
  bool IsObjectExist(const std::string& bucket_name,
                     const std::string& object_name);

  /// \brief List Buckets
  ///        详见: https://cloud.tencent.com/document/api/436/8291
  ///
  /// \param req  GetService请求
  /// \param resp GetService响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetService(const GetServiceReq& req, GetServiceResp* resp);

  /// \brief 创建一个Bucket
  ///        详见: https://www.qcloud.com/document/product/436/7738
  ///
  /// \param req  PutBucket请求
  /// \param resp PutBucket响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucket(const PutBucketReq& req, PutBucketResp* resp);

  /// \brief 确认Bucket是否存在
  ///        (详见:https://cloud.tencent.com/document/product/436/7735)
  ///
  /// \param req  HeadBucket请求
  /// \param resp HeadBucket响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult HeadBucket(const HeadBucketReq& req, HeadBucketResp* resp);

  /// \brief 列出该Bucket下的部分或者全部Object, 需要对Bucket有Read 权限
  ///        详见: https://www.qcloud.com/document/product/436/7734
  ///
  /// \param req   GetBucket请求
  /// \param resp  GetBucket响应
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult GetBucket(const GetBucketReq& req, GetBucketResp* resp);

  CosResult ListMultipartUpload(const ListMultipartUploadReq& req,
                                ListMultipartUploadResp* resp);

  /// \brief 删除Bucket
  ///        详见: https://cloud.tencent.com/document/product/436/7732
  ///
  /// \param req  DeleteBucket请求
  /// \param resp DeleteBucket响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucket(const DeleteBucketReq& req, DeleteBucketResp* resp);

  /// \brief 获得存储桶的版本控制信息
  ///        (详见:https://cloud.tencent.com/document/product/436/8597)
  ///
  /// \param req  GetBucketVersioning请求
  /// \param resp GetBucketVersioning返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketVersioning(const GetBucketVersioningReq& req,
                                GetBucketVersioningResp* resp);

  /// \brief 启用或者暂停存储桶的版本控制功能
  ///        (详见:https://cloud.tencent.com/document/product/436/8591)
  ///
  /// \param req  PutBucketVersioning请求
  /// \param resp PutBucketVersioning返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketVersioning(const PutBucketVersioningReq& req,
                                PutBucketVersioningResp* resp);

  /// \brief 列出Bucket下的跨域复制配置
  ///
  /// \param req  GetBucketReplication请求
  /// \param resp GetBucketReplication返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketReplication(const GetBucketReplicationReq& req,
                                 GetBucketReplicationResp* resp);

  /// \brief 增加/替换Bucket下的跨域复制配置
  ///
  /// \param req  PutBucketReplication请求
  /// \param resp PutBucketReplication返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketReplication(const PutBucketReplicationReq& req,
                                 PutBucketReplicationResp* resp);

  /// \brief 删除Bucket下的跨域复制配置
  ///
  /// \param req  DeleteBucketReplication请求
  /// \param resp DeleteBucketReplication返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketReplication(const DeleteBucketReplicationReq& req,
                                    DeleteBucketReplicationResp* resp);

  /// \brief 列出Bucket下的生命周期配置
  ///
  /// \param req  GetBucketLifecycle请求
  /// \param resp GetBucketLifecycle返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketLifecycle(const GetBucketLifecycleReq& req,
                               GetBucketLifecycleResp* resp);

  /// \brief 增加/替换Bucket下的生命周期配置
  ///
  /// \param req  PutBucketLifecycle请求
  /// \param resp PutBucketLifecycle返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketLifecycle(const PutBucketLifecycleReq& req,
                               PutBucketLifecycleResp* resp);

  /// \brief 删除Bucket下的生命周期配置
  ///
  /// \param req  DeleteBucketLifecycle请求
  /// \param resp DeleteBucketLifecycle返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketLifecycle(const DeleteBucketLifecycleReq& req,
                                  DeleteBucketLifecycleResp* resp);

  /// \brief 列出Bucket下的ACL
  ///
  /// \param req  GetBucketACL请求
  /// \param resp GetBucketACL返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketACL(const GetBucketACLReq& req, GetBucketACLResp* resp);

  /// \brief 增加/替换Bucket下的ACL, 可以通过Header或者Body传入ACL信息
  ///        注意Header 和 Body 只能选择其中一种，否则响应返回会冲突
  ///
  /// \param req  PutBucketACL请求
  /// \param resp PutBucketACL返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketACL(const PutBucketACLReq& req, PutBucketACLResp* resp);

  /// \brief 获取Bucket的权限策略
  ///
  /// \param req  GetBucketPolicy请求
  /// \param resp GetBucketPolicy返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketPolicy(const GetBucketPolicyReq& req, GetBucketPolicyResp* resp);

  /// \brief 写入/替换Bucket的权限策略，通过Body传入
  ///
  /// \param req  PutBucketPolicy请求
  /// \param resp PutBucketPolicy返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketPolicy(const PutBucketPolicyReq& req, PutBucketPolicyResp* resp);

  /// \brief 删除Bucket的权限策略
  ///
  /// \param req  DeleteBucketPolicy请求
  /// \param resp DeleteBucketPolicy返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketPolicy(const DeleteBucketPolicyReq& req, DeleteBucketPolicyResp* resp);

  /// \brief 列出Bucket下的CORS
  ///
  /// \param req  GetBucketCORS请求
  /// \param resp GetBucketCORS返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketCORS(const GetBucketCORSReq& req, GetBucketCORSResp* resp);

  /// \brief 增加/替换Bucket下的CORS
  ///
  /// \param req  PutBucketCORS请求
  /// \param resp PutBucketCORS返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketCORS(const PutBucketCORSReq& req, PutBucketCORSResp* resp);

  /// \brief 删除Bucket下的CORS
  ///
  /// \param req  DeleteBucketCORS请求
  /// \param resp DeleteBucketCORS返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketCORS(const DeleteBucketCORSReq& req,
                             DeleteBucketCORSResp* resp);

  /// \brief 为存储桶设置 Referer 白名单或者黑名单
  ///
  /// \param req  PutBucketReferer请求
  /// \param resp PutBucketReferer响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketReferer(const PutBucketRefererReq& req,
                             PutBucketRefererResp* resp);

  /// \brief 为存储桶设置 Referer 白名单或者黑名单
  ///
  /// \param req  GetBucketReferer请求
  /// \param resp GetBucketReferer响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketReferer(const GetBucketRefererReq& req,
                             GetBucketRefererResp* resp);
  /// \brief 设置源存储桶的日志配置信息。
  ///
  /// \param req  PutBucketLogging请求
  /// \param resp PutBucketLogging返回
  ///
  /// \return 本次请求的调用情况(如状态码等)

  CosResult PutBucketLogging(const PutBucketLoggingReq& req,
                             PutBucketLoggingResp* resp);

  /// \brief 获取源存储桶的日志配置信息。
  ///
  /// \param req  GetBucketLogging请求
  /// \param resp GetBucketLogging返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketLogging(const GetBucketLoggingReq& req,
                             GetBucketLoggingResp* resp);

  /// \brief 设置存储桶自定义域名。
  ///
  /// \param req  PutBucketDomain请求
  /// \param resp PutBucketDomain返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketDomain(const PutBucketDomainReq& req,
                            PutBucketDomainResp* resp);

  /// \brief 获取存储桶自定义域名。
  ///
  /// \param req  GetBucketDomain请求
  /// \param resp GetBucketDomain返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketDomain(const GetBucketDomainReq& req,
                            GetBucketDomainResp* resp);

  /// \brief 为存储桶配置静态网站,可以通过传入 XML
  /// 格式的配置文件进行配置,文件大小限制为64KB.
  ///
  /// \param req  PutBucketWebsite请求
  /// \param resp PutBucketWebsite返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketWebsite(const PutBucketWebsiteReq& req,
                             PutBucketWebsiteResp* resp);

  /// \brief 请求用于获取与存储桶关联的静态网站配置信息.
  ///
  /// \param req  GetBucketWebsite请求
  /// \param resp GetBucketWebsite返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketWebsite(const GetBucketWebsiteReq& req,
                             GetBucketWebsiteResp* resp);

  /// \brief 删除存储桶中的静态网站配置.
  ///
  /// \param req  DeleteBucketWebsite请求
  /// \param resp DeleteBucketWebsite返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketWebsite(const DeleteBucketWebsiteReq& req,
                                DeleteBucketWebsiteResp* resp);

  /// \brief 已存在的Bucket设置标签.
  ///
  /// \param req  PutBucketTagging请求
  /// \param resp PutBucketTagging返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketTagging(const PutBucketTaggingReq& req,
                             PutBucketTaggingResp* resp);

  /// \brief 查询指定存储桶下已有的存储桶标签.
  ///
  /// \param req  GetBucketTagging请求
  /// \param resp GetBucketTagging返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketTagging(const GetBucketTaggingReq& req,
                             GetBucketTaggingResp* resp);

  /// \brief 删除指定存储桶下已有的存储桶标签.
  ///
  /// \param req  DeleteBucketTagging请求
  /// \param resp DeleteBucketTagging返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketTagging(const DeleteBucketTaggingReq& req,
                                DeleteBucketTaggingResp* resp);

  /// \brief 在存储桶中创建清单任务.
  ///
  /// \param req  PutBucketInventory请求
  /// \param resp PutBucketInventory返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketInventory(const PutBucketInventoryReq& req,
                               PutBucketInventoryResp* resp);

  /// \brief 用于查询存储桶中用户的清单任务信息.
  ///
  /// \param req  GetBucketInventory请求
  /// \param resp GetBucketInventory返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketInventory(const GetBucketInventoryReq& req,
                               GetBucketInventoryResp* resp);

  /// \brief 用于请求返回一个存储桶中的所有清单任务.
  ///
  /// \param req  ListBucketInventoryConfigurations请求
  /// \param resp ListBucketInventoryConfigurations返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult ListBucketInventoryConfigurations(
      const ListBucketInventoryConfigurationsReq& req,
      ListBucketInventoryConfigurationsResp* resp);

  /// \brief 用于删除存储桶中指定的清单任务.
  ///
  /// \param req  DeleteBucketinventory请求
  /// \param resp DeleteBucketinventory返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketInventory(const DeleteBucketInventoryReq& req,
                                  DeleteBucketInventoryResp* resp);

  /// \brief 获取对应Object的meta信息数据
  ///        详见: https://www.qcloud.com/document/product/436/7745
  ///
  /// \param req   HeadObject请求
  /// \param resp  HeadObject响应
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult HeadObject(const HeadObjectReq& req, HeadObjectResp* resp);

  /// \brief 下载Bucket中的一个文件至流中
  ///        详见: https://www.qcloud.com/document/product/436/7753
  ///
  /// \param req   GetObjectByStream请求
  /// \param resp  GetObjectByStream返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult GetObject(const GetObjectByStreamReq& req,
                      GetObjectByStreamResp* resp);

  /// \brief 下载Bucket中的一个文件到本地
  ///        详见: https://www.qcloud.com/document/product/436/7753
  ///
  /// \param req   GetObjectByFile请求
  /// \param resp  GetObjectByFile返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult GetObject(const GetObjectByFileReq& req, GetObjectByFileResp* resp);

  /// \brief 返回对象的URL
  ///
  /// \param bucket  存储桶名
  /// \param object  对象名
  /// \param https   是否使用https
  /// \param region  地域名,默认使用配置文件中的地域名
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  std::string GetObjectUrl(const std::string& bucket, const std::string& object,
                           bool https = true, const std::string& region = "");

  /// \brief 将本地的文件上传至指定Bucket中
  ///        详见: https://www.qcloud.com/document/product/436/7749
  ///
  /// \param req   PutObject请求
  /// \param resp  PutObject响应
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult PutObject(const PutObjectByFileReq& req, PutObjectByFileResp* resp);

  /// \brief 将指定流上传至指定Bucket中
  ///        详见: https://www.qcloud.com/document/product/436/7749
  ///
  /// \param req   PutObject请求
  /// \param resp  PutObject响应
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult PutObject(const PutObjectByStreamReq& req,
                      PutObjectByStreamResp* resp);

  /// \brief 删除Object
  ///        详见: https://cloud.tencent.com/document/product/436/7743
  ///
  /// \param req  DeleteObject请求
  /// \param resp DeleteObject响应
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
  ///        详见: https://www.qcloud.com/document/product/436/7746
  ///
  /// \param req   InitMultiUpload请求
  /// \param resp  InitMultiUpload返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult InitMultiUpload(const InitMultiUploadReq& req,
                            InitMultiUploadResp* resp);

  /// \brief
  /// 初始化以后的分片上传,支持的分片的数量为1到10000,片的大小为1MB到5GB
  ///        详见: https://www.qcloud.com/document/product/436/7750
  ///
  /// \param req   UploadPartData请求
  /// \param resp  UploadPartData返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult UploadPartData(const UploadPartDataReq& req,
                           UploadPartDataResp* resp);

  /// \brief
  /// 初始化以后的分片上传复制,支持的片的数量为1到10000,片的大小为1MB到5GB
  ///        详见: https://cloud.tencent.com/document/product/436/8287
  ///
  /// \param req   UploadPartCopyData请求
  /// \param resp  UploadPartCopyData返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult UploadPartCopyData(const UploadPartCopyDataReq& req,
                               UploadPartCopyDataResp* resp);

  /// \brief 完成整个分块上传。当使用 Upload Parts 上传完所有块以后，
  ///        必须调用该 API 来完成整个文件的分块上传
  ///        详见: https://www.qcloud.com/document/product/436/7742
  ///
  /// \param req   CompleteMultiUpload请求
  /// \param resp  CompleteMultiUpload返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult CompleteMultiUpload(const CompleteMultiUploadReq& req,
                                CompleteMultiUploadResp* resp);

  /// \brief 舍弃一个分块上传并删除已上传的块
  ///        详见: https://www.qcloud.com/document/product/436/7740
  ///
  /// \param req  AbortMultiUpload请求
  /// \param resp AbortMultiUpload返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult AbortMultiUpload(const AbortMultiUploadReq& req,
                             AbortMultiUploadResp* resp);

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
  CosResult GetObjectACL(const GetObjectACLReq& req, GetObjectACLResp* resp);

  /// \brief 增加/替换Object下的ACL, 可以通过Header或者Body传入ACL信息
  ///        注意Header 和 Body 只能选择其中一种，否则响应返回会冲突
  ///
  /// \param req  PutObjectACL请求
  /// \param resp PutObjectACL返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutObjectACL(const PutObjectACLReq& req, PutObjectACLResp* resp);

  /// \brief 复制Object, 适用于跨园区且Object小于5G
  ///
  /// \param req  PutObjectCopy请求
  /// \param resp PutObjectCopy返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutObjectCopy(const PutObjectCopyReq& req, PutObjectCopyResp* resp);

  /// \brief 复制Object,
  /// 自动判断源Object大小,调用PutObjectCopy/UploadPartCopyData
  ///
  /// \param req  Copy请求
  /// \param resp Copy返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult Copy(const CopyReq& req, CopyResp* resp);

  /// \brief 对一个通过 COS 归档为 archive 类型的对象进行恢复
  ///
  /// \param req   PostObjectRestore请求
  /// \param resp  PostObjectRestore返回
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult PostObjectRestore(const PostObjectRestoreReq& req,
                              PostObjectRestoreResp* resp);

  /// \brief
  /// 实现Object跨域访问请求的预请求，参考https://cloud.tencent.com/document/product/436/8288
  ///
  /// \param req  OptionsObjectReq 请求
  /// \param resp OptionsObjectResp 响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult OptionsObject(const OptionsObjectReq& req, OptionsObjectResp* resp);

  /// \brief 支持SELECT
  /// 接口，参考https://cloud.tencent.com/document/product/436/37641
  CosResult SelectObjectContent(const SelectObjectContentReq& req,
                                SelectObjectContentResp* resp);

  /// \brief 追加对象, 参考https://cloud.tencent.com/document/product/436/7743
  ///
  /// \param req  AppendObject请求
  /// \param resp AppendObject响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult AppendObject(const AppendObjectReq& req, AppendObjectResp* resp);

  /// \brief 列出Bucket下的部分或者全部Object(包括多版本)
  ///
  /// \param req  GetBucketObjectVersions请求
  /// \param resp GetBucketObjectVersions响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketObjectVersions(const GetBucketObjectVersionsReq& req,
                                    GetBucketObjectVersionsResp* resp);

  /// \brief 创建推流通道
  ///
  /// \param req  PutLiveChannelReq请求
  /// \param resp PutLiveChannelResp响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutLiveChannel(const PutLiveChannelReq& req,
                           PutLiveChannelResp* resp);

  /// \brief 生成推流通道带签名的推流url
  ///
  /// \param bucket 存储桶名称
  /// \param channel 通道名
  /// \param expire 签名过期时间
  /// \param url_params url参数
  ///
  /// \return 带签名的推流url
  std::string
  GetRtmpSignedPublishUrl(const std::string& bucket, const std::string& channel,
                          int expire,
                          const std::map<std::string, std::string> url_params);

  /// \brief 启用或者禁用直播通道
  ///
  /// \param req  PutLiveChannelSwitch请求
  /// \param resp PutLiveChannelSwitch响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutLiveChannelSwitch(const PutLiveChannelSwitchReq& req,
                                 PutLiveChannelSwitchResp* resp);

  /// \brief 获取直播通道配置信息
  ///
  /// \param req  GetLiveChannel请求
  /// \param resp GetLiveChannel响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetLiveChannel(const GetLiveChannelReq& req,
                           GetLiveChannelResp* resp);

  /// \brief 获取直播通道推流历史
  ///
  /// \param req  GetLiveChannelHistory请求
  /// \param resp GetLiveChannelHistory响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetLiveChannelHistory(const GetLiveChannelHistoryReq& req,
                                  GetLiveChannelHistoryResp* resp);

  /// \brief 获取直播通道推流状态
  ///
  /// \param req  GetLiveChannelStatus请求
  /// \param resp GetLiveChannelStatus响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetLiveChannelStatus(const GetLiveChannelStatusReq& req,
                                 GetLiveChannelStatusResp* resp);

  /// \brief 删除直播通道
  ///
  /// \param req  GetLiveChannelStatus请求
  /// \param resp GetLiveChannelStatus响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteLiveChannel(const DeleteLiveChannelReq& req,
                              DeleteLiveChannelResp* resp);

  /// \brief 查询指定通道在指定时间段推流生成的播放列表
  ///
  /// \param req  GetLiveChannelVodPlaylist请求
  /// \param resp GetLiveChannelVodPlaylist响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetLiveChannelVodPlaylist(const GetLiveChannelVodPlaylistReq& req,
                                      GetLiveChannelVodPlaylistResp* resp);

  /// \brief 为指定通道生成一个可供点播例用的播放列
  ///
  /// \param req  PostLiveChannelVodPlaylist请求
  /// \param resp PostLiveChannelVodPlaylist响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PostLiveChannelVodPlaylist(const PostLiveChannelVodPlaylistReq& req,
                                       PostLiveChannelVodPlaylistResp* resp);

  /// \brief 列举通道
  ///
  /// \param req  PostLiveChannelVodPlaylist请求
  /// \param resp PostLiveChannelVodPlaylist响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult ListLiveChannel(const ListLiveChannelReq& req,
                            ListLiveChannelResp* resp);

  /// \brief 配置存储桶智能分层
  ///
  /// \param req  PutBucketIntelligentTiering请求
  /// \param resp PutBucketIntelligentTiering响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult
  PutBucketIntelligentTiering(const PutBucketIntelligentTieringReq& req,
                              PutBucketIntelligentTieringResp* resp);

  /// \brief 获取存储桶智能分层配置
  ///
  /// \param req  GetBucketIntelligentTiering请求
  /// \param resp GetBucketIntelligentTiering响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult
  GetBucketIntelligentTiering(const GetBucketIntelligentTieringReq& req,
                              GetBucketIntelligentTieringResp* resp);

  /* Multithread接口 */

  /// \brief 封装了初始化分块上传、分块上传、完成分块上传三步，支持断点续传
  ///
  /// \param req   MultiPutObject请求
  /// \param resp  MultiPutObject响应
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult MultiPutObject(const MultiPutObjectReq& req,
                           MultiPutObjectResp* resp);

  /// \brief 多线程Range下载对象到本地
  ///        详见: https://www.qcloud.com/document/product/436/7753
  ///
  /// \param req   MultiGetObject请求
  /// \param resp  MultiGetObject响应
  ///
  /// \return 返回HTTP请求的状态码及错误信息
  CosResult MultiGetObject(const MultiGetObjectReq& req,
                           MultiGetObjectResp* resp);

  /* Resumable接口 */

  /// \brief 封装了初始化分块上传、分块上传、完成分块上传三步，支持断点续传
  // CosResult ResumablePutObject(const PutObjectByFile& req,
  // MultiPutObjectResp* resp);

  /// \brief 支持断点下载
  CosResult ResumableGetObject(const GetObjectByFileReq& req,
                               GetObjectByFileResp* resp);

  /* Async接口 */

  /// \brief 异步简单上传本地文件，不支持断点续传
  /// \param req  PutObjectAsync请求
  /// \return 返回context
  SharedAsyncContext AsyncPutObject(const AsyncPutObjectReq& req);
  SharedAsyncContext AsyncPutObject(const AsyncPutObjectReq& req, Poco::TaskManager*& taskManager);

  SharedAsyncContext AsyncPutObject(const AsyncPutObjectByStreamReq& req);
  SharedAsyncContext AsyncPutObject(const AsyncPutObjectByStreamReq& req, Poco::TaskManager*& taskManager);

  /// \brief
  /// 异步上传对象,封装了初始化分块上传、分块上传、完成分块上传三步，支持断点续传
  /// \param req  MultiPutObjectAsync请求
  /// \return 返回context
  SharedAsyncContext AsyncMultiPutObject(const AsyncMultiPutObjectReq& req);
  SharedAsyncContext AsyncMultiPutObject(const AsyncMultiPutObjectReq& req, Poco::TaskManager*& taskManager);

  /// \brief 异步单线程下载对象到本地
  /// \param req  GetObjectAsync请求
  /// \return 返回context
  SharedAsyncContext AsyncGetObject(const AsyncGetObjectReq& req);
  SharedAsyncContext AsyncGetObject(const AsyncGetObjectReq& req, Poco::TaskManager*& taskManager);

  /// \brief 异步多线程下载对象到本地，支持断点续传
  /// \param req  AsyncResumableGetObject
  /// \return 返回context
  SharedAsyncContext AsyncResumableGetObject(const AsyncGetObjectReq& req);
  SharedAsyncContext AsyncResumableGetObject(const AsyncGetObjectReq& req, Poco::TaskManager*& taskManager);;

  /// \brief 异步多线程下载对象到本地
  /// \param req   MultiGetObjectAsync请求
  /// \return 返回context
  SharedAsyncContext AsyncMultiGetObject(const AsyncMultiGetObjectReq& req);
  SharedAsyncContext AsyncMultiGetObject(const AsyncMultiGetObjectReq& req, Poco::TaskManager*& taskManager);

  /* 批量及目录操作接口 */

  /// \brief 批量上传对象
  /// \param req   PutObjects请求
  /// \param resp  PutObjects响应
  /// \return 批量上传结果
  CosResult PutObjects(const PutObjectsByDirectoryReq& req,
                       PutObjectsByDirectoryResp* resp);

  /// \brief 创建目录
  /// \param req   PutDirectory请求
  /// \param resp  PutDirectory响应
  /// \return 创建目录结果
  CosResult PutDirectory(const PutDirectoryReq& req, PutDirectoryResp* resp);

  /// \brief 移动对象
  /// \param req   MoveObject请求
  /// \return 移动对象结果
  CosResult MoveObject(const MoveObjectReq& req);

  /// \brief 按前缀删除Object
  /// \param req  DeleteObjectsByPrefix请求
  /// \param resp DeleteObjectsByPrefix响应
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteObjects(const DeleteObjectsByPrefixReq& req,
                          DeleteObjectsByPrefixResp* resp);

  /* 数据处理接口 */

  /*** 存储桶绑定万象服务 ***/
  CosResult PutBucketToCI(const PutBucketToCIReq& req,
                          PutBucketToCIResp* resp);

  /** 基础图片处理 **/

  /** 图片持久化处理 **/

  /*** 上传时处理 ***/
  CosResult PutImage(PutImageByFileReq& req, PutImageByFileResp* resp);

  /*** 云上数据处理 ***/
  CosResult CloudImageProcess(const CloudImageProcessReq& req,
                              CloudImageProcessResp* resp);

  /*** 下载图片时识别二维码 ***/
  CosResult GetQRcode(const GetQRcodeReq& req, GetQRcodeResp* resp);

  /** 文档处理接口 **/

  /*** 查询已经开通文档预览功能的 Bucket ***/
  // https://cloud.tencent.com/document/product/436/54057
  CosResult DescribeDocProcessBuckets(const DescribeDocProcessBucketsReq& req,
                                      DescribeDocProcessBucketsResp* resp);

  /*** 预览文档 ***/
  // https://cloud.tencent.com/document/product/436/54058
  CosResult DocPreview(const DocPreviewReq& req, DocPreviewResp* resp);

  /*** 存储桶绑定文档预览 ***/
  CosResult CreateDocBucket(const CreateDocBucketReq& req,
                          CreateDocBucketResp* resp);

  /*** 提交一个文档预览任务 ***/
  // https://cloud.tencent.com/document/product/436/54056
  CosResult CreateDocProcessJobs(const CreateDocProcessJobsReq& req,
                                 CreateDocProcessJobsResp* resp);

  /*** 查询指定的文档预览任务 ***/
  // https://cloud.tencent.com/document/product/436/54095
  CosResult DescribeDocProcessJob(const DescribeDocProcessJobReq& req,
                                  DescribeDocProcessJobResp* resp);

  /*** 拉取符合条件的文档预览任务 ***/
  // https://cloud.tencent.com/document/product/436/54096
  CosResult DescribeDocProcessJobs(const DescribeDocProcessJobsReq& req,
                                   DescribeDocProcessJobsResp* resp);

  /*** 查询文档预览队列 ***/
  // https://cloud.tencent.com/document/product/436/54055
  CosResult DescribeDocProcessQueues(const DescribeDocProcessQueuesReq& req,
                                     DescribeDocProcessQueuesResp* resp);

  /*** 更新文档预览队列 ***/
  // https://cloud.tencent.com/document/product/436/54094
  CosResult UpdateDocProcessQueue(const UpdateDocProcessQueueReq& req,
                                  UpdateDocProcessQueueResp* resp);

  /** 媒体处理接口 **/

  /*** 查询已经开通媒体处理功能的存储桶 ***/
  // https://cloud.tencent.com/document/product/436/48988
  CosResult DescribeMediaBuckets(const DescribeMediaBucketsReq& req,
                                 DescribeMediaBucketsResp* resp);

  /*** 存储桶绑定媒体处理 ***/
  CosResult CreateMediaBucket(const CreateMediaBucketReq& req,
                          CreateMediaBucketResp* resp);

  /*** 获取媒体文件某个时间的截图 ***/
  // https://cloud.tencent.com/document/product/436/55671
  CosResult GetSnapshot(const GetSnapshotReq& req, GetSnapshotResp* resp);

  /*** 获取媒体文件的信息 ***/
  // https://cloud.tencent.com/document/product/436/55672
  CosResult GetMediaInfo(const GetMediaInfoReq& req, GetMediaInfoResp* resp);

  /* 内容审核接口 */

  /** 图片审核 **/

  /*** 图片同步审核 ***/
  // https://cloud.tencent.com/document/product/436/45434
  CosResult GetImageAuditing(const GetImageAuditingReq& req,
                             GetImageAuditingResp* resp);

  /*** 图片批量审核 ***/
  // https://cloud.tencent.com/document/product/436/63593
  CosResult BatchImageAuditing(const BatchImageAuditingReq& req,
                               BatchImageAuditingResp* resp);

  /*** 查询图片审核任务结果 ***/
  // https://cloud.tencent.com/document/product/436/68904
  CosResult DescribeImageAuditingJob(const DescribeImageAuditingJobReq& req,
                                     DescribeImageAuditingJobResp* resp);

  /*** 提交视频审核任务 ***/
  // https://cloud.tencent.com/document/product/436/47316
  CosResult CreateVideoAuditingJob(const CreateVideoAuditingJobReq& req,
                                   CreateVideoAuditingJobResp* resp);

  /*** 查询视频审核任务结果 ***/
  // https://cloud.tencent.com/document/product/436/47317
  CosResult DescribeVideoAuditingJob(const DescribeVideoAuditingJobReq& req,
                               DescribeVideoAuditingJobResp* resp);

  /*** 提交音频审核任务 ***/
  // https://cloud.tencent.com/document/product/436/54063
  CosResult CreateAudioAuditingJob(const CreateAudioAuditingJobReq& req,
                                   CreateAudioAuditingJobResp* resp);

  /*** 查询音频审核任务结果 ***/
  // https://cloud.tencent.com/document/product/436/54064
  CosResult DescribeAudioAuditingJob(const DescribeAudioAuditingJobReq& req,
                               DescribeAudioAuditingJobResp* resp);

  /*** 提交文本审核任务 ***/
  // https://cloud.tencent.com/document/product/436/56289
  CosResult CreateTextAuditingJob(const CreateTextAuditingJobReq& req,
                                   CreateTextAuditingJobResp* resp);

  /*** 查询文本审核任务结果 ***/
  // https://cloud.tencent.com/document/product/436/56288
  CosResult DescribeTextAuditingJob(const DescribeTextAuditingJobReq& req,
                               DescribeTextAuditingJobResp* resp);

  /*** 提交文档审核任务 ***/
  // https://cloud.tencent.com/document/product/436/59381
  CosResult CreateDocumentAuditingJob(const CreateDocumentAuditingJobReq& req,
                                   CreateDocumentAuditingJobResp* resp);

  /*** 查询文档审核任务结果 ***/
  // https://cloud.tencent.com/document/product/436/59382
  CosResult DescribeDocumentAuditingJob(const DescribeDocumentAuditingJobReq& req,
                               DescribeDocumentAuditingJobResp* resp);

  /*** 提交网页审核任务 ***/
  // https://cloud.tencent.com/document/product/436/63958
  CosResult CreateWebPageAuditingJob(const CreateWebPageAuditingJobReq& req,
                                   CreateWebPageAuditingJobResp* resp);

  /*** 查询网页审核任务结果 ***/
  // https://cloud.tencent.com/document/product/436/63959
  CosResult DescribeWebPageAuditingJob(const DescribeWebPageAuditingJobReq& req,
                               DescribeWebPageAuditingJobResp* resp);

 private:
  int CosInit();
  void CosUInit();

 private:
  // Be careful with the m_config order
  SharedConfig m_config;
  ObjectOp m_object_op;    // 内部封装object相关的操作
  BucketOp m_bucket_op;    // 内部封装bucket相关的操作
  ServiceOp m_service_op;  // 内部封装service相关的操作

  static bool s_init;
  static bool s_poco_init;
  static int s_cos_obj_num;
};

}  // namespace qcloud_cos
#endif
