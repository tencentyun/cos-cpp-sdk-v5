#include "cos_api.h"

#include <pthread.h>

#include "threadpool/boost/threadpool.hpp"

#include "cos_sys_config.h"

namespace qcloud_cos {

int CosAPI::s_init = 0;
int CosAPI::s_cos_obj_num = 0;
SimpleMutex CosAPI::s_init_mutex = SimpleMutex();
boost::threadpool::pool* g_threadpool = NULL;

CosAPI::CosAPI(CosConfig& config) : m_object_op(config), m_bucket_op(config) {
    CosInit();
}

CosAPI::~CosAPI() {
    CosUInit();
}

int CosAPI::CosInit() {
    SimpleMutexLocker locker(&s_init_mutex);
    ++s_cos_obj_num;
    if (!s_init) {
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

CosResult CosAPI::GetBucket(const GetBucketReq& request, GetBucketResp* response) {
    return m_bucket_op.GetBucket(request, response);
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

CosResult CosAPI::HeadObject(const HeadObjectReq& request, HeadObjectResp* response) {
    return m_object_op.HeadObject(request, response);
}

CosResult CosAPI::InitMultiUpload(const InitMultiUploadReq& request, InitMultiUploadResp* response) {
    return m_object_op.InitMultiUpload(request, response);
}

CosResult CosAPI::UploadPartData(const UploadPartDataReq& request, UploadPartDataResp* response) {
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

} // namespace qcloud_cos
