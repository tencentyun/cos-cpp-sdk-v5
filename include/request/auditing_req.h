#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "Poco/Dynamic/Struct.h"
#include "Poco/JSON/Object.h"
#include "request/bucket_req.h"
#include "request/object_req.h"

namespace qcloud_cos {

struct Location {
  Location() {}
  float x;                 // 该参数用于返回检测框左上角位置的横坐标（x）所在的像素位置，结合剩余参数可唯一确定检测框的大小和位置
  float y;                 // 该参数用于返回检测框左上角位置的纵坐标（y）所在的像素位置，结合剩余参数可唯一确定检测框的大小和位置
  float width;             // 该参数用于返回检测框的宽度（由左上角出发在 x 轴向右延伸的长度），结合剩余参数可唯一确定检测框的大小和位置
  float height;            // 该参数用于返回检测框的高度（由左上角出发在 y 轴向下延伸的长度），结合剩余参数可唯一确定检测框的大小和位置
  float rotate;            // 该参数用于返回检测框的旋转角度，该参数结合 X 和 Y 两个坐标参数可唯一确定检测框的具体位置；取值：0-360（角度制），方向为逆时针旋转
  std::string to_string() const {
    std::stringstream ss;
    ss << "x: " << std::to_string(x) << ", y: " << std::to_string(y)
       << ", width: " << std::to_string(width) << ", height: " << std::to_string(height)
       << ", rotate: " << std::to_string(rotate) << std::endl;
    return ss.str();
  }
};

struct OcrResult {
  OcrResult() {}
  std::string text;                   // 图片 OCR 文本识别出的具体文本内容
  std::vector<std::string> key_words; // 在当前审核场景下命中的关键词
  Location location;                  // 该参数用于返回 OCR 检测框在图片中的位置（左上角 xy 坐标、长宽、旋转角度），以方便快速定位识别文字的相关信息
  std::string to_string() const { 
    std::stringstream ss;
      ss << "text: " << text;
      for (int i = 0; i < key_words.size(); i++) {
        if (i == 0) {
          ss << "keywords: " << key_words[i];
          continue;
        } 
        ss << "," << key_word;
      }
      ss << ", location: " << location.to_string();
      return ss.str();
  }
};

struct ObjectResults {
  ObjectResults() {}
  std::string name;               // 识别出的实体名称
  Location location;              // 检测结果在图片中的位置信息
  std::string to_string() const { 
    std::stringstream ss;
      ss << "name: " << name
         << ", location: " << location.to_string();
      return ss.str();
  }
};

struct SceneResultInfo {
  SceneResultInfo() {}
  int code;                       // 单个审核场景的错误码，0为成功，其他为失败
  std::string msg;                // 具体错误信息，如正常为OK
  int hit_flag;                   // 是否命中该审核分类，0表示未命中，1表示命中，2表示疑似
  int score;                      // 该字段表示审核结果命中审核信息的置信度，取值范围：0（置信度最低）-100（置信度最高），越高代表该内容越有可能属于当前返回审核信息
                                  // 其中0 - 60分表示图片正常，61 - 90分表示图片疑似敏感，91 - 100分表示图片确定敏感
  std::string lable;              // 该字段表示该截图的综合结果标签（可能为 SubLabel，可能为人物名字等）
  std::string category;           // 该字段为Label的子集，表示审核命中的具体审核类别。例如 Sexy，表示色情标签中的性感类别
  std::string sub_lable;          // 该字段表示审核命中的具体子标签，例如：Porn 下的 SexBehavior 子标签。 可能为空，表示未命中字段
  int count;                      // 命中审核分类的截图数量
  OcrResult ocr_results;          // OCR 文本识别的详细检测结果，包括文本坐标信息、文本识别结果等信息，有相关违规内容时返回
  ObjectResults object_results;   // 识别出的实体详细检测结果，包括实体名和在图片中的详细位置
  std::string to_string() const {
      std::stringstream ss;
      ss << "code: " << std::to_string(code) << ", msg: " << msg
       << ", hit_flag: " << std::to_string(hit_flag) << ", score: " << std::to_string(score)
       << ", lable: " << lable << ", category: " << category
       << ", sub_lable: " << sub_lable << ", count: " << count
       << ", ocr_results: " << ocr_results.to_string()
       << ", object_results: " << object_results.to_string();
      return ss.str();
  }
};

struct UserInfo {
  UserInfo() {}
  std::string token_id;     // 用户业务 TokenId，长度不超过128字节
  std::string nick_name;    // 用户业务 Nickname，长度不超过128字节
  std::string device_id;    // 用户业务 DeviceId，长度不超过128字节
  std::string app_id;       // 用户业务 AppId，长度不超过128字节
  std::string room;         // 用户业务 room，长度不超过128字节
  std::string ip;           // 用户业务 IP，长度不超过128字节
  std::string type;         // 用户业务 type，长度不超过128字节
  std::string to_string() const {
      std::stringstream ss;
      ss << "token_id: " << token_id << ", nick_name: " << nick_name
       << ", device_id: " << device_id << ", app_id: " << app_id
       << ", room: " << room << ", ip: " << ip
       << ", type: " << type;
  }
};

struct SegmentResult {
  SegmentResult() {}
  std::string url;                  // 截图/音频片段的访问地址 
  int snap_shot_time;               // 截图位于视频中的时间，单位毫秒
  int offset_time;                  // 当前声音片段位于视频中的饿时间，单位毫秒
  int duration;                     // 当前声音片段的时长，单位毫秒
  std::string text;                 // 文本识别结果
  std::string lable;                // 检测结果中优先级最高的恶意标签
  int result;                       // 审核结果，0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  SceneResultInfo porn_info;        // 审核场景为涉黄的审核结果信息
  SceneResultInfo ads_info;         // 审核场景为广告引导的审核结果信息
  SceneResultInfo illegal_info;     // 审核场景为违法的审核结果信息
  SceneResultInfo abuse_info;       // 审核场景为谩骂的审核结果信息
  SceneResultInfo politics_info;    // 审核场景为违法的审核结果信息
  SceneResultInfo terrorism_info;   // 审核场景为谩骂的审核结果信息
  
