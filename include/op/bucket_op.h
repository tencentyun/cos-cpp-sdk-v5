// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/13/17
// Description:

#pragma once

#include "cos_sys_config.h"
#include "op/base_op.h"
#include "op/cos_result.h"
#include "request/bucket_req.h"
#include "request/data_process_req.h"
#include "request/auditing_req.h"
#include "response/bucket_resp.h"
#include "response/data_process_resp.h"
#include "response/auditing_resp.h"

namespace qcloud_cos {

/// \brief 封装了Bucket相关的操作
class BucketOp : public BaseOp {
 public:
  /// \brief BucketOp构造函数
  ///
  /// \param cos_conf Cos配置
  explicit BucketOp(const SharedConfig& config) : BaseOp(config) {}

  /// \brief BucketOp析构函数
  virtual ~BucketOp() {}

  /// \brief 判断bucket是否存在
  bool IsBucketExist(const std::string& bucket_name);

  /// \brief 获取Bucket所在Location
  std::string GetBucketLocation(const std::string& bucket_name);

  /// \brief 创建一个Bucket
  ///        (详见:https://www.qcloud.com/document/product/436/7738)
  ///
  /// \param req  PutBucket请求
  /// \param resp PutBucket返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucket(const PutBucketReq& req, PutBucketResp* resp,
                        bool change_backup_domain = false);

