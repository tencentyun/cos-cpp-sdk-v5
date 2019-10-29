#include "cos_api.h"

#include "boost/thread/lock_guard.hpp"
#include "threadpool/boost/threadpool.hpp"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/HTTPSStreamFactory.h"
#include "Poco/Net/SSLManager.h"

#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

boost::mutex CosAPI::s_init_mutex;
bool CosAPI::s_init = false;
bool CosAPI::s_poco_init = false;
int CosAPI::s_cos_obj_num = 0;

boost::threadpool::pool* g_threadpool = NULL;

CosAPI::CosAPI(CosConfig& config)
    : m_config(new CosConfig(config)), m_object_op(m_config), m_bucket_op(m_config), m_service_op(m_config) {
    CosInit();
}

CosAPI::~CosAPI() {
    CosUInit();
}

int CosAPI::CosInit() {
	boost::lock_guard<boost::mutex> locker(s_init_mutex);
    ++s_cos_obj_num;
    if (!s_init) {
        if (!s_poco_init) {
            Poco::Net::HTTPStreamFactory::registerFactory();
            Poco::Net::HTTPSStreamFactory::registerFactory();
            Poco::Net::initializeSSL();
            s_poco_init = true;
        }

        g_threadpool = new boost::threadpool::pool(CosSysConfig::GetAsynThreadPoolSize());
        s_init = true;
    }

    return 0;
}

void CosAPI::CosUInit() {
	boost::lock_guard<boost::mutex> locker(s_init_mutex);
    --s_cos_obj_num;
    if (s_init && s_cos_obj_num == 0) {
        if (g_threadpool){
            g_threadpool->wait();
            delete g_threadpool;
            g_threadpool = NULL;
        }

        s_init = false;
    }
}

void CosAPI::SetCredentail(const std::string& ak, const std::string& sk, const std::string& token){
    m_config->SetConfigCredentail(ak,sk,token);
}

bool CosAPI::IsBucketExist(const std::string& bucket_name) {
    return m_bucket_op.IsBucketExist(bucket_name);
}

bool CosAPI::IsObjectExist(const std::string& bucket_name, const std::string& object_name) {
    return m_object_op.IsObjectExist(bucket_name, object_name);
}

std::string CosAPI::GeneratePresignedUrl(const GeneratePresignedUrlReq& request) {
    return m_object_op.GeneratePresignedUrl(request);
}

std::string CosAPI::GeneratePresignedUrl(const std::string& bucket_name,
                                         const std::string& object_name,
                                         uint64_t start_time_in_s,
                                         uint64_t end_time_in_s,
                                         HTTP_METHOD http_method) {
    GeneratePresignedUrlReq req(bucket_name, object_name, http_method);
    req.SetStartTimeInSec(start_time_in_s);
    req.SetExpiredTimeInSec(end_time_in_s - start_time_in_s);

    return GeneratePresignedUrl(req);
}

std::string CosAPI::GeneratePresignedUrl(const std::string& bucket_name,
                                         const std::string& key,
                                         uint64_t start_time_in_s,
                                         uint64_t end_time_in_s) {
    return GeneratePresignedUrl(bucket_name, key, start_time_in_s, end_time_in_s, HTTP_GET);
}

std::string CosAPI::GetBucketLocation(const std::string& bucket_name) {
    return m_bucket_op.GetBucketLocation(bucket_name);
}

CosResult CosAPI::GetService(const GetServiceReq& request, GetServiceResp* response) {
    return m_service_op.GetService(request, response);
}

CosResult CosAPI::HeadBucket(const HeadBucketReq& request, HeadBucketResp* response) {
    return m_bucket_op.HeadBucket(request, response);
}

CosResult CosAPI::PutBucket(const PutBucketReq& request, PutBucketResp* response) {
    return m_bucket_op.PutBucket(request, response);
}

CosResult CosAPI::GetBucket(const GetBucketReq& request, GetBucketResp* response) {
    return m_bucket_op.GetBucket(request, response);
}

CosResult CosAPI::ListMultipartUpload(const ListMultipartUploadReq& request, ListMultipartUploadResp* response) {
    return m_bucket_op.ListMultipartUpload(request, response);
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

CosResult CosAPI::GetBucketObjectVersions(const GetBucketObjectVersionsReq& request,
                                          GetBucketObjectVersionsResp* response) {
    return m_bucket_op.GetBucketObjectVersions(request, response);
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

CosResult CosAPI::DeleteObjects(const DeleteObjectsReq& request,
                                DeleteObjectsResp* response) {
    return m_object_op.DeleteObjects(request, response);
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

CosResult CosAPI::UploadPartCopyData(const UploadPartCopyDataReq& request,
                                     UploadPartCopyDataResp* response) {
    return m_object_op.UploadPartCopyData(request, response);
}

CosResult CosAPI::CompleteMultiUpload(const CompleteMultiUploadReq& request,
                                      CompleteMultiUploadResp* response) {
    return m_object_op.CompleteMultiUpload(request, response);
}

void TransferSubmit(ObjectOp* op, const MultiUploadObjectReq& req,
                        Poco::SharedPtr<TransferHandler>& handler,
                        MultiUploadObjectResp* resp) {
    if(op){
        op->MultiUploadObject(req, handler, resp);
    }

}

// Async to transfer
Poco::SharedPtr<TransferHandler> CosAPI::TransferUploadObject(const MultiUploadObjectReq& request,
                                                              MultiUploadObjectResp* response) {
    // Create the handler
    Poco::SharedPtr<TransferHandler> handler = CreateUploadHandler(request.GetBucketName(), request.GetObjectName(),
                                                                   request.GetLocalFilePath());
    // Use the cos's boost thread pool to submit the task
    if(g_threadpool) {
        g_threadpool->schedule(boost::bind(&TransferSubmit, &m_object_op, request, handler, response));
    }else {
        handler->UpdateStatus(TransferStatus::FAILED);
        request.TriggerTransferStatusUpdateCallback(handler);
    }
    // Return the handler outside.
    return handler;
}

CosResult CosAPI::MultiUploadObject(const MultiUploadObjectReq& request,
                                    MultiUploadObjectResp* response) {
    
    return m_object_op.MultiUploadObject(request, response);
}

Poco::SharedPtr<TransferHandler> CosAPI::CreateUploadHandler(const std::string& bucket_name, const std::string& object_name,
                                                             const std::string& local_path) {
    return m_object_op.CreateUploadHandler(bucket_name, object_name, local_path);
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

CosResult CosAPI::Copy(const CopyReq& request,
                       CopyResp* response) {
    return m_object_op.Copy(request, response);
}

CosResult CosAPI::PostObjectRestore(const PostObjectRestoreReq& request,
                                    PostObjectRestoreResp* response) {
    return m_object_op.PostObjectRestore(request, response);
}

} // namespace qcloud_cos
