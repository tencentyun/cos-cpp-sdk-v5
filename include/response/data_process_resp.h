#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"
#include "request/data_process_req.h"
#include "response/object_resp.h"

namespace qcloud_cos {

class ImageRespBase : virtual public BaseResp {
 public:
  ImageRespBase() {}

  virtual ~ImageRespBase() {}

  virtual bool ParseFromXmlString(const std::string& body);

  UploadResult GetUploadResult() const { return m_upload_result; }

  static bool ParseObject(rapidxml::xml_node<>* root, Object& object);
  static bool ParseQRcodeInfo(rapidxml::xml_node<>* root, QRcodeInfo& qr_code);

 protected:
  bool ParseOriginalInfo(rapidxml::xml_node<>* root);
  UploadResult m_upload_result;
};

class PutImageByFileResp : public ImageRespBase, public PutObjectByFileResp {
 public:
  PutImageByFileResp() {}
  virtual ~PutImageByFileResp() {}
};

class CloudImageProcessResp : public ImageRespBase {
 public:
  CloudImageProcessResp() {}
  virtual ~CloudImageProcessResp() {}
};

class GetQRcodeResp : public BaseResp {
 public:
  GetQRcodeResp() {}
  virtual ~GetQRcodeResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  GetQRcodeResult GetResult() const { return m_result; }

 private:
  GetQRcodeResult m_result;
};

class DescribeDocProcessBucketsResp : public BaseResp {
 public:
  DescribeDocProcessBucketsResp() {}
  virtual ~DescribeDocProcessBucketsResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  DocBucketResponse GetResult() const { return m_result; }

  static bool ParseBucketInfo(rapidxml::xml_node<>* root,
                              BucketInfo& bucket_info);

 private:
  DocBucketResponse m_result;
};

class DocPreviewResp : public GetObjectByFileResp {
 public:
  DocPreviewResp() {}
  virtual ~DocPreviewResp() {}

  void ParseFromHeaders(const std::map<std::string, std::string>& headers);

  // 返回文档总页数（表格文件表示当前 sheet 转换后的总图片数），异常时为空
  unsigned GetTotalPage() const { return m_x_total_page; }
  // 当异常时返回错误码
  std::string GetErrNo() const { return m_x_errno; }
  // 返回文档中总表数
  unsigned GetTotalSheet() const { return m_x_total_sheet; }
  // 返回当前 sheet 名
  std::string GetSheetName() const { return m_sheet_name; }

 private:
  unsigned m_x_total_page;
  std::string m_x_errno;
  unsigned m_x_total_sheet;
  std::string m_sheet_name;
};

class DocProcessJobBase : public BaseResp {
 public:
  DocProcessJobBase() {}
  virtual ~DocProcessJobBase() {}
  virtual bool ParseFromXmlString(const std::string& body);
  JobsDetail GetJobsDetail() const { return m_jobs_detail; }

 protected:
  bool ParseJobsDetail(rapidxml::xml_node<>* root, JobsDetail& jobs_detail);
  bool ParseOperation(rapidxml::xml_node<>* root, Operation& operation);
  bool ParseDocProcess(rapidxml::xml_node<>* root, DocProcess& doc_process);
  bool ParseDocProcessResult(rapidxml::xml_node<>* root,
                             DocProcessResult& doc_process_result);

 private:
  JobsDetail m_jobs_detail;
};

class CreateDocProcessJobsResp : public DocProcessJobBase {
 public:
  CreateDocProcessJobsResp() {}
  virtual ~CreateDocProcessJobsResp() {}
};

class DescribeDocProcessJobResp : public DocProcessJobBase {
 public:
  DescribeDocProcessJobResp() {}
  virtual ~DescribeDocProcessJobResp() {}
};

class DescribeDocProcessJobsResp : public DocProcessJobBase {
 public:
  DescribeDocProcessJobsResp() {}
  virtual ~DescribeDocProcessJobsResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  std::vector<JobsDetail> GetJobsDetails() const { return m_jobs_details; }

  std::string GetNextToken() const { return m_next_token; }

 private:
  std::vector<JobsDetail> m_jobs_details;
  std::string m_next_token;
};

class QueuesBase : public BaseResp {
 public:
  QueuesBase() {}
  virtual ~QueuesBase() {}