  std::string to_string() const {
    // todo
    std::stringstream ss;
    return ss.str();
  }
}

struct Lables {
  Lables() {}
  SceneResultInfo porn_info;    // 审核场景为涉黄的审核结果信息
  SceneResultInfo ads_info;     // 审核场景为广告引导的审核结果信息
  SceneResultInfo illegal_info;     // 审核场景为违法的审核结果信息
  SceneResultInfo abuse_info;       // 审核场景为谩骂的审核结果信息
  SceneResultInfo politics_info;    // 审核场景为违法的审核结果信息
  SceneResultInfo terrorism_info;   // 审核场景为谩骂的审核结果信息
  std::string to_string() const {
    std::stringstream ss;
    ss << "pron_info: " << pron_info.to_string()
       << ", ads_info: " << ads_info.to_string();
    return ss.str();
  }
}

struct Result {
  Result() {}
  std::string url;              // 截图/音频片段的访问地址 
  int page_number;              // 截图位于视频中的时间，单位毫秒
  int sheet_number;             // 当前声音片段位于视频中的饿时间，单位毫秒
  std::string text;             // 文本识别结果
  std::string lable;            // 检测结果中优先级最高的恶意标签
  int suggestion;               // 审核结果，0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  SceneResultInfo porn_info;    // 审核场景为涉黄的审核结果信息
  SceneResultInfo ads_info;     // 审核场景为广告引导的审核结果信息
  SceneResultInfo illegal_info;     // 审核场景为违法的审核结果信息
  SceneResultInfo abuse_info;       // 审核场景为谩骂的审核结果信息
  SceneResultInfo politics_info;    // 审核场景为违法的审核结果信息
  SceneResultInfo terrorism_info;   // 审核场景为谩骂的审核结果信息
  std::string to_string() const {
    // todo
    std::stringstream ss;
    return ss.str();
  }
};

struct PageSegment {
  PageSegment() {}
  std::vector<Result> results;
  std::string to_string() const {
    std::stringstream ss;
      ss << "results: " << text;
      for (int i = 0; i < results.size(); i++) {
        if (i == 0) {
          ss << "results: " << results[i].to_string();
          continue;
        } 
        ss << ", " << key_word;
      }
    return ss.str();
  }
};


struct JobsDetail {
  JobsDetail() {}
  std::string code;                               // 错误码，只有 State 为 Failed 时有意义
  std::string message;                            // 错误描述，只有 State 为 Failed 时有意义
  std::string data_id;                            // 提交任务时设置了DataId参数时返回，返回原始内容，长度限制为512字节
  std::string job_id;                             // 新创建任务的 ID
  std::string state;                              // 任务的状态，为 Submitted、Auditing、Success、Failed其中一个
  std::string creation_time;                      // 任务的创建时间
  std::string object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  int compression_result;                         // 图片是否被压缩处理，值为 0（未压缩），1（正常压缩）
  std::string text;                               // 图片中检测的文字内容（OCR），当审核策略开启文本内容检测时返回
  std::string audio_text;                         // 音频文件中已识别的对应文本内容
  std::string lable;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int result;                                     // 检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  int suggestion;                                 // 文档审核的检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  int page_count;                                 // 文档审核会将文档转图片进行审核，该字段表示文档转换图片数
  std::string category;                           // Lable字段子集，表示审核命中的具体审核类别。例如 Sexy，表示色情标签中的性感类别
  std::string sub_lable;                          // 命中的二级标签结果
  SceneResultInfo porn_info;                      // 审核场景为涉黄的审核结果信息
  SceneResultInfo ads_info;                       // 审核场景为广告引导的审核结果信息
  SceneResultInfo illegal_info;                   // 审核场景为违法的审核结果信息
  SceneResultInfo abuse_info;                     // 审核场景为谩骂的审核结果信息
  SceneResultInfo politics_info;                  // 审核场景为违法的审核结果信息
  SceneResultInfo terrorism_info;                 // 审核场景为谩骂的审核结果信息
  int section_count;                              // 审核文本的分片数，默认为1
  std::string snap_shot_count;                    // 视频截图总数量
  std::vector<SegmentResult> snap_shot;           // 视频画面截图的审核结果
  std::vector<SegmentResult> audio_section;       // 视频中声音的审核结果
  std::vector<SegmentResult> section;             // 音频（文本）审核中对音频（文本）片段的审核结果
  UserInfo user_info;                             // 用户业务字段。创建任务时未设置UserInfo则无此字段
  std::string to_string() const {
    // todo
    std::stringstream ss;
    return ss.str();
  }
};

struct ImageAuditingJobsDetail {
  ImageAuditingJobsDetail() {}
  std::string code;                               // 错误码，只有 State 为 Failed 时有意义
  std::string message;                            // 错误描述，只有 State 为 Failed 时有意义
  std::string data_id;                            // 提交任务时设置了DataId参数时返回，返回原始内容，长度限制为512字节
  std::string job_id;                             // 新创建任务的 ID
  std::string state;                              // 任务的状态，为 Submitted、Auditing、Success、Failed其中一个
  std::string creation_time;                      // 任务的创建时间
  std::string object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  int compression_result;                         // 图片是否被压缩处理，值为 0（未压缩），1（正常压缩）
  std::string text;                               // 图片中检测的文字内容（OCR），当审核策略开启文本内容检测时返回
  std::string lable;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int result;                                     // 检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  std::string category;                           // Lable字段子集，表示审核命中的具体审核类别。例如 Sexy，表示色情标签中的性感类别
  std::string sub_lable;                          // 命中的二级标签结果
  SceneResultInfo porn_info;                      // 审核场景为涉黄的审核结果信息
  SceneResultInfo ads_info;                       // 审核场景为广告引导的审核结果信息
  SceneResultInfo illegal_info;                   // 审核场景为违法的审核结果信息
  SceneResultInfo abuse_info;                     // 审核场景为谩骂的审核结果信息
  SceneResultInfo politics_info;                  // 审核场景为违法的审核结果信息
  SceneResultInfo terrorism_info;                 // 审核场景为谩骂的审核结果信息
  UserInfo user_info;                             // 用户业务字段。创建任务时未设置UserInfo则无此字段
  std::string to_string() const {
    // todo
    std::stringstream ss;
    return ss.str();
  }
}

struct VideoAuditingJobsDetail {
  VideoAuditingJobsDetail() {}
  std::string code;                               // 错误码，只有 State 为 Failed 时有意义
  std::string message;                            // 错误描述，只有 State 为 Failed 时有意义
  std::string data_id;                            // 提交任务时设置了DataId参数时返回，返回原始内容，长度限制为512字节
  std::string job_id;                             // 新创建任务的 ID
  std::string state;                              // 任务的状态，为 Submitted、Auditing、Success、Failed其中一个
  std::string creation_time;                      // 任务的创建时间
  std::string object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  std::string snap_shot_count;                    // 视频截图总数量
  std::string lable;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int result;                                     // 检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  SceneResultInfo porn_info;                      // 审核场景为涉黄的审核结果信息
  SceneResultInfo ads_info;                       // 审核场景为广告引导的审核结果信息
  SceneResultInfo illegal_info;                   // 审核场景为违法的审核结果信息
  SceneResultInfo abuse_info;                     // 审核场景为谩骂的审核结果信息
  SceneResultInfo politics_info;                  // 审核场景为违法的审核结果信息
  SceneResultInfo terrorism_info;                 // 审核场景为谩骂的审核结果信息  
  std::vector<SegmentResult> snap_shot;           // 视频画面截图的审核结果
  std::vector<SegmentResult> audio_section;       // 视频中声音的审核结果
  UserInfo user_info;                             // 用户业务字段。创建任务时未设置UserInfo则无此字段
  std::string to_string() const {
    // todo
    std::stringstream ss;
    return ss.str();
  }
};

struct AudioAuditingJobsDetail {
  AudioAuditingJobsDetail() {}
  std::string code;                               // 错误码，只有 State 为 Failed 时有意义
  std::string message;                            // 错误描述，只有 State 为 Failed 时有意义
  std::string data_id;                            // 提交任务时设置了DataId参数时返回，返回原始内容，长度限制为512字节
  std::string job_id;                             // 新创建任务的 ID
  std::string state;                              // 任务的状态，为 Submitted、Auditing、Success、Failed其中一个
  std::string creation_time;                      // 任务的创建时间
  std::string object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  std::string lable;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int result;                                     // 检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  std::string audio_text;                         // 音频文件中已识别的对应文本内容
  SceneResultInfo porn_info;                      // 审核场景为涉黄的审核结果信息
  SceneResultInfo ads_info;                       // 审核场景为广告引导的审核结果信息
  SceneResultInfo illegal_info;                   // 审核场景为违法的审核结果信息
  SceneResultInfo abuse_info;                     // 审核场景为谩骂的审核结果信息
  SceneResultInfo politics_info;                  // 审核场景为违法的审核结果信息
  SceneResultInfo terrorism_info;                 // 审核场景为谩骂的审核结果信息  
  std::vector<SegmentResult> section;             // 音频（文本）审核中对音频（文本）片段的审核结果
  UserInfo user_info;                             // 用户业务字段。创建任务时未设置UserInfo则无此字段
  std::string to_string() const {
    // todo
    std::stringstream ss;
    return ss.str();
  }
};

struct TextAuditingJobsDetail {
  TextAuditingJobsDetail() {}
  std::string code;                               // 错误码，只有 State 为 Failed 时有意义
  std::string message;                            // 错误描述，只有 State 为 Failed 时有意义
  std::string data_id;                            // 提交任务时设置了DataId参数时返回，返回原始内容，长度限制为512字节
  std::string job_id;                             // 新创建任务的 ID
  std::string state;                              // 任务的状态，为 Submitted、Auditing、Success、Failed其中一个
  std::string creation_time;                      // 任务的创建时间
  std::string object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string content;                            // 文本内容的Base64编码，创建任务使用Content时返回 
  int section_count;                              // 审核文本的分片数，默认为1
  std::string lable;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int result;                                     // 检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  SceneResultInfo porn_info;                      // 审核场景为涉黄的审核结果信息
  SceneResultInfo ads_info;                       // 审核场景为广告引导的审核结果信息
  SceneResultInfo illegal_info;                   // 审核场景为违法的审核结果信息
  SceneResultInfo abuse_info;                     // 审核场景为谩骂的审核结果信息
  SceneResultInfo politics_info;                  // 审核场景为违法的审核结果信息
  SceneResultInfo terrorism_info;                 // 审核场景为谩骂的审核结果信息
  std::vector<SegmentResult> section;             // 音频（文本）审核中对音频（文本）片段的审核结果
  UserInfo user_info;                             // 用户业务字段。创建任务时未设置UserInfo则无此字段
  std::string to_string() const {
    // todo
    std::stringstream ss;
    return ss.str();
  }
};

struct DocumentAuditingJobsDetail {
  DocumentAuditingJobsDetail() {}
  std::string code;                               // 错误码，只有 State 为 Failed 时有意义
  std::string message;                            // 错误描述，只有 State 为 Failed 时有意义
  std::string data_id;                            // 提交任务时设置了DataId参数时返回，返回原始内容，长度限制为512字节
  std::string job_id;                             // 新创建任务的 ID
  std::string state;                              // 任务的状态，为 Submitted、Auditing、Success、Failed其中一个
  std::string creation_time;                      // 任务的创建时间
  std::string object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  std::string lable;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int suggestion;                                 // 文档审核的检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  int page_count;                                 // 文档审核会将文档转图片进行审核，该字段表示文档转换图片数
  Lables lables;                                  // 命中的审核场景及对应的结果
  UserInfo user_info;                             // 用户业务字段。创建任务时未设置UserInfo则无此字段
  PageSegment page_segment;                       // 文档转换为图片后，具体每张图片的审核结果信息
  std::string to_string() const {
    // todo
    std::stringstream ss;
    return ss.str();
  }
};

struct WebPageAuditingJobsDetail {
  WebPageAuditingJobsDetail() {}
  std::string code;                               // 错误码，只有 State 为 Failed 时有意义
  std::string message;                            // 错误描述，只有 State 为 Failed 时有意义
  std::string data_id;                            // 提交任务时设置了DataId参数时返回，返回原始内容，长度限制为512字节
  std::string job_id;                             // 新创建任务的 ID
  std::string state;                              // 任务的状态，为 Submitted、Auditing、Success、Failed其中一个
  std::string creation_time;                      // 任务的创建时间
  std::string url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  std::string lable;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int suggestion;                                 // 网页审核的检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  int page_count;                                 // 网页审核会将网页中的图片链接和文本分开送审，该字段表示送审的链接和文本总数量。
  Result image_results;                           // 网页内图片的审核结果
  Result text_results;                            // 网页内文本的审核结果
  std::string high_light_html;                    // 对违规关键字高亮处理的 Html 网页内容，请求内容指定 ReturnHighlightHtml 时返回
  Lables lables;                                  // 命中的审核场景及对应的结果
  UserInfo user_info;                             // 用户业务字段。创建任务时未设置UserInfo则无此字段
  std::string to_string() const {
    // todo
    std::stringstream ss;
    return ss.str();
  }
};


class Input {
 public:
  Input() : m_mask(0x00000000u) {}
  
