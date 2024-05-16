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
  Input() : object(""), region(""), bucket("") {}

  std::string object;  // 文件在 COS 上的文件路径，Bucket 由 Host 指定
  std::string region;
  std::string bucket;
  std::string url;
  std::string to_string() const {
    std::stringstream ss;
    ss << "object: " << object;
    ss << " bucket: " << bucket;
    ss << " region: " << region;
    ss << " url: " << url;
    return ss.str();
  }
};

struct Output {
  Output() : region(""), bucket(""), object("") {}
  std::string region;  // 存储桶的地域
  std::string bucket;  // 存储结果的存储桶
  std::string object;  // 输出文件路径
  std::string sprite_object; //雪碧图输出
  std::string au_object; // 人声结果文件
  std::string bass_object; // Bass声结果文件
  std::string drum_object; // 鼓声结果文件
  std::string to_string() const {
    std::stringstream ss;
    ss << "region: " << region << ", bucket: " << bucket
       << ", object: " << object << ", sprite_object: " << sprite_object
       << ", au_object: " << au_object << ", bass_object: " << bass_object
       << ", drum_object: " << drum_object << std::endl;
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

  std::vector<PicRules> GetRules() const { return rules; }

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

struct BucketInfo {
  std::string bucket_id;        // 存储桶 ID
  std::string name;             // 存储桶名称，同 BucketId
  std::string region;           // 所在的地域
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

struct CreateDocBucketResult {
  std::string request_id;                 // 请求的唯一ID
  BucketInfo doc_bucket;                // 媒体Bucket
  std::string to_string() const {
    std::stringstream ss;
    ss << "request_id: " << request_id << std::endl;
    ss << doc_bucket.to_string() << std::endl;
    ss << std::endl;
    return ss.str();
  }
};

struct DocBucketResponse {
  std::string request_id;  // 请求的唯一 ID
  int total_count;         // 文档预览 Bucket 总数
  int page_number;         // 当前页数，同请求中的 pageNumber
  int page_size;           // 每页个数，同请求中的 pageSize
  std::vector<BucketInfo> doc_bucket_list;  // 文档预览 Bucket 列表
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
  std::string state;    // 任务的状态，为
                        // Submitted、Running、Success、Failed、Pause、Cancel 其中一个
  std::string create_time;  // 任务的创建时间
  std::string end_time;     //
  std::string queue_id;     // 任务所属的队列 ID
  Input input;              // 该任务的输入文件路径
  Operation operation;      // 文档该任务的规则
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
  std::string queue_id;        // 队列 ID
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

struct DescribeMediaBucketsResult {
  std::string request_id;                     // 请求的唯一ID
  int total_count;                            // 媒体Bucket总数
  int page_number;                            // 当前页数
  int page_size;                              // 每页个数
  std::vector<BucketInfo> media_bucket_list;  // 媒体Bucket列表
  std::string to_string() const {
    std::stringstream ss;
    ss << "request_id: " << request_id << std::endl
       << "total_count: " << total_count << std::endl
       << "page_number: " << page_number << std::endl
       << "page_size: " << page_size << std::endl;
    for (auto& bucket : media_bucket_list) {
      ss << bucket.to_string() << std::endl;
    }
    ss << std::endl;
    return ss.str();
  }
};

struct CreateMediaBucketResult {
  std::string request_id;                 // 请求的唯一ID
  BucketInfo media_bucket;                // 媒体Bucket
  std::string to_string() const {
    std::stringstream ss;
    ss << "request_id: " << request_id << std::endl;
    ss << media_bucket.to_string() << std::endl;
    ss << std::endl;
    return ss.str();
  }
};

struct CreateFileBucketResult {
  std::string request_id;                 // 请求的唯一ID
  BucketInfo file_bucket;                // 文件Bucket
  std::string to_string() const {
    std::stringstream ss;
    ss << "request_id: " << request_id << std::endl;
    ss << file_bucket.to_string() << std::endl;
    ss << std::endl;
    return ss.str();
  }
};

struct DescribeFileBucketsResult {
  std::string request_id;                     // 请求的唯一ID
  int total_count;                            // 媒体Bucket总数
  int page_number;                            // 当前页数
  int page_size;                              // 每页个数
  std::vector<BucketInfo> file_bucket_list;  // 文件Bucket列表
  std::string to_string() const {
    std::stringstream ss;
    ss << "request_id: " << request_id << std::endl
       << "total_count: " << total_count << std::endl
       << "page_number: " << page_number << std::endl
       << "page_size: " << page_size << std::endl;
    for (auto& bucket : file_bucket_list) {
      ss << bucket.to_string() << std::endl;
    }
    ss << std::endl;
    return ss.str();
  }
};

struct VideoInfo {
  int index;                     // 该流的编号
  std::string codec_name;        // 编解码格式名字
  std::string codec_long_name;   // 编解码格式的详细名字
  std::string codec_time_base;   // 编码时基
  std::string codec_tag_string;  // 编码标签名
  std::string codec_tag;         // 编码标签
  std::string color_primaries;   // 色原
  std::string color_range;       // 色彩范围
  std::string color_transfer;    // 色彩通道
  std::string profile;           // 视频编码档位
  int height;                    // 视频高，单位 px
  int width;                     // 视频宽，单位 px
  int has_bframe;                // 是否有B帧。1表示有，0表示无
  int ref_frames;                // 视频编码的参考帧个数
  std::string sar;               // 采样宽高比
  std::string dar;               // 显示宽高比
  std::string pix_format;        // 像素格式
  std::string field_order;       // 场的顺序
  int level;                     // 视频编码等级
  int fps;                       // 视频帧率
  std::string avg_fps;           // 平均帧率
  std::string time_base;         // 时基
  float start_time;              // 视频开始时间，单位为秒
  float duration;                // 视频时长，单位为秒
  float bitrate;                 // 比特率，单位为 kbps
  int num_frames;                // 总帧数
  std::string language;          // 语言
  std::string to_string() const {
    std::stringstream ss;
    ss << "index: " << index << std::endl
       << "codec_name: " << codec_name << std::endl
       << "codec_long_name: " << codec_long_name << std::endl
       << "codec_time_base: " << codec_time_base << std::endl
       << "codec_tag_string: " << codec_tag_string << std::endl
       << "codec_tag: " << codec_tag << std::endl
       << "color_primaries: " << color_primaries << std::endl
       << "color_transfer: " << color_transfer << std::endl
       << "color_range: " << color_range << std::endl
       << "profile: " << profile << std::endl
       << "height: " << height << std::endl
       << "width: " << width << std::endl
       << "has_bframe: " << has_bframe << std::endl
       << "ref_frames: " << ref_frames << std::endl
       << "sar: " << sar << std::endl
       << "dar: " << dar << std::endl
       << "pix_format: " << pix_format << std::endl
       << "field_order: " << field_order << std::endl
       << "level: " << level << std::endl
       << "fps: " << fps << std::endl
       << "avg_fps: " << avg_fps << std::endl
       << "time_base: " << time_base << std::endl
       << "start_time: " << start_time << std::endl
       << "duration: " << duration << std::endl
       << "bitrate: " << bitrate << std::endl
       << "num_frames: " << num_frames << std::endl
       << "language: " << language << std::endl;
    return ss.str();
  }
};

struct AudioInfo {
  int index;                     // 该流的编号
  std::string codec_name;        // 编解码格式名字
  std::string codec_long_name;   // 编解码格式的详细名字
  std::string codec_time_base;   // 编码时基
  std::string codec_tag_string;  // 编码标签名
  std::string codec_tag;         // 编码标签
  std::string sample_fmt;        // 采样格式
  int sample_rate;               // 采样率
  int channel;                   // 通道数量
  std::string channel_layout;    // 通道格式
  std::string time_base;         // 时基
  float start_time;              // 音频开始时间，单位秒
  float duration;                // 音频时长，单位秒
  float bitrate;                 // 比特率，单位为 kbps
  std::string language;          // 语言
  std::string to_string() const {
    std::stringstream ss;
    ss << "index: " << index << std::endl
       << "codec_name: " << codec_name << std::endl
       << "codec_long_name: " << codec_long_name << std::endl
       << "codec_time_base: " << codec_time_base << std::endl
       << "codec_tag_string: " << codec_tag_string << std::endl
       << "codec_tag: " << codec_tag << std::endl
       << "sample_fmt: " << sample_fmt << std::endl
       << "sample_rate: " << sample_rate << std::endl
       << "channel: " << channel << std::endl
       << "channel_layout: " << channel_layout << std::endl
       << "time_base: " << time_base << std::endl
       << "start_time: " << start_time << std::endl
       << "duration: " << duration << std::endl
       << "bitrate: " << bitrate << std::endl
       << "language: " << language << std::endl;
    return ss.str();
  }
};

struct SubtitleInfo {
  int index;             // 该流的编号
  std::string language;  // 语言
  std::string to_string() const {
    std::stringstream ss;
    ss << "index: " << index << std::endl
       << "language: " << language << std::endl;
    return ss.str();
  }
};

struct StreamInfo {
  VideoInfo video;        // 视频信息
  AudioInfo audio;        // 音频信息
  SubtitleInfo subtitle;  // 字幕信息
  std::string to_string() const {
    std::stringstream ss;
    ss << "video: " << video.to_string() << std::endl
       << "audio: " << audio.to_string() << std::endl
       << "subtitle: " << subtitle.to_string() << std::endl;
    return ss.str();
  }
};
struct FormatInfo {
  int num_stream;   // Stream（包含 Video、Audio、Subtitle）的数量
  int num_program;  // 节目的数量
  std::string format_name;       // 容器格式名字
  std::string format_long_name;  // 容器格式的详细名字
  float start_time;              //起始时间，单位为秒
  float duration;                //时长，单位为秒
  float bitrate;                 //比特率，单位为 kbps
  int size;                      //大小，单位为 Byte
  std::string to_string() const {
    std::stringstream ss;
    ss << "num_stream: " << num_stream << std::endl
       << "num_program: " << num_program << std::endl
       << "format_name: " << format_name << std::endl
       << "format_long_name: " << format_long_name << std::endl
       << "start_time: " << start_time << std::endl
       << "duration: " << duration << std::endl
       << "bitrate: " << bitrate << std::endl
       << "size: " << size << std::endl;
    return ss.str();
  }
};
struct MediaInfo {
  StreamInfo stream;  // 流信息
  FormatInfo format;  // 格式信息

  std::string to_string() const {
    std::stringstream ss;
    ss << "stream: " << stream.to_string() << std::endl
       << "format: " << format.to_string() << std::endl;
    return ss.str();
  }
};

struct GetMediaInfoResult {
  MediaInfo media_info;  // 媒体信息
  std::string to_string() const {
    std::stringstream ss;
    ss << "media_info: " << media_info.to_string() << std::endl;
    return ss.str();
  }
};

struct Md5Info {
  Md5Info() : object_name(""), md5("") {}
  std::string object_name;
  std::string md5;
  std::string to_string() const { 
    std::stringstream ss;
    ss << "md5: " << md5 << std::endl
      << "object_name: " << object_name << std::endl;
    return ss.str();
  }    
};

struct OutputFile {
  OutputFile() : bucket(""), region(""), object_name(""), md5_info() {}
  std::string bucket;
  std::string region;
  std::string object_name;
  Md5Info md5_info;
  std::string to_string() const { 
    std::stringstream ss;
    ss << "bucket: " << bucket << std::endl
      << "region: " << region << std::endl
      << "object_name: " << object_name << std::endl
      << "md5_info: " << md5_info.to_string() << std::endl;
    return ss.str();
  }  
};

struct MediaResult {
  MediaResult() : output_file() {}
  OutputFile output_file;
  std::string to_string() const { 
    std::stringstream ss;
    ss << "output_file: " << output_file.to_string() << std::endl;
    return ss.str();
  }
};

struct CallBackkMqConfig {
  CallBackkMqConfig() : 
    mq_region(""),
    mq_mode(""),
    mq_name("") {}
  std::string mq_region;
  std::string mq_mode;
  std::string mq_name;
};

struct SpriteSnapShotConfig {
  SpriteSnapShotConfig() : 
    cell_width(""),
    cell_height(""),
    padding(""),
    margin(""),
    color(""),
    columns(""),
    lines(""),
    scale_method("") {}
  std::string cell_width;     // 单图宽度，非必选，值范围：[8，4096]，单位：px，默认原视频宽度
  std::string cell_height;    // 单图高度，非必选，值范围：[8，4096]，单位：px，默认原视频高度
  std::string padding;        // 雪碧图内边距大小，非必选，值范围：[0，1024]，单位：px，默认0
  std::string margin;         // 雪碧图外边距大小，非必选，值范围：[0，1024]，单位：px，默认0
  std::string color;          // 背景颜色，非必选，支持颜色详见https://www.ffmpeg.org/ffmpeg-utils.html#color-syntax
  std::string columns;        // 雪碧图列数，非必选，值范围：[1，10000]
  std::string lines;          // 雪碧图行数，值范围：[1，10000]
  std::string scale_method;   // 雪碧图缩放模式，非必选，取值范围为 DirectScale: 指定宽高缩放，MaxWHScaleAndPad: 指定最大宽高缩放填充，MaxWHScale: 指定最大宽高缩放，默认值为DirectScale，主动设置 CellWidth 和CellHeight 时生效
  std::string to_string() const {
    std::stringstream ss;
    ss << "cell_width: " << cell_width << std::endl
       << "cell_height: " << cell_height << std::endl
       << "padding: " << padding << std::endl
       << "margin: " << margin << std::endl
       << "color: " << color << std::endl
       << "columns: " << columns << std::endl
       << "lines: " << lines << std::endl
       << "scale_method: " << scale_method << std::endl;
    return ss.str();
  }  
};

struct Snapshot {
  Snapshot() :
    mode(""),
    start(""),
    time_interval(""), 
    count(""), 
    width(""), 
    height(""), 
    ci_param(""), 
    is_check_black(""), 
    is_check_count(""),
    black_level(""),
    pixel_black_threshold(""),
    snap_shot_out_mode(""),
    sprite_snapshot_config() {}
  std::string mode;                            // 截图模式，非必选，取值范围：Interval 间隔模式、Average 平均模式、 KeyFrame 关键帧模式，默认值 Interval
  std::string start;                           // 开始时间，非必选，单位秒，默认0，支持float格式，当 Mode 为 Interval、Average 时生效
  std::string time_interval;                   // 截图时间间隔，非必选，单位秒，取值范围(0, 3600]，支持float格式，当 Mode 为 Interval 时生效
  std::string count;                           // 截图数量，必选，取值范围（0，10000]
  std::string width;                           // 宽，取值范围[128,4096]，单位px，若只设置 Width 时，按照视频原始比例计算 Height，默认原视频宽
  std::string height;                          // 高，取值范围[128,4096]，单位px，若只设置 Height 时，按照视频原始比例计算 Width，默认原视频高
  std::string ci_param;                        // 图片处理参数，非必选，参考 https://cloud.tencent.com/document/product/460/36540
  std::string is_check_black;                  // 是否开启黑屏检测，非必选，取值 true、false，默认false
  std::string is_check_count;                  // 是否强制检查截图个数，非必选，取值 true、false，默认false，使用自定义间隔模式截图时，视频时长不够截取 Count 个截图，可以转为平均截图模式截取 Count 个截图
  std::string black_level;                     // 截图黑屏检测参数，非必选，当 IsCheckBlack=true 时有效，值参考范围[30，100]，表示黑色像素的占比值，值越小，黑色占比越小；Start>0，参数设置无效，不做过滤黑屏；Start=0 参数有效，截帧的开始时间为第一帧非黑屏开始
  std::string pixel_black_threshold;           // 截图黑屏检测参数，非必选，当 IsCheckBlack=true 时有效 判断像素点是否为黑色点的阈值，取值范围：[0，255]
  std::string snap_shot_out_mode;              // 截图输出模式参数，非必选，取值范围为 OnlySnapshot 仅输出截图模式，OnlySprite 仅输出雪碧图模式，SnapshotAndSprite 输出截图与雪碧图模式，默认值 OnlySnapshot
  SpriteSnapShotConfig sprite_snapshot_config; // 雪碧图输出配置，非必选
  std::string to_string() const {
    std::stringstream ss;
    ss << "mode: " << mode << std::endl
       << "start: " << start << std::endl
       << "time_interval: " << time_interval << std::endl
       << "count: " << count << std::endl
       << "width: " << width << std::endl
       << "height: " << height << std::endl
       << "ci_param: " << ci_param << std::endl
       << "is_check_black: " << is_check_black << std::endl
       << "black_level: " << black_level << std::endl
       << "pixel_black_threshold: " << pixel_black_threshold << std::endl
       << "snap_shot_out_mode: " << snap_shot_out_mode << std::endl
       << "sprite_snapshot_config: " << sprite_snapshot_config.to_string() << std::endl;
    return ss.str();
  }
};


struct FileUncompressConfig {
  FileUncompressConfig() :
    prefix(""),
    un_compress_key(""),
    prefix_replaced("") {}
  std::string prefix;
  std::string un_compress_key;
  std::string prefix_replaced;
  std::string to_string() const {
    std::stringstream ss;
    ss << "prefix: " << prefix << std::endl
       << "prefix_replaced: " << prefix_replaced << std::endl
       << "un_compress_key: " << un_compress_key << std::endl;
    return ss.str();
  }
};

struct FileUncompressResult {
  FileUncompressResult () :
    region(""),
    bucket(""),
    file_count("") {};
  std::string region;
  std::string bucket;
  std::string file_count;
  std::string to_string() const {
    std::stringstream ss;
    ss << "region: " << region << std::endl
       << "bucket: " << bucket << std::endl
       << "file_count: " << file_count << std::endl;
    return ss.str();
  }  
};

struct TimeInterval {
  TimeInterval () :
    start(""),
    duration("") {};
  std::string start;        // 开始时间, 单位为秒，支持floar格式，默认0
  std::string duration;     // 持续时间，单位为秒，支持float格式，默认原始视频时长
  std::string to_string() const {
    std::stringstream ss;
    ss << "start: " << start << std::endl
       << "duration: " << duration << std::endl;
    return ss.str();
  }    
};

struct ClipConfig {
  ClipConfig () :
    duration("") {}
  std::string duration;     // 分片时长，默认5s
  std::string to_string() const {
    std::stringstream ss;
    ss << "duration: " << duration << std::endl;
    return ss.str();
  }  
};

struct Container {
  Container () :
    format(""),
    clip_config() {}
  std::string format;       // 封装格式，不同任务类型取值范围不同，取值详见API接口文档
  ClipConfig clip_config;   // 分片配置，当format为 hls 或 dash 时生效
  std::string to_string() const {
    std::stringstream ss;
    ss << "format: " << format << std::endl
       << "clip_config: " << clip_config.to_string() << std::endl;
    return ss.str();
  }  
};

struct Video {
  Video () : 
    codec(""),
    width(""),
    height(""),
    fps(""),
    remove("false"),
    profile(""),
    bit_rate(""),
    crf(""),
    gop(""),
    preset(""),
    buf_size(""),
    max_rate(""),
    pixfmt(""),
    long_short_mode(""),
    rotate(""),
    roi(""),
    crop(""),
    interlaced(""),
    animate_only_keep_key_frame(""),
    animate_time_interval_of_frame(""),
    animate_frames_per_second(""),
    quality("") {};
  std::string codec;                          // 编码格式，默认 H.264，不同任务类型取值范围不同，取值详情见API接口文档
  std::string width;                          // 宽，单位px，取值范围[128,4096]，必须为偶数，默认为视频原始宽，若只设置 Width 时，按照视频原始比例计算 Height
  std::string height;                         // 高，单位px，取值范围[128,4096]，必须为偶数，默认为视频原始高，若只设置 Height 时，按照视频原始比例计算 Width
  std::string fps;                            // 帧率，取值范围(0,60]，单位fps
  std::string remove;                         // 是否删除视频流，取值 true 或 false，默认 false
  std::string profile;                        // 编码级别，当编码格式为 H.264 时支持此参数，取值范围支持 baseline、main、high、auto，详见 https://cloud.tencent.com/document/product/460
  std::string bit_rate;                       // 视频输出文件码率，取值范围[10.50000]，单位Kbps，设置为 auto 表示自适应码率
  std::string crf;                            // 码率-质量控制因子，取值范围(0,51]，如果设置了Crf，则BitRate失效，当BitRate为空时，Crf默认25
  std::string gop;                            // 关键帧间最大帧数，取值范围[1,100000]
  std::string preset;                         // 视频算法器预置，详见 https://cloud.tencent.com/document/product/460
  std::string buf_size;                       // 缓冲区大小，取值范围[1000,128000]，单位Kb，Codec为 VP8/VP9 时不支持该参数
  std::string max_rate;                       // 视频码率峰值，取值范围[10,50000]，单位Kbps，Codec为 VP8/VP9 时不支持该参数
  std::string pixfmt;                         // 视频颜色格式，Codec为H.264时支持 yuv420p、yuv422p、yuv444p、yuvj420p、yuvj422p、yuvj444p、auto，Codec为H.265支持：yuv420p、yuv420p10le、auto, Codec为 VP8/VP9/AV1 时不支持
  std::string long_short_mode;                // 长短边自适应，取值 true或 false。Codec 为 VP8/VP9/AV1 时不支持此参数
  std::string rotate;                         // 旋转角度，值范围：[0, 360)
  std::string roi;                            // Roi 强度 取值为 none、low、medium、high Codec 为 VP8/VP9 时不支持此参数
  std::string crop;                           // 自由裁剪，自定义裁切: width:height:left:top。示例:1280:800:0:140，width和height的值需要大于0，left和top的值需要大于等于0
  std::string interlaced;                     // 开启隔行扫描，取值 false/true
  std::string animate_only_keep_key_frame;    // 动图只保留关键帧，取值 true ：AnimateTimeIntervalOfFrame 和 AnimateFramesPerSecond 无效； false：AnimateTimeIntervalOfFrame  和 AnimateFramesPerSecond 必填
  std::string animate_time_interval_of_frame; // 动图抽帧间隔时间 取值范围：（0，视频时长] 若设置 TimeInterval.Duration，则小于该值
  std::string animate_frames_per_second;      // 每秒抽帧帧数，取值范围：（0，视频帧率）
  std::string quality;                        // 相对质量，取值范围[1,100)，webp 图像质量设定生效，gif 没有质量参数
  std::string to_string() const {
    std::stringstream ss;
    ss << "codec: " << codec << std::endl
       << "width: " << width << std::endl
       << "height: " << height << std::endl
       << "remove: " << remove << std::endl
       << "profile: " << profile << std::endl
       << "bit_rate: " << bit_rate << std::endl
       << "crf: " << crf << std::endl
       << "gop: " << gop << std::endl
       << "preset: " << preset << std::endl
       << "buf_size: " << buf_size << std::endl
       << "max_rate: " << max_rate << std::endl
       << "pixfmt: " << pixfmt << std::endl
       << "long_short_mode: " << long_short_mode << std::endl
       << "rotate: " << rotate << std::endl
       << "roi: " << roi << std::endl
       << "crop: " << crop << std::endl
       << "interlaced: " << interlaced << std::endl
       << "animate_only_keep_key_freme: " << animate_only_keep_key_frame << std::endl
       << "animate_time_interval_of_frame: " << animate_time_interval_of_frame << std::endl
       << "animate_frames_per_second: " << animate_frames_per_second << std::endl
       << "quality: " << quality << std::endl;
    return ss.str();    
  }
};

struct Audio {
  Audio() :
    codec(""),
    sample_rate(""),
    bit_rate(""),
    channels(""),
    remove(""),
    sample_format("") {};
  std::string codec;                // 编解码格式，取值 aac、mp3、flac、amr、Vorbis、opus、pcm_s16le, 默认 aac
  std::string sample_rate;          // 采样率，单位 Hz，取值 8000、11025、12000、16000、22050、24000、32000、44100、48000、88200、96000，默认 44100
  std::string bit_rate;             // 原始音频码率，单位 Kbps，取值范围[8,1000]
  std::string channels;             // 声道数
  std::string remove;               // 是否删除音频流，取值 true/false，默认false
  std::string sample_format;        // 采样位宽
  std::string to_string() const {
    std::stringstream ss;
    ss << "codec: " << codec << std::endl
       << "remove: " << remove << std::endl
       << "bit_rate: " << bit_rate << std::endl
       << "sample_rate: " << sample_rate << std::endl
       << "channels: " << channels << std::endl
       << "sample_format: " << sample_format << std::endl;
    return ss.str();    
  }
};

struct HlsEncrypt {
  HlsEncrypt() :
    is_hls_encrypt(""),
    url_key("") {};
  std::string is_hls_encrypt;     // 是否开启 HLS 加密，取值 true、false 当 Container.Format 为 hls 时支持加密
  std::string url_key;            // HLS 加密的 key，当 IsHlsEncrypt 为 true 时，该参数才有意义
  std::string to_string() const {
    std::stringstream ss;
    ss << "is_hls_encrypt: " << is_hls_encrypt << std::endl
       << "url_key: " << url_key << std::endl;
    return ss.str();    
  }
};

struct DashEncrypt {
  DashEncrypt() :
    is_encrypt(""),
    url_key("") {};
  std::string is_encrypt;         // 是否开启 DASH 加密，取值 true、false 当 Container.Format 为 dash 时支持加密
  std::string url_key;            // DASH 加密的 key 当 IsEncrypt 为 true 时，该参数才有意义
  std::string to_string() const {
    std::stringstream ss;
    ss << "is_encrypt: " << is_encrypt << std::endl
       << "url_key: " << url_key << std::endl;
    return ss.str();    
  }  
};

struct TransConfig {
  TransConfig() :
    adj_dar_method(""),
    is_check_reso(""),
    is_check_video_bit_rate(""),
    video_bit_rate_adj_method(""),
    is_check_audio_bit_rate(""),
    audio_bit_rate_adj_method(""),
    is_check_video_fps(""),
    video_fps_adj_method(""),
    delete_meta_data(""),
    is_hdr_2_sdr(""),
    transcode_index(""),
    hls_encrypt(),
    dash_encrypt() {}
  std::string adj_dar_method;               // 分辨率调整方式，取值 scale、crop、pad、none
  std::string is_check_reso;                // 是否检查分辨率，取值 true、false
  std::string reso_adj_method;              // 分辨率调整方式，当 IsCheckReso 为 true 时生效，取值0、1；0 表示使用原视频分辨率，1 表示返回转码失败
  std::string is_check_video_bit_rate;      // 是否检查视频码率，取值 true、false
  std::string video_bit_rate_adj_method;    // 视频码率调整方式，IsCheckVideoBitrate 为 true 时生效，取值0、1；当输出视频码率大于原视频码率时，0表示使用原视频码率；1表示返回转码失败
  std::string is_check_audio_bit_rate;      // 是否检查音频码率，取值 true、false
  std::string audio_bit_rate_adj_method;    // 音频码率调整方式，IsCheckAudioBitrate 为 true 时生效，取值0、1； 当输出音频码率大于原音频码率时，0表示使用原音频码率；1表示返回转码失败
  std::string is_check_video_fps;           // 是否检查视频帧率，取值 true、false
  std::string video_fps_adj_method;         // 视频帧率调整方式 IsCheckVideoFps 为 true 时生效，取值0、1； 当输出视频帧率大于原视频帧率时，0表示使用原视频帧率；1表示返回转码失败
  std::string delete_meta_data;             // 是否删除文件中的 MetaData 信息，取值 true、false
  std::string is_hdr_2_sdr;                 // 是否开启 HDR 转 SDR，取值 true、false
  std::string transcode_index;             // 指定处理的流编号，对应媒体信息中的 Response.MediaInfo.Stream.Video.Index 和 Response.MediaInfo.Stream.Audio.Index，详见 https://cloud.tencent.com/document/product/460/49284#MediaInfo
  HlsEncrypt hls_encrypt;                   // hls 加密配置
  DashEncrypt dash_encrypt;                 // dash 加密配置
  std::string to_string() const {
    std::stringstream ss;
    ss << "adj_dar_method: " << adj_dar_method << std::endl
       << "is_check_reso: " << is_check_reso << std::endl
       << "reso_adj_method: " << reso_adj_method << std::endl
       << "is_check_video_bit_rate: " << is_check_video_bit_rate << std::endl
       << "video_bit_rate_adj_method: " << video_bit_rate_adj_method << std::endl
       << "is_check_audio_bit_rate: " << is_check_audio_bit_rate << std::endl
       << "audio_bit_rate_adj_method: " << audio_bit_rate_adj_method << std::endl
       << "is_check_video_fps: " << is_check_video_fps << std::endl
       << "video_fps_adj_method: " << video_fps_adj_method << std::endl
       << "delete_meta_data: " << delete_meta_data << std::endl
       << "is_hdr_2_sdr: " << is_hdr_2_sdr << std::endl
       << "trans_code_index: " << transcode_index << std::endl
       << "hls_encrypt: " << hls_encrypt.to_string() << std::endl
       << "interdash_encryptlaced: " << dash_encrypt.to_string() << std::endl;
    return ss.str();    
  }
};

struct EffectConfig {
  EffectConfig() :
    enable_start_fade_in(""),
    start_fade_in_time(""),
    enable_end_fade_out(""),
    end_fade_out_time(""),
    enable_bgm_fade(""),
    bgm_fade_time("") {};
  std::string enable_start_fade_in;     // 开始淡出，取值 true/false，默认false
  std::string start_fade_in_time;       // 淡入时长，大于0，支持浮点数
  std::string enable_end_fade_out;      // 开始淡出，取值 true/false，默认false
  std::string end_fade_out_time;        // 淡出时长，大于0，支持浮点数
  std::string enable_bgm_fade;          // 开始 bgm 转换淡入，取值 true/false，默认 false
  std::string bgm_fade_time;            // bgm 转换淡入时长，大于0，支持浮点数
  std::string to_string() const {
    std::stringstream ss;
    ss << "enable_start_fade_in: " << enable_start_fade_in << std::endl
       << "start_fade_in_time: " << start_fade_in_time << std::endl
       << "enable_end_fade_out: " << enable_end_fade_out << std::endl
       << "end_fade_out_time: " << end_fade_out_time << std::endl
       << "enable_bgm_fade: " << enable_bgm_fade << std::endl
       << "bgm_fade_time: " << bgm_fade_time << std::endl;
    return ss.str();    
  }  
};

struct AudioMix {
  AudioMix() : 
    audio_source(""),
    mix_mode(""),
    replace(""),
    effect_config() {};
  std::string audio_source;     // 需要被混音的音轨媒体地址, 需要做 URLEncode, 需要和Input媒体文件在同一个bucket
  std::string mix_mode;         // 混音模式，取值 Repeat 循环混音、Once 混音一次播放，默认Repeat
  std::string replace;          // 是否用混音音轨媒体替换Input媒体文件的原音频，取值 true/false，默认 false
  EffectConfig effect_config;   // 混音淡入淡出配置
  std::string to_string() const {
    std::stringstream ss;
    ss << "audio_source: " << audio_source << std::endl
       << "mix_mode: " << mix_mode << std::endl
       << "replace: " << replace << std::endl
       << "effect_config: " << effect_config.to_string() << std::endl;
    return ss.str();    
  }    
};

struct SlideConfig {
  SlideConfig() :
    slide_mode(""),
    x_slide_speed(""),
    y_slide_speed("") {};
  std::string slide_mode;       // 滑动模式，取值 Default 默认不开启，ScrollFromLeft: 从左到右滚动，若设置了ScrollFromLeft模式，则Watermark.Pos参数不生效
  std::string x_slide_speed;    // 横向滑动速度，取值范围：[0,10]内的整数，默认0
  std::string y_slide_speed;    // 纵向滑动速度，取值范围：[0,10]内的整数，默认0
    std::string to_string() const {
    std::stringstream ss;
    ss << "slide_mode: " << slide_mode << std::endl
       << "x_slide_speed: " << x_slide_speed << std::endl
       << "y_slide_speed: " << y_slide_speed << std::endl;
    return ss.str();    
  }    
};

struct ImageWatermark {
  ImageWatermark() :
    url(""),
    mode(""),
    width(""),
    height(""),
    transparency(""),
    backgroud("") {};
  std::string url;            // 水印图地址(需要 Urlencode 后传入)
  std::string mode;           // 尺寸模式，取值 Original：原有尺寸 Proportion：按比例 Fixed：固定大小
  std::string width;          // 宽，取值详见https://cloud.tencent.com/document/product/460/84725#Watermark
  std::string height;         // 高，取值详见https://cloud.tencent.com/document/product/460/84725#Watermark
  std::string transparency;  // 透明度，值范围：[1 100]，单位%
  std::string backgroud;     // 是否背景图，取值 true、false
  std::string to_string() const {
    std::stringstream ss;
    ss << "url: " << url << std::endl
       << "mode: " << mode << std::endl
       << "width: " << width << std::endl
       << "height: " << height << std::endl
       << "transparency: " << transparency << std::endl
       << "backgroud: " << backgroud << std::endl;
    return ss.str();    
  }  
};

struct TextWatermark {
  TextWatermark() :
    font_size(""),
    font_type(""),
    font_color(""),
    transparency(""),
    text("") {}
  std::string font_size;      // 字体大小，值范围：[5 100]，单位 px
  std::string font_type;      // 字体类型，详见https://cloud.tencent.com/document/product/460/84725#Watermark
  std::string font_color;     // 字体颜色，格式：0xRRGGBB
  std::string transparency;  // 透明度，值范围：[1 100]，单位%
  std::string text;           // 水印内容，长度不超过64个字符，仅支持中文、英文、数字、_、-和*
  std::string to_string() const {
    std::stringstream ss;
    ss << "font_size: " << font_size << std::endl
       << "font_type: " << font_type << std::endl
       << "font_color: " << font_color << std::endl
       << "transparency: " << transparency << std::endl
       << "text: " << text << std::endl;
    return ss.str();
  }
};

struct Watermark {
  Watermark() :
    type(""),
    pos(""),
    loc_mode(""),
    dx(""),
    dy(""),
    start_time(""),
    end_time(""),
    slide_config(),
    image(),
    text() {};
  std::string type;           // 水印类型，取值 Text 文字水印，Image 图片水印 
  std::string pos;            // 位置基准，取值 TopRight/TopLeft/BottomRight/BottomLeft/Left/Right/Top/Bottom/Center
  std::string loc_mode;       // 偏移方式，取值 Relativity 按比例，Absolute 固定位置
  std::string dx;             // 水平偏移，取值详见 https://cloud.tencent.com/document/product/460/84725#Watermark
  std::string dy;             // 垂直偏移，取值详见 https://cloud.tencent.com/document/product/460/84725#Watermark
  std::string start_time;     // 开始时间，取值[0,视频时长]，单位秒，支持float格式，执行精度到毫秒
  std::string end_time;       // 结束时间，取值[0,视频时长]，单位秒，支持float格式，执行精度到毫秒
  SlideConfig slide_config;   // 水印滑动配置，配置该参数后水印位移设置不生效，极速高清/H265转码暂时不支持该参数
  ImageWatermark image;       // 图片水印节点
  TextWatermark text;         // 文本水印节点
  std::string to_string() const {
    std::stringstream ss;
    ss << "type: " << type << std::endl
       << "pos: " << pos << std::endl
       << "loc_mode: " << loc_mode << std::endl
       << "dx: " << dx << std::endl
       << "dy: " << dy << std::endl
       << "start_time: " << start_time << std::endl
       << "slide_config: " << slide_config.to_string() << std::endl
       << "image: " << image.to_string() << std::endl
       << "text: " << text.to_string() << std::endl;
    return ss.str();    
  }    
};

struct RemoveWatermark {
  RemoveWatermark() :
    dx(""),
    dy(""),
    width(""),
    height("") {};
  std::string dx;       // 距离左上角原点 x 偏移，范围为[1, 4096]
  std::string dy;       // 距离左上角原点 y 偏移，范围为[1, 4096]
  std::string width;    // 宽，范围为[1, 4096]
  std::string height;   // 高，范围为[1, 4096]
  std::string to_string() const {
    std::stringstream ss;
    ss << "width: " << width << std::endl
       << "height: " << height << std::endl
       << "dx: " << dx << std::endl
       << "dy: " << dy << std::endl;
    return ss.str();    
  }    
};

struct Subtitle {
  Subtitle() :
    url("") {}
  std::string url;  // 同 bucket 的字幕地址，需要 encode
  std::string to_string() const {
    std::stringstream ss;
    ss << "url: " << url << std::endl;
    return ss.str();    
  }  
};

struct Subtitles {
  Subtitles() : 
    subtitle(std::vector<Subtitle>()) {};
  std::vector<Subtitle> subtitle;   // 字幕参数
  std::string to_string() const {
    std::stringstream ss;
    ss << "subtitle: ";
    for (std::vector<Subtitle>::const_iterator iter = subtitle.begin(); iter != subtitle.end(); iter++) {
      ss << iter->to_string() << std::endl;  
    }
    return ss.str();    
  }  
};

struct DigitalWatermark {
  DigitalWatermark() : 
    message(""),
    type(""),
    version(""),
    ignore_error(""),
    state("") {};
  std::string message;       // 嵌入数字水印的水印信息，长度不超过64个字符，仅支持中文、英文、数字、_、-和*
  std::string type;          // 数字水印类型，当前仅可设置为 Text
  std::string version;       // 数字水印版本，当前仅可设置为 V1
  std::string ignore_error;  // 当添加水印失败是否忽略错误继续执行任务，限制为 true/false，在非 DigitalWatermark 任务中生效
  std::string state;         // 添加水印是否成功，执行中为Running，成功为 Success，失败为 Failed，该字段不能主动设置，当任务提交成功时，会返回该字段
  std::string to_string() const {
    std::stringstream ss;
      ss << "message: " << message << std::endl
        << "type: " << type << std::endl
        << "version: " << version << std::endl
        << "ignore_error: " << ignore_error << std::endl
        << "state: " << state << std::endl;
    return ss.str();    
  } 
};

struct ExtractDigitalWatermark {
  ExtractDigitalWatermark() : type(""), version(""), message("") {}
  std::string type;       // 数字水印类型，当前仅可设置为 Text
  std::string version;    // 数字水印版本，当前仅可设置为 V1
  std::string message;    // 提取出的数字水印字符串信息
  std::string to_string() const {
    std::stringstream ss;
      ss << "message: " << message << std::endl
        << "type: " << type << std::endl
        << "version: " << version << std::endl;
    return ss.str();      
  }
};


struct Transcode {
  Transcode():
    time_interval(),
    container(),
    video(),
    audio(),
    trans_config(),
    audio_mix(),
    audio_mix_array(std::vector<AudioMix>()) {};
  TimeInterval time_interval;             // 时间区间
  Container container;                    // 容器格式
  Video video;                            // 视频信息
  Audio audio;                            // 音频信息
  TransConfig trans_config;               // 转码配置
  AudioMix audio_mix;                     // 混音参数
  std::vector<AudioMix> audio_mix_array;  // 混音参数，最多能传两个
  std::string to_string() const {
    std::stringstream ss;
    ss << "time_interval: " << time_interval.to_string() << std::endl
        << "container: " << container.to_string() << std::endl
        << "video: " << video.to_string() << std::endl
        << "audio: " << audio.to_string() << std::endl
        << "trans_config: " << trans_config.to_string() << std::endl
        << "audio_mix: " << audio_mix.to_string() << std::endl;
    ss << "audio_mix_array: ";
    for (std::vector<AudioMix>::const_iterator iter = audio_mix_array.begin(); iter != audio_mix_array.end(); iter++) {
      ss << iter->to_string() << std::endl;  
    }        
    return ss.str();    
  }   
};

struct Animation {
  Animation():
    time_interval(),
    container(),
    video() {}
  TimeInterval time_interval;             // 时间区间
  Container container;                    // 容器格式
  Video video;                            // 视频信息
  std::string to_string() const {
    std::stringstream ss;
    ss << "time_interval: " << time_interval.to_string() << std::endl
        << "container: " << container.to_string() << std::endl
        << "video: " << video.to_string() << std::endl;
    return ss.str();    
  }   
};

struct SmartCover {
  SmartCover():
    format(""),
    width(""),
    height(""),
    count(""),
    delete_duplicates("") {};
  std::string format;                      // 图片格式，支持 jpg、png、webp，默认jpg
  std::string width;                       // 宽，取值范围[128,4096]，单位px，若只设置 Width 时，按照视频原始比例计算 Heigth，默认视频原始宽度
  std::string height;                      // 窄，取值范围[128,4096]，单位px，若只设置 Height 时，按照视频原始比例计算 Width，默认视频原始高度
  std::string count;                       // 截图数量，取值范围 [1,10]，默认3
  std::string delete_duplicates;           // 封面去重，取值true/false，默认false
  std::string to_string() const {
    std::stringstream ss;
    ss << "format: " << format << std::endl
       << "width: " << width << std::endl
       << "height: " << height << std::endl
       << "count: " << count << std::endl
       << "delete_duplicates: " << delete_duplicates << std::endl;
    return ss.str();    
  }       
};

struct Segment {
  Segment():
    format(""),
    duration(""),
    transcode_index(""),
    hls_encrypt(),
    start_time(""),
    end_time("") {}
  std::string format;                         // 封装格式，支持 aac、mp3、flac、mp4、ts、mkv、avi、hls、m3u8
  std::string duration;                       // 转封装时长，单位：秒，不小于5的整数，不设置 Duration 时，表示只转封装格式不分段
  std::string transcode_index;                // 处理的流编号
  HlsEncrypt hls_encrypt;                     // hls 加密配置,当封装格式为 hls 和 m3u8 时生效
  std::string start_time;                     // 开始时间 取值范围：[0,视频时长]，默认值为0，单位为秒，支持 float 格式，执行精度精确到毫秒
  std::string end_time;                       // 结束时间 取值范围：[0,视频时长]，默认值为0，单位为秒，支持 float 格式，执行精度精确到毫秒
  std::string to_string() const {
    std::stringstream ss;
    ss << "format: " << format << std::endl
       << "duration: " << duration << std::endl
       << "transcode_index: " << transcode_index << std::endl
       << "start_time: " << start_time << std::endl
       << "end_time: " << end_time << std::endl
       << "hls_encrypt: " << hls_encrypt.to_string() << std::endl;
    return ss.str();    
  }     
};

struct AudioConfig {
  AudioConfig() {};
  std::string codec;          // 编解码格式，取值 aac、mp3、flac、amr。当 Request.AudioMode 为 MusicMode 时，仅支持 mp3、wav、acc
  std::string sample_rate;    // 采样率 单位：Hz 可选 8000、11025、22050、32000、44100、48000、96000。 当 Codec 设置为 aac/flac 时，不支持 8000；当 Codec 设置为 mp3 时，不支持 8000 和 96000；当 Codec 设置为 amr 时，只支持 8000；当 Request.AudioMode 为 MusicMode 时，该参数无效
  std::string bit_rate;       // 音频码率 单位：Kbps 值范围：[8，1000] 当 Request.AudioMode 为 MusicMode 时，该参数无效
  std::string channels;       // 声道数 当 Codec 设置为 aac/flac，支持1、2、4、5、6、8；当 Codec 设置为 mp3，支持1、2；当 Codec 设置为 amr，只支持1；当 Request.AudioMode 为 MusicMode 时，该参数无效
    std::string to_string() const {
    std::stringstream ss;
    ss << "codec: " << codec << std::endl
       << "sample_rate: " << sample_rate << std::endl
       << "bit_rate: " << bit_rate << std::endl
       << "channels: " << channels << std::endl;
    return ss.str();    
  }    
};

struct VoiceSeparate {
  VoiceSeparate():
    audio_mode(""),
    audio_config() {}
  std::string audio_mode;                   // 输出音频模式，IsAudio：输出人声 IsBackground：输出背景声 AudioAndBackground：输出人声和背景声 MusicMode：输出人声、背景声、Bass声、鼓声
  AudioConfig audio_config;                 // 音频配置
  std::string to_string() const {
    std::stringstream ss;
    ss << "audio_mode: " << audio_mode << std::endl
       << "audio_config: " << audio_config.to_string() << std::endl;
    return ss.str();    
  }       
};

struct VideoMontage {
  VideoMontage() : 
    audio(),
    video(),
    container(),
    audio_mix(),
    audio_mix_array(std::vector<AudioMix>()),
    scene("") {};
  Audio audio;                                  // 音频参数
  Video video;                                  // 视频参数
  Container container;                          // 封装格式
  AudioMix audio_mix;                           // 混音参数
  std::vector<AudioMix> audio_mix_array;        // 混音参数数组
  std::string scene;                            // 精彩集锦场景 Video：普通视频 Soccer： 足球
  std::string to_string() const {
    std::stringstream ss;
    ss << "container: " << container.to_string() << std::endl
      << "video: " << video.to_string() << std::endl
      << "audio: " << audio.to_string() << std::endl
      << "scene: " << scene << std::endl
      << "audio_mix: " << audio_mix.to_string() << std::endl;
    ss << "audio_mix_array: ";
    for (std::vector<AudioMix>::const_iterator iter = audio_mix_array.begin(); iter != audio_mix_array.end(); iter++) {
      ss << iter->to_string() << std::endl;  
    }    
    return ss.str();    
  }      
};

struct ConcatFragment {
  ConcatFragment() :
    url(""),
    fragment_index(""),
    start_time(""),
    end_time("") {};
  std::string url;                // 同 bucket 对象地址，需要 urlEncode
  std::string fragment_index;     // 拼接对象的索引位置，大于等于0的整数
  std::string start_time;         // 开始时间，取值范围[0, 视频时长] 单位为秒
  std::string end_time;           // 结束时间，取值范围[0, 视频时长] 单位为秒
  std::string to_string() const {
    std::stringstream ss;
    ss << "url: " << url << std::endl
       << "fragment_index: " << fragment_index << std::endl
       << "start_time: " << start_time << std::endl
       << "end_time: " << end_time << std::endl;
    return ss.str();    
  }     
};

struct SceneChnageInfo {
  SceneChnageInfo():
    mode(""),
    time("") {}
  std::string mode;     // 转场模式，取值：Default 不添加转场特效、FADE 淡入淡出、GRADIENT 渐变
  std::string time;     // 转场时间，单位秒，取值范围(0,5]，支持小数
  std::string to_string() const {
    std::stringstream ss;
    ss << "mode: " << mode << std::endl
       << "time: " << time << std::endl;
    return ss.str();    
  }     
  
};

struct Concat {
  Concat() : 
    concat_fragment(std::vector<ConcatFragment>()),
    audio(),
    video(),
    container(),
    audio_mix(),
    audio_mix_array(std::vector<AudioMix>()),
    index(""),
    direct_concat("") {};
  std::vector<ConcatFragment> concat_fragment;  // 拼接节点，支持多个文件，按照文件顺序拼接
  Audio audio;                                  // 音频参数
  Video video;                                  // 视频参数
  Container container;                          // 封装格式
  AudioMix audio_mix;                           // 混音参数
  std::vector<AudioMix> audio_mix_array;        // 混音参数数组
  std::string index;                            // Input 节点位于 ConcatFragment 序列索引，不能大于 ConcatFragment 长度
  std::string direct_concat;                    // 简单拼接方式（不转码直接拼接），其他的视频和音频参数失效，取值 true/false
  SceneChnageInfo scene_change_info;            // 转场特效
  std::string to_string() const {
    std::stringstream ss;
    ss << "index: " << index << std::endl
      << "container: " << container.to_string() << std::endl
      << "video: " << video.to_string() << std::endl
      << "audio: " << audio.to_string() << std::endl
      << "derect_concat: " << direct_concat << std::endl
      << "scene_change_info: " << scene_change_info.to_string() << std::endl
      << "audio_mix: " << audio_mix.to_string() << std::endl;
    ss << "audio_mix_array: ";
    for (std::vector<AudioMix>::const_iterator iter = audio_mix_array.begin(); iter != audio_mix_array.end(); iter++) {
      ss << iter->to_string() << std::endl;  
    }   
    ss << "concat_fragment";
    for (std::vector<ConcatFragment>::const_iterator iter = concat_fragment.begin(); iter != concat_fragment.end(); iter++) {
      ss << iter->to_string() << std::endl;  
    }   
    return ss.str();    
  }      
};

struct JobsOperation {
  JobsOperation() : 
    job_level(0),
    user_data(""),
    template_id(""),
    template_name(""),
    snapshot(),
    media_result(),
    file_uncompress_config(),
    file_uncompress_result(),
    smart_cover(),
    concat(),
    video_montage(),
    voice_separate(),
    segment() {}
  Output output;                                    // 输出配置，非必选
  int job_level;                                    // 任务优先级，非必选，级别限制：0 、1 、2 。级别越大任务优先级越高，默认为0
  std::string user_data;                            // 用户透传信息，非必选
  std::string template_id;                          // 模版ID，非必选
  std::string template_name;                        // 模版名称，非必选
  Snapshot snapshot;                                // 截图参数，非必选，Tag 为 Snapshot 时生效
  Transcode transcode;                              // 转码参数，非必选，Tag 为 Transcode 时生效
  std::vector<Watermark> watermarks;                // 水印参数，非必选，Tag 为 Transocde、Watermark 时生效
  std::vector<std::string> watermark_template_id;   // 水印模版ID，非必选，Tag 为 Transocde、Watermark 时生效
  RemoveWatermark remove_watermark;                 // 去除水印参数，非必选，Tag 为 Transcode 时生效
  Subtitles subtitles;                              // 字幕参数，非必选，Tag 为 Transcode 时生效
  DigitalWatermark digital_watermark;               // 数字水印参数，非必选，Tag 为 Transocde、DigitalWatermark 时生效
  ExtractDigitalWatermark extract_digital_watermark;// 提取数字水印参数，非必选，Tag 为 ExtractDigitalWatermark 时生效
  MediaResult media_result;                         // 输出文件基本信息，响应字段，非请求参数
  MediaInfo media_info;                             // 输出文件媒体信息，响应字段，非请求参数
  FileUncompressConfig file_uncompress_config;      // 文件压缩配置
  FileUncompressResult file_uncompress_result;      // 文件压缩结果信息
  Animation animation;                              // 动图参数，非必选，Tag 为 Animation 时生效
  SmartCover smart_cover;                           // 智能封面参数，非必选，Tag 为 SmartCover 时生效
  Concat concat;                                    // 拼接参数，非必选，Tag 为 Concat 时生效
  VideoMontage video_montage;                       // 精彩锦集参数，非必选，Tag 为 VideoMontage 时生效
  VoiceSeparate voice_separate;                     // 人声分离参数，非必选，Tag 为 VoiceSeparate 时生效
  Segment segment;                                  // 转封装参数，非必选，Tag 为 Segment 时生效
  std::string to_string() const {
    std::stringstream ss;
    ss << "output: " << output.to_string() << std::endl
       << "job_level: " << job_level << std::endl
       << "template_id: " << template_id << std::endl
       << "template_name: " << template_name << std::endl
       << "user_data: " << user_data << std::endl
       << "file_uncompress_config: " << file_uncompress_config.to_string() << std::endl
       << "file_uncompress_result: " << file_uncompress_result.to_string() << std::endl
       << "snapshot: " << snapshot.to_string() << std::endl
       << "transcode: " << transcode.to_string() << std::endl
       << "remove_watermark: " << remove_watermark.to_string() << std::endl
       << "subtitles: " << subtitles.to_string() << std::endl
       << "digital_watermark: " << digital_watermark.to_string() << std::endl
       << "extract_digital_watermark: " << extract_digital_watermark.to_string() << std::endl
       << "media_result: " << media_result.to_string() << std::endl
       << "media_info: " << media_info.to_string() << std::endl
       << "animation: " << animation.to_string() << std::endl
       << "video_montage: " << video_montage.to_string() << std::endl
       << "voice_separate: " << voice_separate.to_string() << std::endl
       << "smart_cover: " << smart_cover.to_string() << std::endl
       << "segment: " << segment.to_string() << std::endl;
    ss << "watermarks: ";
    for (std::vector<Watermark>::const_iterator iter = watermarks.begin(); iter != watermarks.end(); iter++) {
      ss << iter->to_string() << std::endl;  
    }   
    ss << "watermark_template_id: ";
    for (std::vector<std::string>::const_iterator iter = watermark_template_id.begin(); iter != watermark_template_id.end(); iter++) {
      ss << iter->c_str() << ",";  
    }   
    ss << std::endl;
    return ss.str();
  }
};

struct JobsOptions {
  JobsOptions():
    tag(""),
    input(),
    operation(),
    queue_id(),
    queue_type(),
    callback_format(),
    callback_type(),
    callback(),
    callback_mq_config() {}
  std::string tag;
  Input input;
  JobsOperation operation;
  std::string queue_id;
  std::string queue_type;
  std::string callback_format;
  std::string callback_type;
  std::string callback;
  CallBackkMqConfig callback_mq_config;
};

struct JobsDetails {
  std::string code;     // 错误码，只有 State 为 Failed 时有意义
  std::string message;  // 错误描述，只有 State 为 Failed 时有意义
  std::string job_id;   // 新创建任务的 ID
  std::string tag;      // 新创建任务的 Tag：DocProcess
  std::string state;    // 任务的状态，为
                        // Submitted、Running、Success、Failed、Pause、Cancel 其中一个
  std::string create_time;  // 任务的创建时间
  std::string end_time;     //
  std::string queue_id;     // 任务所属的队列 ID
  std::string start_time;  // 任务开始时间
  std::string queue_type; // 队列类型
  std::string progress;   // 任务进度
  Input input;              // 该任务的输入文件路径
  JobsOperation operation; // 任务operation
  std::string to_string() const {
    std::stringstream ss;
    ss << "code: " << code << std::endl;
    ss << "message: " << message << std::endl;
    ss << "job_id: " << job_id << std::endl;
    ss << "tag: " << tag << std::endl;
    ss << "state: " << state << std::endl;
    ss << "start_time: " << start_time << std::endl;
    ss << "create_time: " << create_time << std::endl;
    ss << "end_time: " << end_time << std::endl;
    ss << "queue_id: " << queue_id << std::endl;
    ss << "queue_type: " << queue_type << std::endl;
    ss << "progress: " << progress << std::endl;
    ss << "input: " << input.to_string() << std::endl;
    ss << "operation: " << operation.to_string() << std::endl;
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

  PicOperation GetPictureOperation() const { return m_pic_operation; }

  void SetPicOperation(const PicOperation& pic_operation) {
    m_pic_operation = pic_operation;
    AddHeader("Pic-Operations", m_pic_operation.GenerateJsonString());
  }

  // 检查图片处理的效果图文件是否覆盖了原图
  void CheckCoverOriginImage();  

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

class CreateDocBucketReq : public BucketReq{
  public:
  explicit CreateDocBucketReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    m_method = "POST";
    m_path = "/docbucket";
    SetHttps();
  }

  virtual ~CreateDocBucketReq() {}
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

  // 源数据的后缀类型，当前文档转换根据 COS 对象的后缀名来确定源数据类型。当
  // COS 对象没有后缀名时，可以设置该值
  void SetSrcType(const std::string& src_type) {
    AddParam("srcType", src_type);
  }

  // 需转换的文档页码，默认从1开始计数；表格文件中 page 表示转换的第 X 个
  // sheet 的第 X 张图
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

class DescribeQueuesReq : public BucketReq {
 public:
  DescribeQueuesReq(const std::string bucket_name)
    :BucketReq(bucket_name) {
    m_method = "GET";

    m_path = "/queue";

    SetHttps();
  }
  virtual ~DescribeQueuesReq() {}

  // 队列 ID，以“,”符号分割字符串。
  void SetQueueId(const std::string& queue_id) {
    AddParam("queueId", queue_id);
  }

  // 队列状态
  // 1. Active 表示队列内的作业会被媒体处理服务调度执行。
  // 2. Paused 表示队列暂停，作业不再会被媒体处理调度执行，队列内的所有作业状态维持在暂停状态，已经执行中的任务不受影响。
  void SetState(const std::string& state) { AddParam("states", state); }
  
  // 第几页，默认值1。
  void SetPageNumber(const std::string& page_number) {
    AddParam("pageNumber", page_number);
  }

  // 每页个数，默认值10
  void SetPageSize(const std::string& page_size) {
    AddParam("PageSize", page_size);
  }
};

class UpdateQueueReq : public BucketReq {
 public:
  UpdateQueueReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/queue");
    AddHeader("Content-Type", "application/xml");
    SetHttps();
  }
  virtual ~UpdateQueueReq() {}
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

class DescribeDocProcessQueuesReq : public DescribeQueuesReq {
 public:
  DescribeDocProcessQueuesReq(const std::string& bucket_name)
      : DescribeQueuesReq(bucket_name) {
    SetPath("/docqueue");
  }
  virtual ~DescribeDocProcessQueuesReq() {}
};

class UpdateDocProcessQueueReq : public UpdateQueueReq {
 public:
  UpdateDocProcessQueueReq(const std::string& bucket_name)
      : UpdateQueueReq(bucket_name) {
    SetPath("/docqueue");
  }
  virtual ~UpdateDocProcessQueueReq() {}
};

class DescribeMediaBucketsReq : public BucketReq {
 public:
  DescribeMediaBucketsReq() {
    m_method = "GET";

    m_path = "/mediabucket";

    // 该接口只支持https
    SetHttps();
  }
  virtual ~DescribeMediaBucketsReq() {}

  // 地域信息，例如
  // ap-shanghai、ap-beijing，若查询多个地域以“,”分隔字符串，支持中国大陆地域
  void SetRegions(const std::string& regions) { AddParam("regions", regions); }

  // 存储桶名称，以“,”分隔，支持多个存储桶，精确搜索
  void SetBucketNames(const std::string& bucket_names) {
    AddParam("bucketNames", bucket_names);
  }

  // 存储桶名称前缀，前缀搜索
  void SetBucketName(const std::string& bucket_name) {
    AddParam("bucketName", bucket_name);
  }

  // 第几页
  void SetPageNumber(const std::string& page_number) {
    AddParam("pageNumber", page_number);
  }

  // 每页个数
  void SetPageSize(const std::string& page_size) {
    AddParam("PageSize", page_size);
  }
};

class GetSnapshotReq : public GetObjectByFileReq {
 public:
  GetSnapshotReq(const std::string& bucket_name, const std::string& object_name,
                 const std::string& local_file)
      : GetObjectByFileReq(bucket_name, object_name, local_file) {
    AddParam("ci-process", "snapshot");
    AddParam("time", "0");
  }

  virtual ~GetSnapshotReq() {}

  // 截图的时间点，单位为秒，必选
  void SetTime(float time) { AddParam("time", std::to_string(time)); }

  // 截图的宽,默认为0,当 width 和 height
  // 都为0时，表示使用视频的宽高；如果单个为0，则以另外一个值按视频宽高比例自动适应
  void SetWitdh(int width) { AddParam("width", std::to_string(width)); }

  // 截图的高,默认为0,当 width 和 height
  // 都为0时，表示使用视频的宽高；如果单个为0，则以另外一个值按视频宽高比例自动适应
  void SetHeight(int height) { AddParam("height", std::to_string(height)); }

  // 截图的格式，支持 jpg 和 png，默认 jpg
  void SetFormat(const std::string& format) { AddParam("format", format); }

  // 图片旋转方式
  // auto：按视频旋转信息进行自动旋转，off：不旋转，默认值为 auto
  void SetRotate(const std::string& rotate) { AddParam("rotate", rotate); }

  // 截帧方式
  // keyframe：截取指定时间点之前的最近的一个关键帧，
  // exactframe：截取指定时间点的帧， 默认值为 exactframe
  void SetMode(const std::string& mode) { AddParam("mode", mode); }
};

class PutBucketToCIReq : public BucketReq{
  public:
  explicit PutBucketToCIReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    m_method = "PUT";
  }

  virtual ~PutBucketToCIReq() {}
};

class CreateMediaBucketReq : public BucketReq{
  public:
  explicit CreateMediaBucketReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    m_method = "POST";
    m_path = "/mediabucket";
    SetHttps();
  }

  virtual ~CreateMediaBucketReq() {}
};


class CreateFileBucketReq : public BucketReq{
  public:
  explicit CreateFileBucketReq(const std::string& bucket_name) : BucketReq(bucket_name) {
    m_method = "POST";
    m_path = "/file_bucket";
    SetHttps();
  }

  virtual ~CreateFileBucketReq() {}
};

class DescribeFileBucketsReq : public DescribeMediaBucketsReq {
 public:
  DescribeFileBucketsReq() {
    m_method = "GET";

    m_path = "/file_bucket";

    // 该接口只支持https
    SetHttps();
  }
  virtual ~DescribeFileBucketsReq() {}
};

class GetMediaInfoReq : public ObjectReq {
 public:
  GetMediaInfoReq(const std::string& bucket_name,
                  const std::string& object_name)
      : ObjectReq(bucket_name, object_name) {
    AddParam("ci-process", "videoinfo");
    m_method = "GET";
  }

  virtual ~GetMediaInfoReq() {}
};

class GetPm3u8Req : public GetObjectByFileReq {
 public:
  GetPm3u8Req(const std::string& bucket_name, const std::string& object_name,
              const std::string& local_file)
      : GetObjectByFileReq(bucket_name, object_name, local_file) {
    AddParam("ci-process", "pm3u8");
    AddParam("expires", "3600");
  }

  virtual ~GetPm3u8Req() {}

  // 私有 ts 资源 url 下载凭证的相对有效期，单位为秒，范围为[3600, 43200]，必选
  void SetExpires(int expires) { AddParam("expires", std::to_string(expires)); }
};


class DescribeMediaQueuesReq : public DescribeQueuesReq {
  public:
  DescribeMediaQueuesReq(const std::string bucket_name)
    : DescribeQueuesReq(bucket_name) {
    m_path = "/queue";
  }
  virtual ~DescribeMediaQueuesReq() {} 

  // 队列类型
  // 1. CateAll：所有类型。
  // 2. Transcoding：媒体处理队列。
  // 3. SpeedTranscoding：媒体处理倍速转码队列。
  // 4. 默认为 Transcoding。
  void SetCategory(const std::string& category) {
    AddParam("category", category);
  }
};

class UpdateMediaQueueReq : public UpdateQueueReq {
 public:
  UpdateMediaQueueReq(const std::string& bucket_name)
      : UpdateQueueReq(bucket_name) {
    SetPath("/docqueue");
  }
  virtual ~UpdateMediaQueueReq() {}
};

class CreateDataProcessJobsReq : public BucketReq {
 public:
  CreateDataProcessJobsReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("POST");
    SetPath("/jobs");
    SetHttps();
    AddHeader("Content-Type", "application/xml");
  }

  virtual ~CreateDataProcessJobsReq() {}

  virtual bool GenerateRequestBody(std::string* body) const;
  static void GenerateVideoNode(rapidxml::xml_document<>& doc, const Video& video, rapidxml::xml_node<>* video_node);
  static void GenerateAudioNode(rapidxml::xml_document<>& doc, const Audio& audio, rapidxml::xml_node<>* node);
  static void GenerateContainerNode(rapidxml::xml_document<>& doc, const Container& container, rapidxml::xml_node<>* node);
  static void GenerateAudioMixNode(rapidxml::xml_document<>& doc, const AudioMix& audio_mix, rapidxml::xml_node<>* node);



  void setOperation(JobsOptions operation) {
    options_ = operation;
  } 

  private:
  JobsOptions options_;
  // virtual bool GenerateRequestBody(std::string* body) const {};
};

class DescribeDataProcessJobReq : public BucketReq {
 public:
  DescribeDataProcessJobReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetPath("/jobs");
    SetHttps();
  }
  virtual ~DescribeDataProcessJobReq() {}
  void SetJobId(const std::string& job_id) { m_path += "/" + job_id; }
};

class CancelDataProcessJobReq : public BucketReq {
  public:
    CancelDataProcessJobReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("PUT");
    SetPath("/jobs");
    SetHttps();
  }
  virtual ~CancelDataProcessJobReq() {}
  void SetJobId(const std::string& job_id) { m_path += "/" + job_id; }
};

}  // namespace qcloud_cos


