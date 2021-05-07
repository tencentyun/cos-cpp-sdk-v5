#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "Poco/Dynamic/Struct.h"
#include "Poco/JSON/Object.h"
#include "request/bucket_req.h"
#include "request/object_req.h"

namespace qcloud_cos {

struct Input {
  Input() : object("") {}

  std::string object;  // 文件在 COS 上的文件路径，Bucket 由 Host 指定

  std::string to_string() const {
    std::stringstream ss;
    ss << "object: " << object;
    return ss.str();
  }
};

struct Output {
  Output() : region(""), bucket(""), object("") {}
  std::string region;  // 存储桶的地域
  std::string bucket;  // 存储结果的存储桶
  std::string object;  // 输出文件路径

  std::string to_string() const {
    std::stringstream ss;
    ss << "region: " << region << ", bucket: " << bucket
       << ", object: " << object;
    return ss.str();
  }
};

struct DocProcess {
  DocProcess()
      : src_type(""),
        tgt_type(""),
        sheet_id(1),
        start_page(1),
        end_page(-1),
        image_params(""),
        doc_passwd(""),
        comments(0),
        paper_direction(0),
        quality(100),
        zoom(100) {}
  std::string src_type;      // 源数据的后缀类型
  std::string tgt_type;      // 转换输出目标文件类型
  int sheet_id;              // 表格文件参数
  int start_page;            // 从第 X 页开始转换
  int end_page;              // 转换至第 X 页
  int page_size;             //
  std::string image_params;  // 转换后的图片处理参数
  std::string doc_passwd;    //  Office 文档的打开密码
  int comments;              // 是否隐藏批注和应用修订，默认为0
  int paper_direction;  // 表格文件转换纸张方向，0代表垂直方向，非0代表水平方向，默认为0
  int quality;  // 生成预览图的图片质量，取值范围 [1-100]，默认值100。
                // 例：值为100，代表生成图片质量为100%
  int zoom;  // 预览图片的缩放参数

  std::string to_string() const {
    std::stringstream ss;
    ss << "src_type: " << src_type << ", tgt_type: " << tgt_type
       << ", sheet_id: " << sheet_id << ", start_page: " << start_page
       << ", end_page: " << end_page << ", page_size: " << page_size
       << ", image_params: " << image_params << ", doc_passwd: " << doc_passwd
       << ", comments: " << comments << ", paper_direction: " << paper_direction
       << ", quality: " << quality << ", zoom: " << zoom;
    return ss.str();
  }
};

struct PageInfo {
  PageInfo()
      : page_no(0), tgt_uri(""), x_sheet_pics(0), pic_index(0), pic_num(0) {}
  int page_no;  // 预览产物页码,源文件为 Excel 格式时表示 SheetId
  std::string tgt_uri;  // 预览产物生成的 cos 桶路径
  int x_sheet_pics;  // 当前 Sheet 生成的图片总数（源文件为 Excel 特有参数）
  int pic_index;  // 当前预览产物在整个源文件中的序号（源文件为 Excel 特有参数）
  int pic_num;  // 当前预览产物在 Sheet 中的序号（源文件为 Excel 特有参数）

  std::string to_string() const {
    std::stringstream ss;
    ss << "page_no: " << page_no << ", tgt_uri: " << tgt_uri
       << ", x_sheet_pics: " << x_sheet_pics << ", pic_index: " << pic_index
       << ", pic_num: " << pic_num;
    return ss.str();
  }
};

struct DocProcessResult {
  DocProcessResult()
      : tgt_type(""),
        total_pageount(0),
        succ_pagecount(0),
        fail_pagecount(0),
        total_sheetcount(0) {}
  std::vector<PageInfo> page_infos;  // 预览任务产物详情
  std::string tgt_type;              // 预览产物目标格式
  std::string task_id;               //
  int total_pageount;                // 预览任务产物的总数
  int succ_pagecount;                // 预览任务产物的成功数
  int fail_pagecount;                // 预览任务产物的失败数
  int total_sheetcount;  // 预览任务的 Sheet 总数（源文件为 Excel 特有参数）

  std::string to_string() const {
    std::stringstream ss;
    for (auto& page_info : page_infos) {
      ss << ", page_info: " << page_info.to_string();
    }
    ss << ", tgt_type: " << tgt_type << ", task_id: " << task_id
       << ", total_pageount: " << total_pageount
       << ", succ_pagecount: " << succ_pagecount
       << ", fail_pagecount: " << fail_pagecount
       << ", total_sheetcount: " << total_sheetcount;
    return ss.str();
  }
};

struct Operation {
  Output output;  // 当 Tag 为 DocProcess 时有效，指定该任务的参数
  DocProcess doc_process;  // 结果输出地址
  DocProcessResult
      doc_process_result;  // 在 job 的类型为 DocProcess 且 job 状态为 success
                           // 时，返回文档预览任务结果详情