  void SetObject(const std::string& object) { 
    m_mask |= 0x00000001u;
    m_object = object; 
  }

  void SetUrl(const std::string& url) { 
    m_mask |= 0x00000002u;
    m_url = url; 
  }

  void SetInterval(int interval) { 
    m_mask |= 0x00000004u;
    m_interval = interval; 
  }

  void SetMaxFrames(int max_frames) { 
    m_mask |= 0x00000008u;
    m_max_frames = max_frames; 
  }

  void SetDataId(const std::string& data_id) {
    m_mask |= 0x00000010u;
    m_data_id = data_id; 
  }

  void SetLargeImageDetect(int large_image_detect) { 
    m_mask |= 0x00000020u;
    m_large_image_detect = large_image_detect; 
  }

  void SetUserInfo(const std::string& user_info) { 
    m_mask |= 0x00000040u;
    m_user_info = user_info; 
  }

  void SetContent(const std::string& content) { 
    m_mask |= 0x00000080u;
    m_content = content; 
  }

  void SetContent(const std::string& type) { 
    m_mask |= 0x00000100u;
    m_type = type; 
  }
  
  std::string GetObject() const { return m_object; }

  std::string GetUrl() const { return m_url; }

  int GetInterval() const { return m_interval; }

  int GetMaxFrames() const { return m_max_frames; }