  /// \brief 确认Bucket是否存在
  ///        (详见:https://cloud.tencent.com/document/product/436/7735)
  ///
  /// \param req  HeadBucket请求
  /// \param resp HeadBucket返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult HeadBucket(const HeadBucketReq& req, HeadBucketResp* resp,
                              bool change_backup_domain = false);

  /// \brief 列出Bucket下的部分或者全部Object
  ///        (详见:https://www.qcloud.com/document/product/436/7734)
  ///
  /// \param req  GetBucket请求
  /// \param resp GetBucket返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucket(const GetBucketReq& req, GetBucketResp* resp,
                          bool change_backup_domain = false);

  CosResult ListMultipartUpload(const ListMultipartUploadReq& req,
                                ListMultipartUploadResp* resp,
                                bool change_backup_domain = false);
  /// \brief 删除Bucket
  ///
  /// \param req  DeleteBucket请求
  /// \param resp DeleteBucket返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucket(const DeleteBucketReq& req, DeleteBucketResp* resp,
                            bool change_backup_domain = false);

  /// \brief 获得存储桶的版本控制信息
  ///        (详见:https://cloud.tencent.com/document/product/436/8597)
  ///
  /// \param req  GetBucketVersioning请求
  /// \param resp GetBucketVersioning返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketVersioning(const GetBucketVersioningReq& req,
                                GetBucketVersioningResp* resp,
                                bool change_backup_domain = false);

  /// \brief 启用或者暂停存储桶的版本控制功能
  ///        (详见:https://cloud.tencent.com/document/product/436/8591)
  ///
  /// \param req  PutBucketVersioning请求
  /// \param resp PutBucketVersioning返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketVersioning(const PutBucketVersioningReq& req,
                                PutBucketVersioningResp* resp,
                                bool change_backup_domain = false);

  /// \brief 列出Bucket下的跨域复制配置
  ///
  /// \param req  GetBucketReplication请求
  /// \param resp GetBucketReplication返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketReplication(const GetBucketReplicationReq& req,
                                 GetBucketReplicationResp* resp,
                                 bool change_backup_domain = false);

  /// \brief 增加/替换Bucket下的跨域复制配置
  ///
  /// \param req  PutBucketReplication请求
  /// \param resp PutBucketReplication返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketReplication(const PutBucketReplicationReq& req,
                                 PutBucketReplicationResp* resp,
                                 bool change_backup_domain = false);

  /// \brief 删除Bucket下的跨域复制配置
  ///
  /// \param req  DeleteBucketReplication请求
  /// \param resp DeleteBucketReplication返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketReplication(const DeleteBucketReplicationReq& req,
                                    DeleteBucketReplicationResp* resp,
                                    bool change_backup_domain = false);

  /// \brief 列出Bucket下的生命周期配置
  ///
  /// \param req  GetBucketLifecycle请求
  /// \param resp GetBucketLifecycle返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketLifecycle(const GetBucketLifecycleReq& req,
                               GetBucketLifecycleResp* resp,
                               bool change_backup_domain = false);

  /// \brief 增加/替换Bucket下的生命周期配置
  ///
  /// \param req  PutBucketLifecycle请求
  /// \param resp PutBucketLifecycle返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketLifecycle(const PutBucketLifecycleReq& req,
                               PutBucketLifecycleResp* resp,
                               bool change_backup_domain = false);

  /// \brief 删除Bucket下的生命周期配置
  ///
  /// \param req  DeleteBucketLifecycle请求
  /// \param resp DeleteBucketLifecycle返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketLifecycle(const DeleteBucketLifecycleReq& req,
                                  DeleteBucketLifecycleResp* resp,
                                  bool change_backup_domain = false);

  /// \brief 列出Bucket下的ACL
  ///
  /// \param req  GetBucketACL请求
  /// \param resp GetBucketACL返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketACL(const GetBucketACLReq& req, GetBucketACLResp* resp,
                                bool change_backup_domain = false);

  /// \brief 增加/替换Bucket下的ACL, 可以通过Header或者Body传入ACL信息
  ///        注意Header 和 Body 只能选择其中一种，否则响应返回会冲突
  ///
  /// \param req  PutBucketACL请求
  /// \param resp PutBucketACL返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketACL(const PutBucketACLReq& req, PutBucketACLResp* resp,
                            bool change_backup_domain = false);

  /// \brief 获取Bucket的权限策略
  ///
  /// \param req  GetBucketPolicy请求
  /// \param resp GetBucketPolicy返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketPolicy(const GetBucketPolicyReq& req, GetBucketPolicyResp* resp,
                                bool change_backup_domain = false);

  /// \brief 写入/替换Bucket的权限策略，通过Body传入
  ///
  /// \param req  PutBucketPolicy请求
  /// \param resp PutBucketPolicy返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketPolicy(const PutBucketPolicyReq& req, PutBucketPolicyResp* resp,
                                bool change_backup_domain = false);

  /// \brief 删除Bucket的权限策略
  ///
  /// \param req  DeleteBucketPolicy请求
  /// \param resp DeleteBucketPolicy返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketPolicy(const DeleteBucketPolicyReq& req, DeleteBucketPolicyResp* resp,
                                    bool change_backup_domain = false);

  /// \brief 列出Bucket下的CORS
  ///
  /// \param req  GetBucketCORS请求
  /// \param resp GetBucketCORS返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketCORS(const GetBucketCORSReq& req, GetBucketCORSResp* resp,
                                bool change_backup_domain = false);

  /// \brief 增加/替换Bucket下的CORS
  ///
  /// \param req  PutBucketCORS请求
  /// \param resp PutBucketCORS返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketCORS(const PutBucketCORSReq& req, PutBucketCORSResp* resp,
                            bool change_backup_domain = false);

  /// \brief 删除Bucket下的CORS
  ///
  /// \param req  DeleteBucketCORS请求
  /// \param resp DeleteBucketCORS返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketCORS(const DeleteBucketCORSReq& req,
                             DeleteBucketCORSResp* resp,
                             bool change_backup_domain = false);

  /// \brief 列出Bucket下的部分或者全部Object(包括多版本)
  ///
  /// \param req  GetBucketObjectVersions请求
  /// \param resp GetBucketObjectVersions返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketObjectVersions(const GetBucketObjectVersionsReq& req,
                                    GetBucketObjectVersionsResp* resp,
                                    bool change_backup_domain = false);

  /// \brief 获取Bucket所在Location
  ///
  /// \param req  GetBucketLocation请求
  /// \param resp GetBucketLocation返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketLocation(const GetBucketLocationReq& req,
                              GetBucketLocationResp* resp,
                              bool change_backup_domain = false);
  // TODO(sevenyou)
  // std::string ListMultipartUploads();

  /// \brief 为源存储桶开启日志记录
  /// \brief https://cloud.tencent.com/document/product/436/17054
  /// \param req  PutBucketLogging请求
  /// \param resp PutBucketLogging返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketLogging(const PutBucketLoggingReq& req,
                             PutBucketLoggingResp* resp,
                             bool change_backup_domain = false);

  /// \brief 用于获取源存储桶的日志配置信息
  /// \brief https://cloud.tencent.com/document/product/436/17053
  /// \param req  GetBucketLogging请求
  /// \param resp GetBucketLogging返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketLogging(const GetBucketLoggingReq& req,
                             GetBucketLoggingResp* resp,
                             bool change_backup_domain = false);

  /// \brief 用于存储桶绑定自定义域名
  /// \brief
  /// \param req  PutBucketDomain请求
  /// \param resp PutBucketDomain返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketDomain(const PutBucketDomainReq& req,
                            PutBucketDomainResp* resp,
                            bool change_backup_domain = false);

  /// \brief 用于获取存储桶绑定自定义域名
  /// \brief
  /// \param req  GetBucketDomain请求
  /// \param resp GetBucketDomain返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketDomain(const GetBucketDomainReq& req,
                            GetBucketDomainResp* resp,
                            bool change_backup_domain = false);

  /// \brief 存储桶配置静态网站，通过传入 XML
  /// 格式的配置文件进行配置，文件大小限制为64KB \brief
  /// https://cloud.tencent.com/document/product/436/31930 \param req
  /// PutBucketWebsite请求 \param resp PutBucketWebsite返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketWebsite(const PutBucketWebsiteReq& req,
                             PutBucketWebsiteResp* resp,
                             bool change_backup_domain = false);

  /// \brief 获取与存储桶关联的静态网站配置信息.
  /// \brief https://cloud.tencent.com/document/product/436/31929
  /// \param req  GetBucketWebsite请求
  /// \param resp GetBucketWebsite返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketWebsite(const GetBucketWebsiteReq& req,
                             GetBucketWebsiteResp* resp,
                             bool change_backup_domain = false);

  /// \brief 删除存储桶中的静态网站配置.
  /// \brief https://cloud.tencent.com/document/product/436/31928
  /// \param req  DeleteBucketWebsite请求
  /// \param resp DeleteBucketWebsite返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketWebsite(const DeleteBucketWebsiteReq& req,
                                DeleteBucketWebsiteResp* resp,
                                bool change_backup_domain = false);

  /// \brief 为已存在的Bucket设置标签.
  /// \brief https://cloud.tencent.com/document/product/436/34838
  /// \param req  PutBucketTagging请求
  /// \param resp PutBucketTagging返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketTagging(const PutBucketTaggingReq& req,
                             PutBucketTaggingResp* resp,
                             bool change_backup_domain = false);

  /// \brief 查询指定存储桶下已有的存储桶标签.
  /// \brief https://cloud.tencent.com/document/product/436/34837
  /// \param req  GetBucketTagging请求
  /// \param resp GetBucketTagging返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketTagging(const GetBucketTaggingReq& req,
                             GetBucketTaggingResp* resp,
                             bool change_backup_domain = false);

  /// \brief 删除指定存储桶下已有的存储桶标签.
  /// \brief https://cloud.tencent.com/document/product/436/34836
  /// \param req  DeleteBucketTagging请求
  /// \param resp DeleteBucketTagging返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketTagging(const DeleteBucketTaggingReq& req,
                                DeleteBucketTaggingResp* resp,
                                bool change_backup_domain = false);

  /// \brief 用于在存储桶中创建清单任务.
  /// \brief https://cloud.tencent.com/document/product/436/33707
  /// \param req  PutBucketinventory请求
  /// \param resp PutBucketinventory返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketInventory(const PutBucketInventoryReq& req,
                               PutBucketInventoryResp* resp,
                               bool change_backup_domain = false);

  /// \brief 查询存储桶中用户的清单任务信息.
  /// \brief https://cloud.tencent.com/document/product/436/33705
  /// \param req  GetBucketinventory请求
  /// \param resp GetBucketinventory返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketInventory(const GetBucketInventoryReq& req,
                               GetBucketInventoryResp* resp,
                               bool change_backup_domain = false);

  /// \brief 请求返回一个存储桶中的所有清单任务.
  /// \brief https://cloud.tencent.com/document/product/436/33706
  /// \param req  ListBucketInventoryConfigurations请求
  /// \param resp ListBucketInventoryConfigurations返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult ListBucketInventoryConfigurations(
      const ListBucketInventoryConfigurationsReq& req,
      ListBucketInventoryConfigurationsResp* resp,
      bool change_backup_domain = false);

  /// \brief 用于删除存储桶中指定的清单任务.
  /// \brief https://cloud.tencent.com/document/product/436/33704
  /// \param req  DeleteBucketinventory请求
  /// \param resp DeleteBucketinventory返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DeleteBucketInventory(const DeleteBucketInventoryReq& req,
                                  DeleteBucketInventoryResp* resp,
                                  bool change_backup_domain = false);
  /// \brief 用于为存储桶设置 Referer 白名单或者黑名单.
  /// \brief https://cloud.tencent.com/document/product/436/32492
  /// \param req  PutBucketReferer请求
  /// \param resp PutBucketReferer响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketReferer(const PutBucketRefererReq& req,
                             PutBucketRefererResp* resp,
                             bool change_backup_domain = false);
  /// \brief 读取存储桶 Referer 白名单或者黑名单.
  /// \brief https://cloud.tencent.com/document/product/436/32493
  /// \param req  GetBucketReferer请求
  /// \param resp GetBucketReferer响应
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketReferer(const GetBucketRefererReq& req,
                             GetBucketRefererResp* resp,
                             bool change_backup_domain = false);
  /// \brief 列举直播通道
  /// \param req  ListLiveChannelReq请求
  /// \param resp ListLiveChannelResp返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult ListLiveChannel(const ListLiveChannelReq& req,
                            ListLiveChannelResp* resp,
                            bool change_backup_domain = false);

  /// \brief 配置存储桶智能分层
  ///
  /// \param req  PutBucketIntelligentTiering请求
  /// \param resp PutBucketIntelligentTiering返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketIntelligentTiering(
      const PutBucketIntelligentTieringReq& req,
      PutBucketIntelligentTieringResp* resp,
      bool change_backup_domain = false);

  /// \brief 获取存储桶智能分层配置
  ///
  /// \param req  GetBucketIntelligentTiering请求
  /// \param resp GetBucketIntelligentTiering返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult GetBucketIntelligentTiering(
      const GetBucketIntelligentTieringReq& req,
      GetBucketIntelligentTieringResp* resp,
      bool change_backup_domain = false);

  /// \brief 存储桶绑定ci
  ///
  /// \param req  PutBucketToCIReq请求
  /// \param resp PutBucketToCIResp返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult PutBucketToCI(const PutBucketToCIReq& req,
                             PutBucketToCIResp* resp);

  /// \brief 开通文档预览
  /// \param req  CreateDocBucket请求
  /// \param resp CreateDocBucket返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult CreateDocBucket(const CreateDocBucketReq& req,
                          CreateDocBucketResp* resp);

  /// \brief 提交文档转码任务
  /// \brief https://cloud.tencent.com/document/product/436/54056
  /// \param req  CreateDocProcessJobs请求
  /// \param resp CreateDocProcessJobs返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult CreateDocProcessJobs(const CreateDocProcessJobsReq& req,
                                 CreateDocProcessJobsResp* resp);

  /// \brief 查询指定的文档转码任务
  /// \brief https://cloud.tencent.com/document/product/436/54095
  /// \param req  DescribeDocProcessJob请求
  /// \param resp DescribeDocProcessJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DescribeDocProcessJob(const DescribeDocProcessJobReq& req,
                                  DescribeDocProcessJobResp* resp);

  /// \brief 拉取符合条件的的文档转码任务
  /// \brief https://cloud.tencent.com/document/product/436/54096
  /// \param req  DescribeDocProcessJobs请求
  /// \param resp DescribeDocProcessJobs返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DescribeDocProcessJobs(const DescribeDocProcessJobsReq& req,
                                   DescribeDocProcessJobsResp* resp);

  /// \brief 查询文档转码队列
  /// \brief https://cloud.tencent.com/document/product/436/54055
  /// \param req  DescribeDocProcessQueues请求
  /// \param resp DescribeDocProcessQueues返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DescribeDocProcessQueues(const DescribeDocProcessQueuesReq& req,
                                     DescribeDocProcessQueuesResp* resp);
  /// \brief 更新文档转码队列
  /// \brief https://cloud.tencent.com/document/product/436/54094
  /// \param req  UpdateDocProcessQueue请求
  /// \param resp UpdateDocProcessQueue返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult UpdateDocProcessQueue(const UpdateDocProcessQueueReq& req,
                                  UpdateDocProcessQueueResp* resp);

  /// \brief 查询媒体处理开通状态
  /// \brief https://cloud.tencent.com/document/product/436/48988
  /// \param req  DescribeMediaBuckets请求
  /// \param resp DescribeMediaBuckets返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DescribeMediaBuckets(const DescribeMediaBucketsReq& req,
                                 DescribeMediaBucketsResp* resp);

  /// \brief 开通媒体处理
  /// \brief https://cloud.tencent.com/document/product/436/72824
  /// \param req  CreateMediaBucketReq请求
  /// \param resp CreateMediaBucketResp返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult CreateMediaBucket(const CreateMediaBucketReq& req,
                          CreateMediaBucketResp* resp);

  /// \brief 获取媒体文件信息
  /// \brief https://cloud.tencent.com/document/product/436/55672
  /// \param req  GetMediainfo请求
  /// \param resp GetMediainfo返回
  ///
  /// \return 本次请求的调用情况(如状态码等)                                 
  CosResult GetMediainfo(const GetMediaInfoReq& req, GetMediaInfoResp* resp);

  /// \brief 搜索媒体处理队列
  /// \brief https://cloud.tencent.com/document/product/436/54045
  /// \param req  DescribeMediaQueues请求
  /// \param resp DescribeMediaQueues返回
  ///
  /// \return 本次请求的调用情况(如状态码等)                                 
  CosResult DescribeMediaQueues(const DescribeMediaQueuesReq& req, DescribeQueuesResp* resp);

  /// \brief 更新媒体处理队列
  /// \brief https://cloud.tencent.com/document/product/436/54046
  /// \param req  UpdateMediaQueue请求
  /// \param resp UpdateMediaQueue返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult UpdateMediaQueue(const UpdateMediaQueueReq& req,
                                  UpdateQueueResp* resp);

  /// \brief 查询文件处理开通状态
  /// \brief https://cloud.tencent.com/document/product/460/95747
  /// \param req  DescribeFileBuckets请求
  /// \param resp DescribeFileBuckets返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DescribeFileBuckets(const DescribeFileBucketsReq& req,
                                 DescribeFileBucketsResp* resp);

  /// \brief 开通文件处理
  /// \brief https://cloud.tencent.com/document/product/460/86377
  /// \param req  CreateFileBucketReq请求
  /// \param resp CreateFileBucketResp返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult CreateFileBucket(const CreateFileBucketReq& req,
                          CreateFileBucketResp* resp);

  /// \brief 提交数据处理任务
  /// \brief https://cloud.tencent.com/document/product/436/83110
  /// \param req  CreateFileProcessJobs请求
  /// \param resp CreateFileProcessJobs返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult CreateDataProcessJobs(const CreateDataProcessJobsReq& req,
                                 CreateDataProcessJobsResp* resp);

  /// \brief 取消数据处理任务
  /// \brief https://cloud.tencent.com/document/product/436/85082
  /// \param req  CancelFileProcessJobs请求
  /// \param resp CancelFileProcessJobs返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult CancelDataProcessJob(const CancelDataProcessJobReq& req,
                                 CancelDataProcessJobResp* resp);

  /// \brief 提交数据处理任务
  /// \param req  DescribeDataProcessJobs请求
  /// \param resp DescribeDataProcessJobs返回
  ///
  /// \return 本次请求的调用情况(如状态码等)
  CosResult DescribeDataProcessJob(const DescribeDataProcessJobReq& req,
                                 DescribeDataProcessJobResp* resp);                       
  /// \brief 图片批量审核
  /// \brief https://cloud.tencent.com/document/product/436/63593
  /// \param req  BatchImageAuditing请求
  /// \param resp BatchImageAuditing返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult BatchImageAuditing(const BatchImageAuditingReq& req,
                               BatchImageAuditingResp* resp);

  /// \brief 查询图片审核任务结果
  /// \brief https://cloud.tencent.com/document/product/436/68904
  /// \param req  DescribeImageAuditingJob请求
  /// \param resp DescribeImageAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult DescribeImageAuditingJob(const DescribeImageAuditingJobReq& req,
                                     DescribeImageAuditingJobResp* resp);                               

  /// \brief 提交视频审核任务
  /// \brief https://cloud.tencent.com/document/product/436/47316
  /// \param req  DescribeVideoAuditingJob请求
  /// \param resp DescribeVideoAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult CreateVideoAuditingJob(const CreateVideoAuditingJobReq& req,
                                   CreateVideoAuditingJobResp* resp);   

  /// \brief 查询视频审核任务结果
  /// \brief https://cloud.tencent.com/document/product/436/47317
  /// \param req  DescribeVideoAuditingJob请求
  /// \param resp DescribeVideoAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult DescribeVideoAuditingJob(const DescribeVideoAuditingJobReq& req,
                                     DescribeVideoAuditingJobResp* resp);    

  /// \brief 提交音频审核任务
  /// \brief https://cloud.tencent.com/document/product/436/54063
  /// \param req  DescribeAudioAuditingJob请求
  /// \param resp DescribeAudioAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult CreateAudioAuditingJob(const CreateAudioAuditingJobReq& req,
                                   CreateAudioAuditingJobResp* resp);   

  /// \brief 查询音频审核任务结果
  /// \brief https://cloud.tencent.com/document/product/436/54064
  /// \param req  DescribeAudioAuditingJob请求
  /// \param resp DescribeAudioAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult DescribeAudioAuditingJob(const DescribeAudioAuditingJobReq& req,
                                     DescribeAudioAuditingJobResp* resp);

  /// \brief 提交文本审核任务
  /// \brief https://cloud.tencent.com/document/product/436/56289
  /// \param req  DescribeTextAuditingJob请求
  /// \param resp DescribeTextAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult CreateTextAuditingJob(const CreateTextAuditingJobReq& req,
                                  CreateTextAuditingJobResp* resp);   

  /// \brief 查询文本审核任务结果
  /// \brief https://cloud.tencent.com/document/product/436/56288
  /// \param req  DescribeTextAuditingJob请求
  /// \param resp DescribeTextAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult DescribeTextAuditingJob(const DescribeTextAuditingJobReq& req,
                                    DescribeTextAuditingJobResp* resp);                                       

  /// \brief 提交文档审核任务
  /// \brief https://cloud.tencent.com/document/product/436/59381
  /// \param req  DescribeDocumentAuditingJob请求
  /// \param resp DescribeDocumentAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult CreateDocumentAuditingJob(const CreateDocumentAuditingJobReq& req,
                                      CreateDocumentAuditingJobResp* resp);   

  /// \brief 查询文档审核任务结果
  /// \brief https://cloud.tencent.com/document/product/436/59382
  /// \param req  DescribeDocumentAuditingJob请求
  /// \param resp DescribeDocumentAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult DescribeDocumentAuditingJob(const DescribeDocumentAuditingJobReq& req,
                                        DescribeDocumentAuditingJobResp* resp);                                       

  /// \brief 提交网页审核任务
  /// \brief https://cloud.tencent.com/document/product/436/63958
  /// \param req  DescribeWebPageAuditingJob请求
  /// \param resp DescribeWebPageAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult CreateWebPageAuditingJob(const CreateWebPageAuditingJobReq& req,
                                      CreateWebPageAuditingJobResp* resp);   

  /// \brief 查询网页审核任务结果
  /// \brief https://cloud.tencent.com/document/product/436/63959
  /// \param req  DescribeWebPageAuditingJob请求
  /// \param resp DescribeWebPageAuditingJob返回
  ///
  /// \return 本次请求的调用情况(如状态码等)     
  CosResult DescribeWebPageAuditingJob(const DescribeWebPageAuditingJobReq& req,
                                        DescribeWebPageAuditingJobResp* resp);                                       



 private:
  /// \brief 处理CI请求
  CosResult ProcessReq(const BucketReq& req, BaseResp* resp,
                       bool is_ci_req = false);
};

}  // namespace qcloud_cos