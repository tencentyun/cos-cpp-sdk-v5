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

struct CodeLocation {
  std::vector<std::string> points;  // 二维码坐标点
  std::string to_string() const {
    std::stringstream ss;
    for (auto& point : points) {
      ss << "point: " << point;
    }
    ss << std::endl;
    return ss.str();
  }
};

struct QRcodeInfo {
  std::string code_url;        // 二维码的内容。可能识别不出
  CodeLocation code_location;  // 图中识别到的二维码位置坐标

  std::string to_string() const {
    std::stringstream ss;
    ss << "code_url: " << code_url
       << ", code_location: " << code_location.to_string() << std::endl;
    return ss.str();
  }
};

struct Object {
  std::string key;       // 文件名
  std::string location;  // 图片路径
  std::string format;    // 图片格式
  int width;             // 图片宽度
  int height;            // 图片高度
  int size;              // 图片大小
  int quality;           // 图片质量
  std::string etag;      // 处理结果图 ETag 信息
  int code_status;  // 二维码识别结果。0表示未识别到二维码，1表示识别到二维码
  int watermark_status;  // 当 type
                         // 为2时返回该字段，表示提取到全盲水印的可信度。
                         //  具体为0-100的数字，75分以上表示确定有盲水印，60-75表示疑似有盲水印，60以下可认为未提取到盲水印
  std::vector<QRcodeInfo> qr_code_info;  // 二维码识别结果，可能有多个
  std::string to_string() const {
    std::stringstream ss;
    ss << "key: " << key << ", location: " << location << ", format: " << format
       << ", width: " << width << ", heihth: " << height << ", size: " << size
       << ", quality: " << quality << ", etag: " << etag
       << ", watermark_status: " << watermark_status;
    for (auto& qr_code : qr_code_info) {
      ss << "qr_code: " << qr_code.to_string();
    }
    ss << std::endl;
    return ss.str();
  }
};

struct ProcessResults {
  std::vector<Object> objects;  // 可能有多个对象
  std::string to_string() const {
    std::stringstream ss;
    for (auto& object : objects) {
      ss << "object: " << object.to_string();
    }
    ss << std::endl;
    return ss.str();
  }
};

struct ImageInfo {
  std::string format;  // 格式
  int width;           // 图片宽度
  int height;          // 图片高度
  int quality;         // 图片质量
  std::string ave;     // 图片主色调
  int orientation;     // 图片旋转角度
  std::string to_string() const {
    std::stringstream ss;
    ss << "format: " << format << ", width: " << width << ", heihth: " << height
       << ", quality: " << quality << ", ave: " << ave
       << ", orientation: " << orientation << std::endl;
    return ss.str();
  }
};

struct OriginalInfo {
  std::string key;       // 原图文件名
  std::string location;  // 图片路径
  ImageInfo image_info;  // 原图图片信息
  std::string etag;      // 原图 ETag 信息
  std::string to_string() const {
    std::stringstream ss;
    ss << "key: " << key << ", location: " << location
       << ", image_info: " << image_info.to_string() << ", etag: " << etag
       << std::endl;
    return ss.str();
  }
};

struct UploadResult {
  OriginalInfo original_info;     // 原图信息
  ProcessResults process_result;  // 图片处理结果
  std::string to_string() const {
    std::stringstream ss;
    ss << "original_info: " << original_info.to_string()
       << ", process_result: " << process_result.to_string() << std::endl;
    return ss.str();
  }
};

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

struct GetQRcodeResult {
  int code_status;  // 二维码识别结果。0表示未识别到二维码，1表示识别到二维码
  std::vector<QRcodeInfo> qr_code_info;  // 二维码识别结果，可能有多个
  std::string
      result_image;  // 处理后的图片 base64数据，请求参数 cover 为1时返回

  std::string to_string() const {
    std::stringstream ss;
    ss << "code_status: " << code_status;
    for (auto& qr_code : qr_code_info) {
      ss << "qr_code: " << qr_code.to_string();
    }
    ss << "result_image: " << result_image << std::endl;
    return ss.str();
  }
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

struct DocBucketList {
  std::string bucket_id;        //存储桶 ID
  std::string name;             // 存储桶名称，同 BucketId
  std::string region;           //所在的地域
  std::string create_time;      // 创建时间
  std::string alias_bucket_id;  // 存储桶别名