  int GetLargeImageDetect() const { return m_large_image_detect; }

  std::string GetDataId() const { return m_data_id; }

  UserInfo GetUserInfo() const { return m_user_info; }

  std::string GetContent() const { return m_content; }

  std::string GetType() const { return m_type; }

  bool HasObject() const { return (m_mask & 0x00000001u) != 0; }

  bool HasUrl() const { return (m_mask & 0x00000002u) != 0; }

  bool HasInterval() const { return (m_mask & 0x00000004) != 0; }

  bool HasMaxFrames() const { return (m_mask & 0x00000008u) != 0;}

  bool HasDataId() const { return (m_mask & 0x00000010u) != 0; }

  bool HasLargeImageDetect() const { return (m_mask & 0x00000020u) != 0; }

  bool HasUserInfo() const { return (m_mask & 0x00000040u) != 0; }

  bool HasContent() const { return (m_mask & 0x00000080u) != 0; }

  bool HasType() const { return (m_mask & 0x00000100u) != 0; }


 private:
  uint64_t m_mask;
  std::string m_object;            // 存储在 COS 存储桶中的文件名称
  std::string m_url;               // 文件的链接地址， object 和 url 只能选择其中其中
  int m_interval;                  // 截帧频率，GIF专用，默认为5，表示从第1帧（包含）开始间隔5帧截取一帧
  int m_max_frames;                // 最大截帧数量，GIF 图检测专用，默认值为5，表示只截取 GIF 的5帧图片进行审核，必须大于0
  std::string m_data_id;           // 图片标识，该字段在结果中返回原始内容，长度限制为512字节
  int m_large_image_detect;        // 对于超过大小限制的图片是否进行压缩后再审核，取值为： 0（不压缩），1（压缩）。默认为0
  UserInfo m_user_info;            // 用户业务字段
  std::string m_content;           // 文本审核时传入的纯文本信息
  std::string m_type;              // 文档审核的文档文件类型

};

struct SnapShotConf {
  SnapShotConf() {}
  std::string mode;                // 截帧模式, 默认为Intrval，取值为Interval、Average、Fps，
                                   // 详见https://cloud.tencent.com/document/product/436/47316
  int count;                       // 视频截帧数量
  float time_interval;             // 视频截帧频率
  std::string to_string() const { 
    std::stringstream ss; 
    ss << "mode: " << mode << ", count: " << std::to_string(count)
       << ", time_interval: " << std::to_string(time_interval);
    return ss.str();
  }
};

class Conf {
 public:
  Conf() : m_mask(0x00000000u) {}

