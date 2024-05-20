#include "gtest/gtest.h"
#include "request/auditing_req.h"

namespace qcloud_cos {
TEST(BatchImageAuditingReqTest, BatchImageAuditingReqTest) {
	BatchImageAuditingReq req = BatchImageAuditingReq("test-123");
	std::string body;
	ASSERT_FALSE(req.GenerateRequestBody(&body));
	AuditingInput input = AuditingInput();
	input.SetUrl("url");
	input.SetInterval(15);
	input.SetMaxFrames(15);
	input.SetLargeImageDetect(15);
	UserInfo userinfo = UserInfo();
	userinfo.SetTokenId("15");
	userinfo.SetNickName("15");
	userinfo.SetDeviceId("15");
	userinfo.SetAppId("15");
	userinfo.SetRoom("15");
	userinfo.SetIp("15");
	userinfo.SetType("15");
	userinfo.SetReceiveTokenId("15");
	userinfo.SetGender("15");
	userinfo.SetLevel("15");
	userinfo.SetRole("15");
	input.SetUserInfo(userinfo);
	req.AddInput(input);
	Conf conf = Conf();
	conf.SetBizType("15");
	conf.SetDetectType("15");
	conf.SetCallBack("15");
	conf.SetCallBackVersion("15");
	conf.SetDetectContent(15);
	conf.SetReturnHighlightHtml(true);
	conf.SetAsync(15);
	conf.SetTimeout(15);
	SnapShotConf snap_shot_conf = SnapShotConf();
	snap_shot_conf.SetMode("15");
	snap_shot_conf.SetCount(15);
	snap_shot_conf.SetTimeIterval(15);
	conf.SetSnapShot(snap_shot_conf);
	Freeze freeze = Freeze();
	freeze.SetPornScore(15);
	freeze.SetAdsScore(15);
	conf.SetFreeze(freeze);
	req.SetConf(conf);
	ASSERT_TRUE(req.GenerateRequestBody(&body));
}

TEST(CreateAuditingJobReqTest, CreateAuditingJobReqTest) {
	CreateAuditingJobReq req = CreateAuditingJobReq("test-123");
	std::string body;
	AuditingInput input = AuditingInput();
	input.SetUrl("url");
	input.SetInterval(15);
	input.SetMaxFrames(15);
	input.SetLargeImageDetect(15);
	UserInfo userinfo = UserInfo();
	userinfo.SetTokenId("15");
	userinfo.SetNickName("15");
	userinfo.SetDeviceId("15");
	userinfo.SetAppId("15");
	userinfo.SetRoom("15");
	userinfo.SetIp("15");
	userinfo.SetType("15");
	userinfo.SetReceiveTokenId("15");
	userinfo.SetGender("15");
	userinfo.SetLevel("15");
	userinfo.SetRole("15");
	input.SetUserInfo(userinfo);
	req.SetInput(input);
	Conf conf = Conf();
	conf.SetBizType("15");
	conf.SetDetectType("15");
	conf.SetCallBack("15");
	conf.SetCallBackVersion("15");
	conf.SetDetectContent(15);
	conf.SetReturnHighlightHtml(true);
	conf.SetAsync(15);
	conf.SetTimeout(15);
	SnapShotConf snap_shot_conf = SnapShotConf();
	snap_shot_conf.SetMode("15");
	snap_shot_conf.SetCount(15);
	snap_shot_conf.SetTimeIterval(15);
	conf.SetSnapShot(snap_shot_conf);
	Freeze freeze = Freeze();
	freeze.SetPornScore(15);
	freeze.SetAdsScore(15);
	conf.SetFreeze(freeze);
	req.SetConf(conf);
	ASSERT_TRUE(req.GenerateRequestBody(&body));
}
}