  std::string to_string() const {
    std::stringstream ss;
    ss << "output: " << output.to_string()
       << ", doc_process: " << doc_process.to_string()
       << ", doc_process_result: " << doc_process_result.to_string();
    return ss.str();
  }
};

struct CodeLocation {
  std::vector<std::string> points;  // 二维码坐标点
  std::string to_string() const {
    std::stringstream ss;
    for (auto& point : points) {
      ss << ", point: " << point;
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

struct PicRules {
  std::string bucket;  // 存储结果的目标存储桶名称，格式为
                       // BucketName-APPID，如果不指定的话默认保存到当前存储桶
  std::string
      fileid;  // 处理结果的文件路径名称，如以/开头，则存入指定文件夹中，否则，存入原图文件存储的同目录
  std::string
      rule;  // 处理参数，参见对象存储图片处理
             // API。若按指定样式处理，则以style/开头，后加样式名，如样式名为test，则
             // rule 字段为style/test
};

class PicOperation {
 public:
  PicOperation() : is_pic_info(true) {}
  virtual ~PicOperation() {}

  void AddRule(const PicRules& rule) { rules.push_back(rule); }

  void TurnOffPicInfo() { is_pic_info = false; }

  std::string GenerateJsonString() const {
    Poco::JSON::Object::Ptr json_root = new Poco::JSON::Object();
    if (is_pic_info) {
      json_root->set("is_pic_info", 1);
    } else {
      json_root->set("is_pic_info", 0);
    }

    Poco::JSON::Array rules_array;
    for (auto& it : rules) {
      Poco::JSON::Object::Ptr rule = new Poco::JSON::Object();
      if (!it.bucket.empty()) {
        rule->set("bucket", it.bucket);
      }
      rule->set("fileid", it.fileid);
      rule->set("rule", it.rule);
      rules_array.add(rule);
    }

    json_root->set("rules", rules_array);
    std::ostringstream oss;
    Poco::JSON::Stringifier::stringify(json_root, oss);
    return oss.str();
  }

 private:
  bool
      is_pic_info;  // 是否返回原图信息，0不返回原图信息，1返回原图信息，默认为0
  std::vector<PicRules>
      rules;  //处理规则，一条规则对应一个处理结果（目前支持五条规则），不填则不进行图片处理
};

struct DocBucketList {
  std::string bucket_id;        //存储桶 ID
  std::string name;             // 存储桶名称，同 BucketId
  std::string region;           //所在的地域
  std::string create_time;      // 创建时间
  std::string alias_bucket_id;  // 存储桶别名

  std::string to_string() const {
    std::stringstream ss;
    ss << "bucket_id: " << bucket_id << ", name: " << name
       << ", region: " << region << ", create_time: " << create_time
       << ", alias_bucket_id: " << alias_bucket_id;
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
    ss << "request_id: " << request_id << ", total_count: " << total_count
       << ", page_number: " << page_number << ", page_size: " << page_size;
    for (auto& bucket : doc_bucket_list) {
      ss << ", bucket: " << bucket.to_string();
    }
    ss << std::endl;
    return ss.str();
  }
};

struct JobsDetail {
  JobsDetail() {}
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
  std::string to_string() const {
    std::stringstream ss;
    ss << "code: " << code << ", message: " << message << ", job_id: " << job_id
       << ", tag: " << tag << ", state: " << state
       << ", create_time: " << create_time << ", end_time: " << end_time
       << ", queue_id: " << queue_id << ". input: " << input.to_string()
       << ", operation: " << operation.to_string();
    return ss.str();
  }
};

struct NotifyConfig {
  std::string url;    // 回调地址
  std::string state;  // 开关状态，On 或者 Off
  std::string type;   // 回调类型，Url
  std::string event;  // 触发回调的事件
  std::string to_string() const {
    std::stringstream ss;
    ss << "url: " << url << ", state: " << state << ", type: " << type
       << ", event: " << event;
    return ss.str();
  }
};

struct NonExistPIDs {
  std::vector<std::string> queue_id;  // NonExistPIDs
  std::string to_string() const {
    std::stringstream ss;
    for (auto& q : queue_id) {
      ss << ", queue_id: " << q;
    }
    return ss.str();
  }
};

struct QueueList {
  std::string queue_id;        //队列 ID
  std::string name;            // 队列名字
  std::string state;           // 当前状态，Active 或者 Paused
  NotifyConfig notify_config;  // 回调配置
  int max_size;                // 队列最大长度
  int max_concurrent;          // 当前队列最大并行执行的任务数
  std::string update_time;     // 更新时间
  std::string create_time;     // 创建时间
  std::string bucket_id;       //
  std::string category;        //
  std::string to_string() const {
    std::stringstream ss;
    ss << "queue_id: " << queue_id << ", name: " << name << ", state: " << state
       << ", notify_config: " << notify_config.to_string()
       << ", max_size: " << max_size << ", max_concurrent: " << max_concurrent
       << ", update_time: " << update_time << ", create_time: " << create_time
       << ". bucket_id: " << bucket_id << ", category: " << category;
    return ss.str();
  }
};

class PutImageByFileReq : public PutObjectByFileReq {
 public:
  PutImageByFileReq(const std::string& bucket_name,
                    const std::string& object_name,
                    const std::string& local_image)
      : PutObjectByFileReq(bucket_name, object_name, local_image) {}

  virtual ~PutImageByFileReq() {}

  void SetPicOperation(const PicOperation& pic_operation) {
    m_pic_operation = pic_operation;
    AddHeader("Pic-Operations", m_pic_operation.GenerateJsonString());
  }

 private:
  PicOperation m_pic_operation;
};

class CloudImageProcessReq : public ObjectReq {
 public:
  CloudImageProcessReq(const std::string& bucket_name,
                       const std::string& object_name)
      : ObjectReq(bucket_name, object_name) {
    m_method = "POST";
  }
  virtual ~CloudImageProcessReq() {}

  void SetPicOperation(const PicOperation& pic_operation) {
    m_pic_operation = pic_operation;
    AddHeader("Pic-Operations", m_pic_operation.GenerateJsonString());
    AddParam("image_process", "");
  }

 private:
  PicOperation m_pic_operation;
};

class GetQRcodeReq : public ObjectReq {
 public:
  GetQRcodeReq(const std::string& bucket_name, const std::string& object_name)
      : ObjectReq(bucket_name, object_name) {
    AddParam("ci-process", "QRcode");
    AddParam("cover", "0");

    m_method = "GET";
  }

  virtual ~GetQRcodeReq() {}

  // 二维码覆盖功能，将对识别出的二维码覆盖上马赛克。取值为0或1。0表示不开启二维码覆盖，1表示开启二维码覆盖，默认值0
  void EnableCover() { AddParam("cover", "1"); }
};

class DescribeDocProcessBucketsReq : public BaseReq {
 public:
  DescribeDocProcessBucketsReq() {
    m_method = "GET";

    m_path = "/docbucket";

    // 该接口只支持https
    SetHttps();
  }
  virtual ~DescribeDocProcessBucketsReq() {}

  void SetRegions(const std::string& regions) { AddParam("regions", regions); }

  void SetBucketNames(const std::string& bucket_names) {
    AddParam("bucketNames", bucket_names);
  }

  void SetBucketName(const std::string& bucket_name) {
    AddParam("bucketName", bucket_name);
  }

  void SetPageNumber(const std::string& page_number) {
    AddParam("pageNumber", page_number);
  }

  void SetPageSize(const std::string& page_size) {
    AddParam("PageSize", page_size);
  }
};

class DocPreviewReq : public GetObjectByFileReq {
 public:
  DocPreviewReq(const std::string& bucket_name, const std::string& object_name,
                const std::string& local_file)
      : GetObjectByFileReq(bucket_name, object_name, local_file) {
    AddParam("ci-process", "doc-preview");

    AddParam("page", "1");

    AddParam("sheet", "1");

    AddParam("dstType", "jpg");

    AddParam("comment", "0");

    AddParam("excelPaperDirection", "0");

    AddParam("quality", "100");

    AddParam("scale", "100");
  }

  virtual ~DocPreviewReq() {}

  // 源数据的后缀类型，当前文档转换根据 COS 对象的后缀名来确定源数据类型。当 COS
  // 对象没有后缀名时，可以设置该值
  void SetSrcType(const std::string& src_type) {
    AddParam("srcType", src_type);
  }

  // 需转换的文档页码，默认从1开始计数；表格文件中 page 表示转换的第 X 个 sheet
  // 的第 X 张图
  void SetPage(const int page) { AddParam("page", std::to_string(page)); }
  // 转换后的图片处理参数，支持 基础图片处理 所有处理参数，多个处理参数可通过
  // 管道操作符 分隔，从而实现在一次访问中按顺序对图片进行不同处理
  void SetImageParams(const std::string& image_param) {
    AddParam("ImageParams", image_param);
  }

  // 表格文件参数，转换第 X 个表，默认为1
  void SetSheet(const int sheet) {
    AddParam("ImageParams", std::to_string(sheet));
  }

  // 转换输出目标文件类型
  void SetDstType(const std::string& dst_type) {
    AddParam("dstType", dst_type);
  }

  // Office 文档的打开密码，如果需要转换有密码的文档，请设置该字段
  void SetPasswd(const std::string& passwd) { AddParam("password", passwd); }
  // 是否隐藏批注和应用修订，默认为
  void SetComment(const int comment) {
    AddParam("comment", std::to_string(comment));
  }

  // 表格文件转换纸张方向，0代表垂直方向，非0代表水平方向，默认为0
  void SetExcelPaperDirection(const int direction) {
    AddParam("excelPaperDirection", std::to_string(direction));
  }

  // 生成预览图的图片质量，取值范围 [1-100]，默认值100。
  // 例：值为100，代表生成图片质量为100%
  void SetQuality(const int quality) {
    AddParam("quality", std::to_string(quality));
  }

  // 预览图片的缩放参数，取值范围[10-200]， 默认值100。
  // 例：值为200，代表图片缩放比例为200% 即放大两倍
  void SetScale(const int scale) { AddParam("scale", std::to_string(scale)); }
};

class CreateDocProcessJobsReq : public BucketReq {
 public:
  CreateDocProcessJobsReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("POST");
    SetPath("/doc_jobs");
    SetHttps();
    AddHeader("Content-Type", "application/xml");
    m_tag = "DocProcess";
  }

  virtual ~CreateDocProcessJobsReq() {}

  void SetTag(const std::string& tag) { m_tag = tag; }
  void SetOperation(const Operation& operation) { m_operation = operation; }
  void SetInput(const Input& input) { m_input = input; }
  void SetQueueId(const std::string& queue_id) { m_queue_id = queue_id; }
  bool GenerateRequestBody(std::string* body) const;

 private:
  std::string m_tag;       // 创建任务的 Tag，目前仅支持：DocProcess
  Input m_input;           // 待操作的文件对象
  Operation m_operation;   // 操作规则
  std::string m_queue_id;  // 任务所在的队列 ID
};

class DescribeDocProcessJobReq : public BucketReq {
 public:
  DescribeDocProcessJobReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/doc_jobs");
    SetHttps();
  }
  virtual ~DescribeDocProcessJobReq() {}
  void SetJobId(const std::string& job_id) { m_path += "/" + job_id; }
};

class DescribeDocProcessJobsReq : public BucketReq {
 public:
  DescribeDocProcessJobsReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/doc_jobs");
    SetHttps();
    AddParam("tag", "DocProcess");