  void SetBizType(const std::string& biz_type) { 
    m_mask |= 0x00000001u;
    m_biz_type = biz_type; 
  }

  void SetDetectType(const std::string& detect_type) {
    m_mask |= 0x00000002u;
    m_biz_type = detect_type;
  }

  void SetSnapShot(const SnapShotConf snap_shot) {
    m_mask |= 0x00000004u;
    m_snap_shot = snap_shot;
  }

  void SetCallBack(const std::string callback) {
    m_mask = m_mask | 0x00000008u;
    m_callback = callback;
  }

  void SetCallBackVersion(const std::string callbcak_version) {
    m_mask = m_mask | 0x00000010u;
    m_callback_version = callbcak_version;
  }

  void SetDetectContent(const std::string detect_content) {
    m_mask = m_mask | 0x00000020u;
    m_detect_content = detect_content;
  }

  void ReturnHighlightHtml(const std::string return_hightlight_html) {
    m_mask = m_mask | 0x00000040u;
    m_return_highlight_html = return_hightlight_html;
  }	

  std::string GetBizType() const { return m_biz_type;}

  std::string GetDetectType() const { return m_biz_type; }

  SnapShotConf GetSnapShot() const { return m_snapshot; }

  std::string GetCallBack() const { return m_callback; }

  std::string GetCallBackVersion() const { return m_callback_version; }