 protected:
  bool ParseNonExistPIDs(rapidxml::xml_node<>* root,
                         NonExistPIDs& non_exist_pids);
  bool ParseQueueList(rapidxml::xml_node<>* root, QueueList& queue_list);
};

class DescribeQueuesResp : public QueuesBase {
 public:
  DescribeQueuesResp() {}
  virtual ~DescribeQueuesResp() {}
  virtual bool ParseFromXmlString(const std::string& body);

  int GetTotalCount() const { return m_total_count; }
  std::string GetRequestId() const { return m_request_id; }
  int GetPageNumber() const { return m_page_number; }
  int GetPageSize() const { return m_page_size; }
  QueueList GetQueueList() const { return m_queue_list; }
  NonExistPIDs GetNonExistPIDs() const { return m_non_exist_pids; }

 private:
  int m_total_count;
  std::string m_request_id;
  int m_page_number;
  int m_page_size;
  QueueList m_queue_list;
  NonExistPIDs m_non_exist_pids;
};

class UpdateQueueResp : public QueuesBase {
 public:
  UpdateQueueResp() {}
  virtual ~UpdateQueueResp() {}
  virtual bool ParseFromXmlString(const std::string& body);

  std::string GetRequestId() const { return m_request_id; }
  QueueList GetQueueList() const { return m_queue; }

 private:
  std::string m_request_id;
  QueueList m_queue;
};

class DescribeDocProcessQueuesResp : public DescribeQueuesResp {
 public:
  DescribeDocProcessQueuesResp() {}
  virtual ~DescribeDocProcessQueuesResp() {}
//   virtual bool ParseFromXmlString(const std::string& body);

//   int GetTotalCount() const { return m_total_count; }
//   std::string GetRequestId() const { return m_request_id; }
//   int GetPageNumber() const { return m_page_number; }
//   int GetPageSize() const { return m_page_size; }
//   QueueList GetQueueList() const { return m_queue_list; }
//   NonExistPIDs GetNonExistPIDs() const { return m_non_exist_pids; }

//  private:
//   int m_total_count;
//   std::string m_request_id;
//   int m_page_number;
//   int m_page_size;
//   QueueList m_queue_list;
//   NonExistPIDs m_non_exist_pids;
};

class UpdateDocProcessQueueResp : public UpdateQueueResp {
 public:
  UpdateDocProcessQueueResp() {}
  virtual ~UpdateDocProcessQueueResp() {}
//   virtual bool ParseFromXmlString(const std::string& body);

//   std::string GetRequestId() const { return m_request_id; }
//   QueueList GetQueueList() const { return m_queue; }

//  private:
//   std::string m_request_id;
//   QueueList m_queue;
};

class DescribeMediaBucketsResp : public BaseResp {
 public:
  DescribeMediaBucketsResp() {}
  virtual ~DescribeMediaBucketsResp() {}
  virtual bool ParseFromXmlString(const std::string& body);

  DescribeMediaBucketsResult GetResult() const { return m_result; }

 private:
  DescribeMediaBucketsResult m_result;
};

class PutBucketToCIResp : public BaseResp {
 public:
  PutBucketToCIResp() {}
  virtual ~PutBucketToCIResp() {}
};

class CreateDocBucketResp : public BaseResp {
 public:
  CreateDocBucketResp() {}
  virtual ~CreateDocBucketResp() {}
  virtual bool ParseFromXmlString(const std::string& body);

  CreateDocBucketResult GetResult() const { return m_result; }

 private:
  CreateDocBucketResult m_result;
};

class CreateMediaBucketResp : public BaseResp {
 public:
  CreateMediaBucketResp() {}
  virtual ~CreateMediaBucketResp() {}
  virtual bool ParseFromXmlString(const std::string& body);

  CreateMediaBucketResult GetResult() const { return m_result; }

 private:
  CreateMediaBucketResult m_result;
};

class GetSnapshotResp : public GetObjectByFileResp {
 public:
  GetSnapshotResp() {}
  virtual ~GetSnapshotResp() {}
};

class GetMediaInfoResp : public BaseResp {
 public:
  GetMediaInfoResp() {}
  virtual ~GetMediaInfoResp() {}
  virtual bool ParseFromXmlString(const std::string& body);

  GetMediaInfoResult GetResult() const { return m_result; }

 private:
  bool ParseVideo(rapidxml::xml_node<>* root, VideoInfo& video_info);
  bool ParseAudio(rapidxml::xml_node<>* root, AudioInfo& audio_info);
  bool ParseSubtitle(rapidxml::xml_node<>* root, SubtitleInfo& subtitle_info);
  bool ParseFormat(rapidxml::xml_node<>* root, FormatInfo& format_info);

