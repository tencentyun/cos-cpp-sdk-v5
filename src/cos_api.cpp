#include "cos_api.h"

#include <pthread.h>

#include "threadpool/boost/threadpool.hpp"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/HTTPSStreamFactory.h"
#include "Poco/Net/SSLManager.h"

#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

bool CosAPI::s_init = false;
bool CosAPI::s_poco_init = false;
int CosAPI::s_cos_obj_num = 0;
SimpleMutex CosAPI::s_init_mutex = SimpleMutex();
//boost::threadpool::pool* g_threadpool = NULL;

CosAPI::CosAPI(CosConfig& config)
    : m_config(new CosConfig(config)), m_object_op(m_config), m_bucket_op(m_config), m_service_op(m_config) {
    CosInit();
}

CosAPI::~CosAPI() {
    CosUInit();
}

int CosAPI::CosInit() {
    SimpleMutexLocker locker(&s_init_mutex);
    ++s_cos_obj_num;
    if (!s_init) {
        if (!s_poco_init) {
            Poco::Net::HTTPStreamFactory::registerFactory();
            Poco::Net::HTTPSStreamFactory::registerFactory();
            Poco::Net::initializeSSL();
            s_poco_init = true;
        }

        //g_threadpool = new boost::threadpool::pool(CosSysConfig::GetAsynThreadPoolSize());
        s_init = true;
    }

    return 0;
}