  std::string GetDetectContent() const { return m_detect_content; }

  bool HasBizType() const { return (m_mask & 0x00000001u) != 0; }

  bool HasDetectType() const { return (m_mask & 0x00000002u) != 0;}

  bool HasSnapShot() const { return (m_mask & 0x00000004u) != 0; }

  bool HasCallback() const { return (m_mask & 0x00000008u) != 0; }

  bool HasCallbackVersion() const { return (m_mask & 0x00000010u) != 0; }

  bool HasDetectContent() const { return (m_mask & 0x00000020u) != 0; }


  std::string to_string() const {
    std::stringstream ss;
    ss << "biz_type: " << biz_type << ", detect_type: " << detect_type
       << ", snap_shot: " << m_snap_shot.to_string() 
       << ", callback: " << m_callback
       << ", callbcak_version: " << m_callback_version
       << ", detect_content: " << m_detect_content;
    return ss.str();
  }

 private:
  uint64_t m_mask;
  std::string m_biz_type;           // 审核策略的唯一标识
  std::string m_detect_type;        // 审核的场景类型
  SnapShotConf m_snap_shot;         // 审核视频截帧配置
  std::string m_callback;           // 回调规则
  std::string m_callback_version;   // 回调内容结构，有效值为Simple、Detail，默认Simple
  int m_detect_content;             // 指定是否审核视频声音，0 指审核视频截图，1 审核视频截图和声音，默认为0
  bool m_return_highlight_html;     // 网页审核时，是否需要高亮网页耶的违规文本
};

class GetImageAuditingReq : public ObjectReq {
 public:
  GetImageAuditingReq(const std::string& bucket_name)
      : ObjectReq(bucket_name, "") {
    AddParam("ci-process", "sensitive-content-recognition");
  }

  virtual ~GetImageAuditingReq() {}

  // COS 存储桶中的图片文件名称，需要审核图片文件对象
  void SetObjectKey(const std::string& object_name) { 
    SetObjectName(object_name); 
  }

  // 审核策略biz-type，不设置自动使用默认审核策略
  void SetBizType(const std::string& biz_type) { 
    AddParam("biz-type", biz_type); 
  }

  // 审核场景类型，有效值：Porn（涉黄）、Ads（广告）等，可以传入多种类型，不同类型以逗号分隔
  void SetDetectType(const std::string& detect_type) { 
    AddParam("detect-type", detect_type); 
  }

  // 审核图片Url，可为任意公网可访问图片链接
  // 设置了detect-url时，默认审核detect-url，无需填写ObjectKey
  // 不设置detect-url时，默认审核ObjectKey
  void SetDetectUrl(const std::string& detect_url) { 
    AddParam("detect-url", detect_url); 
  }

  // 审核GIF动图时，使用该参数截帧审核
  // 例如值设为5，则表示从第1帧开始截取，每隔5帧截取一帧，默认值5
  void SetInterval(int interval) { 
    AddParam("interval", std::to_string(interval)); 
  }

