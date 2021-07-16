#ifndef COS_API_H
#define COS_API_H

#include "op/bucket_op.h"
#include "op/cos_result.h"
#include "op/object_op.h"
#include "op/service_op.h"
#include "util/auth_tool.h"
#include "util/codec_util.h"
#include "trsf/transfer_handler.h"

namespace qcloud_cos {

class CosAPI {
public:
    /// \brief CosAPI构造函数
    ///
    /// \param config    cos配置
    explicit CosAPI(CosConfig& config);

    ~CosAPI();

    /// \brief 设置密钥
    void SetCredentail(const std::string& ak, const std::string& sk, const std::string& token);

    /// \brief 获取 Bucket 所在的地域信息
    std::string GetBucketLocation(const std::string& bucket_name);

    /// \brief 生成一个预签名链接
    std::string GeneratePresignedUrl(const GeneratePresignedUrlReq& request);

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
    bool IsObjectExist(const std::string& bucket_name, const std::string& object_name);

    /// \brief 创建一个Bucket
    ///        详见: https://cloud.tencent.com/document/api/436/8291
    ///
    /// \param request  GetService请求
    /// \param response GetService返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetService(const GetServiceReq& request, GetServiceResp* response);

    /// \brief 创建一个Bucket
    ///        详见: https://www.qcloud.com/document/product/436/7738
    ///
    /// \param req  PutBucket请求
    /// \param resp PutBucket返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucket(const PutBucketReq& request, PutBucketResp* response);

    /// \brief 确认Bucket是否存在
    ///        (详见:https://cloud.tencent.com/document/product/436/7735)
    ///
    /// \param req  HeadBucket请求
    /// \param resp HeadBucket返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult HeadBucket(const HeadBucketReq& request, HeadBucketResp* response);

    /// \brief 列出该Bucket下的部分或者全部Object, 需要对Bucket有Read 权限
    ///        详见: https://www.qcloud.com/document/product/436/7734
    ///
    /// \param request   GetBucket请求
    /// \param response  GetBucket返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult GetBucket(const GetBucketReq& request, GetBucketResp* response);

    CosResult ListMultipartUpload(const ListMultipartUploadReq& request,
                                          ListMultipartUploadResp* response); 

    /// \brief 删除Bucket
    ///        详见: https://cloud.tencent.com/document/product/436/7732
    ///
    /// \param req  DeleteBucket请求
    /// \param resp DeleteBucket返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucket(const DeleteBucketReq& request, DeleteBucketResp* response);

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
    CosResult GetBucketReplication(const GetBucketReplicationReq& request,
                                   GetBucketReplicationResp* response);

    /// \brief 增加/替换Bucket下的跨域复制配置
    ///
    /// \param req  PutBucketReplication请求
    /// \param resp PutBucketReplication返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketReplication(const PutBucketReplicationReq& request,
                                   PutBucketReplicationResp* response);