  GetMediaInfoResult m_result;
};

class GetPm3u8Resp : public GetObjectByFileResp {
 public:
  GetPm3u8Resp() {}
  virtual ~GetPm3u8Resp() {}
};

class CreateFileBucketResp : public BaseResp {
 public:
  CreateFileBucketResp() {}
  virtual ~CreateFileBucketResp() {}
  virtual bool ParseFromXmlString(const std::string& body);

  CreateFileBucketResult GetResult() const { return m_result; }

 private:
  CreateFileBucketResult m_result;
};

class DescribeFileBucketsResp : public BaseResp {
 public:
  DescribeFileBucketsResp() {}
  virtual ~DescribeFileBucketsResp() {}
  virtual bool ParseFromXmlString(const std::string& body);
  DescribeFileBucketsResult GetResult() const { return m_result; }

 private:
  DescribeFileBucketsResult m_result;
};



class DataProcessJobBase : public BaseResp {
 public:
  DataProcessJobBase() {}
  virtual ~DataProcessJobBase() {}
  virtual bool ParseFromXmlString(const std::string& body);
  JobsDetails GetJobsDetail() const { return m_jobs_detail; }
 protected:
  static bool ParseTranscode(rapidxml::xml_node<>* root, Transcode& transcode);
  static bool ParseAudioMix(rapidxml::xml_node<>* root, AudioMix& audio_mix);
  static bool ParseVideo(rapidxml::xml_node<>* root, Video& video);
  static bool ParseTimeInterval(rapidxml::xml_node<>* root, TimeInterval& time_interval);
  static bool ParseContainer(rapidxml::xml_node<>* root, Container& container);
  static bool ParseAudio(rapidxml::xml_node<>* root, Audio& audio);
  static bool ParseTransConfig(rapidxml::xml_node<>* root, TransConfig& trans_config);
  static bool ParseSnapshot(rapidxml::xml_node<>* root, Snapshot& snapshot);
  static bool ParseWatermark(rapidxml::xml_node<>* root, Watermark& watermark);
  static bool ParseMediaResult(rapidxml::xml_node<>* root, MediaResult& media_result);
  static bool ParseOutput(rapidxml::xml_node<>* root, Output& output);
  static bool ParseRemoveWatermark(rapidxml::xml_node<>* root, RemoveWatermark& remove_watermark);
  static bool ParseSubtitles(rapidxml::xml_node<>* root, Subtitles& subtitles);
  static bool ParseFileUncompressConfig(rapidxml::xml_node<>* root, FileUncompressConfig& file_uncompress_config);
  static bool ParseFileUncompressResult(rapidxml::xml_node<>* root, FileUncompressResult& file_uncompress_result);
  static bool ParseAnimation(rapidxml::xml_node<>* root, Animation& animation);
  static bool ParseInput(rapidxml::xml_node<>* root, Input& input);
  static bool ParseSmartCover(rapidxml::xml_node<>* root, SmartCover& smartcover);
  static bool ParseConcat(rapidxml::xml_node<>* root, Concat& concat);
  static bool ParseConcatFragment(rapidxml::xml_node<>* root, ConcatFragment& concat_fragment);
  static bool ParseDigitalWatermark(rapidxml::xml_node<>* root, DigitalWatermark& digital_watermark);
  static bool ParseExtractDigitalWatermark(rapidxml::xml_node<>* root, ExtractDigitalWatermark& extract_digital_watermark);
  static bool ParseVideoMontage(rapidxml::xml_node<>* root, VideoMontage& video_montage);
  static bool ParseVoiceSeparate(rapidxml::xml_node<>* root, VoiceSeparate& voice_separate);
  static bool ParseSegment(rapidxml::xml_node<>* root, Segment& segment);


 private:
  JobsDetails m_jobs_detail;
};

class CreateDataProcessJobsResp : public DataProcessJobBase {
 public:
  CreateDataProcessJobsResp() {}
  virtual ~CreateDataProcessJobsResp() {}
  // virtual bool ParseFromXmlString(const std::string& body);
};

class DescribeDataProcessJobResp : public DataProcessJobBase {
 public:
  DescribeDataProcessJobResp() {}
  virtual ~DescribeDataProcessJobResp() {}
};

class CancelDataProcessJobResp : public BaseResp {
  public:
  CancelDataProcessJobResp() {}
  virtual ~CancelDataProcessJobResp() {}
};

}  // namespace qcloud_cos
