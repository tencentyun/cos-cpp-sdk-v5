#ifndef COS_API_H
#define COS_API_H

#include "op/bucket_op.h"
#include "op/cos_result.h"
#include "op/object_op.h"
#include "util/simple_mutex.h"

namespace qcloud_cos {

class CosAPI {
public:
    /// \brief CosAPI构造函数
    ///
    /// \param config    cos配置
    explicit CosAPI(CosConfig& config);
    ~CosAPI();

    /// \brief 创建一个Bucket
    ///        详见: https://www.qcloud.com/document/product/436/7738
    ///
    /// \param req  PutBucket请求
    /// \param resp PutBucket返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutBucket(const PutBucketReq& request, PutBucketResp* response);

    /// \brief 列出该Bucket下的部分或者全部Object, 需要对Bucket有Read 权限
    ///        详见: https://www.qcloud.com/document/product/436/7734
    ///
    /// \param request   GetBucket请求
    /// \param response  GetBucket返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult GetBucket(const GetBucketReq& request, GetBucketResp* response);

    /// \brief 删除Bucket
    ///        详见: https://cloud.tencent.com/document/product/436/7732
    ///
    /// \param req  DeleteBucket请求
    /// \param resp DeleteBucket返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult DeleteBucket(const DeleteBucketReq& request, DeleteBucketResp* response);

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

    /// \brief 多线程下载Bucket中的一个文件到本地
    ///        详见: https://www.qcloud.com/document/product/436/7753
    ///
    /// \param request   MultiGetObject请求
    /// \param response  MultiGetObject返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult GetObject(const MultiGetObjectReq& request, MultiGetObjectResp* response);

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

    /// \brief 请求实现初始化分片上传,成功执行此请求以后会返回UploadId用于后续的Upload Part请求
    ///        详见: https://www.qcloud.com/document/product/436/7746
    ///
    /// \param request   InitMultiUpload请求
    /// \param response  InitMultiUpload返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult InitMultiUpload(const InitMultiUploadReq& request, InitMultiUploadResp* response);

    /// \brief 初始化以后的分块上传,支持的块的数量为1到10000,块的大小为1MB到5GB
    ///        详见: https://www.qcloud.com/document/product/436/7750
    ///
    /// \param request   UploadPartData请求
    /// \param response  UploadPartData返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult UploadPartData(const UploadPartDataReq& request, UploadPartDataResp* response);

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

    /// \brief 封装了初始化分块上传、分块上传、完成分块上传三步
    ///
    /// \param request   MultiUploadObject请求
    /// \param response  MultiUploadObject返回
    ///
    /// \return 返回HTTP请求的状态码及错误信息
    CosResult MultiUploadObject(const MultiUploadObjectReq& request,
                                MultiUploadObjectResp* response);

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

    /// \brief 复制Object
    ///
    /// \param req  PutObjectCopy请求
    /// \param resp PutObjectCopy返回
    ///
    /// \return 本次请求的调用情况(如状态码等)
    CosResult PutObjectCopy(const PutObjectCopyReq& request,
                            PutObjectCopyResp* response);

private:
    int CosInit();
    void CosUInit();

    // 从本地文件src中读取数据并填入到file_content中, 返回文件长度
    uint64_t GetContent(const std::string& src, std::string* file_content);

private:
    ObjectOp m_object_op; // 内部封装object相关的操作
    BucketOp m_bucket_op; // 内部封装bucket相关的操作

    static SimpleMutex s_init_mutex;
    static int s_init;
    static int s_cos_obj_num;
};

} // namespace qcloud_cos
#endif
