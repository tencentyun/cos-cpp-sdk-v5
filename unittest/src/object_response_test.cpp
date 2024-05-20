// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 07/25/17
// Description:

#include "gtest/gtest.h"
#include "response/object_resp.h"
#include "response/data_process_resp.h"
#include "response/auditing_resp.h"

namespace qcloud_cos {

TEST(ObjectRespTest, DocRespTest) {
  {
    DocPreviewResp resp;
    std::map<std::string, std::string> headers = {
      {kRespHeaderXTotalPage, "2"},
      {kRespHeaderXErrNo, "0"},
      {kRespHeaderXTotalSheet, "1"},
      {kRespHeaderXSheetName, "test1"}
    };
    resp.ParseFromHeaders(headers);
    ASSERT_EQ(resp.GetTotalPage(), 2);
    ASSERT_EQ(resp.GetErrNo(), "0");
    ASSERT_EQ(resp.GetTotalSheet(), 1);
    ASSERT_EQ(resp.GetSheetName(), "test1");
  }
  std::string base_body;
  {
    CreateDocProcessJobsResp resp;
    std::string body;
    body +=      "<Response>\n";
    base_body += "        <JobsDetail>\n";
    base_body += "                <Code>0</Code>\n";
    base_body += "                <CreationTime>1</CreationTime>\n";
    base_body += "                <EndTime>2</EndTime>\n";
    base_body += "                <Input>\n";
    base_body += "                    <Object>3</Object>\n";
    base_body += "                </Input>\n";
    base_body += "                <JobId>4</JobId>\n";
    base_body += "                <Message/>\n";
    base_body += "                <Operation>\n";
    base_body += "                    <DocProcess>\n";
    base_body += "                        <SrcType>5</SrcType>\n";
    base_body += "                        <TgtType>6</TgtType>\n";
    base_body += "                        <StartPage>7</StartPage>\n";
    base_body += "                        <EndPage>8</EndPage>\n";
    base_body += "                        <SheetId>16</SheetId>\n";
    base_body += "                        <DocPassword>17</DocPassword>\n";
    base_body += "                        <Comments>18</Comments>\n";
    base_body += "                        <PaperDirection>19</PaperDirection>\n";
    base_body += "                        <Quality>20</Quality>\n";
    base_body += "                        <Quality>20</Quality>\n";
    base_body += "                        <Zoom>21</Zoom>\n";
    base_body += "                        <ImageParams>9</ImageParams>\n";
    base_body += "                    </DocProcess>\n";
    base_body += "                    <Output>\n";
    base_body += "                        <Bucket>10</Bucket>\n";
    base_body += "                        <Object>11</Object>\n";
    base_body += "                        <Region>12</Region>\n";
    base_body += "                    </Output>\n";
    base_body += "                    <DocProcessResult>\n";
    base_body += "                        <PageInfo>\n";
    base_body += "                            <PageNo>22</PageNo>\n";
    base_body += "                            <TgtUri>23</TgtUri>\n";
    base_body += "                            <X-SheetPics>24</X-SheetPics>\n";
    base_body += "                            <PicIndex>32</PicIndex>\n";
    base_body += "                            <PicNum>25</PicNum>\n";
    base_body += "                        </PageInfo>\n";
    base_body += "                        <TgtType>26</TgtType>\n";
    base_body += "                        <TotalPageCount>27</TotalPageCount>\n";
    base_body += "                        <SuccPageCount>28</SuccPageCount>\n";
    base_body += "                        <FailPageCount>29</FailPageCount>\n";
    base_body += "                        <TotalSheetCount>30</TotalSheetCount>\n";
    base_body += "                        <TaskId>31</TaskId>\n";
    base_body += "                    </DocProcessResult>\n";
    base_body += "                </Operation>\n";
    base_body += "                <QueueId>13</QueueId>\n";
    base_body += "                <State>14</State>\n";
    base_body += "                <Tag>15</Tag>\n";
    base_body += "        </JobsDetail>\n";
    body += base_body + "</Response>";
    resp.ParseFromXmlString(body);
    ASSERT_EQ(resp.GetJobsDetail().code, "0");
    ASSERT_EQ(resp.GetJobsDetail().create_time, "1");
    ASSERT_EQ(resp.GetJobsDetail().end_time, "2");
    ASSERT_EQ(resp.GetJobsDetail().input.object, "3");
    resp.GetJobsDetail().input.to_string();
    ASSERT_EQ(resp.GetJobsDetail().job_id, "4");
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.src_type, "5");
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.tgt_type, "6");
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.sheet_id, 16);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.start_page, 7);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.end_page, 8);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.image_params, "9");
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.doc_passwd, "17");
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.comments, 18);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.paper_direction, 19);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.quality, 20);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process.zoom, 21);
    resp.GetJobsDetail().operation.doc_process.to_string();
    ASSERT_EQ(resp.GetJobsDetail().operation.output.bucket, "10");
    ASSERT_EQ(resp.GetJobsDetail().operation.output.object, "11");
    ASSERT_EQ(resp.GetJobsDetail().operation.output.region, "12");
    resp.GetJobsDetail().operation.output.to_string();
    ASSERT_EQ(resp.GetJobsDetail().queue_id, "13");
    ASSERT_EQ(resp.GetJobsDetail().state, "14");
    ASSERT_EQ(resp.GetJobsDetail().tag, "15");
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.page_infos[0].page_no, 22);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.page_infos[0].tgt_uri, "23");
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.page_infos[0].x_sheet_pics, 24);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.page_infos[0].pic_index, 32);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.page_infos[0].pic_num, 25);
    resp.GetJobsDetail().operation.doc_process_result.page_infos[0].to_string();
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.tgt_type, "26");
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.task_id, "31");
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.total_pageount, 27);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.succ_pagecount, 28);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.fail_pagecount, 29);
    ASSERT_EQ(resp.GetJobsDetail().operation.doc_process_result.total_sheetcount, 30);
    resp.GetJobsDetail().operation.doc_process_result.to_string();
    resp.GetJobsDetail().operation.to_string();
    resp.GetJobsDetail().to_string();
  }
  {
    DescribeDocProcessJobsResp resp;
    std::string body;
    body += "<Response>\n";
    body += base_body;
    body += "        <NextToken>32</NextToken>\n";
    body += "</Response>";
    // std::cout << body << std::endl;
    resp.ParseFromXmlString(body);
    ASSERT_EQ(resp.GetNextToken(), "32");
  }
  {
    std::string body = "<Response>\n";
    body += "        <TotalCount>1</TotalCount>\n";
    body += "        <RequestId>RequestId</RequestId>\n";
    body += "        <PageNumber>1</PageNumber>\n";
    body += "        <PageSize>2</PageSize>\n";
    body += "        <QueueList>\n";
    body += "                <QueueId>QueueId</QueueId>\n";
    body += "                <Name>QueueName</Name>\n";
    body += "                <State>Active</State>\n";
    body += "                <NotifyConfig>\n";
    body += "                        <Url>url</Url>\n";
    body += "                        <Event>Event</Event>\n";
    body += "                        <Type>Type</Type>\n";
    body += "                        <State>Off</State>\n";
    body += "                </NotifyConfig>\n";
    body += "                <MaxSize>10000</MaxSize>\n";
    body += "                <MaxConcurrent>10</MaxConcurrent>\n";
    body += "                <CreateTime>CreateTime</CreateTime>\n";
    body += "                <UpdateTime>UpdateTime</UpdateTime>\n";
    body += "                <BucketId>BucketId</BucketId>\n";
    body += "                <Category>DocProcess</Category>\n";
    body += "        </QueueList>\n";
    body += "        <NonExistPIDs>\n";
    body += "                <QueueId>3</QueueId>\n";
    body += "        </NonExistPIDs>\n";
    body += "</Response>";
    DescribeDocProcessQueuesResp resp;
    resp.ParseFromXmlString(body);
    ASSERT_EQ(resp.GetTotalCount(), 1);
    ASSERT_EQ(resp.GetRequestId(), "RequestId");
    ASSERT_EQ(resp.GetPageNumber(), 1);
    ASSERT_EQ(resp.GetPageSize(), 2);
    ASSERT_EQ(resp.GetQueueList().queue_id, "QueueId");
    ASSERT_EQ(resp.GetQueueList().name, "QueueName");
    ASSERT_EQ(resp.GetQueueList().state, "Active");
    ASSERT_EQ(resp.GetQueueList().notify_config.url, "url");
    ASSERT_EQ(resp.GetQueueList().notify_config.event, "Event");
    ASSERT_EQ(resp.GetQueueList().notify_config.type, "Type");
    ASSERT_EQ(resp.GetQueueList().notify_config.state, "Off");
    ASSERT_EQ(resp.GetQueueList().max_size, 10000);
    ASSERT_EQ(resp.GetQueueList().max_concurrent, 10);
    ASSERT_EQ(resp.GetQueueList().create_time, "CreateTime");
    ASSERT_EQ(resp.GetQueueList().update_time, "UpdateTime");
    ASSERT_EQ(resp.GetQueueList().bucket_id, "BucketId");
    ASSERT_EQ(resp.GetQueueList().category, "DocProcess");
    resp.GetQueueList().to_string();
    ASSERT_EQ(resp.GetNonExistPIDs().queue_id[0], "3");
  }
  {
    std::string body = "<Response>\n";
    body += "        <RequestId>RequestId</RequestId>\n";
    body += "        <Queue>\n";
    body += "                <QueueId>QueueId</QueueId>\n";
    body += "                <Name>QueueName</Name>\n";
    body += "                <State>Active</State>\n";
    body += "                <NotifyConfig>\n";
    body += "                        <Url>url</Url>\n";
    body += "                        <Event>Event</Event>\n";
    body += "                        <Type>Type</Type>\n";
    body += "                        <State>Off</State>\n";
    body += "                </NotifyConfig>\n";
    body += "                <MaxSize>10000</MaxSize>\n";
    body += "                <MaxConcurrent>10</MaxConcurrent>\n";
    body += "                <CreateTime>CreateTime</CreateTime>\n";
    body += "                <UpdateTime>UpdateTime</UpdateTime>\n";
    body += "                <BucketId>BucketId</BucketId>\n";
    body += "                <Category>DocProcess</Category>\n";
    body += "        </Queue>\n";
    body += "</Response>";
    UpdateDocProcessQueueResp resp;
    resp.ParseFromXmlString(body);
    ASSERT_EQ(resp.GetRequestId(), "RequestId");
    ASSERT_EQ(resp.GetQueueList().queue_id, "QueueId");
    ASSERT_EQ(resp.GetQueueList().name, "QueueName");
    ASSERT_EQ(resp.GetQueueList().state, "Active");
    ASSERT_EQ(resp.GetQueueList().notify_config.url, "url");
    ASSERT_EQ(resp.GetQueueList().notify_config.event, "Event");
    ASSERT_EQ(resp.GetQueueList().notify_config.type, "Type");
    ASSERT_EQ(resp.GetQueueList().notify_config.state, "Off");
    ASSERT_EQ(resp.GetQueueList().max_size, 10000);
    ASSERT_EQ(resp.GetQueueList().max_concurrent, 10);
    ASSERT_EQ(resp.GetQueueList().create_time, "CreateTime");
    ASSERT_EQ(resp.GetQueueList().update_time, "UpdateTime");
    ASSERT_EQ(resp.GetQueueList().bucket_id, "BucketId");
    ASSERT_EQ(resp.GetQueueList().category, "DocProcess");
  }
}

