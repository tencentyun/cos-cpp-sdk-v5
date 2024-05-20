#include <sstream>

#include "cos_api.h"
#include "util/test_utils.h"
#include "gtest/gtest.h"

namespace qcloud_cos {

class LiveChannelOpTest : public testing::Test {
 protected:
  static void SetUpTestCase() {
    std::cout << "================SetUpTestCase Begin===================="
              << std::endl;
    m_config = new CosConfig("./config.json");
    m_config->SetAccessKey(GetEnvVar("CPP_SDK_V5_ACCESS_KEY"));
    m_config->SetSecretKey(GetEnvVar("CPP_SDK_V5_SECRET_KEY"));
    m_config->SetRegion(GetEnvVar("CPP_SDK_V5_REGION"));
    m_client = new CosAPI(*m_config);

    m_bucket_name = "jackytestgz1" + GetEnvVar("COS_CPP_V5_TAG") + "-" +
                    GetEnvVar("CPP_SDK_V5_APPID");
    {
      PutBucketReq req(m_bucket_name);
      PutBucketResp resp;
      CosResult result = m_client->PutBucket(req, &resp);
      ASSERT_TRUE(result.IsSucc());
    }

    std::cout << "================SetUpTestCase End===================="
              << std::endl;
  }

  static void TearDownTestCase() {
    std::cout << "================TearDownTestCase Begin===================="
              << std::endl;
#if 0
        // 1. 删除所有Object
        {
            {
                GetBucketReq req(m_bucket_name);
                GetBucketResp resp;
                CosResult result = m_client->GetBucket(req, &resp);
                ASSERT_TRUE(result.IsSucc());

                const std::vector<Content>& contents = resp.GetContents();
                for (std::vector<Content>::const_iterator c_itr = contents.begin();
                     c_itr != contents.end(); ++c_itr) {
                    const Content& content = *c_itr;
                    DeleteObjectReq del_req(m_bucket_name, content.m_key);
                    DeleteObjectResp del_resp;
                    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
                    EXPECT_TRUE(del_result.IsSucc());
                    if (!del_result.IsSucc()) {
                        std::cout << "DeleteObject Failed, check object=" << content.m_key << std::endl;
                    }
                }
            }

            {
                GetBucketReq req(m_bucket_name2);
                GetBucketResp resp;
                CosResult result = m_client->GetBucket(req, &resp);
                ASSERT_TRUE(result.IsSucc());

                const std::vector<Content>& contents = resp.GetContents();
                for (std::vector<Content>::const_iterator c_itr = contents.begin();
                     c_itr != contents.end(); ++c_itr) {
                    const Content& content = *c_itr;
                    DeleteObjectReq del_req(m_bucket_name2, content.m_key);
                    DeleteObjectResp del_resp;
                    CosResult del_result = m_client->DeleteObject(del_req, &del_resp);
                    EXPECT_TRUE(del_result.IsSucc());
                    if (!del_result.IsSucc()) {
                        std::cout << "DeleteObject Failed, check object=" << content.m_key << std::endl;
                    }
                }
            }
        }

        // 2. 删除所有未complete的分块
        {
            for (std::map<std::string, std::string>::const_iterator c_itr = m_to_be_aborted.begin();
                 c_itr != m_to_be_aborted.end(); ++c_itr) {
                AbortMultiUploadReq req(m_bucket_name, c_itr->first, c_itr->second);
                AbortMultiUploadResp resp;

                CosResult result = m_client->AbortMultiUpload(req, &resp);
                EXPECT_TRUE(result.IsSucc());
                if (!result.IsSucc()) {
                    std::cout << "AbortMultiUpload Failed, object=" << c_itr->first
                        << ", upload_id=" << c_itr->second << std::endl;
                }
            }
        }

        // 3. 删除Bucket
        {
            {
                DeleteBucketReq req(m_bucket_name);
                DeleteBucketResp resp;
                CosResult result = m_client->DeleteBucket(req, &resp);
                ASSERT_TRUE(result.IsSucc());
            }

            {
                DeleteBucketReq req(m_bucket_name2);
                DeleteBucketResp resp;
                CosResult result = m_client->DeleteBucket(req, &resp);
                ASSERT_TRUE(result.IsSucc());
            }
        }

        delete m_client;
        delete m_config;
#endif
    std::cout << "================TearDownTestCase End===================="
              << std::endl;
  }