    AddParam("orderByTime", "Desc");
    AddParam("size", "10");
    AddParam("states", "All");
  }
  virtual ~DescribeDocProcessJobsReq() {}
  void SetQueueId(const std::string& queue_id) {
    AddParam("queueId", queue_id);
  }
  void SetOrderByTime(const std::string& order) {
    AddParam("orderByTime", order);
  }

  void SetNextToken(const std::string& next_token) {
    AddParam("nextToken", next_token);
  }

  void SetSize(const int size) {
    AddParam("size", StringUtil::IntToString(size));
  }
  void SetStates(const std::string& states) { AddParam("states", states); }
  void SetStartCreationTime(const std::string& start_time) {
    AddParam("startCreationTime", start_time);
  }

  void SetEndCreationTime(const std::string& end_time) {
    AddParam("endCreationTime", end_time);
  }
};

class DescribeDocProcessQueuesReq : public BucketReq {
 public:
  DescribeDocProcessQueuesReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/docqueue");
    SetHttps();
  }
  virtual ~DescribeDocProcessQueuesReq() {}
  void SetQueueIds(const std::string& queue_ids) {
    AddParam("queueIds", queue_ids);
  }
  void SetState(const std::string& state) { AddParam("state", state); }
  void SetPageNumber(const std::string& page_number) {
    AddParam("pageNumber", page_number);
  }

  void SetPageSize(const std::string& page_size) {
    AddParam("pageSize", page_size);
  }
};

class UpdateDocProcessQueueReq : public BucketReq {
 public:
  UpdateDocProcessQueueReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/docqueue");
    AddHeader("Content-Type", "application/xml");
    SetHttps();
  }
  virtual ~UpdateDocProcessQueueReq() {}
  void SetQueueId(const std::string& queue_id) {
    m_queue_id = queue_id;
    m_path.append("/" + queue_id);
  }

  void SetName(const std::string& name) { m_name = name; }
  void SetState(const std::string& state) { m_state = state; }
  void SetNotifyConfig(const NotifyConfig& notify_config) {
    m_notify_config = notify_config;
  }
  bool GenerateRequestBody(std::string* body) const;

 private:
  std::string m_name;            // 队列名称
  std::string m_queue_id;        // 队列 ID
  std::string m_state;           // 队列状态
  NotifyConfig m_notify_config;  // 通知渠道
};

}  // namespace qcloud_cos