void CosAPI::CosUInit() {
    SimpleMutexLocker locker(&s_init_mutex);
    --s_cos_obj_num;
    if (s_init && s_cos_obj_num == 0) {
        //if (g_threadpool){
        //    g_threadpool->wait();
        //    delete g_threadpool;
        //    g_threadpool = NULL;
        //}

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

CosResult CosAPI::MultiUploadObject(const MultiUploadObjectReq& request,
                                    MultiUploadObjectResp* response) {
    // 1. list uploads first
    ListMultipartUploadReq list_mp_req(request.GetBucketName());
    ListMultipartUploadResp list_mp_resp;
    CosResult result;
    std::vector<Upload> uploads;
    std::string upload_id;

    // get up to 1000 result, if there are more, just ignore
    list_mp_req.SetPrefix(request.GetObjectName());
    list_mp_req.SetMaxUploads("1000");
    result = m_bucket_op.ListMultipartUpload(list_mp_req, &list_mp_resp);
    if (!result.IsSucc()) {
        return result;
    }

    uploads = list_mp_resp.GetUpload();
    if (uploads.size() > 0) {
        // get recent upload
        std::vector<Upload>::reverse_iterator rit = uploads.rbegin();
        for (; rit != uploads.rend(); ++rit) {
            if (rit->m_key == request.GetObjectName()) {
                upload_id = rit->m_uploadid;
                break;
            }
        }
    }

    // 2. check should we use resume upload
    if (!upload_id.empty()) {
        std::map<uint32_t, std::string> existing_part_map;
        uint64_t existing_part_size;
        bool use_resume_upload = 
            m_object_op.CheckResumeUpload(upload_id,
                                            request.GetBucketName(),
                                            request.GetObjectName(),
                                            request.GetLocalFilePath(),
                                            &existing_part_map,
                                            &existing_part_size);
        if (use_resume_upload) {
            // use resume upload
            SDK_LOG_INFO("use resume upload");
            return m_object_op.ResumeUploadObject(request, response, upload_id,
                                                  existing_part_map, existing_part_size);
        }
    }
    
    // use normal upload
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

CosResult CosAPI::Copy(const CopyReq& request,
                       CopyResp* response) {
    return m_object_op.Copy(request, response);
}

CosResult CosAPI::PostObjectRestore(const PostObjectRestoreReq& request,
                                    PostObjectRestoreResp* response) {
    return m_object_op.PostObjectRestore(request, response);
}

CosResult CosAPI::OptionsObject(const OptionsObjectReq& request,
                             OptionsObjectResp* response) {
    return m_object_op.OptionsObject(request, response);
}

CosResult CosAPI::SelectObjectContent(const SelectObjectContentReq& request,
                             SelectObjectContentResp* response) {
    return m_object_op.SelectObjectContent(request, response);
}

CosResult CosAPI::PutBucketLogging(const PutBucketLoggingReq& request, 
                                   PutBucketLoggingResp* response) {
    return m_bucket_op.PutBucketLogging(request, response);
}

CosResult CosAPI::GetBucketLogging(const GetBucketLoggingReq& request, 
                                   GetBucketLoggingResp* response) {
    return m_bucket_op.GetBucketLogging(request, response);
}

CosResult CosAPI::PutBucketDomain(const PutBucketDomainReq& request, 
                                  PutBucketDomainResp* response) { 
    return m_bucket_op.PutBucketDomain(request, response);
}

CosResult CosAPI::GetBucketDomain(const GetBucketDomainReq& request,
				  GetBucketDomainResp* response) {									  
    return m_bucket_op.GetBucketDomain(request, response);						  
}

CosResult CosAPI::PutBucketWebsite(const PutBucketWebsiteReq& request,
				   PutBucketWebsiteResp* response) {									  
    return m_bucket_op.PutBucketWebsite(request, response);						  
}

CosResult CosAPI::GetBucketWebsite(const GetBucketWebsiteReq& request,
				   GetBucketWebsiteResp* response) {									  
    return m_bucket_op.GetBucketWebsite(request, response);						  
}

CosResult CosAPI::DeleteBucketWebsite(const DeleteBucketWebsiteReq& request,
				      DeleteBucketWebsiteResp* response) {									  
    return m_bucket_op.DeleteBucketWebsite(request, response);						  
}

CosResult CosAPI::PutBucketTagging(const PutBucketTaggingReq& request,
				      PutBucketTaggingResp* response) {									  
    return m_bucket_op.PutBucketTagging(request, response);						  
}

CosResult CosAPI::GetBucketTagging(const GetBucketTaggingReq& request,
				      GetBucketTaggingResp* response) {									  
    return m_bucket_op.GetBucketTagging(request, response);						  
}

CosResult CosAPI::DeleteBucketTagging(const DeleteBucketTaggingReq& request,
				      DeleteBucketTaggingResp* response) {									  
    return m_bucket_op.DeleteBucketTagging(request, response);						  
}

CosResult CosAPI::PutBucketInventory(const PutBucketInventoryReq& request,
				      PutBucketInventoryResp* response) {									  
    return m_bucket_op.PutBucketInventory(request, response);						  
}

CosResult CosAPI::GetBucketInventory(const GetBucketInventoryReq& request,
				      GetBucketInventoryResp* response) {									  
    return m_bucket_op.GetBucketInventory(request, response);						  
}

CosResult CosAPI::ListBucketInventoryConfigurations(const ListBucketInventoryConfigurationsReq& request,
				      ListBucketInventoryConfigurationsResp* response) {									  
    return m_bucket_op.ListBucketInventoryConfigurations(request, response);						  
}

CosResult CosAPI::DeleteBucketInventory(const DeleteBucketInventoryReq& request,
				      DeleteBucketInventoryResp* response) {									  
    return m_bucket_op.DeleteBucketInventory(request, response);						  
}

////////////////////////////////////////////////////////////////////////////////////////
// live channel api
CosResult CosAPI::PutLiveChannel(const PutLiveChannelReq& request,
                                    PutLiveChannelResp* response) {
    return m_object_op.PutLiveChannel(request, response);
}

std::string CosAPI::GetRtmpSignedPublishUrl(const std::string& bucket, const std::string& channel,
                                        int expire, const std::map<std::string, std::string> url_params) {
    std::string rtmp_signed_url =
            "rtmp://" + bucket + ".cos." + m_config->GetRegion() + ".myqcloud.com/live/" + channel;
    std::string sign_info = AuthTool::RtmpSign(m_config->GetAccessKey(), m_config->GetSecretKey(),
                                               m_config->GetTmpToken(), bucket, channel, url_params, expire);
    return rtmp_signed_url + "?" + sign_info;
}

CosResult CosAPI::PutLiveChannelSwitch(const PutLiveChannelSwitchReq& request,
                                    PutLiveChannelSwitchResp* response) {
    return m_object_op.PutLiveChannelSwitch(request, response);
}

CosResult CosAPI::GetLiveChannel(const GetLiveChannelReq& request,
                                    GetLiveChannelResp* response) {
    return m_object_op.GetLiveChannel(request, response);
}

CosResult CosAPI::GetLiveChannelHistory(const GetLiveChannelHistoryReq& request,
                                    GetLiveChannelHistoryResp* response) {
    return m_object_op.GetLiveChannelHistory(request, response);
}

CosResult CosAPI::GetLiveChannelStatus(const GetLiveChannelStatusReq& request,
                                    GetLiveChannelStatusResp* response) {
    return m_object_op.GetLiveChannelStatus(request, response);
}

CosResult CosAPI::DeleteLiveChannel(const DeleteLiveChannelReq& request,
                                    DeleteLiveChannelResp* response) {
    return m_object_op.DeleteLiveChannel(request, response);
}

CosResult CosAPI::GetLiveChannelVodPlaylist(const GetLiveChannelVodPlaylistReq& request,
                                    GetLiveChannelVodPlaylistResp* response) {
    return m_object_op.GetLiveChannelVodPlaylist(request, response);
}

CosResult CosAPI::PostLiveChannelVodPlaylist(const PostLiveChannelVodPlaylistReq& request,
                                        PostLiveChannelVodPlaylistResp* response) {
    return m_object_op.PostLiveChannelVodPlaylist(request, response);
}

CosResult CosAPI::ListLiveChannel(const ListLiveChannelReq& request, ListLiveChannelResp* response) {
    return m_bucket_op.ListLiveChannel(request, response);
}

} // namespace qcloud_cos