 protected:
  static CosConfig* m_config;
  static CosAPI* m_client;
  static std::string m_bucket_name;
};

std::string LiveChannelOpTest::m_bucket_name = "";
CosConfig* LiveChannelOpTest::m_config = NULL;
CosAPI* LiveChannelOpTest::m_client = NULL;

TEST_F(LiveChannelOpTest, LiveChannelTest1) {
  std::string channel_name = "test-ch-1";
  // put live channel
  {
    PutLiveChannelReq req(m_bucket_name, channel_name);
    PutLiveChannelResp resp;
    LiveChannelConfiguration config("test", "Enabled", "HLS", 5, 10,
                                    "playlist.m3u8");
    req.SetLiveChannelConfig(config);
    req.SetExpire(1000);
    qcloud_cos::CosResult result = m_client->PutLiveChannel(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    ASSERT_TRUE(StringUtil::StringStartsWith(resp.GetPublishUrl(), "rtmp://"));
    ASSERT_TRUE(StringUtil::StringStartsWith(resp.GetPlayUrl(), "http://"));
  }
}
TEST_F(LiveChannelOpTest, LiveChannelTest2) {
  std::string channel_name = "test-ch-1";

  // get live channel
  {
    GetLiveChannelReq req(m_bucket_name, channel_name);
    GetLiveChannelResp resp;
    qcloud_cos::CosResult result = m_client->GetLiveChannel(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    const LiveChannelConfiguration& conf = resp.GetLiveChannelConf();
    ASSERT_TRUE(conf.GetDescription() == "test");
    ASSERT_TRUE(conf.GetSwitch() == "Enabled");
    ASSERT_TRUE(conf.GetType() == "HLS");
    ASSERT_TRUE(conf.GetFragDuration() == 5);
    ASSERT_TRUE(conf.GetFragCount() == 10);
  }
}
TEST_F(LiveChannelOpTest, LiveChannelTest3) {
  std::string channel_name = "test-ch-1";

  // generate rtmp signed url
  {
    std::string url =
        m_client->GetRtmpSignedPublishUrl(m_bucket_name, channel_name, 3600,
                                          std::map<std::string, std::string>());
    ASSERT_TRUE(StringUtil::StringStartsWith(url, "rtmp://"));
  }

}
TEST_F(LiveChannelOpTest, LiveChannelTest4) {
  std::string channel_name = "test-ch-1";
  
  // put switch
  {
    PutLiveChannelSwitchReq req(m_bucket_name, channel_name);
    PutLiveChannelSwitchResp resp;
    qcloud_cos::CosResult result;
    req.SetDisabled();
    result = m_client->PutLiveChannelSwitch(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    req.SetEnabled();
    result = m_client->PutLiveChannelSwitch(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
}
TEST_F(LiveChannelOpTest, LiveChannelTest5) {
  std::string channel_name = "test-ch-1";

  // get live channel history
  {
    GetLiveChannelHistoryReq req(m_bucket_name, channel_name);
    GetLiveChannelHistoryResp resp;
    qcloud_cos::CosResult result = m_client->GetLiveChannelHistory(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
}
TEST_F(LiveChannelOpTest, LiveChannelTest6) {
  std::string channel_name = "test-ch-1";

  // get live channel status
  {
    GetLiveChannelStatusReq req(m_bucket_name, channel_name);
    GetLiveChannelStatusResp resp;
    qcloud_cos::CosResult result = m_client->GetLiveChannelStatus(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    LiveChannelStatus status = resp.GetLiveChannelStatus();
    ASSERT_TRUE(status.m_status == "Idle");
  }
}
TEST_F(LiveChannelOpTest, LiveChannelTest7) {
  std::string channel_name = "test-ch-1";

  // list live channel
  {
    ListLiveChannelReq req(m_bucket_name);
    ListLiveChannelResp resp;
    qcloud_cos::CosResult result = m_client->ListLiveChannel(req, &resp);
    ASSERT_TRUE(result.IsSucc());
    ListLiveChannelResult list_result = resp.GetListResult();
    ASSERT_TRUE(list_result.m_channels.size() > 0);
  }
}
TEST_F(LiveChannelOpTest, LiveChannelTest8) {
  std::string channel_name = "test-ch-1";

  // post vod
  {
    PostLiveChannelVodPlaylistReq req(m_bucket_name, channel_name);
    PostLiveChannelVodPlaylistResp resp;
    req.SetTime(time(NULL) - 10000, time(NULL));
    req.SetPlaylistName("newplaylist.m3u8");
    qcloud_cos::CosResult result =
        m_client->PostLiveChannelVodPlaylist(req, &resp);
    ASSERT_TRUE(result.IsSucc());

    req.SetTime(time(NULL), time(NULL) - 10000);
    result = m_client->PostLiveChannelVodPlaylist(req, &resp);
    ASSERT_TRUE(result.GetHttpStatus() == 400);
  }
}
TEST_F(LiveChannelOpTest, LiveChannelTest9) {
  std::string channel_name = "test-ch-1";

  // get vod
  {
    GetLiveChannelVodPlaylistReq req(m_bucket_name, channel_name);
    GetLiveChannelVodPlaylistResp resp;
    req.SetTime(time(NULL) - 10000, time(NULL));
    qcloud_cos::CosResult result =
        m_client->GetLiveChannelVodPlaylist(req, &resp);
    ASSERT_TRUE(result.IsSucc());

    req.SetTime(time(NULL), time(NULL) - 10000);
    result = m_client->GetLiveChannelVodPlaylist(req, &resp);
    ASSERT_TRUE(result.GetHttpStatus() == 400);
  }
}
TEST_F(LiveChannelOpTest, LiveChannelTest10) {
  std::string channel_name = "test-ch-1";
  // delete live channel
  {
    DeleteLiveChannelReq req(m_bucket_name, channel_name);
    DeleteLiveChannelResp resp;
    qcloud_cos::CosResult result = m_client->DeleteLiveChannel(req, &resp);
    ASSERT_TRUE(result.IsSucc());
  }
}
TEST_F(LiveChannelOpTest, ListLiveChannelRespTest) {
  {
    std::string body;
    body += "<ListLiveChannelResult>";
    body += "    <MaxKeys>111</MaxKeys>";
    body += "    <Prefix>prefix</Prefix>";
    body += "    <Marker>marker</Marker>";
    body += "    <NextMarker>1000</NextMarker>";
    body += "    <IsTruncated>false</IsTruncated>";
    body += "    <LiveChannel>";
    body += "	       <Name>prefixA_0</Name>";
    body += "        <LastModified>2024-05-20T06:42:19.000Z</LastModified>";
    body += "    </LiveChannel>";
    body += "</ListLiveChannelResult>";

    ListLiveChannelResp resp;
    resp.ParseFromXmlString(body);
    ASSERT_EQ(resp.GetListResult().m_max_keys, "111");
    ASSERT_EQ(resp.GetListResult().m_marker, "marker");
    ASSERT_EQ(resp.GetListResult().m_prefix, "prefix");
    ASSERT_EQ(resp.GetListResult().m_is_truncated, "false");
  }
  //异常情况
  {
    std::string body;
    body += "<ListLiveChannelResult>";
    body += "    <MaxKeys>111</MaxKeys>";
    body += "    <Prefix>prefix</Prefix>";
    body += "    <Marker>marker</Marker>";
    body += "    <NextMarker>1000</NextMarker>";
    body += "    <IsTruncated>false</IsTruncated>";
    body += "    <LiveChannel>";
    body += "	       <Name>prefixA_0</Name>";
    body += "        <LastModified>2024-05-20T06:42:19.000Z</LastModified>";
    body += "        <Unknown>sevenyou_e1</Unknown>";
    body += "    </LiveChannel>";
    body += "    <Unknown>sevenyou_e1</Unknown>";
    body += "</ListLiveChannelResult>";

    ListLiveChannelResp resp;
    bool result = resp.ParseFromXmlString(body);
    ASSERT_TRUE(!result);
  }
  {
      ListLiveChannelResp resp;
      bool result = resp.ParseFromXmlString("xsxsxxxs");
      ASSERT_TRUE(!result);
  }
  {
      std::string body = "<null>error</null>";
      ListLiveChannelResp resp;
      bool result = resp.ParseFromXmlString(body);
      ASSERT_TRUE(!result);
  }

}
}  // namespace qcloud_cos
