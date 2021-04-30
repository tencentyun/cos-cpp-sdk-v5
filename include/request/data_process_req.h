#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "Poco/Dynamic/Struct.h"
#include "Poco/JSON/Object.h"
#include "request/bucket_req.h"
#include "request/object_req.h"

namespace qcloud_cos {

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

/*文档预览异步任务接口*/
struct Input {
  Input() : object("") {}

  std::string object;  // 文件在 COS 上的文件路径，Bucket 由 Host 指定
};

struct Output {
  Output() : region(""), bucket(""), object("") {}
  std::string region;  // 存储桶的地域
  std::string bucket;  // 存储结果的存储桶
  std::string object;  // 输出文件路径
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
  std::string image_params;  // 转换后的图片处理参数
  std::string doc_passwd;    //  Office 文档的打开密码
  int comments;              // 是否隐藏批注和应用修订，默认为0
  int paper_direction;  // 表格文件转换纸张方向，0代表垂直方向，非0代表水平方向，默认为0
  int quality;  // 生成预览图的图片质量，取值范围 [1-100]，默认值100。
                // 例：值为100，代表生成图片质量为100%
  int zoom;  // 预览图片的缩放参数
};

struct PageInfo {
  PageInfo()
      : page_no(0), tgt_uri(""), x_sheet_pics(0), pic_index(0), pic_num(0) {}
  int page_no;  // 预览产物页码,源文件为 Excel 格式时表示 SheetId
  std::string tgt_uri;  // 预览产物生成的 cos 桶路径
  int x_sheet_pics;  // 当前 Sheet 生成的图片总数（源文件为 Excel 特有参数）
  int pic_index;  // 当前预览产物在整个源文件中的序号（源文件为 Excel 特有参数）
  int pic_num;  // 当前预览产物在 Sheet 中的序号（源文件为 Excel 特有参数）
};

struct DocProcessResult {
  DocProcessResult()
      : page_info(),
        tgt_type(""),
        total_pageount(0),
        succ_pagecount(0),
        fail_pagecount(0),
        total_sheetcount(0) {}
  PageInfo page_info;    // 预览任务产物详情
  std::string tgt_type;  // 预览产物目标格式
  int total_pageount;    // 预览任务产物的总数
  int succ_pagecount;    // 预览任务产物的成功数
  int fail_pagecount;    // 预览任务产物的失败数
  int total_sheetcount;  // 预览任务的 Sheet 总数（源文件为 Excel 特有参数）
};

struct Operation {
  Output output;  // 当 Tag 为 DocProcess 时有效，指定该任务的参数
  DocProcess doc_process;  // 结果输出地址
  DocProcessResult
      doc_process_result;  // 在 job 的类型为 DocProcess 且 job 状态为 success
                           // 时，返回文档预览任务结果详情
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

}  // namespace qcloud_cos