  // 审核动图时最大截帧数量，默认为5
  void SetMaxFrames(int max_frames) { 
    AddParam("max-frames", std::to_string(max_frames)); 
  }

  // 对于超过大小限制的图片是否进行压缩后再审核，取值为： 0（不压缩），1（压缩）。默认为0。
  void SetLargeImageDetect(int large_image_detect) { 
    AddParam("large-image-detect", std::to_string(large_image_detect)); 
  }

  // 图片标识，该字段返回原始内容，长度限制为512字节
  void SetDataId(const std::string& data_id) { 
    AddParam("dataid", data_id); 
  }
};

class BatchImageAuditingReq : public BucketReq {
 public:
  BatchImageAuditingReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("POST");
    SetPath("/image/audting");
    SetHttps();
    AddHeader("Content-Type", "application/xml");
  }

  virtual ~BatchImageAuditingReq() {}

  void SetConf(const Conf& conf) { m_conf = conf; }
  void SetBizType(const std::string& biz_type) { m_conf.SetBizType(biz_type); }
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void AddInput(const Input& input) { m_inputs.push_back(input); }
  void SetInputs(const std::vector<Input>& inputs) { m_inputs = inputs; }
  bool GenerateRequestBody(std::string* body) const;

 private:
  std::vector<Input> m_inputs; // 需要审核的内容，传入多个Input则审核多个内容
  Conf m_conf;  // 审核配置规则配置
};

class DescribeAuditingJobReq : public BucketReq {
 public:
  DescribeAuditingJobReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("GET");
    SetHttps();
  }
  virtual ~DescribeAuditingJobReq() {}

  void SetJobId(const std::string& job_id) { m_path += "/" + job_id; }
};

class DescribeImageAuditingJobReq : public DescribeAuditingJobReq {
 public:
  DescribeImageAuditingJobReq(const std::string& bucket_name)
      : DescribeAuditingJobReq(bucket_name) {
    SetPath("/image/auditing");
  } 
  virtual ~DescribeImageAuditingJobReq() {}
};

class CreateAudtingJobReq : public BucketReq {
 public:
  CreateAudtingJobReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("POST");
    SetHttps();
    AddHeader("Content-Type", "application/xml");
  }
  virtual ~CreateVideoAudtingJobReq() {}
  
  void SetInput(const Input& input) { m_input = input; }
  void SetConf(const Conf& conf) { m_conf = conf; }
  bool GenerateRequestBody(std::string* body) const;
 
 protected:
  Input m_input;       // 需要审核的内容
  Conf m_conf;         // 审核配置规则
};

class CreateVideoAudtingJobReq : public CreateAudtingJobReq {
 public:
  CreateVideoAudtingJobReq(const std::string& bucket_name)
      : CreateAudtingJobReq(bucket_name) {
    SetPath("/video/audting");
  }
  virtual ~CreateVideoAudtingJobReq() {}
  
  void SetBizType(const std::string& biz_type) { m_conf.SetBizType(biz_type); }
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void SetSnapShot(const SnapShotConf& snap_shot) { m_conf.SetSnapShot(snap_shot); }
  void SetCallBack(const std::string& callback) { m_conf.SetCallBack(callback); }
  void SetCallBackVersion(const std::string& callback_version) { m_conf.SetCallBackVersion(callback_version); }

  void SetObject(const std::string& object) { m_input.SetObject(object); }
  void SetUrl(const std::string& url) { m_input.SetUrl(url); } 
  void SetDataid(const std::string& data_id) { m_input.SetDataid(data_id); }
  void SetUserInfo(const UserInfo& user_info) { m_input.SetUserInfo(user_info); }
};

class DescribeVideoAuditingJobReq : public DescribeAuditingJobReq {
 public:
  DescribeVideoAuditingJobReq(const std::string& bucket_name)
      : DescribeAuditingJobReq(bucket_name) {
    SetPath("/video/auditing");
  } 
  virtual ~DescribeVideoAuditingJobReq() {}
};

class CreateAudioAudtingJobReq : public CreateAudtingJobReq {
 public:
  CreateAudioAudtingJobReq(const std::string& bucket_name)
      : CreateAudtingJobReq(bucket_name) {
    SetPath("/audio/audting");
  }
  virtual ~CreateAudioAudtingJobReq() {}
  
  void SetBizType(const std::string& biz_type) { m_conf.SetBizType(biz_type); }
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void SetCallBack(const std::string& callback) { m_conf.SetCallBack(callback); }
  void SetCallBackVersion(const std::string& callback_version) { m_conf.SetCallBackVersion(callback_version); }

