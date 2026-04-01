#include "object_op_test_common.h"

namespace qcloud_cos {

// 静态成员定义
std::string ObjectOpTest::m_bucket_name = "";
std::string ObjectOpTest::m_bucket_name2 = "";
CosConfig* ObjectOpTest::m_config = NULL;
CosAPI* ObjectOpTest::m_client = NULL;

int SslCtxCallback(void* ssl_ctx, void* data) { return 0; }

void ObjectOpTest::InitConfig() {
    m_config = new CosConfig("./config.json");
    m_config->SetIsUseIntranetAddr(false);
    m_config->SetIntranetAddr("");
    m_config->SetDestDomain("");
    m_config->SetDomainSameToHost(false);
    CosSysConfig::SetKeepAlive(false);
    CosSysConfig::SetKeepIdle(20);
    CosSysConfig::SetKeepIntvl(5);
    CosSysConfig::SetUploadCopyPartSize(kPartSize1M * 20);
    CosSysConfig::SetDownThreadPoolSize(10);
    CosSysConfig::SetDownSliceSize(10);
    CosSysConfig::SetDownSliceSize(20 * 1024 * 1024);
    CosSysConfig::SetDownSliceSize(4194304);
    CosSysConfig::SetLogCallback(nullptr);
    CosSysConfig::SetRetryChangeDomain(false);

    m_config->SetAccessKey(GetEnvVar("CPP_SDK_V5_ACCESS_KEY"));
    m_config->SetSecretKey(GetEnvVar("CPP_SDK_V5_SECRET_KEY"));
    m_config->SetRegion(GetEnvVar("CPP_SDK_V5_REGION"));
    if (GetEnvVar("COS_CPP_V5_USE_DNS_CACHE") == "true") {
        std::cout << "================USE DNS CACHE====================" << std::endl;
        CosSysConfig::SetUseDnsCache(true);
    }
    m_client = new CosAPI(*m_config);
}

void ObjectOpTest::SetUpTestCase() {
    std::cout << "================SetUpTestCase Begin====================" << std::endl;
    InitConfig();

    m_bucket_name = "coscppsdkv5ut-obj-" + GetEnvVar("CPP_SDK_V5_APPID");
    m_bucket_name2 = "coscppsdkv5ut-obj-copy-" + GetEnvVar("CPP_SDK_V5_APPID");

    // 先清理旧桶，确保干净环境
    std::vector<std::string> buckets_to_handle = { m_bucket_name, m_bucket_name2 };
    for (auto& bucket : buckets_to_handle) {
        // 删除桶内所有对象
        GetBucketReq get_req(bucket);
        GetBucketResp get_resp;
        CosResult get_result = m_client->GetBucket(get_req, &get_resp);
        if (get_result.IsSucc()) {
            const std::vector<Content>& contents = get_resp.GetContents();
            for (auto c_itr = contents.begin(); c_itr != contents.end(); ++c_itr) {
                DeleteObjectReq del_req(bucket, c_itr->m_key);
                DeleteObjectResp del_resp;
                m_client->DeleteObject(del_req, &del_resp);
            }
        }
        // 删除所有未完成的分块上传
        ListMultipartUploadReq list_mp_req(bucket);
        ListMultipartUploadResp list_mp_resp;
        CosResult list_mp_result = m_client->ListMultipartUpload(list_mp_req, &list_mp_resp);
        if (list_mp_result.IsSucc()) {
            std::vector<Upload> rst = list_mp_resp.GetUpload();
            for (auto itr = rst.begin(); itr != rst.end(); ++itr) {
                AbortMultiUploadReq abort_req(bucket, itr->m_key, itr->m_uploadid);
                AbortMultiUploadResp abort_resp;
                m_client->AbortMultiUpload(abort_req, &abort_resp);
            }
        }
        // 删除桶
        DeleteBucketReq del_req(bucket);
        DeleteBucketResp del_resp;
        m_client->DeleteBucket(del_req, &del_resp);
    }

    // 创建桶
    for (auto& bucket : buckets_to_handle) {
        PutBucketReq req(bucket);
        PutBucketResp resp;
        CosResult result = m_client->PutBucket(req, &resp);
    }

    std::cout << "================SetUpTestCase End====================" << std::endl;
}

/*
 * 不删除Bucket，保留桶防止配额用完，下次执行时无法重新绑定媒体服务等导致执行失败
 *
 * 下次执行时会在 SetUpTestCase 中先清理旧桶再创建
 */
void ObjectOpTest::TearDownTestCase() {
    std::cout << "================TearDownTestCase Begin====================" << std::endl;
    delete m_client;
    delete m_config;
    std::cout << "================TearDownTestCase End====================" << std::endl;
}

} // namespace qcloud_cos