TEST(ObjectRespTest, ObjectTest) {
  {
    std::string body = "<CompleteMultipartUploadResult>\n";
    body += "        <Location>Location</Location>\n";
    body += "        <Bucket>Bucket</Bucket>\n";
    body += "        <Key>Key</Key>\n";
    body += "        <ETag>ETag</ETag>\n";
    body += "</CompleteMultipartUploadResult>";
    MultiPutObjectResp resp;
    resp.ParseFromXmlString(body);
    ASSERT_EQ(resp.GetLocation(), "Location");
    ASSERT_EQ(resp.GetBucket(), "Bucket");
    ASSERT_EQ(resp.GetKey(), "Key");
    ASSERT_EQ(resp.GetEtag(), "ETag");
  }
  {
    std::string body;
    body += "<LiveChannelHistory>\n";
    body += "        <LiveRecord>\n";
    body += "                <StartTime>StartTime</StartTime>\n";
    body += "                <EndTime>EndTime</EndTime>\n";
    body += "                <RemoteAddr>RemoteAddr</RemoteAddr>\n";
    body += "                <RequestId>RequestId</RequestId>\n";
    body += "        </LiveRecord>\n";
    body += "</LiveChannelHistory>";
    GetLiveChannelHistoryResp resp;
    resp.ParseFromXmlString(body);
    ASSERT_EQ(resp.GetChanHistory()[0].m_start_time, "StartTime");
    ASSERT_EQ(resp.GetChanHistory()[0].m_end_time, "EndTime");
    ASSERT_EQ(resp.GetChanHistory()[0].m_remote_addr, "RemoteAddr");
    ASSERT_EQ(resp.GetChanHistory()[0].m_request_id, "RequestId");
  }
  {
    std::string body;
    body += "<LiveChannelStatus>\n";
    body += "        <Status>Live</Status>\n";
    body += "        <ConnectedTime>ConnectedTime</ConnectedTime>\n";
    body += "        <RemoteAddr>RemoteAddr</RemoteAddr>\n";
    body += "        <RequestId>RequestId</RequestId>\n";
    body += "        <Video>\n";
    body += "                <Width>Width</Width>\n";
    body += "                <Height>Height</Height>\n";
    body += "                <FrameRate>FrameRate</FrameRate>\n";
    body += "                <Bandwidth>Bandwidth</Bandwidth>\n";
    body += "                <Codec>Codec</Codec>\n";
    body += "        </Video>\n";
    body += "        <Audio>\n";
    body += "                <Bandwidth>Bandwidth</Bandwidth>\n";
    body += "                <SampleRate>SampleRate</SampleRate>\n";
    body += "                <Codec>Codec</Codec>\n";
    body += "        </Audio>\n";
    body += "</LiveChannelStatus>";
    GetLiveChannelStatusResp resp;
    resp.ParseFromXmlString(body);
    ASSERT_EQ(resp.GetLiveChannelStatus().m_status, "Live");
    ASSERT_EQ(resp.GetLiveChannelStatus().m_connected_time, "ConnectedTime");
    ASSERT_EQ(resp.GetLiveChannelStatus().m_remote_addr, "RemoteAddr");
    ASSERT_EQ(resp.GetLiveChannelStatus().m_request_id, "RequestId");
    ASSERT_EQ(resp.GetLiveChannelStatus().m_video.m_width, "Width");
    ASSERT_EQ(resp.GetLiveChannelStatus().m_video.m_heigh, "Height");
    ASSERT_EQ(resp.GetLiveChannelStatus().m_video.m_framerate, "FrameRate");
    ASSERT_EQ(resp.GetLiveChannelStatus().m_video.m_bandwidth, "Bandwidth");
    ASSERT_EQ(resp.GetLiveChannelStatus().m_video.m_codec, "Codec");
    ASSERT_TRUE(resp.GetLiveChannelStatus().m_has_audio);
    ASSERT_EQ(resp.GetLiveChannelStatus().m_audio.m_bandwidth, "Bandwidth");
    ASSERT_EQ(resp.GetLiveChannelStatus().m_audio.m_samplerate, "SampleRate");
    ASSERT_EQ(resp.GetLiveChannelStatus().m_audio.m_codec, "Codec");

  }
  {
    std::string body ;
    body += "<DeleteResult>\n";
    body += "        <Deleted>\n";
    body += "                <Key>string1</Key>\n";
    body += "                <DeleteMarker>true</DeleteMarker>\n";
    body += "                <DeleteMarkerVersionId>string1</DeleteMarkerVersionId>\n";
    body += "        </Deleted>\n";
    body += "        <Deleted>\n";
    body += "                <Key>string2</Key>\n";
    body += "                <DeleteMarkerVersionId>string2</DeleteMarkerVersionId>\n";
    body += "        </Deleted>\n";
    body += "        <Deleted>\n";
    body += "                <Key>string3</Key>\n";
    body += "                <DeleteMarker>false</DeleteMarker>\n";
    body += "                <DeleteMarkerVersionId>string3</DeleteMarkerVersionId>\n";
    body += "                <VersionId>string3</VersionId>\n";
    body += "        </Deleted>\n";
    body += "        <Error>\n";
    body += "                <Key>string4</Key>\n";
    body += "                <Code>string4</Code>\n";
    body += "                <Message>string4</Message>\n";
    body += "                <VersionId>string4</VersionId>\n";
    body += "        </Error>\n";
    body += "</DeleteResult>";
    DeleteObjectsResp resp;
    resp.ParseFromXmlString(body);
    ASSERT_EQ(resp.GetErrorMsgs()[0].m_code, "string4");
    ASSERT_EQ(resp.GetErrorMsgs()[0].m_key, "string4");
    ASSERT_EQ(resp.GetErrorMsgs()[0].m_message, "string4");
    ASSERT_EQ(resp.GetErrorMsgs()[0].m_version_id, "string4");
    ASSERT_EQ(resp.GetDeletedInfos()[0].m_key, "string1");
    ASSERT_TRUE(resp.GetDeletedInfos()[0].m_delete_marker);
    ASSERT_EQ(resp.GetDeletedInfos()[0].m_delete_marker_version_id, "string1");
    ASSERT_EQ(resp.GetDeletedInfos()[1].m_key, "string2");
    ASSERT_EQ(resp.GetDeletedInfos()[1].m_delete_marker_version_id, "string2");
    ASSERT_EQ(resp.GetDeletedInfos()[2].m_key, "string3");
    ASSERT_TRUE(!resp.GetDeletedInfos()[2].m_delete_marker);
    ASSERT_EQ(resp.GetDeletedInfos()[2].m_delete_marker_version_id, "string3");
    ASSERT_EQ(resp.GetDeletedInfos()[2].m_version_id, "string3");
  }
}