  void SetObject(const std::string& object) { m_input.SetObject(object); }
  void SetUrl(const std::string& url) { m_input.SetUrl(url); } 
  void SetDataid(const std::string& data_id) { m_input.SetDataid(data_id); }
  void SetUserInfo(const UserInfo& user_info) { m_input.SetUserInfo(user_info); }
};

class DescribeAudioAuditingJobReq : public DescribeAuditingJobReq {
 public:
  DescribeAudioAuditingJobReq(const std::string& bucket_name)
      : DescribeAuditingJobReq(bucket_name) {
    SetPath("/audio/auditing");
  } 
  virtual ~DescribeAudioAuditingJobReq() {}
};

class CreateTextAudtingJobReq : public CreateAudtingJobReq {
 public:
  CreateTextAudtingJobReq(const std::string& bucket_name)
      : CreateAudtingJobReq(bucket_name) {
    SetPath("/text/audting");
  }
  virtual ~CreateTextAudtingJobReq() {}
  
  void SetBizType(const std::string& biz_type) { m_conf.SetBizType(biz_type); }
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void SetCallBack(const std::string& callback) { m_conf.SetCallBack(callback); }
  void SetCallBackVersion(const std::string& callback_version) { m_conf.SetCallBackVersion(callback_version); }

  void SetObject(const std::string& object) { m_input.SetObject(object); }
  void SetUrl(const std::string& url) { m_input.SetUrl(url); } 
  void SetDataid(const std::string& data_id) { m_input.SetDataid(data_id); }
  void SetContenct(const std::string& content) { m_input.SetContenct(content); }
  void SetUserInfo(const UserInfo& user_info) { m_input.SetUserInfo(user_info); }
};

class DescribeTextAuditingJobReq : public DescribeAuditingJobReq {
 public:
  DescribeTextAuditingJobReq(const std::string& bucket_name)
      : DescribeAuditingJobReq(bucket_name) {
    SetPath("/text/auditing");
  } 
  virtual ~DescribeTextAuditingJobReq() {}
};

class CreateDocumentAudtingJobReq : public CreateAudtingJobReq {
 public:
  CreateDocumentAudtingJobReq(const std::string& bucket_name)
      : CreateAudtingJobReq(bucket_name) {
    SetPath("/document/audting");
  }
  virtual ~CreateDocumentAudtingJobReq() {}
  
  void SetBizType(const std::string& biz_type) { m_conf.SetBizType(biz_type); }
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void SetCallBack(const std::string& callback) { m_conf.SetCallBack(callback); }

  void SetObject(const std::string& object) { m_input.SetObject(object); }
  void SetUrl(const std::string& url) { m_input.SetUrl(url); } 
  void SetDataid(const std::string& data_id) { m_input.SetDataid(data_id); }
  void SetUserInfo(const UserInfo& user_info) { m_input.SetUserInfo(user_info); }
  void SetType(const std::string& type) { m_input.SetType(type); };
};

class DescribeDocumentAuditingJobReq : public DescribeAuditingJobReq {
 public:
  DescribeDocumentAuditingJobReq(const std::string& bucket_name)
      : DescribeAuditingJobReq(bucket_name) {
    SetPath("/document/auditing");
  } 
  virtual ~DescribeDocumentAuditingJobReq() {}
};

class DescribeTextAuditingJobReq : public DescribeAuditingJobReq {
 public:
  DescribeTextAuditingJobReq(const std::string& bucket_name)
      : DescribeAuditingJobReq(bucket_name) {
    SetPath("/text/auditing");
  } 
  virtual ~DescribeTextAuditingJobReq() {}
};

class CreateWebPageAudtingJobReq : public CreateAudtingJobReq {
 public:
  CreateWebPageAudtingJobReq(const std::string& bucket_name)
      : CreateAudtingJobReq(bucket_name) {
    SetPath("/webpage/audting");
  }
  virtual ~CreateWebPageAudtingJobReq() {}
  
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void SetCallBack(const std::string& callback) { m_conf.SetCallBack(callback); }

  void SetObject(const std::string& object) { m_input.SetObject(object); }
  void SetDataid(const std::string& data_id) { m_input.SetDataid(data_id); }
  void SetUserInfo(const UserInfo& user_info) { m_input.SetUserInfo(user_info); }
};

class DescribeWebPageAuditingJobReq : public DescribeAuditingJobReq {
 public:
  DescribeWebPageAuditingJobReq(const std::string& bucket_name)
      : DescribeAuditingJobReq(bucket_name) {
    SetPath("/webpage/auditing");
  } 
  virtual ~DescribeWebPageAuditingJobReq() {}
};

}

