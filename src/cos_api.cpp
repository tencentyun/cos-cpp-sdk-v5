#include "cos_api.h"

#include <pthread.h>

#include "threadpool/boost/threadpool.hpp"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/HTTPSStreamFactory.h"
#include "Poco/Net/SSLManager.h"

#include "cos_sys_config.h"

namespace qcloud_cos {

int CosAPI::s_init = 0;
int CosAPI::s_cos_obj_num = 0;
SimpleMutex CosAPI::s_init_mutex = SimpleMutex();
boost::threadpool::pool* g_threadpool = NULL;

CosAPI::CosAPI(CosConfig& config)
    : m_object_op(config), m_bucket_op(config), m_service_op(config) {
    CosInit();
}

CosAPI::~CosAPI() {
    CosUInit();
}

int CosAPI::CosInit() {
    SimpleMutexLocker locker(&s_init_mutex);
    ++s_cos_obj_num;
    if (!s_init) {
        Poco::Net::HTTPStreamFactory::registerFactory();
        Poco::Net::HTTPSStreamFactory::registerFactory();
        Poco::Net::initializeSSL();

        s_init = true;
        g_threadpool = new boost::threadpool::pool(CosSysConfig::GetAsynThreadPoolSize());
    }

    return 0;
}

void CosAPI::CosUInit() {
    SimpleMutexLocker locker(&s_init_mutex);
    --s_cos_obj_num;
    if (s_init && s_cos_obj_num == 0) {
        if (g_threadpool){
            g_threadpool->wait();
            delete g_threadpool;
        }

        s_init = false;
    }
}

CosResult CosAPI::GetService(const GetServiceReq& request, GetServiceResp* response) {
    return m_service_op.GetService(request, response);
}

CosResult CosAPI::PutBucket(const PutBucketReq& request, PutBucketResp* response) {
    return m_bucket_op.PutBucket(request, response);
}

CosResult CosAPI::GetBucket(const GetBucketReq& request, GetBucketResp* response) {
    return m_bucket_op.GetBucket(request, response);
}

CosResult CosAPI::DeleteBucket(const DeleteBucketReq& request, DeleteBucketResp* response) {
    return m_bucket_op.DeleteBucket(request, response);
}

CosResult CosAPI::GetBucketVersioning(const GetBucketVersioningReq& request,
                                      GetBucketVersioningResp* response) {
    return m_bucket_op.GetBucketVersioning(request, response);
}

CosResult CosAPI::PutBucketVersioning(const PutBucketVersioningReq& request,
                                      PutBucketVersioningResp* response) {
    return m_bucket_op.PutBucketVersioning(request, response);
}

CosResult CosAPI::GetBucketReplication(const GetBucketReplicationReq& request,
                                       GetBucketReplicationResp* response) {
    return m_bucket_op.GetBucketReplication(request, response);
}

CosResult CosAPI::PutBucketReplication(const PutBucketReplicationReq& request,
                                       PutBucketReplicationResp* response) {
    return m_bucket_op.PutBucketReplication(request, response);
}

CosResult CosAPI::DeleteBucketReplication(const DeleteBucketReplicationReq& request,
                                          DeleteBucketReplicationResp* response) {
    return m_bucket_op.DeleteBucketReplication(request, response);
}

CosResult CosAPI::GetBucketLifecycle(const GetBucketLifecycleReq& request,
                                     GetBucketLifecycleResp* response) {
    return m_bucket_op.GetBucketLifecycle(request, response);
}

CosResult CosAPI::PutBucketLifecycle(const PutBucketLifecycleReq& request,
                                     PutBucketLifecycleResp* response) {
    return m_bucket_op.PutBucketLifecycle(request, response);
}

CosResult CosAPI::DeleteBucketLifecycle(const DeleteBucketLifecycleReq& request,
                                        DeleteBucketLifecycleResp* response) {
    return m_bucket_op.DeleteBucketLifecycle(request, response);
}

CosResult CosAPI::GetBucketACL(const GetBucketACLReq& request,
                               GetBucketACLResp* response) {
    return m_bucket_op.GetBucketACL(request, response);
}

CosResult CosAPI::PutBucketACL(const PutBucketACLReq& request,
                               PutBucketACLResp* response) {
    return m_bucket_op.PutBucketACL(request, response);
}

CosResult CosAPI::GetBucketCORS(const GetBucketCORSReq& request,
                                GetBucketCORSResp* response) {
    return m_bucket_op.GetBucketCORS(request, response);
}

CosResult CosAPI::PutBucketCORS(const PutBucketCORSReq& request,
                                PutBucketCORSResp* response) {
    return m_bucket_op.PutBucketCORS(request, response);
}

CosResult CosAPI::DeleteBucketCORS(const DeleteBucketCORSReq& request,
                                   DeleteBucketCORSResp* response) {
    return m_bucket_op.DeleteBucketCORS(request, response);
}

CosResult CosAPI::PutObject(const PutObjectByFileReq& request,
                            PutObjectByFileResp* response) {
    return m_object_op.PutObject(request, response);
}

CosResult CosAPI::PutObject(const PutObjectByStreamReq& request,
                            PutObjectByStreamResp* response) {
    return m_object_op.PutObject(request, response);
}

CosResult CosAPI::GetObject(const GetObjectByStreamReq& request,
                            GetObjectByStreamResp* response) {
    return m_object_op.GetObject(request, response);
}

CosResult CosAPI::GetObject(const GetObjectByFileReq& request,
                            GetObjectByFileResp* response) {
    return m_object_op.GetObject(request, response);
}

CosResult CosAPI::GetObject(const MultiGetObjectReq& request,
                            MultiGetObjectResp* response) {
    return m_object_op.GetObject(request, response);
}

CosResult CosAPI::DeleteObject(const DeleteObjectReq& request,
                               DeleteObjectResp* response) {
    return m_object_op.DeleteObject(request, response);
}

CosResult CosAPI::HeadObject(const HeadObjectReq& request,
                             HeadObjectResp* response) {
    return m_object_op.HeadObject(request, response);
}

CosResult CosAPI::InitMultiUpload(const InitMultiUploadReq& request,
                                  InitMultiUploadResp* response) {
    return m_object_op.InitMultiUpload(request, response);
}

CosResult CosAPI::UploadPartData(const UploadPartDataReq& request,
                                 UploadPartDataResp* response) {
    return m_object_op.UploadPartData(request, response);
}

CosResult CosAPI::CompleteMultiUpload(const CompleteMultiUploadReq& request,
                                      CompleteMultiUploadResp* response) {
    return m_object_op.CompleteMultiUpload(request, response);
}

CosResult CosAPI::MultiUploadObject(const MultiUploadObjectReq& request,
                                    MultiUploadObjectResp* response) {
    return m_object_op.MultiUploadObject(request, response);
}

CosResult CosAPI::AbortMultiUpload(const AbortMultiUploadReq& request,
                                   AbortMultiUploadResp* response) {
    return m_object_op.AbortMultiUpload(request, response);
}

CosResult CosAPI::ListParts(const ListPartsReq& request,
                            ListPartsResp* response) {
    return m_object_op.ListParts(request, response);
}

CosResult CosAPI::GetObjectACL(const GetObjectACLReq& request,
                               GetObjectACLResp* response) {
    return m_object_op.GetObjectACL(request, response);
}

CosResult CosAPI::PutObjectACL(const PutObjectACLReq& request,
                               PutObjectACLResp* response) {
    return m_object_op.PutObjectACL(request, response);
}

CosResult CosAPI::PutObjectCopy(const PutObjectCopyReq& request,
                                PutObjectCopyResp* response) {
    return m_object_op.PutObjectCopy(request, response);
}

} // namespace qcloud_cos