  std::string to_string() const {
    std::stringstream ss;
    ss << "bucket_id: " << bucket_id << "name: " << name << "region: " << region
       << "create_time: " << create_time
       << "alias_bucket_id: " << alias_bucket_id << std::endl;
    return ss.str();
  }
};

struct DocBucketResponse {
  std::string request_id;  // 请求的唯一 ID
  int total_count;         // 文档预览 Bucket 总数
  int page_number;         // 当前页数，同请求中的 pageNumber
  int page_size;           // 每页个数，同请求中的 pageSize
  std::vector<DocBucketList> doc_bucket_list;  // 文档预览 Bucket 列表
  std::string to_string() const {
    std::stringstream ss;
    ss << "request_id: " << request_id << "total_count: " << total_count
       << "page_number: " << page_number << "page_size: " << page_size;
    for (auto& bucket : doc_bucket_list) {
      ss << "bucket: " << bucket.to_string();
    }
    ss << std::endl;
    return ss.str();
  }
};

class DescribeDocProcessBucketsResp : public BaseResp {
 public:
  DescribeDocProcessBucketsResp() {}
  virtual ~DescribeDocProcessBucketsResp() {}

  virtual bool ParseFromXmlString(const std::string& body);

  DocBucketResponse GetResult() const { return m_result; }

 private:
  bool ParseDocBucketList(rapidxml::xml_node<>* root,
                          DocBucketList& bucket_list);
  DocBucketResponse m_result;
};

class DocPreviewResp : public GetObjectByFileResp {
 public:
  DocPreviewResp() {}
  virtual ~DocPreviewResp() {}

  void ParseFromHeaders(const std::map<std::string, std::string>& headers);

  // 返回文档总页数（表格文件表示当前 sheet 转换后的总图片数），异常时为空
  int GetTotalPage() const { return m_x_total_page; }
  // 当异常时返回错误码
  std::string GetErrNo() const { return m_x_errno; }
  // 返回文档中总表数
  int GetTotalSheet() const { return m_x_total_sheet; }
  // 返回当前 sheet 名
  std::string GetSheetName() const { return m_sheet_name; }

 private:
  int m_x_total_page;
  std::string m_x_errno;
  int m_x_total_sheet;
  std::string m_sheet_name;
};

struct JobsDetail {
  JobsDetail()
      : code(""),
        message(""),
        job_id(""),
        tag(""),
        state(""),
        create_time(""),
        end_time(""),
        queue_id("") {}
  std::string code;     // 错误码，只有 State 为 Failed 时有意义
  std::string message;  // 错误描述，只有 State 为 Failed 时有意义
  std::string job_id;   // 新创建任务的 ID
  std::string tag;      // 新创建任务的 Tag：DocProcess
  std::string
      state;  // 任务的状态，为
              // Submitted、Running、Success、Failed、Pause、Cancel 其中一个
  std::string create_time;  // 任务的创建时间
  std::string end_time;     //
  std::string queue_id;     // 任务所属的队列 ID
  Input input;              // 该任务的输入文件路径
  Operation operation;      // 该任务的规则
};

class DocProcessJobBase : public BaseResp {
 public:
  DocProcessJobBase() { memset(&m_jobs_detail, 0, sizeof(JobsDetail)); }
  virtual ~DocProcessJobBase() {}
  virtual bool ParseFromXmlString(const std::string& body);
  virtual bool ParseOperation(rapidxml::xml_node<>* root, Operation& operation);
  bool ParseDocProcess(rapidxml::xml_node<>* root, DocProcess& doc_process);
  bool ParseDocProcessResult(rapidxml::xml_node<>* root,
                             DocProcessResult& doc_process_result);
  JobsDetail GetJobsDetail() const { return m_jobs_detail; }

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

}  // namespace qcloud_cos