    /// \brief 删除Bucket下的跨域复制配置
    ///
    /// \param req  DeleteBucketReplication请求
    /// \param resp DeleteBucketReplication返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketReplication(const DeleteBucketReplicationReq& request,
                                      DeleteBucketReplicationResp* response);

    /// \brief 列出Bucket下的生命周期配置
    ///
    /// \param req  GetBucketLifecycle请求
    /// \param resp GetBucketLifecycle返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketLifecycle(const GetBucketLifecycleReq& request,
                                 GetBucketLifecycleResp* response);

    /// \brief 增加/替换Bucket下的生命周期配置
    ///
    /// \param req  PutBucketLifecycle请求
    /// \param resp PutBucketLifecycle返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketLifecycle(const PutBucketLifecycleReq& request,
                                 PutBucketLifecycleResp* response);

    /// \brief 删除Bucket下的生命周期配置
    ///
    /// \param req  DeleteBucketLifecycle请求
    /// \param resp DeleteBucketLifecycle返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketLifecycle(const DeleteBucketLifecycleReq& request,
                                    DeleteBucketLifecycleResp* response);

    /// \brief 列出Bucket下的ACL
    ///
    /// \param req  GetBucketACL请求
    /// \param resp GetBucketACL返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketACL(const GetBucketACLReq& request,
                           GetBucketACLResp* response);

    /// \brief 增加/替换Bucket下的ACL, 可以通过Header或者Body传入ACL信息
    ///        注意Header 和 Body 只能选择其中一种，否则响应返回会冲突
    ///
    /// \param req  PutBucketACL请求
    /// \param resp PutBucketACL返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketACL(const PutBucketACLReq& request,
                           PutBucketACLResp* response);

    /// \brief 列出Bucket下的CORS
    ///
    /// \param req  GetBucketCORS请求
    /// \param resp GetBucketCORS返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketCORS(const GetBucketCORSReq& request,
                            GetBucketCORSResp* response);

    /// \brief 增加/替换Bucket下的CORS
    ///
    /// \param req  PutBucketCORS请求
    /// \param resp PutBucketCORS返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketCORS(const PutBucketCORSReq& request,
                            PutBucketCORSResp* response);

    /// \brief 删除Bucket下的CORS
    ///
    /// \param req  DeleteBucketCORS请求
    /// \param resp DeleteBucketCORS返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketCORS(const DeleteBucketCORSReq& request,
                               DeleteBucketCORSResp* response);

    /// \brief 获取对应Object的meta信息数据
    ///        详见: https://www.qcloud.com/document/product/436/7745
    ///
    /// \param request   HeadObject请求
    /// \param response  HeadObject返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult HeadObject(const HeadObjectReq& request, HeadObjectResp* response);

    /// \brief 下载Bucket中的一个文件至流中
    ///        详见: https://www.qcloud.com/document/product/436/7753
    ///
    /// \param request   GetObjectByStream请求
    /// \param response  GetObjectByStream返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult GetObject(const GetObjectByStreamReq& request, GetObjectByStreamResp* response);

    /// \brief 下载Bucket中的一个文件到本地
    ///        详见: https://www.qcloud.com/document/product/436/7753
    ///
    /// \param request   GetObjectByFile请求
    /// \param response  GetObjectByFile返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult GetObject(const GetObjectByFileReq& request, GetObjectByFileResp* response);

    /// \brief 返回对象的URL
    ///
    /// \param bucket  存储桶名
    /// \param object  对象名
    /// \param https   是否使用https
    /// \param region  地域名,默认使用配置文件中的地域名
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    std::string GetObjectUrl(const std::string& bucket, const std::string& object, bool https = true, const std::string& region = "");

    /// \brief 将本地的文件上传至指定Bucket中
    ///        详见: https://www.qcloud.com/document/product/436/7749
    ///
    /// \param request   PutObject请求
    /// \param response  PutObject返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult PutObject(const PutObjectByFileReq& request, PutObjectByFileResp* response);

    /// \brief 将指定流上传至指定Bucket中
    ///        详见: https://www.qcloud.com/document/product/436/7749
    ///
    /// \param request   PutObject请求
    /// \param response  PutObject返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult PutObject(const PutObjectByStreamReq& request, PutObjectByStreamResp* response);

    /// \brief 删除Object
    ///        详见: https://cloud.tencent.com/document/product/436/7743
    ///
    /// \param req  DeleteObject请求
    /// \param resp DeleteObject返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteObject(const DeleteObjectReq& request, DeleteObjectResp* response);

    /// \brief 批量删除Object
    ///
    /// \param req  DeleteObjects请求
    /// \param resp DeleteObjects返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteObjects(const DeleteObjectsReq& request, DeleteObjectsResp* response);

    /// \brief 请求实现初始化分片上传,成功执行此请求以后会返回UploadId用于后续的Upload Part请求
    ///        详见: https://www.qcloud.com/document/product/436/7746
    ///
    /// \param request   InitMultiUpload请求
    /// \param response  InitMultiUpload返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult InitMultiUpload(const InitMultiUploadReq& request, InitMultiUploadResp* response);

    /// \brief 初始化以后的分片上传,支持的分片的数量为1到10000,片的大小为1MB到5GB
    ///        详见: https://www.qcloud.com/document/product/436/7750
    ///
    /// \param request   UploadPartData请求
    /// \param response  UploadPartData返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult UploadPartData(const UploadPartDataReq& request, UploadPartDataResp* response);

    /// \brief 初始化以后的分片上传复制,支持的片的数量为1到10000,片的大小为1MB到5GB
    ///        详见: https://cloud.tencent.com/document/product/436/8287
    ///
    /// \param request   UploadPartCopyData请求
    /// \param response  UploadPartCopyData返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult UploadPartCopyData(const UploadPartCopyDataReq& request, UploadPartCopyDataResp* response);

    /// \brief 完成整个分块上传。当使用 Upload Parts 上传完所有块以后，
    ///        必须调用该 API 来完成整个文件的分块上传
    ///        详见: https://www.qcloud.com/document/product/436/7742
    ///
    /// \param request   CompleteMultiUpload请求
    /// \param response  CompleteMultiUpload返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult CompleteMultiUpload(const CompleteMultiUploadReq& request,
                                  CompleteMultiUploadResp* response);

    /// \brief 舍弃一个分块上传并删除已上传的块
    ///        详见: https://www.qcloud.com/document/product/436/7740
    ///
    /// \param request  AbortMultiUpload请求
    /// \param response AbortMultiUpload返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult AbortMultiUpload(const AbortMultiUploadReq& request,
                               AbortMultiUploadResp* response);

    /// \brief 查询特定分块上传中的已上传的块
    ///
    /// \param req  ListParts请求
    /// \param resp ListParts返回
    ///
    /// \return
    CosResult ListParts(const ListPartsReq& request, ListPartsResp* response);

    /// \brief 列出Object下的ACL
    ///
    /// \param req  GetObjectACL请求
    /// \param resp GetObjectACL返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetObjectACL(const GetObjectACLReq& request,
                           GetObjectACLResp* response);

    /// \brief 增加/替换Object下的ACL, 可以通过Header或者Body传入ACL信息
    ///        注意Header 和 Body 只能选择其中一种，否则响应返回会冲突
    ///
    /// \param req  PutObjectACL请求
    /// \param resp PutObjectACL返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutObjectACL(const PutObjectACLReq& request,
                           PutObjectACLResp* response);

    /// \brief 复制Object, 适用于跨园区且Object小于5G
    ///
    /// \param req  PutObjectCopy请求
    /// \param resp PutObjectCopy返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutObjectCopy(const PutObjectCopyReq& request,
                            PutObjectCopyResp* response);

    /// \brief 复制Object, 自动判断源Object大小,调用PutObjectCopy/UploadPartCopyData
    ///
    /// \param req  Copy请求
    /// \param resp Copy返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult Copy(const CopyReq& request, CopyResp* response);

    /// \brief 对一个通过 COS 归档为 archive 类型的对象进行恢复
    ///
    /// \param request   PostObjectRestore请求
    /// \param response  PostObjectRestore返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult PostObjectRestore(const PostObjectRestoreReq& request,
                                PostObjectRestoreResp* response);

    /// \brief 实现Object跨域访问请求的预请求，参考https://cloud.tencent.com/document/product/436/8288
    ///
    /// \param req  OptionsObjectReq 请求
    /// \param resp OptionsObjectResp 响应
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult OptionsObject(const OptionsObjectReq& request,
                                 OptionsObjectResp* response);

    /// \brief 支持SELECT 接口，参考https://cloud.tencent.com/document/product/436/37641
    CosResult SelectObjectContent(const SelectObjectContentReq& request,
                                 SelectObjectContentResp* response);

    /// \brief 列出Bucket下的部分或者全部Object(包括多版本)
    ///
    /// \param req  GetBucketObjectVersions请求
    /// \param resp GetBucketObjectVersions返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketObjectVersions(const GetBucketObjectVersionsReq& request,
                                      GetBucketObjectVersionsResp* response);

    /// \brief 设置源存储桶的日志配置信息。
    ///
    /// \param req  PutBucketLogging请求
    /// \param resp PutBucketLogging返回
    ///
    /// \return 本次请求的调用情况(如状态码等)

    CosResult PutBucketLogging(const PutBucketLoggingReq& request, 
                                     PutBucketLoggingResp* response);
    
    /// \brief 获取源存储桶的日志配置信息。
    ///
    /// \param req  GetBucketLogging请求
    /// \param resp GetBucketLogging返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketLogging(const GetBucketLoggingReq& request, 
                                     GetBucketLoggingResp* response);

    
    /// \brief 设置存储桶自定义域名。
    ///
    /// \param req  PutBucketDomain请求
    /// \param resp PutBucketDomain返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketDomain(const PutBucketDomainReq& request,
                                    PutBucketDomainResp* response);
    
    
    /// \brief 获取存储桶自定义域名。
    ///
    /// \param req  GetBucketDomain请求
    /// \param resp GetBucketDomain返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketDomain(const GetBucketDomainReq& request,
                                    GetBucketDomainResp* response);	
									
	   
    /// \brief 为存储桶配置静态网站,可以通过传入 XML 格式的配置文件进行配置,文件大小限制为64KB.
    ///
    /// \param req  PutBucketWebsite请求
    /// \param resp PutBucketWebsite返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketWebsite(const PutBucketWebsiteReq& request,
                                    PutBucketWebsiteResp* response);	
	
    /// \brief 请求用于获取与存储桶关联的静态网站配置信息.
    ///
    /// \param req  GetBucketWebsite请求
    /// \param resp GetBucketWebsite返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketWebsite(const GetBucketWebsiteReq& request,
                                    GetBucketWebsiteResp* response);
											
    /// \brief 删除存储桶中的静态网站配置.
    ///
    /// \param req  DeleteBucketWebsite请求
    /// \param resp DeleteBucketWebsite返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketWebsite(const DeleteBucketWebsiteReq& request,
                                    DeleteBucketWebsiteResp* response);
									
									
    /// \brief 已存在的Bucket设置标签.
    ///
    /// \param req  PutBucketTagging请求
    /// \param resp PutBucketTagging返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketTagging(const PutBucketTaggingReq& request,
                                    PutBucketTaggingResp* response);
									
									
    /// \brief 查询指定存储桶下已有的存储桶标签.
    ///
    /// \param req  GetBucketTagging请求
    /// \param resp GetBucketTagging返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketTagging(const GetBucketTaggingReq& request,
                                    GetBucketTaggingResp* response);
									
									
    /// \brief 删除指定存储桶下已有的存储桶标签.
    ///
    /// \param req  DeleteBucketTagging请求
    /// \param resp DeleteBucketTagging返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketTagging(const DeleteBucketTaggingReq& request,
                                    DeleteBucketTaggingResp* response);
								
									
    /// \brief 在存储桶中创建清单任务.
    ///
    /// \param req  PutBucketInventory请求
    /// \param resp PutBucketInventory返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketInventory(const PutBucketInventoryReq& request,
                                    PutBucketInventoryResp* response);
									
     /// \brief 用于查询存储桶中用户的清单任务信息.
    ///
    /// \param req  GetBucketInventory请求
    /// \param resp GetBucketInventory返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketInventory(const GetBucketInventoryReq& request,
                                    GetBucketInventoryResp* response);
									
    /// \brief 用于请求返回一个存储桶中的所有清单任务.
    ///
    /// \param req  ListBucketInventoryConfigurations请求
    /// \param resp ListBucketInventoryConfigurations返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult ListBucketInventoryConfigurations(const ListBucketInventoryConfigurationsReq& request,
                                    ListBucketInventoryConfigurationsResp* response);
									
    /// \brief 用于删除存储桶中指定的清单任务.
    ///
    /// \param req  DeleteBucketinventory请求
    /// \param resp DeleteBucketinventory返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketInventory(const DeleteBucketInventoryReq& request,
                                    DeleteBucketInventoryResp* response);

    /// \brief 创建推流通道
    ///
    /// \param req  PutLiveChannelReq请求
    /// \param resp PutLiveChannelResp返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutLiveChannel(const PutLiveChannelReq& request,
                            PutLiveChannelResp* response);

    /// \brief 生成推流通道带签名的推流url
    ///
    /// \param bucket 存储桶名称
    /// \param channel 通道名
    /// \param expire 签名过期时间
    /// \param url_params url参数
    ///
    /// \return 带签名的推流url
    std::string GetRtmpSignedPublishUrl(const std::string& bucket, const std::string& channel,
                                        int expire, const std::map<std::string, std::string> url_params);

    /// \brief 启用或者禁用直播通道
    ///
    /// \param req  PutLiveChannelSwitchReq请求
    /// \param resp PutLiveChannelSwitchResp返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutLiveChannelSwitch(const PutLiveChannelSwitchReq& request,
                                    PutLiveChannelSwitchResp* response);

    /// \brief 获取直播通道配置信息
    ///
    /// \param req  GetLiveChannelReq请求
    /// \param resp GetLiveChannelResp返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetLiveChannel(const GetLiveChannelReq& request,
                                  GetLiveChannelResp* response);

    /// \brief 获取直播通道推流历史
    ///
    /// \param req  GetLiveChannelHistoryReq请求
    /// \param resp GetLiveChannelHistoryResp返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetLiveChannelHistory(const GetLiveChannelHistoryReq& request,
                                           GetLiveChannelHistoryResp* response);

    /// \brief 获取直播通道推流状态
    ///
    /// \param req  GetLiveChannelStatusReq请求
    /// \param resp GetLiveChannelStatusResp返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetLiveChannelStatus(const GetLiveChannelStatusReq& request,
                                           GetLiveChannelStatusResp* response);

    /// \brief 删除直播通道
    ///
    /// \param req  GetLiveChannelStatusReq请求
    /// \param resp GetLiveChannelStatusResp返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteLiveChannel(const DeleteLiveChannelReq& request,
                                    DeleteLiveChannelResp* response);

    /// \brief 查询指定通道在指定时间段推流生成的播放列表
    ///
    /// \param req  GetLiveChannelVodPlaylistReq请求
    /// \param resp GetLiveChannelVodPlaylistResp返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetLiveChannelVodPlaylist(const GetLiveChannelVodPlaylistReq& request,
                                        GetLiveChannelVodPlaylistResp* response);

    /// \brief 为指定通道生成一个可供点播例用的播放列
    ///
    /// \param req  PostLiveChannelVodPlaylistReq请求
    /// \param resp PostLiveChannelVodPlaylistResp返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PostLiveChannelVodPlaylist(const PostLiveChannelVodPlaylistReq& request,
                                        PostLiveChannelVodPlaylistResp* response);

    /// \brief 列举通道
    ///
    /// \param req  PostLiveChannelVodPlaylistReq请求
    /// \param resp PostLiveChannelVodPlaylistResp返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult ListLiveChannel(const ListLiveChannelReq& request,
                                   ListLiveChannelResp* response);

    /// \brief 配置存储桶智能分层
    ///
    /// \param req  PutBucketIntelligentTiering请求
    /// \param resp PutBucketIntelligentTiering返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketIntelligentTiering(const PutBucketIntelligentTieringReq& request,
                                                PutBucketIntelligentTieringResp* response);

    /// \brief 获取存储桶智能分层配置
    ///
    /// \param req  GetBucketIntelligentTiering请求
    /// \param resp GetBucketIntelligentTiering返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketIntelligentTiering(const GetBucketIntelligentTieringReq& request,
                                                GetBucketIntelligentTieringResp* response);

    /*Multithread接口*/

    /// \brief 封装了初始化分块上传、分块上传、完成分块上传三步，支持断点续传
    ///
    /// \param request   MultiUploadObject请求
    /// \param response  MultiUploadObject返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult MultiUploadObject(const MultiUploadObjectReq& request,
                                MultiUploadObjectResp* response);

    /// \brief 多线程Range下载Bucket中的一个文件到本地
    ///        详见: https://www.qcloud.com/document/product/436/7753
    ///
    /// \param request   MultiGetObject请求
    /// \param response  MultiGetObject返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult GetObject(const MultiGetObjectReq& request, MultiGetObjectResp* response);

    /*Resumable接口*/

    /// \brief 封装了初始化分块上传、分块上传、完成分块上传三步，支持断点续传
    // CosResult ResumablePutObject(const MultiUploadObjectReq& request, MultiUploadObjectResp* response);

    /// \brief 支持断点下载
    CosResult ResumableGetObject(const MultiGetObjectReq& request, MultiGetObjectResp* response);

    /*Async接口*/
    
    /// \brief 异步上传对象,封装了初始化分块上传、分块上传、完成分块上传三步，支持断点续传
    /// \param request   MultiUploadObject请求
    /// \param response  MultiUploadObject返回
    /// \return 返回handler
    SharedTransferHandler PutObjectAsync(const MultiUploadObjectReq& request,
                                             MultiUploadObjectResp* response);

    /// \brief 异步多线程下载对象，支持更新下载进度
    /// \param request   MultiGetObject请求
    /// \param response  MultiGetObject返回
    /// \return 返回handler
    SharedTransferHandler GetObjectAsync(const MultiGetObjectReq& request,
                                             MultiGetObjectResp* response);

    /*批量及目录操作接口*/

    /// \brief 批量上传对象
    /// \param request   PutObjectsByDirectory请求
    /// \param response  PutObjectByDirectoryResp返回
    /// \return 批量上传结果
    CosResult PutObjects(const PutObjectsByDirectoryReq& request, PutObjectsByDirectoryResp* response);

    /// \brief 创建目录
    /// \param request   PutObjectsByDirectory请求
    /// \param response  PutObjectByDirectoryResp返回
    /// \return 创建目录结果
    CosResult PutDirectory(const PutDirectoryReq& request, PutDirectoryResp* response);

    /// \brief 移动对象
    /// \param request   MoveObjectReq请求
    /// \param response  MoveObjectResp返回
    /// \return 移动对象结果
    CosResult MoveObject(const MoveObjectReq& request, MoveObjectResp* response);

    /// \brief 按前缀删除Object
    /// \param req  DeleteObjectsByPrefixReq请求
    /// \param resp DeleteObjectsByPrefixResp返回
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteObjects(const DeleteObjectsByPrefixReq& request, DeleteObjectsByPrefixResp* response);

    /*数据处理接口*/

    /**基础图片处理**/

    /**图片持久化处理**/

    /***上传时处理***/
    CosResult PutImage(const PutImageByFileReq& request, PutImageByFileResp* response);

    /***云上数据处理***/
    CosResult CloudImageProcess(const CloudImageProcessReq& request, CloudImageProcessResp* response);

    /***下载图片时识别二维码***/
    CosResult GetQRcode(const GetQRcodeReq& request, GetQRcodeResp* response);


    /*文档处理接口*/

    /**查询已经开通文档预览功能的 Bucket**/
    // https://cloud.tencent.com/document/product/436/54057
    CosResult DescribeDocProcessBuckets(const DescribeDocProcessBucketsReq& request, DescribeDocProcessBucketsResp *response);

    /**预览文档**/
    // https://cloud.tencent.com/document/product/436/54058
    CosResult DocPreview(const DocPreviewReq& request, DocPreviewResp *response);

    /**提交一个文档预览任务**/
    // https://cloud.tencent.com/document/product/436/54056
    CosResult CreateDocProcessJobs(const CreateDocProcessJobsReq& request, CreateDocProcessJobsResp *response);

    /**查询指定的文档预览任务**/
    // https://cloud.tencent.com/document/product/436/54095
    CosResult DescribeDocProcessJob(const DescribeDocProcessJobReq& request, DescribeDocProcessJobResp *response);

    /**拉取符合条件的文档预览任务**/
    // https://cloud.tencent.com/document/product/436/54096
    CosResult DescribeDocProcessJobs(const DescribeDocProcessJobsReq& request, DescribeDocProcessJobsResp *response);

    /**查询文档预览队列**/
    // https://cloud.tencent.com/document/product/436/54055
    CosResult DescribeDocProcessQueues(const DescribeDocProcessQueuesReq& request, DescribeDocProcessQueuesResp *response);

    /**更新文档预览队列**/
    // https://cloud.tencent.com/document/product/436/54094
    CosResult UpdateDocProcessQueue(const UpdateDocProcessQueueReq& request, UpdateDocProcessQueueResp *response);

private:
    int CosInit();
    void CosUInit();

private:
    // Be careful with the m_config order
    SharedConfig m_config;
    ObjectOp m_object_op; // 内部封装object相关的操作
    BucketOp m_bucket_op; // 内部封装bucket相关的操作
    ServiceOp m_service_op; // 内部封装service相关的操作

    static bool s_init;
    static bool s_poco_init;
    static int s_cos_obj_num;
};

} // namespace qcloud_cos
#endif
