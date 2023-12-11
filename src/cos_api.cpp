#include "cos_api.h"
#include "Poco/Net/HTTPSStreamFactory.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/TaskManager.h"
#include "cos_sys_config.h"
#include "trsf/async_context.h"
#include "trsf/async_task.h"
#include "util/file_util.h"
#include "util/string_util.h"
#include <mutex>

namespace qcloud_cos {

bool CosAPI::s_init = false;
bool CosAPI::s_poco_init = false;
int CosAPI::s_cos_obj_num = 0;
std::mutex g_init_lock;

Poco::TaskManager& GetGlobalTaskManager() {
  static Poco::ThreadPool async_thread_pool("aysnc_pool", 2, CosSysConfig::GetAsynThreadPoolSize());
  static Poco::TaskManager task_manager(async_thread_pool);
  return task_manager;
}

CosAPI::CosAPI(CosConfig& config)
    : m_config(new CosConfig(config)), m_object_op(m_config),
      m_bucket_op(m_config), m_service_op(m_config) {
  CosInit();
}

CosAPI::~CosAPI() { CosUInit(); }

int CosAPI::CosInit() {
  std::lock_guard<std::mutex> lock(g_init_lock);
  ++s_cos_obj_num;
  if (!s_init) {
    if (!s_poco_init) {
      Poco::Net::HTTPStreamFactory::registerFactory();
      Poco::Net::HTTPSStreamFactory::registerFactory();
      Poco::Net::initializeSSL();
      s_poco_init = true;
    }
    s_init = true;
  }

  return 0;
}

void CosAPI::CosUInit() {
  std::lock_guard<std::mutex> lock(g_init_lock);
  --s_cos_obj_num;
  if (s_init && s_cos_obj_num == 0) {
    s_init = false;
  }
}

void CosAPI::SetCredentail(const std::string& ak, const std::string& sk,
                           const std::string& token) {
  m_config->SetConfigCredentail(ak, sk, token);
}

bool CosAPI::IsBucketExist(const std::string& bucket_name) {
  return m_bucket_op.IsBucketExist(bucket_name);
}

bool CosAPI::IsObjectExist(const std::string& bucket_name,
                           const std::string& object_name) {
  return m_object_op.IsObjectExist(bucket_name, object_name);
}

std::string CosAPI::GeneratePresignedUrl(const GeneratePresignedUrlReq& req) {
  return m_object_op.GeneratePresignedUrl(req);
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
  return GeneratePresignedUrl(bucket_name, key, start_time_in_s, end_time_in_s,
                              HTTP_GET);
}

std::string CosAPI::GetBucketLocation(const std::string& bucket_name) {
  return m_bucket_op.GetBucketLocation(bucket_name);
}

CosResult CosAPI::GetService(const GetServiceReq& req, GetServiceResp* resp) {
  return m_service_op.GetService(req, resp);
}

CosResult CosAPI::HeadBucket(const HeadBucketReq& req, HeadBucketResp* resp) {
  CosResult result = m_bucket_op.HeadBucket(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.HeadBucket(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucket(const PutBucketReq& req, PutBucketResp* resp) {
  CosResult result = m_bucket_op.PutBucket(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucket(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucket(const GetBucketReq& req, GetBucketResp* resp) {
  CosResult result = m_bucket_op.GetBucket(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucket(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::ListMultipartUpload(const ListMultipartUploadReq& req,
                                      ListMultipartUploadResp* resp) {
  CosResult result = m_bucket_op.ListMultipartUpload(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.ListMultipartUpload(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteBucket(const DeleteBucketReq& req,
                               DeleteBucketResp* resp) {
  CosResult result = m_bucket_op.DeleteBucket(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.DeleteBucket(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketVersioning(const GetBucketVersioningReq& req,
                                      GetBucketVersioningResp* resp) {
  CosResult result = m_bucket_op.GetBucketVersioning(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketVersioning(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketVersioning(const PutBucketVersioningReq& req,
                                      PutBucketVersioningResp* resp) {
  CosResult result = m_bucket_op.PutBucketVersioning(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketVersioning(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketReplication(const GetBucketReplicationReq& req,
                                       GetBucketReplicationResp* resp) {
  CosResult result = m_bucket_op.GetBucketReplication(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketReplication(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketReplication(const PutBucketReplicationReq& req,
                                       PutBucketReplicationResp* resp) {
  CosResult result = m_bucket_op.PutBucketReplication(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketReplication(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteBucketReplication(const DeleteBucketReplicationReq& req,
                                          DeleteBucketReplicationResp* resp) {
  CosResult result = m_bucket_op.DeleteBucketReplication(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.DeleteBucketReplication(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketLifecycle(const GetBucketLifecycleReq& req,
                                     GetBucketLifecycleResp* resp) {
  CosResult result = m_bucket_op.GetBucketLifecycle(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketLifecycle(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketLifecycle(const PutBucketLifecycleReq& req,
                                     PutBucketLifecycleResp* resp) {
  CosResult result = m_bucket_op.PutBucketLifecycle(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketLifecycle(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteBucketLifecycle(const DeleteBucketLifecycleReq& req,
                                        DeleteBucketLifecycleResp* resp) {
  CosResult result = m_bucket_op.DeleteBucketLifecycle(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.DeleteBucketLifecycle(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketACL(const GetBucketACLReq& req,
                               GetBucketACLResp* resp) {
  CosResult result = m_bucket_op.GetBucketACL(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketACL(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketACL(const PutBucketACLReq& req,
                               PutBucketACLResp* resp) {
  CosResult result = m_bucket_op.PutBucketACL(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketACL(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketPolicy(const PutBucketPolicyReq& req,
                                   PutBucketPolicyResp* resp) {
  CosResult result = m_bucket_op.PutBucketPolicy(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketPolicy(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketPolicy(const GetBucketPolicyReq& req,
                                   GetBucketPolicyResp* resp) {
  CosResult result = m_bucket_op.GetBucketPolicy(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketPolicy(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteBucketPolicy(const DeleteBucketPolicyReq& req,
                                      DeleteBucketPolicyResp* resp) {
  CosResult result = m_bucket_op.DeleteBucketPolicy(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.DeleteBucketPolicy(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketCORS(const GetBucketCORSReq& req,
                                GetBucketCORSResp* resp) {
  CosResult result = m_bucket_op.GetBucketCORS(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketCORS(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketCORS(const PutBucketCORSReq& req,
                                PutBucketCORSResp* resp) {
  CosResult result = m_bucket_op.PutBucketCORS(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketCORS(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteBucketCORS(const DeleteBucketCORSReq& req,
                                   DeleteBucketCORSResp* resp) {
  CosResult result = m_bucket_op.DeleteBucketCORS(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.DeleteBucketCORS(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketReferer(const PutBucketRefererReq& req,
                                   PutBucketRefererResp* resp) {
  CosResult result = m_bucket_op.PutBucketReferer(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketReferer(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketReferer(const GetBucketRefererReq& req,
                                   GetBucketRefererResp* resp) {
  CosResult result = m_bucket_op.GetBucketReferer(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketReferer(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketLogging(const PutBucketLoggingReq& req,
                                   PutBucketLoggingResp* resp) {
  CosResult result = m_bucket_op.PutBucketLogging(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketLogging(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketLogging(const GetBucketLoggingReq& req,
                                   GetBucketLoggingResp* resp) {
  CosResult result = m_bucket_op.GetBucketLogging(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketLogging(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketDomain(const PutBucketDomainReq& req,
                                  PutBucketDomainResp* resp) {
  CosResult result = m_bucket_op.PutBucketDomain(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketDomain(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketDomain(const GetBucketDomainReq& req,
                                  GetBucketDomainResp* resp) {
  CosResult result = m_bucket_op.GetBucketDomain(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketDomain(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketWebsite(const PutBucketWebsiteReq& req,
                                   PutBucketWebsiteResp* resp) {
  CosResult result = m_bucket_op.PutBucketWebsite(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketWebsite(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketWebsite(const GetBucketWebsiteReq& req,
                                   GetBucketWebsiteResp* resp) {
  CosResult result = m_bucket_op.GetBucketWebsite(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketWebsite(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteBucketWebsite(const DeleteBucketWebsiteReq& req,
                                      DeleteBucketWebsiteResp* resp) {
  CosResult result = m_bucket_op.DeleteBucketWebsite(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.DeleteBucketWebsite(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketTagging(const PutBucketTaggingReq& req,
                                   PutBucketTaggingResp* resp) {
  CosResult result = m_bucket_op.PutBucketTagging(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketTagging(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketTagging(const GetBucketTaggingReq& req,
                                   GetBucketTaggingResp* resp) {
  CosResult result = m_bucket_op.GetBucketTagging(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketTagging(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteBucketTagging(const DeleteBucketTaggingReq& req,
                                      DeleteBucketTaggingResp* resp) {
  CosResult result = m_bucket_op.DeleteBucketTagging(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.DeleteBucketTagging(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutBucketInventory(const PutBucketInventoryReq& req,
                                     PutBucketInventoryResp* resp) {
  CosResult result = m_bucket_op.PutBucketInventory(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketInventory(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketInventory(const GetBucketInventoryReq& req,
                                     GetBucketInventoryResp* resp) {
  CosResult result = m_bucket_op.GetBucketInventory(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketInventory(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::ListBucketInventoryConfigurations(
    const ListBucketInventoryConfigurationsReq& req,
    ListBucketInventoryConfigurationsResp* resp) {
  CosResult result = m_bucket_op.ListBucketInventoryConfigurations(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.ListBucketInventoryConfigurations(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteBucketInventory(const DeleteBucketInventoryReq& req,
                                        DeleteBucketInventoryResp* resp) {
  CosResult result = m_bucket_op.DeleteBucketInventory(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.DeleteBucketInventory(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetBucketObjectVersions(const GetBucketObjectVersionsReq& req,
                                          GetBucketObjectVersionsResp* resp) {
  CosResult result = m_bucket_op.GetBucketObjectVersions(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketObjectVersions(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutObject(const PutObjectByFileReq& req,
                            PutObjectByFileResp* resp) {
  CosResult result = m_object_op.PutObject(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.PutObject(req, resp, nullptr, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutObject(const PutObjectByStreamReq& req,
                            PutObjectByStreamResp* resp) {
  CosResult result = m_object_op.PutObject(req, resp);
  std::cout << m_object_op.UseDefaultDomain() << std::endl;
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.PutObject(req, resp, nullptr, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetObject(const GetObjectByStreamReq& req,
                            GetObjectByStreamResp* resp) {
  CosResult result = m_object_op.GetObject(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    std::streambuf* os_buf = req.GetStream().rdbuf();  
    os_buf->pubseekpos(0, std::ios_base::out);
    req.GetStream().clear();
    result = m_object_op.GetObject(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetObject(const GetObjectByFileReq& req,
                            GetObjectByFileResp* resp) {
  CosResult result = m_object_op.GetObject(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.GetObject(req, resp, nullptr, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::MultiGetObject(const MultiGetObjectReq& req,
                                 MultiGetObjectResp* resp) {
  return m_object_op.MultiGetObject(static_cast<GetObjectByFileReq>(req), resp);
}

std::string CosAPI::GetObjectUrl(const std::string& bucket,
                                 const std::string& object, bool https,
                                 const std::string& region) {
  std::string object_url;
  if (https) {
    object_url = "https://";
  } else {
    object_url = "http://";
  }
  std::string destdomain = m_config->GetDestDomain().empty() ? 
                          CosSysConfig::GetDestDomain() : m_config->GetDestDomain();
  if (!destdomain.empty()) {
    object_url += destdomain;
  } else {
    object_url += bucket + ".cos.";
    if (!region.empty()) {
      object_url += region;
    } else {
      object_url += m_config->GetRegion();
    }
    object_url += ".myqcloud.com";
  }
  object_url += "/" + object;
  return object_url;
}

CosResult CosAPI::DeleteObject(const DeleteObjectReq& req,
                               DeleteObjectResp* resp) {
  CosResult result = m_object_op.DeleteObject(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.DeleteObject(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteObjects(const DeleteObjectsReq& req,
                                DeleteObjectsResp* resp) {
  CosResult result = m_object_op.DeleteObjects(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.DeleteObjects(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::HeadObject(const HeadObjectReq& req, HeadObjectResp* resp) {
  CosResult result = m_object_op.HeadObject(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.HeadObject(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::InitMultiUpload(const InitMultiUploadReq& req,
                                  InitMultiUploadResp* resp) {
  CosResult result = m_object_op.InitMultiUpload(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.InitMultiUpload(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::UploadPartData(const UploadPartDataReq& req,
                                 UploadPartDataResp* resp) {
  CosResult result = m_object_op.UploadPartData(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.UploadPartData(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::UploadPartCopyData(const UploadPartCopyDataReq& req,
                                     UploadPartCopyDataResp* resp) {
  CosResult result = m_object_op.UploadPartCopyData(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.UploadPartCopyData(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::CompleteMultiUpload(const CompleteMultiUploadReq& req,
                                      CompleteMultiUploadResp* resp) {
  CosResult result = m_object_op.CompleteMultiUpload(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.CompleteMultiUpload(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::MultiPutObject(const MultiPutObjectReq& req,
                                 MultiPutObjectResp* resp) {
  CosResult result = m_object_op.MultiUploadObject(static_cast<PutObjectByFileReq>(req), resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.MultiUploadObject(static_cast<PutObjectByFileReq>(req), resp, nullptr, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::AbortMultiUpload(const AbortMultiUploadReq& req,
                                   AbortMultiUploadResp* resp) {
  CosResult result = m_object_op.AbortMultiUpload(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.AbortMultiUpload(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::ListParts(const ListPartsReq& req, ListPartsResp* resp) {
  CosResult result = m_object_op.ListParts(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.ListParts(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetObjectACL(const GetObjectACLReq& req,
                               GetObjectACLResp* resp) {
  CosResult result = m_object_op.GetObjectACL(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.GetObjectACL(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutObjectACL(const PutObjectACLReq& req,
                               PutObjectACLResp* resp) {
  CosResult result = m_object_op.PutObjectACL(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.PutObjectACL(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutObjectCopy(const PutObjectCopyReq& req,
                                PutObjectCopyResp* resp) {
  CosResult result = m_object_op.PutObjectCopy(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.PutObjectCopy(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::Copy(const CopyReq& req, CopyResp* resp) {
  CosResult result = m_object_op.Copy(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.Copy(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PostObjectRestore(const PostObjectRestoreReq& req,
                                    PostObjectRestoreResp* resp) {
  CosResult result = m_object_op.PostObjectRestore(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.PostObjectRestore(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::OptionsObject(const OptionsObjectReq& req,
                                OptionsObjectResp* resp) {
  CosResult result = m_object_op.OptionsObject(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.OptionsObject(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::SelectObjectContent(const SelectObjectContentReq& req,
                                      SelectObjectContentResp* resp) {
  CosResult result = m_object_op.SelectObjectContent(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.SelectObjectContent(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::AppendObject(const AppendObjectReq& req,
                               AppendObjectResp* resp) {
  return m_object_op.AppendObject(req, resp);
}

CosResult CosAPI::PutLiveChannel(const PutLiveChannelReq& req,
                                 PutLiveChannelResp* resp) {
  CosResult result = m_object_op.PutLiveChannel(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.PutLiveChannel(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

std::string CosAPI::GetRtmpSignedPublishUrl(
    const std::string& bucket, const std::string& channel, int expire,
    const std::map<std::string, std::string> url_params) {
  std::string rtmp_signed_url = "rtmp://" + bucket + ".cos." +
                                m_config->GetRegion() + ".myqcloud.com/live/" +
                                channel;
  std::string sign_info = AuthTool::RtmpSign(
      m_config->GetAccessKey(), m_config->GetSecretKey(),
      m_config->GetTmpToken(), bucket, channel, url_params, expire);
  return rtmp_signed_url + "?" + sign_info;
}

CosResult CosAPI::PutLiveChannelSwitch(const PutLiveChannelSwitchReq& req,
                                       PutLiveChannelSwitchResp* resp) {
  CosResult result = m_object_op.PutLiveChannelSwitch(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.PutLiveChannelSwitch(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetLiveChannel(const GetLiveChannelReq& req,
                                 GetLiveChannelResp* resp) {
  CosResult result = m_object_op.GetLiveChannel(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.GetLiveChannel(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetLiveChannelHistory(const GetLiveChannelHistoryReq& req,
                                        GetLiveChannelHistoryResp* resp) {
  CosResult result = m_object_op.GetLiveChannelHistory(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.GetLiveChannelHistory(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::GetLiveChannelStatus(const GetLiveChannelStatusReq& req,
                                       GetLiveChannelStatusResp* resp) {
  CosResult result = m_object_op.GetLiveChannelStatus(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.GetLiveChannelStatus(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteLiveChannel(const DeleteLiveChannelReq& req,
                                    DeleteLiveChannelResp* resp) {
  CosResult result = m_object_op.DeleteLiveChannel(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.DeleteLiveChannel(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult
CosAPI::GetLiveChannelVodPlaylist(const GetLiveChannelVodPlaylistReq& req,
                                  GetLiveChannelVodPlaylistResp* resp) {
  CosResult result = m_object_op.GetLiveChannelVodPlaylist(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.GetLiveChannelVodPlaylist(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult
CosAPI::PostLiveChannelVodPlaylist(const PostLiveChannelVodPlaylistReq& req,
                                   PostLiveChannelVodPlaylistResp* resp) {
  CosResult result = m_object_op.PostLiveChannelVodPlaylist(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.PostLiveChannelVodPlaylist(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::ListLiveChannel(const ListLiveChannelReq& req,
                                  ListLiveChannelResp* resp) {
  CosResult result = m_bucket_op.ListLiveChannel(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.ListLiveChannel(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult
CosAPI::PutBucketIntelligentTiering(const PutBucketIntelligentTieringReq& req,
                                    PutBucketIntelligentTieringResp* resp) {
  CosResult result = m_bucket_op.PutBucketIntelligentTiering(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.PutBucketIntelligentTiering(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult
CosAPI::GetBucketIntelligentTiering(const GetBucketIntelligentTieringReq& req,
                                    GetBucketIntelligentTieringResp* resp) {
  CosResult result = m_bucket_op.GetBucketIntelligentTiering(req, resp);
  if (m_bucket_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_bucket_op.GetBucketIntelligentTiering(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::ResumableGetObject(const GetObjectByFileReq& req,
                                     GetObjectByFileResp* resp) {
  CosResult result = m_object_op.ResumableGetObject(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.ResumableGetObject(req, resp, nullptr, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

SharedAsyncContext CosAPI::AsyncPutObject(const AsyncPutObjectReq& req) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  handler->SetTotalSize(req.GetLocalFileSize());
  TaskFunc fn = [=]() {
    PutObjectByFileResp resp;
    m_object_op.PutObject(req, &resp, handler);
  };
  GetGlobalTaskManager().start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

SharedAsyncContext CosAPI::AsyncPutObject(const AsyncPutObjectReq& req, Poco::TaskManager*& taskManager) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  handler->SetTotalSize(req.GetLocalFileSize());
  TaskFunc fn = [=]() {
    PutObjectByFileResp resp;
    m_object_op.PutObject(req, &resp, handler);
  };
  taskManager = &GetGlobalTaskManager();
  (*taskManager).start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

SharedAsyncContext CosAPI::AsyncPutObject(const AsyncPutObjectByStreamReq& req) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  auto& is = req.GetStream();
  is.seekg(0, std::ios::end);
  handler->SetTotalSize(is.tellg());
  is.seekg(0, std::ios::beg);
  TaskFunc fn = [=]() {
    PutObjectByStreamResp resp;
    m_object_op.PutObject(req, &resp, handler);
  };
  GetGlobalTaskManager().start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

SharedAsyncContext CosAPI::AsyncPutObject(const AsyncPutObjectByStreamReq& req, Poco::TaskManager*& taskManager) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  auto& is = req.GetStream();
  is.seekg(0, std::ios::end);
  handler->SetTotalSize(is.tellg());
  is.seekg(0, std::ios::beg);
  TaskFunc fn = [=]() {
    PutObjectByStreamResp resp;
    m_object_op.PutObject(req, &resp, handler);
  };
  taskManager = &GetGlobalTaskManager();
  (*taskManager).start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}


SharedAsyncContext CosAPI::AsyncMultiPutObject(const AsyncMultiPutObjectReq& req) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  handler->SetTotalSize(req.GetLocalFileSize());
  TaskFunc fn = [=]() {
    MultiPutObjectResp resp;
    m_object_op.MultiUploadObject(req, &resp, handler);
  };
  GetGlobalTaskManager().start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

SharedAsyncContext CosAPI::AsyncMultiPutObject(const AsyncMultiPutObjectReq& req, Poco::TaskManager*& taskManager) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  handler->SetTotalSize(req.GetLocalFileSize());
  TaskFunc fn = [=]() {
    MultiPutObjectResp resp;
    m_object_op.MultiUploadObject(req, &resp, handler);
  };
  taskManager = &GetGlobalTaskManager();
  (*taskManager).start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

SharedAsyncContext CosAPI::AsyncGetObject(const AsyncGetObjectReq& req) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  TaskFunc fn = [=]() {
    GetObjectByFileResp resp;
    m_object_op.GetObject(req, &resp, handler);
  };
  GetGlobalTaskManager().start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

SharedAsyncContext CosAPI::AsyncGetObject(const AsyncGetObjectReq& req, Poco::TaskManager*& taskManager) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  TaskFunc fn = [=]() {
    GetObjectByFileResp resp;
    m_object_op.GetObject(req, &resp, handler);
  };
  taskManager = &GetGlobalTaskManager();
  (*taskManager).start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

SharedAsyncContext CosAPI::AsyncResumableGetObject(const AsyncGetObjectReq& req) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  TaskFunc fn = [=]() {
    GetObjectByFileResp resp;
    m_object_op.ResumableGetObject(req, &resp, handler);
  };
  GetGlobalTaskManager().start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

SharedAsyncContext CosAPI::AsyncResumableGetObject(const AsyncGetObjectReq& req, Poco::TaskManager*& taskManager) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  TaskFunc fn = [=]() {
    GetObjectByFileResp resp;
    m_object_op.ResumableGetObject(req, &resp, handler);
  };
  taskManager = &GetGlobalTaskManager();
  (*taskManager).start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

SharedAsyncContext CosAPI::AsyncMultiGetObject(const AsyncMultiGetObjectReq& req) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  TaskFunc fn = [=]() {
    GetObjectByFileResp resp;
    m_object_op.MultiThreadDownload(req, &resp, handler);
  };
  GetGlobalTaskManager().start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

SharedAsyncContext CosAPI::AsyncMultiGetObject(const AsyncMultiGetObjectReq& req, Poco::TaskManager*& taskManager) {
  SharedTransferHandler handler(new TransferHandler());
  handler->SetRequest(reinterpret_cast<const void*>(&req));
  TaskFunc fn = [=]() {
    GetObjectByFileResp resp;
    m_object_op.MultiThreadDownload(req, &resp, handler);
  };
  taskManager = &GetGlobalTaskManager();
  (*taskManager).start(new AsyncTask(std::move(fn)));
  SharedAsyncContext context(new AsyncContext(handler));
  return context;
}

CosResult CosAPI::PutObjects(const PutObjectsByDirectoryReq& req,
                             PutObjectsByDirectoryResp* resp) {
  CosResult result = m_object_op.PutObjects(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.PutObjects(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::PutDirectory(const PutDirectoryReq& req,
                               PutDirectoryResp* resp) {
  CosResult result = m_object_op.PutDirectory(req, resp);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.PutDirectory(req, resp, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}

CosResult CosAPI::DeleteObjects(const DeleteObjectsByPrefixReq& req,
                                DeleteObjectsByPrefixResp* resp) {
  CosResult get_bucket_result;
  GetBucketReq get_bucket_req(req.GetBucketName());
  get_bucket_req.SetPrefix(req.GetPrefix());
  bool is_truncated = false;

  do {
    GetBucketResp get_bucket_resp;
    get_bucket_result = m_bucket_op.GetBucket(get_bucket_req, &get_bucket_resp);
    if (get_bucket_result.IsSucc()) {
      std::vector<Content> contents = get_bucket_resp.GetContents();
      for (auto& content : contents) {
        DeleteObjectReq del_req(req.GetBucketName(), content.m_key);
        DeleteObjectResp del_resp;
        CosResult del_result = DeleteObject(del_req, &del_resp);
        if (del_result.IsSucc()) {
          resp->m_succ_del_objs.push_back(content.m_key);
        } else {
          return del_result;
        }
      }

      get_bucket_req.SetMarker(get_bucket_resp.GetNextMarker());
      is_truncated = get_bucket_resp.IsTruncated();
    }else{
      get_bucket_result = m_bucket_op.GetBucket(get_bucket_req, &get_bucket_resp, COS_CHANGE_BACKUP_DOMAIN);
    }
  } while (get_bucket_result.IsSucc() && is_truncated);

  return get_bucket_result;
};

CosResult CosAPI::MoveObject(const MoveObjectReq& req) {
  CosResult result = m_object_op.MoveObject(req);
  if (m_object_op.UseDefaultDomain() && !result.IsSucc()){
    result = m_object_op.MoveObject(req, COS_CHANGE_BACKUP_DOMAIN);
  }
  return result;
}
CosResult CosAPI::PutBucketToCI(const PutBucketToCIReq& req,
                          PutBucketToCIResp* resp) {
  return m_bucket_op.PutBucketToCI(req, resp);
}

CosResult CosAPI::PutImage(PutImageByFileReq& req,
                           PutImageByFileResp* resp) {
  req.CheckCoverOriginImage();
  return m_object_op.PutImage(req, resp);
}

CosResult CosAPI::CloudImageProcess(const CloudImageProcessReq& req,
                                    CloudImageProcessResp* resp) {
  return m_object_op.CloudImageProcess(req, resp);
}

CosResult CosAPI::GetQRcode(const GetQRcodeReq& req, GetQRcodeResp* resp) {
  return m_object_op.GetQRcode(req, resp);
}

CosResult
CosAPI::DescribeDocProcessBuckets(const DescribeDocProcessBucketsReq& req,
                                  DescribeDocProcessBucketsResp* resp) {
  return m_object_op.DescribeDocProcessBuckets(req, resp);
}

CosResult CosAPI::CreateDocBucket(const CreateDocBucketReq& req,
                                    CreateDocBucketResp* resp) {
  return m_bucket_op.CreateDocBucket(req, resp);
}

CosResult CosAPI::DocPreview(const DocPreviewReq& req, DocPreviewResp* resp) {
  return m_object_op.DocPreview(req, resp);
}

CosResult CosAPI::CreateDocProcessJobs(const CreateDocProcessJobsReq& req,
                                       CreateDocProcessJobsResp* resp) {
  return m_bucket_op.CreateDocProcessJobs(req, resp);
}

CosResult CosAPI::DescribeDocProcessJob(const DescribeDocProcessJobReq& req,
                                        DescribeDocProcessJobResp* resp) {
  return m_bucket_op.DescribeDocProcessJob(req, resp);
}

CosResult CosAPI::DescribeDocProcessJobs(const DescribeDocProcessJobsReq& req,
                                         DescribeDocProcessJobsResp* resp) {
  return m_bucket_op.DescribeDocProcessJobs(req, resp);
}

CosResult
CosAPI::DescribeDocProcessQueues(const DescribeDocProcessQueuesReq& req,
                                 DescribeDocProcessQueuesResp* resp) {
  return m_bucket_op.DescribeDocProcessQueues(req, resp);
}

CosResult CosAPI::UpdateDocProcessQueue(const UpdateDocProcessQueueReq& req,
                                        UpdateDocProcessQueueResp* resp) {
  return m_bucket_op.UpdateDocProcessQueue(req, resp);
}

CosResult CosAPI::DescribeMediaBuckets(const DescribeMediaBucketsReq& req,
                                       DescribeMediaBucketsResp* resp) {
  return m_bucket_op.DescribeMediaBuckets(req, resp);
}

CosResult CosAPI::CreateMediaBucket(const CreateMediaBucketReq& req,
                                    CreateMediaBucketResp* resp) {
  return m_bucket_op.CreateMediaBucket(req, resp);
}

CosResult CosAPI::GetSnapshot(const GetSnapshotReq& req,
                              GetSnapshotResp* resp) {
  return m_object_op.GetObject(static_cast<GetObjectByFileReq>(req),
                               static_cast<GetObjectByFileResp*>(resp));
}

CosResult CosAPI::GetMediaInfo(const GetMediaInfoReq& req,
                               GetMediaInfoResp* resp) {
  return m_object_op.GetMediaInfo(req, resp);
}

CosResult CosAPI::GetPm3u8(const GetPm3u8Req& req,
                              GetPm3u8Resp* resp) {
  return m_object_op.GetObject(static_cast<GetObjectByFileReq>(req),
                               static_cast<GetObjectByFileResp*>(resp));
}

CosResult CosAPI::DescribeMediaQueues(const DescribeMediaQueuesReq& req,
                                     DescribeQueuesResp* resp) {
  return m_bucket_op.DescribeMediaQueues(req, resp);
}

CosResult CosAPI::UpdateMediaQueue(const UpdateMediaQueueReq& req,
                                  UpdateQueueResp* resp) {
  return m_bucket_op.UpdateMediaQueue(req, resp);
}

CosResult CosAPI::CreateDataProcessJobs(const CreateDataProcessJobsReq& req,
                               CreateDataProcessJobsResp* resp) {
  return m_bucket_op.CreateDataProcessJobs(req, resp);
}

CosResult CosAPI::DescribeDataProcessJob(const DescribeDataProcessJobReq& req,
                                 DescribeDataProcessJobResp* resp) {
  return m_bucket_op.DescribeDataProcessJob(req, resp);
}

CosResult CosAPI::CancelDataProcessJob(const CancelDataProcessJobReq& req,
                                 CancelDataProcessJobResp* resp) {
  return m_bucket_op.CancelDataProcessJob(req, resp);
}

CosResult CosAPI::GetImageAuditing(const GetImageAuditingReq& req,
                                   GetImageAuditingResp* resp) {
  return m_object_op.GetImageAuditing(req, resp);
}

CosResult CosAPI::BatchImageAuditing(const BatchImageAuditingReq& req,
                                     BatchImageAuditingResp* resp) {
  return m_bucket_op.BatchImageAuditing(req, resp);
}

CosResult CosAPI::DescribeImageAuditingJob(const DescribeImageAuditingJobReq &req,
                                           DescribeImageAuditingJobResp *resp) {
  return m_bucket_op.DescribeImageAuditingJob(req, resp);
}

CosResult CosAPI::CreateVideoAuditingJob(const CreateVideoAuditingJobReq& req,
                                 CreateVideoAuditingJobResp* resp) {
  return m_bucket_op.CreateVideoAuditingJob(req, resp);
}

CosResult CosAPI::DescribeVideoAuditingJob(const DescribeVideoAuditingJobReq& req,
                                   DescribeVideoAuditingJobResp* resp) {
  return m_bucket_op.DescribeVideoAuditingJob(req, resp);
}

CosResult CosAPI::CreateAudioAuditingJob(const CreateAudioAuditingJobReq& req,
                                 CreateAudioAuditingJobResp* resp) {
  return m_bucket_op.CreateAudioAuditingJob(req, resp);
}

CosResult CosAPI::DescribeAudioAuditingJob(const DescribeAudioAuditingJobReq& req,
                                   DescribeAudioAuditingJobResp* resp) {
  return m_bucket_op.DescribeAudioAuditingJob(req, resp);
}

CosResult CosAPI::CreateTextAuditingJob(const CreateTextAuditingJobReq& req,
                                 CreateTextAuditingJobResp* resp) {
  return m_bucket_op.CreateTextAuditingJob(req, resp);
}

CosResult CosAPI::DescribeTextAuditingJob(const DescribeTextAuditingJobReq& req,
                                   DescribeTextAuditingJobResp* resp) {
  return m_bucket_op.DescribeTextAuditingJob(req, resp);
}

CosResult CosAPI::CreateDocumentAuditingJob(const CreateDocumentAuditingJobReq& req,
                                 CreateDocumentAuditingJobResp* resp) {
  return m_bucket_op.CreateDocumentAuditingJob(req, resp);
}

CosResult CosAPI::DescribeDocumentAuditingJob(const DescribeDocumentAuditingJobReq& req,
                                   DescribeDocumentAuditingJobResp* resp) {
  return m_bucket_op.DescribeDocumentAuditingJob(req, resp);
}


CosResult CosAPI::CreateWebPageAuditingJob(const CreateWebPageAuditingJobReq& req,
                                 CreateWebPageAuditingJobResp* resp) {
  return m_bucket_op.CreateWebPageAuditingJob(req, resp);
}

CosResult CosAPI::DescribeWebPageAuditingJob(const DescribeWebPageAuditingJobReq& req,
                                   DescribeWebPageAuditingJobResp* resp) {
  return m_bucket_op.DescribeWebPageAuditingJob(req, resp);
}

}  // namespace qcloud_cos