TEST(ObjectRespTest, AudioAuditingRespTest) {
  std::string base_body;
  {
    AudioAuditingResp resp;
    std::string body;
    body +=      "<Response>\n";
    base_body += "        <JobsDetail>\n";
    base_body += "                <Code>0</Code>\n";
    base_body += "                <Message/>\n";
    base_body += "                <JobId>4</JobId>\n";
    base_body += "                <State>14</State>\n";
    base_body += "                <CreationTime>1</CreationTime>\n";
    base_body += "                <Result>40</Result>\n";
    base_body += "                <Label>41</Label>\n";
    base_body += "                <AudioText>2</AudioText>\n";

    base_body += "                <PornInfo>\n";
    std::string info_body;
    info_body += "                    <Code>3</Code>\n";
    info_body += "                    <Msg>3</Msg>\n";
    info_body += "                    <Count>3</Count>\n";
    info_body += "                    <HitFlag>3</HitFlag>\n";
    info_body += "                    <Score>3</Score>\n";
    info_body += "                    <SubLabel>3</SubLabel>\n";
    info_body += "                    <Category>3</Category>\n";
    info_body += "                    <Keywords>3</Keywords>\n";
    info_body += "                    <OcrResults>\n";
    info_body += "                        <Text>5</Text>\n";
    info_body += "                        <Keywords>6</Keywords>\n";
    info_body += "                        <Location>\n";
    info_body += "                            <X>5</X>\n";
    info_body += "                            <Y>5</Y>\n";
    info_body += "                            <Height>5</Height>\n";
    info_body += "                            <Width>5</Width>\n";
    info_body += "                            <Rotate>5</Rotate>\n";
    info_body += "                        </Location>\n";
    info_body += "                    </OcrResults>\n";
    info_body += "                    <ObjectResults>\n";
    info_body += "                        <Name>5</Name>\n";
    info_body += "                        <Location>\n";
    info_body += "                            <X>5</X>\n";
    info_body += "                            <Y>5</Y>\n";
    info_body += "                            <Height>5</Height>\n";
    info_body += "                            <Width>5</Width>\n";
    info_body += "                            <Rotate>5</Rotate>\n";
    info_body += "                        </Location>\n";
    info_body += "                    </ObjectResults>\n";
    info_body += "			              <RecognitionResults>\n";
    info_body += "			                  <Label>15</Label>\n";
    info_body += "			                  <Score>15</Score>\n";
    info_body += "			                  <StartTime>15</StartTime>\n";
    info_body += "			                  <EndTime>15</EndTime>\n";
    info_body += "			              </RecognitionResults>\n";    
    info_body += "			              <SpeakerResults>\n";
    info_body += "			                  <Label>15</Label>\n";
    info_body += "			                  <Score>15</Score>\n";
    info_body += "			                  <StartTime>15</StartTime>\n";
    info_body += "			                  <EndTime>15</EndTime>\n";
    info_body += "			              </SpeakerResults>\n";	
    info_body += "			              <LibResults>\n";    
    info_body += "			              	  <LibType>15</LibType>\n";    
    info_body += "			              	  <LibName>15</LibName>\n";    
    info_body += "			              	  <Keywords>15</Keywords>\n";    
    info_body += "			              	  <Keywords>15</Keywords>\n";    
    info_body += "			              </LibResults>\n";    
    base_body += info_body;
    base_body += "                </PornInfo>\n";
    
    base_body += "                <AdsInfo>\n";
    base_body += info_body;
    base_body += "                </AdsInfo>\n";
    
    base_body += "                <AbuseInfo>\n";
    base_body += info_body;
    base_body += "                </AbuseInfo>\n";

    base_body += "                <IllegalInfo>\n";
    base_body += info_body;
    base_body += "                </IllegalInfo>\n";

    base_body += "                <PoliticsInfo>\n";
    base_body += info_body;
    base_body += "                </PoliticsInfo>\n";

    base_body += "                <TerrorismInfo>\n";
    base_body += info_body;
    base_body += "                </TerrorismInfo>\n";

	base_body += "                <TeenagerInfo>\n";
    base_body += info_body;
    base_body += "                </TeenagerInfo>\n";

    base_body += "			      <LanguageResults>\n";
    base_body += "			          <Label>15</Label>\n";
    base_body += "			          <Score>15</Score>\n";
    base_body += "			          <StartTime>15</StartTime>\n";
    base_body += "			          <EndTime>15</EndTime>\n";
    base_body += "			      </LanguageResults>\n";  
    base_body += "                <UserInfo>\n";
    base_body += "                    <TokenId>15</TokenId>\n";
    base_body += "                    <Nickname>15</Nickname>\n";
    base_body += "                    <DeviceId>15</DeviceId>\n";
    base_body += "                    <AppId>15</AppId>\n";
    base_body += "                    <Room>15</Room>\n";
    base_body += "                    <IP>15</IP>\n";
    base_body += "                    <Type>15</Type>\n";
    base_body += "                    <ReceiveTokenId>15</ReceiveTokenId>\n";
    base_body += "                    <Gender>15</Gender>\n";
    base_body += "                    <Level>15</Level>\n";
    base_body += "                    <Role>15</Role>\n";
    base_body += "                </UserInfo>\n";
    base_body += "                <DataId>13</DataId>\n";
    base_body += "                <Object>15</Object>\n";
    base_body += "                <Url>15</Url>\n";
    base_body += "                <SubLabel>15</SubLabel>\n";
    base_body += "                <Section>\n";
    base_body += "                    <Url>15</Url>\n";
    base_body += "                    <SnapShotTime>15</SnapShotTime>\n";
    base_body += "                    <Text>15</Text>\n";
    base_body += "                    <OffsetTime>15</OffsetTime>\n";
    base_body += "                    <Duration>15</Duration>\n";
    base_body += "                    <Label>15</Label>\n";
    base_body += "                    <Result>15</Result>\n";
    base_body += "                    <ForbidState>15</ForbidState>\n";
    base_body += "                    <StartByte>15</StartByte>\n";
    base_body += "                </Section>\n";
    base_body += "                <SubLabel>15</SubLabel>\n";
    base_body += "                <ForbidState>15</ForbidState>\n";
    base_body += "                <ListInfo>\n";
	base_body += "                	  <ListResults>\n";
    base_body += "                    		<ListType>15</ListType>\n";
    base_body += "                    		<ListName>15</ListName>\n";
    base_body += "                    		<Entity>15</Entity>\n";
	base_body += "                	  </ListResults>\n";
    base_body += "                </ListInfo>\n";	
    base_body += "        </JobsDetail>\n";
    body += base_body + "</Response>";
    resp.ParseFromXmlString(body);
    ASSERT_EQ(resp.GetJobsDetail().GetCode(), "0");
    ASSERT_EQ(resp.GetJobsDetail().GetJobId(), "4");
    ASSERT_EQ(resp.GetJobsDetail().GetState(), "14");
    ASSERT_EQ(resp.GetJobsDetail().GetCreationTime(), "1");
    ASSERT_EQ(resp.GetJobsDetail().GetResult(), 40);
    ASSERT_EQ(resp.GetJobsDetail().GetLabel(), "41");
    ASSERT_EQ(resp.GetJobsDetail().GetAudioText(), "2");
    ASSERT_EQ(resp.GetJobsDetail().GetForbidState(), 15);
    ASSERT_TRUE(resp.GetJobsDetail().HasCode());
    ASSERT_TRUE(resp.GetJobsDetail().HasJobId());
    ASSERT_TRUE(resp.GetJobsDetail().HasState());
    ASSERT_TRUE(resp.GetJobsDetail().HasCreationTime());
    ASSERT_TRUE(resp.GetJobsDetail().HasResult());
    ASSERT_TRUE(resp.GetJobsDetail().HasLabel());
    ASSERT_TRUE(resp.GetJobsDetail().HasAudioText());
    ASSERT_TRUE(resp.GetJobsDetail().HasLabel());
    ASSERT_TRUE(resp.GetJobsDetail().HasUrl());
    ASSERT_TRUE(resp.GetJobsDetail().HasListInfo());



    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetCode(), 3);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetMsg(), "3");
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetCount(), 3);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetHitFlag(), 3);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetScore(), 3);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetSubLabel(), "3");
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetCategory(), "3");
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetKeyWords()[0], "3");
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetOcrResults()[0].GetKeyWords()[0], "6");
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetOcrResults()[0].GetText(), "5");
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetOcrResults()[0].GetLocation().GetX(), 5);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetOcrResults()[0].GetLocation().GetY(), 5);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetOcrResults()[0].GetLocation().GetWidth(), 5);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetOcrResults()[0].GetLocation().GetHeight(), 5);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetOcrResults()[0].GetLocation().GetRotate(), 5);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetCode(), 3);


    ASSERT_TRUE(resp.GetJobsDetail().HasPornInfo());
    ASSERT_TRUE(resp.GetJobsDetail().HasAbuseInfo());
    ASSERT_TRUE(resp.GetJobsDetail().HasAdsInfo());
    ASSERT_TRUE(resp.GetJobsDetail().HasIllegalInfo());
    ASSERT_TRUE(resp.GetJobsDetail().HasPoliticsInfo());
    ASSERT_TRUE(resp.GetJobsDetail().HasTerrorismInfo());
    ASSERT_TRUE(resp.GetJobsDetail().HasTeenagerInfo());


    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetTokenId(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetNickName(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetDeviceId(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetAppId(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetRoom(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetIp(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetType(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetReceiveTokenId(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetGender(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetLevel(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetUserInfo().GetRole(), "15");
    
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasTokenId());
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasNickName());
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasDeviceId());
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasAppId());
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasRoom());
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasIp());
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasType());
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasReceiveTokenId());
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasGender());
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasLevel());
    ASSERT_TRUE(resp.GetJobsDetail().GetUserInfo().HasRole());

    ASSERT_TRUE(resp.GetJobsDetail().HasUserInfo());
    ASSERT_EQ(resp.GetJobsDetail().GetDataId(), "13");
    ASSERT_EQ(resp.GetJobsDetail().GetUrl(), "15");
    ASSERT_EQ(resp.GetJobsDetail().GetObject(), "15");

    ASSERT_TRUE(resp.GetJobsDetail().HasDataId());
    ASSERT_TRUE(resp.GetJobsDetail().HasUrl());
    ASSERT_TRUE(resp.GetJobsDetail().HasObject());
    resp.GetJobsDetail().to_string();

    ASSERT_EQ(resp.GetJobsDetail().GetSection()[0].GetUrl(),"15");
    ASSERT_EQ(resp.GetJobsDetail().GetSection()[0].GetDuration(),15);
    ASSERT_EQ(resp.GetJobsDetail().GetSection()[0].GetLabel(),"15");
    ASSERT_EQ(resp.GetJobsDetail().GetSection()[0].GetOffsetTime(),15);
    ASSERT_EQ(resp.GetJobsDetail().GetSection()[0].GetResult(),15);
    ASSERT_EQ(resp.GetJobsDetail().GetSection()[0].GetText(),"15");
    ASSERT_EQ(resp.GetJobsDetail().GetSection()[0].GetStartByte(),15);
    ASSERT_EQ(resp.GetJobsDetail().GetSection()[0].GetSnapShotTime(),15);
 
    ASSERT_TRUE(resp.GetJobsDetail().GetSection()[0].HasLabel());
    ASSERT_TRUE(resp.GetJobsDetail().GetSection()[0].HasDuration());
    ASSERT_TRUE(resp.GetJobsDetail().GetSection()[0].HasOffsetTime());
    ASSERT_TRUE(resp.GetJobsDetail().GetSection()[0].HasUrl());
    ASSERT_TRUE(resp.GetJobsDetail().GetSection()[0].HasResult());
    ASSERT_TRUE(resp.GetJobsDetail().GetSection()[0].HasText());
    ASSERT_TRUE(resp.GetJobsDetail().GetSection()[0].HasStartByte());
    ASSERT_TRUE(resp.GetJobsDetail().GetSection()[0].HasSnapShotTime());

    ASSERT_TRUE(!resp.GetJobsDetail().GetSection()[0].HasAdsInfo());
    ASSERT_TRUE(!resp.GetJobsDetail().GetSection()[0].HasIllegalInfo());
    ASSERT_TRUE(!resp.GetJobsDetail().GetSection()[0].HasPoliticsInfo());
    ASSERT_TRUE(!resp.GetJobsDetail().GetSection()[0].HasTerrorismInfo());
    ASSERT_TRUE(!resp.GetJobsDetail().GetSection()[0].HasPornInfo());
    ASSERT_TRUE(!resp.GetJobsDetail().GetSection()[0].HasAbuseInfo());

    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetRecognitionResults()[0].GetLabel(),"15");
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetRecognitionResults()[0].GetScore(),15);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetRecognitionResults()[0].GetStartTime(),15);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetRecognitionResults()[0].GetEndTime(),15);


    ASSERT_TRUE(resp.GetJobsDetail().GetPornInfo().GetRecognitionResults()[0].HasLabel());
    ASSERT_TRUE(resp.GetJobsDetail().GetPornInfo().GetRecognitionResults()[0].HasScore());
    ASSERT_TRUE(resp.GetJobsDetail().GetPornInfo().GetRecognitionResults()[0].HasStartTime());
    ASSERT_TRUE(resp.GetJobsDetail().GetPornInfo().GetRecognitionResults()[0].HasEndTime());
	
	  ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetLibResults()[0].GetLibType(),15);
    ASSERT_EQ(resp.GetJobsDetail().GetPornInfo().GetLibResults()[0].GetLibName(),"15");
	  ASSERT_TRUE(resp.GetJobsDetail().GetPornInfo().GetLibResults()[0].HasLibType());
    ASSERT_TRUE(resp.GetJobsDetail().GetPornInfo().GetLibResults()[0].HasLibName());
    ASSERT_TRUE(resp.GetJobsDetail().GetPornInfo().GetLibResults()[0].HasKeyWords());

	  ASSERT_EQ(resp.GetJobsDetail().GetListInfo().GetListResult()[0].GetListType(),15);
	  ASSERT_EQ(resp.GetJobsDetail().GetListInfo().GetListResult()[0].GetListName(),"15");
	  ASSERT_EQ(resp.GetJobsDetail().GetListInfo().GetListResult()[0].GetEntity(),"15");

    resp.GetJobsDetail().GetSection()[0].to_string();
  }
}

}  // namespace qcloud_cos
