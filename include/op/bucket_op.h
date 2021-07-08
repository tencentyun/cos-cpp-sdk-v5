// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/13/17
// Description:

#ifndef BUCKETOP_H
#define BUCKETOP_H
#pragma once

#include "cos_sys_config.h"
#include "op/base_op.h"
#include "op/cos_result.h"
#include "request/bucket_req.h"
#include "response/bucket_resp.h"
#include "request/data_process_req.h"
#include "response/data_process_resp.h"

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
    CosResult PutBucket(const PutBucketReq& req, PutBucketResp* resp);

    /// \brief 确认Bucket是否存在
    ///        (详见:https://cloud.tencent.com/document/product/436/7735)
    ///
    /// \param req  HeadBucket请求
    /// \param resp HeadBucket返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult HeadBucket(const HeadBucketReq& req, HeadBucketResp* resp);

    /// \brief 列出Bucket下的部分或者全部Object
    ///        (详见:https://www.qcloud.com/document/product/436/7734)
    ///
    /// \param req  GetBucket请求
    /// \param resp GetBucket返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucket(const GetBucketReq& req, GetBucketResp* resp);

    CosResult ListMultipartUpload(const ListMultipartUploadReq& req, ListMultipartUploadResp* resp); 
    /// \brief 删除Bucket
    ///
    /// \param req  DeleteBucket请求
    /// \param resp DeleteBucket返回
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
    CosResult GetBucketACL(const GetBucketACLReq& req,
                           GetBucketACLResp* resp);

    /// \brief 增加/替换Bucket下的ACL, 可以通过Header或者Body传入ACL信息
    ///        注意Header 和 Body 只能选择其中一种，否则响应返回会冲突
    ///
    /// \param req  PutBucketACL请求
    /// \param resp PutBucketACL返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketACL(const PutBucketACLReq& req,
                           PutBucketACLResp* resp);

    /// \brief 列出Bucket下的CORS
    ///
    /// \param req  GetBucketCORS请求
    /// \param resp GetBucketCORS返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketCORS(const GetBucketCORSReq& req,
                            GetBucketCORSResp* resp);

    /// \brief 增加/替换Bucket下的CORS
    ///
    /// \param req  PutBucketCORS请求
    /// \param resp PutBucketCORS返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketCORS(const PutBucketCORSReq& req,
                            PutBucketCORSResp* resp);

    /// \brief 删除Bucket下的CORS
    ///
    /// \param req  DeleteBucketCORS请求
    /// \param resp DeleteBucketCORS返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketCORS(const DeleteBucketCORSReq& req,
                               DeleteBucketCORSResp* resp);

    /// \brief 列出Bucket下的部分或者全部Object(包括多版本)
    ///
    /// \param req  GetBucketObjectVersions请求
    /// \param resp GetBucketObjectVersions返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketObjectVersions(const GetBucketObjectVersionsReq& req,
                                      GetBucketObjectVersionsResp* resp);

    /// \brief 获取Bucket所在Location
    ///
    /// \param req  GetBucketLocation请求
    /// \param resp GetBucketLocation返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketLocation(const GetBucketLocationReq& req,
                                GetBucketLocationResp* resp);
    // TODO(sevenyou)
    // std::string ListMultipartUploads();
	
	
    /// \brief 为源存储桶开启日志记录
    /// \brief https://cloud.tencent.com/document/product/436/17054
    /// \param req  PutBucketLogging请求
    /// \param resp PutBucketLogging返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketLogging(const PutBucketLoggingReq& req, 
                               PutBucketLoggingResp* resp);

    /// \brief 用于获取源存储桶的日志配置信息
    /// \brief https://cloud.tencent.com/document/product/436/17053
    /// \param req  GetBucketLogging请求
    /// \param resp GetBucketLogging返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketLogging(const GetBucketLoggingReq& req, 
                               GetBucketLoggingResp* resp);

    /// \brief 用于存储桶绑定自定义域名
    /// \brief
    /// \param req  PutBucketDomain请求
    /// \param resp PutBucketDomain返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketDomain(const PutBucketDomainReq& req, 
                               PutBucketDomainResp* resp);
							   
    /// \brief 用于获取存储桶绑定自定义域名
    /// \brief
    /// \param req  GetBucketDomain请求
    /// \param resp GetBucketDomain返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketDomain(const GetBucketDomainReq& req, 
                               GetBucketDomainResp* resp);
							   
    /// \brief 存储桶配置静态网站，通过传入 XML 格式的配置文件进行配置，文件大小限制为64KB
    /// \brief https://cloud.tencent.com/document/product/436/31930
    /// \param req  PutBucketWebsite请求
    /// \param resp PutBucketWebsite返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketWebsite(const PutBucketWebsiteReq& req, 
                               PutBucketWebsiteResp* resp);								   
	
    /// \brief 获取与存储桶关联的静态网站配置信息.
    /// \brief https://cloud.tencent.com/document/product/436/31929
    /// \param req  GetBucketWebsite请求
    /// \param resp GetBucketWebsite返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketWebsite(const GetBucketWebsiteReq& req, 
                               GetBucketWebsiteResp* resp);	

    /// \brief 删除存储桶中的静态网站配置.
    /// \brief https://cloud.tencent.com/document/product/436/31928
    /// \param req  DeleteBucketWebsite请求
    /// \param resp DeleteBucketWebsite返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketWebsite(const DeleteBucketWebsiteReq& req, 
                               DeleteBucketWebsiteResp* resp);

    /// \brief 为已存在的Bucket设置标签.
    /// \brief https://cloud.tencent.com/document/product/436/34838
    /// \param req  PutBucketTagging请求
    /// \param resp PutBucketTagging返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketTagging(const PutBucketTaggingReq& req, 
                               PutBucketTaggingResp* resp);


    /// \brief 查询指定存储桶下已有的存储桶标签.
    /// \brief https://cloud.tencent.com/document/product/436/34837
    /// \param req  GetBucketTagging请求
    /// \param resp GetBucketTagging返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketTagging(const GetBucketTaggingReq& req, 
                               GetBucketTaggingResp* resp);	
							   
    /// \brief 删除指定存储桶下已有的存储桶标签.
    /// \brief https://cloud.tencent.com/document/product/436/34836
    /// \param req  DeleteBucketTagging请求
    /// \param resp DeleteBucketTagging返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketTagging(const DeleteBucketTaggingReq& req, 
                               DeleteBucketTaggingResp* resp);
							   
    /// \brief 用于在存储桶中创建清单任务.
    /// \brief https://cloud.tencent.com/document/product/436/33707
    /// \param req  PutBucketinventory请求
    /// \param resp PutBucketinventory返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketInventory(const PutBucketInventoryReq& req, 
                               PutBucketInventoryResp* resp);
							   
    /// \brief 查询存储桶中用户的清单任务信息.
    /// \brief https://cloud.tencent.com/document/product/436/33705
    /// \param req  GetBucketinventory请求
    /// \param resp GetBucketinventory返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketInventory(const GetBucketInventoryReq& req, 
                               GetBucketInventoryResp* resp);
							   
    /// \brief 请求返回一个存储桶中的所有清单任务.
    /// \brief https://cloud.tencent.com/document/product/436/33706
    /// \param req  ListBucketInventoryConfigurations请求
    /// \param resp ListBucketInventoryConfigurations返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult ListBucketInventoryConfigurations(const ListBucketInventoryConfigurationsReq& req, 
                               ListBucketInventoryConfigurationsResp* resp);
							   					   
    /// \brief 用于删除存储桶中指定的清单任务.
    /// \brief https://cloud.tencent.com/document/product/436/33704
    /// \param req  DeleteBucketinventory请求
    /// \param resp DeleteBucketinventory返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucketInventory(const DeleteBucketInventoryReq& req, 
                               DeleteBucketInventoryResp* resp);

    /// \brief 列举直播通道
    /// \param req  ListLiveChannelReq请求
    /// \param resp ListLiveChannelResp返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult ListLiveChannel(const ListLiveChannelReq& req, ListLiveChannelResp* resp);

    /// \brief 配置存储桶智能分层
    ///
    /// \param req  PutBucketIntelligentTiering请求
    /// \param resp PutBucketIntelligentTiering返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucketIntelligentTiering(const PutBucketIntelligentTieringReq& req,
                                                PutBucketIntelligentTieringResp* resp);

    /// \brief 获取存储桶智能分层配置
    ///
    /// \param req  GetBucketIntelligentTiering请求
    /// \param resp GetBucketIntelligentTiering返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult GetBucketIntelligentTiering(const GetBucketIntelligentTieringReq& req,
                                                GetBucketIntelligentTieringResp* resp);

    CosResult CreateDocProcessJobs(const CreateDocProcessJobsReq& req, CreateDocProcessJobsResp *resp);

    CosResult DescribeDocProcessJob(const DescribeDocProcessJobReq& req, DescribeDocProcessJobResp *resp);

    CosResult DescribeDocProcessJobs(const DescribeDocProcessJobsReq& req, DescribeDocProcessJobsResp *resp);

    CosResult DescribeDocProcessQueues(const DescribeDocProcessQueuesReq& req, DescribeDocProcessQueuesResp *resp);

    CosResult UpdateDocProcessQueue(const UpdateDocProcessQueueReq& req, UpdateDocProcessQueueResp *resp);
};

} // namespace qcloud_cos
#endif // BUCKETOP_H
