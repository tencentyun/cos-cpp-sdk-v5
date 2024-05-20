#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "Poco/Dynamic/Struct.h"
#include "Poco/JSON/Object.h"
#include "request/bucket_req.h"
#include "request/object_req.h"

namespace qcloud_cos {

class Location {
 public:
  Location() : m_mask(0x00000000u) {}
  virtual ~Location() {}

  void SetX(const float x) {
    m_mask |= 0x00000001u;
    m_x = x;
  }

  void SetY(const float y) {
    m_mask |= 0x00000002u;
    m_y = y;
  }

  void SetWidth(const float width) {
    m_mask |= 0x00000004u;
    m_width = width;
  }

  void SetHeight(const float height) {
    m_mask |= 0x00000008u;
    m_height = height;
  }

  void SetRotate(const float rotate) {
    m_mask |= 0x00000010u;
    m_rotate = rotate;
  }

  float GetX() const { return m_x; }

  float GetY() const { return m_y; }

  float GetWidth() const { return m_width; }

  float GetHeight() const { return m_height; }

  float GetRotate() const { return m_rotate; }

  bool HasX() const { return (m_mask & 0x00000001u) != 0; }

  bool HasY() const { return (m_mask & 0x00000002u) != 0; }

  bool HasWidth() const { return (m_mask & 0x00000004u) != 0; }

  bool HasHeight() const { return (m_mask & 0x00000008u) != 0; }

  bool HasRotate() const { return (m_mask & 0x00000010u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    if (HasX()) { ss << " x: " << std::to_string(m_x); }
    if (HasY()) { ss << " y: " << std::to_string(m_y); }
    if (HasWidth()) { ss << " width: " << std::to_string(m_width); }
    if (HasHeight()) { ss << " height: " << std::to_string(m_height); }
    if (HasRotate()) { ss << " rotate: " << std::to_string(m_rotate); }
    return ss.str();
  }

 private:
  uint64_t m_mask;
  float m_x;                 // 该参数用于返回检测框左上角位置的横坐标（x）所在的像素位置，结合剩余参数可唯一确定检测框的大小和位置
  float m_y;                 // 该参数用于返回检测框左上角位置的纵坐标（y）所在的像素位置，结合剩余参数可唯一确定检测框的大小和位置
  float m_width;             // 该参数用于返回检测框的宽度（由左上角出发在 x 轴向右延伸的长度），结合剩余参数可唯一确定检测框的大小和位置
  float m_height;            // 该参数用于返回检测框的高度（由左上角出发在 y 轴向下延伸的长度），结合剩余参数可唯一确定检测框的大小和位置
  float m_rotate;            // 该参数用于返回检测框的旋转角度，该参数结合 X 和 Y 两个坐标参数可唯一确定检测框的具体位置；取值：0-360（角度制），方向为逆时针旋转

};

class OcrResult {
 public:
  OcrResult() : m_mask(0x00000000u) {}
  virtual ~OcrResult() {}

  void SetText(const std::string& text) {
    m_mask |= 0x00000001u;
    m_text = text;
  }

  void SetKeyWords(const std::vector<std::string>& key_words) {
    m_mask |= 0x00000002u;
    m_key_words = key_words;
  }

  void AddKeyWord(const std::string& key_word) {
    m_mask |= 0x00000002u;
    m_key_words.push_back(key_word);
  }

  void SetLocation(const Location& location) {
    m_mask |= 0x00000004u;
    m_location = location;
  }

  std::string GetText() const { return m_text; }

  std::vector<std::string> GetKeyWords() const { return m_key_words; }

  Location GetLocation() const { return m_location; }

  bool HasText() const { return (m_mask & 0x00000001u) != 0; }

  bool HasKeyWords() const { return (m_mask & 0x00000002u) != 0; }

  bool HasLocation() const { return (m_mask & 0x00000004u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    if (HasText()) {
      ss << " text: " << m_text;
    }
    if (HasKeyWords()) {
      for (std::vector<std::string>::const_iterator it = m_key_words.begin();
           it != m_key_words.end(); ++it) {
        if (it == m_key_words.begin()) {
          ss << " keywords: " << it->c_str();
          continue;
        }
        ss << "," << it->c_str();
      }
    }
    if (HasLocation()) {
      ss << " Location: " << m_location.to_string();
    }
    return ss.str();
  }

 private:
  uint64_t m_mask;
  std::string m_text;                   // 图片 OCR 文本识别出的具体文本内容
  std::vector<std::string> m_key_words; // 在当前审核场景下命中的关键词
  Location m_location;                  // 该参数用于返回 OCR 检测框在图片中的位置（左上角 xy 坐标、长宽、旋转角度），以方便快速定位识别文字的相关信息

};

class ObjectResults {
 public:
  ObjectResults() : m_mask(0x00000000u) {}
  virtual ~ObjectResults() {}

  void SetName(const std::string& name) {
    m_mask |= 0x00000001u;
    m_name = name;
  }

  void SetLocation(const Location& location) {
    m_mask |= 0x00000002u;
    m_location = location;
  }

  std::string GetName() const { return m_name; }

  Location GetLocation() const { return m_location; }

  bool HasName() const { return (m_mask & 0x00000001u) != 0; }
   
  bool HasLocation() const { return (m_mask & 0x00000002u) != 0; }

  std::string to_string() const { 
    std::stringstream ss;
    if (HasName()) {
      ss << " name: " << m_name;
    }
    if (HasLocation()) {
      ss << " location: " << m_location.to_string();
    }
    return ss.str();
  }
 private:
  uint64_t m_mask;
  std::string m_name;               // 识别出的实体名称
  Location m_location;              // 检测结果在图片中的位置信息
};

class RecognitionResult {
  public:
    RecognitionResult() : m_mask(0x00000000u) {}
    virtual ~RecognitionResult() {}    

    void SetLable(const std::string& label) {
      m_mask |= 0x00000001u;
      m_Label = label;
    }

    void SetScore(const int score) {
      m_mask |= 0x00000002u;
      m_score = score;
    }

    void SetStartTime(const int start_time) {
      m_mask |= 0x00000004u;
      m_start_time = start_time;
    }

    void SetEndTime(const int end_time) {
      m_mask |= 0x00000008u;
      m_end_time = end_time;
    }

    std::string GetLabel() const { return m_Label; }
    
    int GetScore() const { return m_score; }

    int GetStartTime() const { return m_start_time; }

    int GetEndTime() const { return m_end_time; }

    bool HasLabel() const { return (m_mask & 0x00000001u) != 0; }

    bool HasScore() const { return (m_mask & 0x00000002u) != 0; }

    bool HasStartTime() const { return (m_mask & 0x00000004u) != 0; }

    bool HasEndTime() const { return (m_mask & 0x00000008u) != 0; }

    std::string to_string() const { 
      std::stringstream ss;
      if (HasLabel()) { ss << " label: " << m_Label; }
      if (HasScore()) { ss << " score: " << m_score; }
      if (HasStartTime()) { ss << " start_time: " << m_start_time; }
      if (HasEndTime()) { ss << " end_time: " << m_end_time; }
      return ss.str();
    }
 private:
  uint64_t m_mask;    
  std::string m_Label;                  // 该字段表示对应的识别结果类型信息
  int m_score;                          // 该字段表示审核结果命中审核信息的置信度，取值范围：0（置信度最低）-100（置信度最高），越高代表音频越有可能属于当前返回的标签
  int m_start_time;                     // 该字段表示对应标签的片段在音频文件内的开始时间，单位为毫秒。注意：此字段可能未返回，表示取不到有效值
  int m_end_time;                       // 该字段表示对应标签的片段在音频文件内的结束时间，单位为毫秒。注意：此字段可能未返回，表示取不到有效值
};

class LibResults {
  public:
    LibResults() : m_mask(0x00000000u), m_key_words(std::vector<std::string>()) {}
    virtual ~LibResults() {}

  void SetLibType(const int lib_type) { 
    m_mask |= 0x00000001u;
    m_lib_type = lib_type; 
  }

  void SetLibName(const std::string& lib_name) {
    m_mask |= 0x00000002u;
    m_lib_name = lib_name;
  }

  void SetKeyWords(const std::vector<std::string>& key_words) {
    m_mask |= 0x00000004u;
    m_key_words = key_words;
  }

  void AddKeyWord(const std::string& keyword) {
    m_mask |= 0x00000004u;
    m_key_words.push_back(keyword);
  }

  int GetLibType() const { return m_lib_type; }

  std::string GetLibName() const { return m_lib_name; }

  std::vector<std::string> GetKeyWords() const { return m_key_words; }

  bool HasLibType() const { return (m_mask & 0x00000001u) != 0; }

  bool HasLibName() const { return (m_mask & 0x00000001u) != 0; }

  bool HasKeyWords() const { return (m_mask & 0x00000001u) != 0; }

  std::string to_string() const { 
    std::stringstream ss;
    if (HasLibType()) {
      ss << " lib_type: " << m_lib_type;
    }
    if (HasLibName()) {
      ss << " lib_name: " << m_lib_name;
    }
    if (HasKeyWords()) {
      for (std::vector<std::string>::const_iterator it = m_key_words.begin();
           it != m_key_words.end(); ++it) {
        if (it == m_key_words.begin()) {
          ss << " keywords: " << it->c_str();
          continue;
        }
        ss << "," << it->c_str();
      }
    }    
    return ss.str();
  }

 private:
  uint64_t m_mask;    
  int m_lib_type;                       // 命中的风险库类型，取值为1（预设黑白库）和2（自定义风险库）
  std::string m_lib_name;               // 命中的风险库名称
  std::vector<std::string> m_key_words; // 命中的库中关键词
};

class SceneResultInfo {
 public:
  SceneResultInfo() : m_mask(0x00000000u) {}
  virtual ~SceneResultInfo() {}

  void SetCode(const int code) {
    m_mask |= 0x00000001u;
    m_code = code;
  }

  void SetMsg(const std::string &msg) {
    m_mask |= 0x00000002u;
    m_msg = msg;
  }

  void SetHitFlag(const int hit_flag) {
    m_mask |= 0x00000004u;
    m_hit_flag = hit_flag;
  }

  void SetScore(const int score) {
    m_mask |= 0x00000008u;
    m_score = score;
  }

  void SetLabel(const std::string& label) {
    m_mask |= 0x00000010u;
    m_label = label;
  }

  void SetCategory(const std::string& category) {
    m_mask |= 0x00000020u;
    m_category = category;
  }

  void SetSubLabel(const std::string& sub_label) {
    m_mask |= 0x00000040u;
    m_sub_label = sub_label;
  }

  void SetCount(const int count) {
    m_mask |= 0x00000080u;
    m_count = count;
  }

  void SetOcrResults(const std::vector<OcrResult>& ocr_results) {
    m_mask |= 0x00000100u;
    m_ocr_results = ocr_results;
  }

  void AddOcrResults(const OcrResult& ocr_results) {
    m_mask |= 0x00000100u;
    m_ocr_results.push_back(ocr_results);
  }

  void SetObjectResults(const ObjectResults& object_results) {
    m_mask |= 0x00000200u;
    m_object_results = object_results;
  }

  void SetKeyWords(const std::vector<std::string>& key_words) {
    m_mask |= 0x00000400u;
    m_key_words = key_words;
  }

  void AddKeyWord(const std::string& key_word) {
    m_mask |= 0x00000400u;
    m_key_words.push_back(key_word);
  }

  void SetLibResult(const std::vector<LibResults>& results) {
    m_mask |= 0x00000800u;
    m_lib_results = results; 
  }

  void AddLibResult(const LibResults& results) {
    m_mask |= 0x00000800u;
    m_lib_results.push_back(results);
  }

  void SetSpeakerResults(const std::vector<RecognitionResult>& results) {
    m_mask |= 0x00001000u;
    m_speaker_results = results;
  }

  void AddSpeakerResult(const RecognitionResult& result) {
    m_mask |= 0x00001000u;
    m_speaker_results.push_back(result);
  }

  void SetRecognitionResults(const std::vector<RecognitionResult>& results) {
    m_mask |= 0x00002000u;
    m_recognition_results = results;
  }

  void AddRecognitionResult(const RecognitionResult& result) {
    m_mask |= 0x00002000u;
    m_recognition_results.push_back(result);
  }

  int GetCode() const { return m_code; }

  int GetHitFlag() const { return m_hit_flag; }

  std::string GetMsg() const { return m_msg; }

  int GetScore() const { return m_score; }

  std::string GetLabel() const { return m_label; }

  std::string GetCategory() const { return m_category; }

  std::string GetSubLabel() const { return m_sub_label; }

  int GetCount() const { return m_count; }

  std::vector<OcrResult> GetOcrResults() const { return m_ocr_results; }

  ObjectResults GetObjectResults() const { return m_object_results; }

  std::vector<std::string> GetKeyWords() const { return m_key_words; }

  std::vector<LibResults> GetLibResults() const { return m_lib_results; }

  std::vector<RecognitionResult> GetRecognitionResults() const { return m_recognition_results; }

  std::vector<RecognitionResult> GetSpeakerResults() const { return m_speaker_results; }

  bool HasCode() const { return (m_mask & 0x00000001u) != 0; }

  bool HasMsg() const { return (m_mask & 0x00000002u) != 0; }

  bool HasHitFlag() const { return (m_mask & 0x00000004u) != 0; }

  bool HasScore() const { return (m_mask & 0x00000008u) != 0; }

  bool HasLabel() const { return (m_mask & 0x00000010u) != 0; }

  bool HasCategory() const { return (m_mask & 0x00000020u) != 0; }

  bool HasSubLabel() const { return (m_mask & 0x00000040u) != 0; }

  bool HasCount() const { return (m_mask & 0x00000080u) != 0; }

  bool HasOcrResults() const { return (m_mask & 0x00000100u) != 0; }

  bool HasObjectresults() const { return (m_mask & 0x00000200u) != 0; }

  bool HasKeyWords() const { return (m_mask & 0x00000400u) != 0; }

  bool HasLibResults() const { return (m_mask & 0x00000800u) != 0; }

  bool HasSpeakerResults() const { return (m_mask & 0x00001000u) != 0; }

  bool HasRecognitionResults() const { return (m_mask & 0x00002000u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    if (HasCode()) { ss << " code: " << std::to_string(m_code); }
    if (HasMsg()) { ss << " msg: " << m_msg; }
    if (HasHitFlag()) { ss << " hit_flag: " << std::to_string(m_hit_flag); }
    if (HasScore()) { ss << " score: " << std::to_string(m_score); }
    if (HasLabel()) { ss << " label: " << m_label; }
    if (HasCategory()) { ss << " category: " << m_category; }
    if (HasSubLabel()) { ss << " sub_label: " << m_sub_label; }
    if (HasCount()) { ss << " count: " << m_count; }
    if (HasOcrResults()) {
      for (std::vector<OcrResult>::const_iterator it = m_ocr_results.begin();
           it != m_ocr_results.end(); ++it) {
        ss << " ocr_result: {" << it->to_string() << "}";
      }
    }
    if (HasObjectresults()) { ss << " object_result: {" << m_object_results.to_string() << "}"; }
    if (HasKeyWords()) {
      for (std::vector<std::string>::const_iterator it = m_key_words.begin();
           it != m_key_words.end(); ++it) {
        if (it == m_key_words.begin()) {
          ss << " keywords: " << it->c_str();
          continue;
        }
        ss << "," << it->c_str();
      }
    }
    if (HasLibResults()) {
      for (std::vector<LibResults>::const_iterator it = m_lib_results.begin();
           it != m_lib_results.end(); ++it) {
        if (it == m_lib_results.begin()) {
          ss << " lib_results: " << it->to_string();
          continue;
        }
        ss << "," << it->to_string();
      }      
    }
    if (HasSpeakerResults()) {
      for (std::vector<RecognitionResult>::const_iterator it = m_speaker_results.begin();
           it != m_speaker_results.end(); ++it) {
        if (it == m_speaker_results.begin()) {
          ss << " speaker_results: " << it->to_string();
          continue;
        }
        ss << "," << it->to_string();
      }      
    }
    if (HasRecognitionResults()) {
      for (std::vector<RecognitionResult>::const_iterator it = m_recognition_results.begin();
           it != m_recognition_results.end(); ++it) {
        if (it == m_recognition_results.begin()) {
          ss << " recognition_results: " << it->to_string();
          continue;
        }
        ss << "," << it->to_string();
      }      
    }        
    return ss.str();
  }

 private:
  uint64_t m_mask;
  int m_code;                       // 单个审核场景的错误码，0为成功，其他为失败
  std::string m_msg;                // 具体错误信息，如正常为OK
  int m_hit_flag;                   // 是否命中该审核分类，0表示未命中，1表示命中，2表示疑似
  int m_score;                      // 该字段表示审核结果命中审核信息的置信度，取值范围：0（置信度最低）-100（置信度最高），越高代表该内容越有可能属于当前返回审核信息
                                    // 其中0 - 60分表示图片正常，61 - 90分表示图片疑似敏感，91 - 100分表示图片确定敏感
  std::string m_label;              // 该字段表示该截图的综合结果标签（可能为 SubLabel，可能为人物名字等）
  std::string m_category;           // 该字段为Label的子集，表示审核命中的具体审核类别。例如 Sexy，表示色情标签中的性感类别
  std::string m_sub_label;          // 该字段表示审核命中的具体子标签，例如：Porn 下的 SexBehavior 子标签。 可能为空，表示未命中字段
  int m_count;                      // 命中审核分类的截图数量
  std::vector<OcrResult> m_ocr_results;          // OCR 文本识别的详细检测结果，包括文本坐标信息、文本识别结果等信息，有相关违规内容时返回
  ObjectResults m_object_results;   // 识别出的实体详细检测结果，包括实体名和在图片中的详细位置
  std::vector<std::string> m_key_words; // 当前审核场景下命中的关键词
  std::vector<LibResults> m_lib_results; // 该字段用于返回基于风险库识别的结果。注意：未命中风险库中样本时，此字段不返回。
  std::vector<RecognitionResult> m_speaker_results; // 该字段表示声纹的识别详细结果。注意：未开启该功能时不返回该字段。
  std::vector<RecognitionResult> m_recognition_results; // 该字段表示未成年的识别详细结果。注意：未开启该功能时不返回该字段。

};

class UserInfo {
 public:
  UserInfo(): m_mask(0x00000000u) {}
  virtual ~UserInfo() {}

  void SetTokenId(const std::string& token_id) {
    m_mask |= 0x00000001u;
    m_token_id = token_id;
  }

  void SetNickName(const std::string& nick_name) {
    m_mask |= 0x00000002u;
    m_nick_name = nick_name;
  }

  void SetDeviceId(const std::string& device_id) {
    m_mask |= 0x00000004u;
    m_device_id = device_id;
  }

  void SetAppId(const std::string& app_id) {
    m_mask |= 0x00000008u;
    m_app_id = app_id;
  }

  void SetRoom(const std::string& room) {
    m_mask |= 0x00000010u;
    m_room = room;
  }

  void SetIp(const std::string& ip) {
    m_mask |= 0x00000020u;
    m_ip = ip;
  }

  void SetType(const std::string& type) {
    m_mask |= 0x00000040u;
    m_type = type;
  }

  void SetReceiveTokenId(const std::string& token_id) {
    m_mask |= 0x00000080u;
    m_receive_token_id = token_id;
  }

  void SetGender(const std::string& gender) {
    m_mask |= 0x00000100u;
    m_gender = gender;
  }

  void SetLevel(const std::string& level) {
    m_mask |= 0x00000200u;
    m_level = level;
  }

  void SetRole(const std::string& role) {
    m_mask |= 0x00000400u;
    m_role = role;
  }

  std::string GetTokenId() const { return m_token_id; }

  std::string GetNickName() const { return m_nick_name; }

  std::string GetDeviceId() const { return m_device_id; }

  std::string GetAppId() const { return m_app_id; }

  std::string GetRoom() const { return m_device_id; }

  std::string GetIp() const { return m_ip; }

  std::string GetType() const { return m_type; }

  std::string GetReceiveTokenId() const { return m_receive_token_id; }

  std::string GetGender() const { return m_gender; }

  std::string GetLevel() const { return m_level; }

  std::string GetRole() const { return m_role;}

  bool HasTokenId() const { return (m_mask & 0x00000001u) != 0; }

  bool HasNickName() const { return (m_mask & 0x00000002u) != 0; }

  bool HasDeviceId() const { return (m_mask & 0x00000004u) != 0; }

  bool HasAppId() const { return (m_mask & 0x00000008u) != 0; }

  bool HasRoom() const { return (m_mask & 0x00000010u) != 0; }

  bool HasIp() const { return (m_mask & 0x00000020u) != 0; }

  bool HasType() const { return (m_mask & 0x00000040u) != 0; }

  bool HasReceiveTokenId() const { return (m_mask & 0x00000080u) != 0; }

  bool HasGender() const { return (m_mask & 0x00000100u) != 0; }

  bool HasLevel() const { return (m_mask & 0x00000200u) != 0; }

  bool HasRole() const { return (m_mask & 0x00000400u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    if (HasTokenId()) { ss << " token_id: " << m_token_id; }
    if (HasNickName()) { ss << " nick_name: " << m_nick_name; }
    if (HasDeviceId()) { ss << " device_id: " << m_device_id; }
    if (HasAppId()) { ss << " app_id: " << m_app_id; }
    if (HasRoom()) { ss << " room: " << m_room; }
    if (HasIp()) { ss << " ip: " << m_ip; }
    if (HasType()) { ss << " type: " << m_type; }
    if (HasReceiveTokenId()) { ss << " receive_token_id: " << m_receive_token_id; }
    if (HasGender()) { ss << " gender: " << m_gender; }
    if (HasLevel()) { ss << " level: " << m_level; }
    if (HasRole()) { ss << " role: " << m_role; }
    return ss.str();
  }

 private:
  uint64_t m_mask;
  std::string m_token_id;         // 用户业务 TokenId，长度不超过128字节
  std::string m_nick_name;        // 用户业务 Nickname，长度不超过128字节
  std::string m_device_id;        // 用户业务 DeviceId，长度不超过128字节
  std::string m_app_id;           // 用户业务 AppId，长度不超过128字节
  std::string m_room;             // 用户业务 room，长度不超过128字节
  std::string m_ip;               // 用户业务 IP，长度不超过128字节
  std::string m_type;             // 用户业务 type，长度不超过128字节
  std::string m_receive_token_id; // 接收消息的用户账号，长度不超过128字节
  std::string m_gender;           // 一般用于表示性别信息，长度不超过128字节
  std::string m_level;            // 一般用于表示等级信息，长度不超过128字节
  std::string m_role;             // 一般用于表示角色信息，长度不超过128字节

};

class SegmentResult {
 public:
  SegmentResult() : m_mask(0x00000000) {}
  virtual ~SegmentResult() {}

  void SetUrl(const std::string &url) {
    m_mask |= 0x00000001u;
    m_url = url;
  }

  void SetSnapShotTime(int snap_shot_time) {
    m_mask |= 0x00000002u;
    m_snap_shot_time = snap_shot_time;
  }

  void SetOffsetTime(int offset_time) {
    m_mask |= 0x00000004u;
    m_offset_time = offset_time;
  }

  void SetDuration(int duration) {
    m_mask |= 0x00000008u;
    m_duration = duration;
  }

  void SetText(const std::string& text) {
    m_mask |= 0x00000010u;
    m_text = text;
  }

  void SetLabel(const std::string& label) {
    m_mask |= 0x00000020u;
    m_label = label;
  }

  void SetResult(int result) {
    m_mask |= 0x00000040u;
    m_result = result;
  }

  void SetPornInfo(const SceneResultInfo& porn_info) {
    m_mask |= 0x00000080u;
    m_porn_info = porn_info;
  }

  void SetAdsInfo(const SceneResultInfo& ads_info) {
    m_mask |= 0x00000100u;
    m_ads_info = ads_info;
  }

  void SetIllegalInfo(const SceneResultInfo& illegal_info) {
    m_mask |= 0x00000200u;
    m_illegal_info = illegal_info;
  }

  void SetAbuseInfo(const SceneResultInfo& abuse_info) {
    m_mask |= 0x00000400u;
    m_abuse_info = abuse_info;
  }

  void SetPoliticsInfo(const SceneResultInfo& politics_info) {
    m_mask |= 0x00000800u;
    m_politics_info = politics_info;
  }

  void SetTerrorismInfo(const SceneResultInfo& terrorism_info) {
    m_mask |= 0x00001000u;
    m_terrorism_info = terrorism_info;
  }

  void SetStartByte(int start_byte) {
    m_mask |= 0x00002000u;
    m_start_byte = start_byte;
  }

  void SetTeenagerInfo(const SceneResultInfo& teenager_info) {
    m_mask |= 0x00004000u;
    m_teenager_info = teenager_info;
  }

  void SetLanguageResults(const std::vector<RecognitionResult>& results) {
    m_mask |= 0x00008000u;
    m_language_results = results;
  }

  void AddLanguageResults(const RecognitionResult& result) {
    m_mask |= 0x00008000u;
    m_language_results.push_back(result);
  }

  const std::string& GetUrl() const { return m_url; }

  int GetSnapShotTime() const { return m_snap_shot_time; }

  int GetOffsetTime() const { return m_offset_time; }

  int GetDuration() const { return m_duration; }

  const std::string& GetText() const { return m_text; }

  const std::string& GetLabel() const { return m_label; }

  int GetResult() const { return m_result; }

  const SceneResultInfo& GetPornInfo() const { return m_porn_info; }

  const SceneResultInfo& GetAdsInfo() const { return m_ads_info; }

  const SceneResultInfo& GetIllegalInfo() const { return m_illegal_info; }

  const SceneResultInfo& GetAbuseInfo() const { return m_abuse_info; }

  const SceneResultInfo& GetPoliticsInfo() const { return m_politics_info; }

  const SceneResultInfo& GetTerrorismInfo() const { return m_terrorism_info; }

  const SceneResultInfo& GetTeenagerInfo() const { return m_teenager_info; }

  const std::vector<RecognitionResult>& GetLanguageResults() const { return m_language_results; }

  int GetStartByte() const { return m_start_byte; }

  bool HasUrl() const { return (m_mask & 0x00000001) != 0; }

  bool HasSnapShotTime() const { return (m_mask & 0x00000002) != 0; }

  bool HasOffsetTime() const { return (m_mask & 0x00000004) != 0; }

  bool HasDuration() const { return (m_mask & 0x00000008) != 0; }

  bool HasText() const { return (m_mask & 0x00000010) != 0; }

  bool HasLabel() const { return (m_mask & 0x00000020) != 0; }

  bool HasResult() const { return (m_mask & 0x00000040) != 0; }

  bool HasPornInfo() const { return (m_mask & 0x00000080) != 0; }

  bool HasAdsInfo() const { return (m_mask & 0x00000100) != 0; }

  bool HasIllegalInfo() const { return (m_mask & 0x00000200) != 0; }

  bool HasAbuseInfo() const { return (m_mask & 0x00000400) != 0; }

  bool HasPoliticsInfo() const { return (m_mask & 0x00000800) != 0; }

  bool HasTerrorismInfo() const { return (m_mask & 0x00001000) != 0; }

  bool HasStartByte() const { return (m_mask & 0x00002000) != 0; }

  bool HasTeenagerInfo() const { return (m_mask & 0x00004000) != 0;}

  bool HasLanguageResults() const { return (m_mask & 0x00008000u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    if (HasUrl()) { ss << " url: " << m_url; }
    if (HasSnapShotTime()) { ss << " snap_shot_time: " << m_snap_shot_time; }
    if (HasOffsetTime()) { ss << " offset_time: " << std::to_string(m_offset_time); }
    if (HasDuration()) { ss << " duration: " << m_duration; }
    if (HasText()) { ss << " text: " << m_text; }
    if (HasLabel()) { ss << " label: " << m_label; }
    if (HasResult()) { ss << " result: " << m_result; }
    if (HasStartByte()) { ss << " start_type: " << std::to_string(m_start_byte); }
    if (HasPornInfo()) { ss << " porn_info: {" << m_porn_info.to_string() << "}";  }
    if (HasAdsInfo()) { ss << " ads_info: {" << m_ads_info.to_string() << "}"; }
    if (HasIllegalInfo()) { ss << " illegal_info: {" << m_illegal_info.to_string() << "}"; }
    if (HasAbuseInfo()) { ss << " abuse_info: {" << m_abuse_info.to_string() << "}"; }
    if (HasPoliticsInfo()) { ss << " politics_info: {" << m_politics_info.to_string() << "}"; }
    if (HasTerrorismInfo()) { ss << " terrorism_info: {" << m_terrorism_info.to_string() << "}"; }
    if (HasTeenagerInfo()) {ss << " teenager_info: {" << m_teenager_info.to_string() << "}"; }
    if (HasLanguageResults()) {
      for (std::vector<RecognitionResult>::const_iterator it = m_language_results.begin();
           it != m_language_results.end(); ++it) {
        ss << "\n language_result: {\n" << it->to_string() << "} ";
      }
    }    
    return ss.str();
  }

 private:
  uint64_t m_mask;
  std::string m_url;                                  // 截图/音频片段的访问地址
  int m_snap_shot_time;                               // 截图位于视频中的时间，单位毫秒
  int m_offset_time;                                  // 当前声音片段位于视频中的饿时间，单位毫秒
  int m_duration;                                     // 当前声音片段的时长，单位毫秒
  std::string m_text;                                 // 文本识别结果
  std::string m_label;                                // 检测结果中优先级最高的恶意标签
  int m_result;                                       // 审核结果，0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  SceneResultInfo m_porn_info;                        // 审核场景为涉黄的审核结果信息
  SceneResultInfo m_ads_info;                         // 审核场景为广告引导的审核结果信息
  SceneResultInfo m_illegal_info;                     // 审核场景为违法的审核结果信息
  SceneResultInfo m_abuse_info;                       // 审核场景为谩骂的审核结果信息
  SceneResultInfo m_politics_info;                    // 审核场景为违法的审核结果信息
  SceneResultInfo m_terrorism_info;                   // 审核场景为谩骂的审核结果信息
  SceneResultInfo m_teenager_info;                    // 审核场景为未成年的审核结果信息
  std::vector<RecognitionResult> m_language_results;  // 该字段表示音频中语种的识别结果
  int m_start_byte;                                   // 该分片位于文本中的起始位置信息
};


class Labels {
 public:
  Labels() : m_mask(0x00000000) {}
  virtual ~Labels() {}

  void SetPornInfo(const SceneResultInfo& porn_info) {
    m_mask |= 0x00000001u;
    m_porn_info = porn_info;
  }

  void SetAdsInfo(const SceneResultInfo& ads_info) {
    m_mask |= 0x00000002u;
    m_ads_info = ads_info;
  }

  void SetIllegalInfo(const SceneResultInfo& illegal_info) {
    m_mask |= 0x00000004u;
    m_illegal_info = illegal_info;
  }

  void SetAbuseInfo(const SceneResultInfo& abuse_info) {
    m_mask |= 0x00000008u;
    m_abuse_info = abuse_info;
  }

  void SetPoliticsInfo(const SceneResultInfo& politics_info) {
    m_mask |= 0x00000010u;
    m_politics_info = politics_info;
  }

  void SetTerrorismInfo(const SceneResultInfo& terrorism_info) {
    m_mask |= 0x00000020u;
    m_terrorism_info = terrorism_info;
  }
  
  const SceneResultInfo& GetPornInfo() const { return m_porn_info; }
  
  const SceneResultInfo& GetAdsInfo() const { return m_ads_info; }

  const SceneResultInfo& GetIllegalInfo() const { return m_illegal_info; }

  const SceneResultInfo& GetAbuseInfo() const { return m_abuse_info; }

  const SceneResultInfo& GetPoliticsInfo() const { return m_politics_info; }

  const SceneResultInfo& GetTerrorismInfo() const { return m_terrorism_info; }
  
  bool HasPornInfo() const { return (m_mask & 0x00000001u) != 0; }
  
  bool HasAdsInfo() const { return (m_mask & 0x00000002u) != 0; }

  bool HasIllegalInfo() const { return (m_mask & 0x00000004u) != 0; }

  bool HasAbuseInfo() const { return (m_mask & 0x00000008u) != 0; }

  bool HasPoliticsInfo() const { return (m_mask & 0x00000010u) != 0; }

  bool HasTerrorismInfo() const { return (m_mask & 0x00000020u) != 0; }
  
  std::string to_string() const {
    std::stringstream ss;
    if (HasPornInfo()) { ss << " porn_info: {" << m_porn_info.to_string() << "}";  }
    if (HasAdsInfo()) { ss << " ads_info: {" << m_ads_info.to_string() << "}"; }
    if (HasIllegalInfo()) { ss << " illegal_info: {" << m_illegal_info.to_string() << "}"; }
    if (HasAbuseInfo()) { ss << " abuse_info: {" << m_abuse_info.to_string() << "}"; }
    if (HasPoliticsInfo()) { ss << " politics_info: {" << m_politics_info.to_string() << "}"; }
    if (HasTerrorismInfo()) { ss << " terrorism_info: {" << m_terrorism_info.to_string() << "}"; }
    return ss.str();
  }
  
  private:
  uint64_t m_mask;
  SceneResultInfo m_porn_info;        // 审核场景为涉黄的审核结果信息
  SceneResultInfo m_ads_info;         // 审核场景为广告引导的审核结果信息
  SceneResultInfo m_illegal_info;     // 审核场景为违法的审核结果信息
  SceneResultInfo m_abuse_info;       // 审核场景为谩骂的审核结果信息
  SceneResultInfo m_politics_info;    // 审核场景为违法的审核结果信息
  SceneResultInfo m_terrorism_info;   // 审核场景为谩骂的审核结果信息

};

class ListResult {
 public:
  ListResult() : m_mask(0x00000000) {}
  virtual ~ListResult() {}    

  void SetListType(const int list_type) { 
    m_mask |= 0x00000001u;
    m_list_type = list_type; 
  }

  void SetListName(const std::string& list_name) {
    m_mask |= 0x00000002u;
    m_list_name = list_name; 
  }

  void SetEntity(const std::string& entity) {
    m_mask |= 0x00000004u;
    m_entity = entity;
  }

  int GetListType() const { return m_list_type; }

  std::string GetListName() const { return m_list_name; }

  std::string GetEntity() const { return m_entity; }

  bool HasListType() const { return (m_mask & 0x00000001u) != 0; }

  bool HasListName() const { return (m_mask & 0x00000002u) != 0; }

  bool HasEntity() const { return (m_mask & 0x00000004u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    if (HasListType()) { ss << " list_type: {" << std::to_string(m_list_type) << "}";  }
    if (HasListName()) { ss << " list_name: {" << m_list_name << "}"; }
    if (HasEntity()) { ss << " entity: {" << m_entity << "}"; }
    return ss.str();
  }

 private:
  uint64_t m_mask;  
  int m_list_type;            // 命中的名单类型，取值为0（白名单）和1（黑名单）。
  std::string m_list_name;    // 命中的名单名称。
  std::string m_entity;       // 命中了名单中的哪条内容。
};

class ListInfo {
 public:
  ListInfo() : m_mask(0x00000000) {}
  virtual ~ListInfo() {}    

  void SetListResults(const std::vector<ListResult>& list_results) { 
    m_mask |= 0x00000001u;
    m_list_results = list_results;     
  }

  void AddListResult(const ListResult& list_result) {
    m_mask |= 0x00000001u;
    m_list_results.push_back(list_result);
  }

  std::vector<ListResult> GetListResult() const { return m_list_results; }

  bool HasListResult() const { return (m_mask & 0x00000001u) != 0; }

  std::string to_string() const { 
    std::stringstream ss;
    if (HasListResult()) {
        for (std::vector<ListResult>::const_iterator it = m_list_results.begin();
          it != m_list_results.end(); ++it) {
        ss << "list_results: " << it->to_string() << std::endl;
      }
    }
    return ss.str();    
  }

 private:
  uint64_t m_mask;
  std::vector<ListResult> m_list_results;   //命中的所有名单结果
};

class Result {
 public:
  Result() : m_mask(0x00000000) {}
  virtual ~Result() {}

  void SetUrl(const std::string& url) {
    m_mask |= 0x00000001u;
    m_url = url;
  }

  void SetPageNumber(const int page_number) {
    m_mask |= 0x00000002u;
    m_page_number = page_number;
  }

  void SetSheetNumber(const int sheet_number) {
    m_mask |= 0x00000004u;
    m_sheet_number = sheet_number;
  }
  
  void SetText(const std::string& text) {
    m_mask |= 0x00000008u;
    m_text = text;
  }
  
  void SetLabel(const std::string& label) {
    m_mask |= 0x00000010u;
    m_label = label;
  }
  
  void SetSuggestion(const int suggestion) {
    m_mask |= 0x00000020u;
    m_suggestion = suggestion;
  }

  void SetPornInfo(const SceneResultInfo& porn_info) {
    m_mask |= 0x00000040u;
    m_porn_info = porn_info;
  }
  
  void SetAdsInfo(const SceneResultInfo& ads_info) {
    m_mask |= 0x00000080u;
    m_ads_info = ads_info;
  }

  void SetIllegalInfo(const SceneResultInfo& illegal_info) {
    m_mask |= 0x00000100u;
    m_illegal_info = illegal_info;
  }

  void SetAbuseInfo(const SceneResultInfo& abuse_info) {
    m_mask |= 0x00000200u;
    m_abuse_info = abuse_info;
  }

  void SetPoliticsInfo(const SceneResultInfo& politics_info) {
    m_mask |= 0x00000400u;
    m_politics_info = politics_info;
  }

  void SetTerrorismInfo(const SceneResultInfo& terrorism_info) {
    m_mask |= 0x00000800u;
    m_terrorism_info = terrorism_info;
  }

  int SetPageNumber() const { return m_page_number; }

  int SetSheetNumber() const { return m_sheet_number; }
  
  const std::string& SetText() const { return m_text; }
  
  const std::string& SetLabel() const { return m_label; }
  
  int SetSuggestion() const { return m_suggestion; }

  const SceneResultInfo& GetPornInfo() const { return m_porn_info; }

  const SceneResultInfo& GetAdsInfo() const { return m_ads_info; }

  const SceneResultInfo& GetIllegalInfo() const { return m_illegal_info; }

  const SceneResultInfo& GetAbuseInfo() const { return m_abuse_info; }

  const SceneResultInfo& GetPoliticsInfo() const { return m_politics_info; }

  const SceneResultInfo& GetTerrorismInfo() const { return m_terrorism_info; }

  bool HasUrl() const { return (m_mask & 0x00000001u) != 0; }

  bool HasPageNumber() const { return (m_mask & 0x00000002u) != 0; }

  bool HasSheetNumber() const { return (m_mask & 0x00000004u) != 0; }

  bool HasText() const { return (m_mask & 0x00000008u) != 0; }

  bool HasLabel() const { return (m_mask & 0x00000010u) != 0; }

  bool HasSuggestion() const { return (m_mask & 0x00000020u) != 0; }

  bool HasPornInfo() const { return (m_mask & 0x00000040u) != 0; }

  bool HasAdsInfo() const { return (m_mask & 0x00000080u) != 0; }

  bool HasIllegalInfo() const { return (m_mask & 0x00000100u) != 0; }

  bool HasAbuseInfo() const { return (m_mask & 0x00000200u) != 0; }

  bool HasPoliticsInfo() const { return (m_mask & 0x00000400u) != 0; }

  bool HasTerrorismInfo() const { return (m_mask & 0x00000800u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    if (HasUrl()) { ss << "url: {" << m_url << "}" << std::endl; }
    if (HasPageNumber()) { ss << "page_number: " << std::to_string(m_page_number) << std::endl; }
    if (HasSheetNumber()) { ss << "sheet_number: " << std::to_string(m_sheet_number) << std::endl; }
    if (HasText()) { ss << "text: {" << m_text << "} " << std::endl; }
    if (HasLabel()) { ss << "label: " << m_label << std::endl; }
    if (HasSuggestion()) { ss << "suggestion: " << std::to_string(m_suggestion) << std::endl; }
    if (HasPornInfo()) { ss  << "porn_info: {" << m_porn_info.to_string() << "}" << std::endl;  }
    if (HasAdsInfo()) { ss << "ads_info: {" << m_ads_info.to_string() << "}" << std::endl; }
    if (HasIllegalInfo()) { ss << "illegal_info: {" << m_illegal_info.to_string() << "}" << std::endl; }
    if (HasAbuseInfo()) { ss << "abuse_info: {" << m_abuse_info.to_string() << "}" << std::endl; }
    if (HasPoliticsInfo()) { ss << "politics_info: {" << m_politics_info.to_string() << "}" << std::endl; }
    if (HasTerrorismInfo()) { ss << "terrorism_info: {" << m_terrorism_info.to_string() << "}" << std::endl; }
    return ss.str();
  }

  private:
  uint64_t m_mask;
  std::string m_url;                  // 截图/音频片段的访问地址
  int m_page_number;                  // 截图位于视频中的时间，单位毫秒
  int m_sheet_number;                 // 当前声音片段位于视频中的饿时间，单位毫秒
  std::string m_text;                 // 文本识别结果
  std::string m_label;                // 检测结果中优先级最高的恶意标签
  int m_suggestion;                   // 审核结果，0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  SceneResultInfo m_porn_info;        // 审核场景为涉黄的审核结果信息
  SceneResultInfo m_ads_info;         // 审核场景为广告引导的审核结果信息
  SceneResultInfo m_illegal_info;     // 审核场景为违法的审核结果信息
  SceneResultInfo m_abuse_info;       // 审核场景为谩骂的审核结果信息
  SceneResultInfo m_politics_info;    // 审核场景为违法的审核结果信息
  SceneResultInfo m_terrorism_info;   // 审核场景为谩骂的审核结果信息

};

class PageSegment {
 public:
  PageSegment() : m_mask(0x00000000u) {}
  virtual ~PageSegment() {}

  void SetResults(const std::vector<Result>& results) {
    m_mask |= 0x00000001u;
    m_results = results;
  }

  void AddResult(const Result& result) {
    m_mask |= 0x00000001u;
    m_results.push_back(result);
  }

  std::vector<Result> GetResults() const { return m_results; }

  bool HasResults() const { return (m_mask & 0x00000001u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    if (HasResults()) {
      for (std::vector<Result>::const_iterator it = m_results.begin();
           it != m_results.end(); ++it) {
        ss << "\nresults: {\n" << it->to_string() << "} ";
      }
    }
    return ss.str();
  }
  
 private:
  uint64_t m_mask;
  std::vector<Result> m_results;

};

class AuditingJobsDetail {
 public:
  AuditingJobsDetail() : m_mask(0x00000000u) {}
  virtual ~AuditingJobsDetail() {}

  void SetCode(const std::string& code) {
    m_mask |= 0x00000001u;
    m_code = code;
  }
  
  void SetMessage(const std::string& message) {
    m_mask |= 0x00000002u;
    m_message = message;
  }
  
  void SetDataId(const std::string& data_id) {
    m_mask |= 0x00000004u;
    m_data_id = data_id;
  }
  
  void SetJobId(const std::string& job_id) {
    m_mask |= 0x00000008u;
    m_job_id = job_id;
  }
  
  void SetState(const std::string& state) {
    m_mask |= 0x00000010u;
    m_state = state;
  }
  
  void SetCreationTime(const std::string& creation_time) {
    m_mask |= 0x00000020u;
    m_creation_time = creation_time;
  }
  
  void SetUserInfo(const UserInfo& user_info) {
    m_mask |= 0x00000040u;
    m_user_info = user_info;
  }

  std::string GetCode() const { return m_code; }

  std::string GetMessage() const { return m_message; }

  std::string GetDataId() const { return m_data_id; }

  std::string GetJobId() const { return m_job_id; }

  std::string GetState() const { return m_state; }

  std::string GetCreationTime() const { return m_creation_time; }

  UserInfo GetUserInfo() const { return m_user_info; }
  
  bool HasCode() const { return (m_mask & 0x00000001u) != 0; }

  bool HasMessage() const { return (m_mask & 0x00000002u) != 0; }

  bool HasDataId() const { return (m_mask & 0x00000004u) != 0; }

  bool HasJobId() const { return (m_mask & 0x00000008u) != 0; }

  bool HasState() const { return (m_mask & 0x00000010u) != 0; }

  bool HasCreationTime() const { return (m_mask & 0x00000020u) != 0; }

  bool HasUserInfo() const { return (m_mask & 0x00000040u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    if (HasCode()) {
      ss << "code: " << m_code << std::endl;
    }
    if (HasMessage()) {
      ss << "message: " << m_message << std::endl;
    }
    if (HasDataId()) {
      ss << "data_id: " << m_data_id << std::endl;
    }
    if (HasJobId()) {
      ss << "job_id: " << m_job_id << std::endl;
    }
    if (HasState()) {
      ss << "state: " << m_state << std::endl;
    }
    if (HasCreationTime()) {
      ss << "creation_time: " << m_creation_time << std::endl;
    }
    if (HasUserInfo()) {
      ss << "user_info: {" << m_user_info.to_string() << "}" << std::endl;
    }
    return ss.str();
  }

 protected:
  uint64_t m_mask;
  std::string m_code;                               // 错误码，只有 State 为 Failed 时有意义
  std::string m_message;                            // 错误描述，只有 State 为 Failed 时有意义
  std::string m_data_id;                            // 提交任务时设置了DataId参数时返回，返回原始内容，长度限制为512字节
  std::string m_job_id;                             // 新创建任务的 ID
  std::string m_state;                              // 任务的状态，为 Submitted、Auditing、Success、Failed其中一个
  std::string m_creation_time;                      // 任务的创建时间
  UserInfo m_user_info;                               // 用户业务字段。创建任务时未设置UserInfo则无此字段
};



class ImageAuditingJobsDetail : public AuditingJobsDetail {
 public:
  ImageAuditingJobsDetail() : AuditingJobsDetail() {}
  virtual ~ImageAuditingJobsDetail() {
  }

  void SetObject(const std::string& object) {
    m_mask |= 0x00000080u;
    m_object = object;
  }

  void SetUrl(const std::string& url) {
    m_mask |= 0x00000100u;
    m_url = url;
  }

  void SetCompressionResult(int compression_result) {
    m_mask |= 0x00000200u;
    m_compression_result = compression_result;
  }

  void SetText(const std::string& text) {
    m_mask |= 0x00000400u;
    m_text = text;
  }

  void SetLabel(const std::string& label) {
    m_mask |= 0x00000800u;
    m_label = label;
  }

  void SetResult(int result) {
    m_mask |= 0x00001000u;
    m_result = result;
  }

  void SetCategory(const std::string& category) {
    m_mask |= 0x00002000u;
    m_category = category;
  }

  void SetSubLabel(const std::string& sub_label) {
    m_mask |= 0x00004000u;
    m_sub_label = sub_label;
  }

  void SetPornInfo(const SceneResultInfo& porn_info) {
    m_mask |= 0x00008000u;
    m_porn_info = porn_info;
  }

  void SetAdsInfo(const SceneResultInfo& ads_info) {
    m_mask |= 0x00010000u;
    m_ads_info = ads_info;
  }

  void SetIllegalInfo(const SceneResultInfo& illegal_info) {
    m_mask |= 0x00020000u;
    m_illegal_info = illegal_info;
  }

  void SetAbuseInfo(const SceneResultInfo& abuse_info) {
    m_mask |= 0x00040000u;
    m_abuse_info = abuse_info;
  }

  void SetPoliticsInfo(const SceneResultInfo& politics_info) {
    m_mask |= 0x00080000u;
    m_politics_info = politics_info;
  }

  void SetTerrorismInfo(const SceneResultInfo& terrorism_info) {
    m_mask |= 0x00100000u;
    m_terrorism_info = terrorism_info;
  }

  
  void SetUserInfo(const UserInfo& user_info) {
    m_mask |= 0x00200000u;
    m_user_info = user_info;
  }

  void SetForbitState(const int forbit_state) {
    m_mask |= 0x00400000u;
    m_forbid_state = forbit_state;
  }

  void SetListInfo(const ListInfo& list_info) {
    m_mask |= 0x00800000u;
    m_list_info = list_info;
  }  

  std::string GetObject() const { return m_object; }

  std::string GetUrl() const { return m_url; }

  int GetCompressionResult() const { return m_compression_result; }

  std::string GetText() const { return m_text; }

  std::string GetLabel() const { return m_label; }

  int GetResult() const { return m_result; }

  std::string GetCategory() const { return m_category; }

  std::string GetSubLabel() const { return m_sub_label; }

  SceneResultInfo GetPornInfo() const { return m_porn_info; }

  SceneResultInfo GetAdsInfo() const { return m_ads_info; }

  SceneResultInfo GetIllegalInfo() const { return m_illegal_info; }

  SceneResultInfo GetAbuseInfo() const { return m_abuse_info; }

  SceneResultInfo GetPoliticsInfo() const { return m_politics_info; }

  SceneResultInfo GetTerrorismInfo() const { return m_terrorism_info; }

  bool HasObject() const { return (m_mask & 0x00000080u) != 0; }

  bool HasUrl() const { return (m_mask & 0x00000100u) != 0; }

  bool HasCompressionResult() const { return (m_mask & 0x00000200u) != 0; }

  bool HasText() const { return (m_mask & 0x00000400u) != 0; }

  bool HasLabel() const { return (m_mask & 0x00000800u) != 0; }

  bool HasResult() const { return (m_mask & 0x00001000u) != 0; }

  bool HasCategory() const { return (m_mask & 0x00002000u) != 0; }

  bool HasSubLabel() const { return (m_mask & 0x00004000u) != 0; }

  bool HasPornInfo() const { return (m_mask & 0x00008000u) != 0; }

  bool HasAdsInfo() const { return (m_mask & 0x00010000u) != 0; }

  bool HasIllegalInfo() const { return (m_mask & 0x00020000u) != 0; }

  bool HasAbuseInfo() const { return (m_mask & 0x00040000u) != 0; }

  bool HasPoliticsInfo() const { return (m_mask & 0x00080000u) != 0; }

  bool HasTerrorismInfo() const { return (m_mask & 0x00100000u) != 0; }

  bool HasUserInfo() const { return (m_mask & 0x00200000u) != 0; }

  bool HasForbidState() const { return (m_mask & 0x00400000u) != 0; }

  bool HasListInfo() const { return (m_mask & 0x00800000u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    ss << AuditingJobsDetail::to_string();
    if (HasObject()) {
      ss << "object: " << m_object << std::endl;
    }
    if (HasUrl()) {
      ss << "url: " << m_url << std::endl;
    }
    if (HasCompressionResult()) {
      ss <<  "compression_result: " << std::to_string(m_compression_result) << std::endl;
    }
    if (HasText()) {
      ss << "text: " << m_text << std::endl;
    }
    if (HasLabel()) {
      ss << "label: " << m_label << std::endl;
    }
    if (HasResult()) {
      ss << "result: " << std::to_string(m_result) << std::endl;
    }
    if (HasCategory()) {
      ss << "category: " << m_category << std::endl;
    }
    if (HasSubLabel()) {
      ss << "sub_label: " << m_sub_label << std::endl;
    }
    if (HasPornInfo()) {
      ss << "porn_info: {" << m_porn_info.to_string() << "}" << std::endl;
    }
    if (HasAdsInfo()) {
      ss << "ads_info: {" << m_ads_info.to_string() << "}" << std::endl;
    }
    if (HasIllegalInfo()) {
      ss << "illegal_info: {" << m_illegal_info.to_string() << "}" << std::endl;
    }
    if (HasAbuseInfo()) {
      ss << "abuse_info: {" << m_abuse_info.to_string() << "}" << std::endl;
    }
    if (HasPoliticsInfo()) {
      ss << "politics_info: {" << m_politics_info.to_string() << "}" << std::endl;
    }
    if (HasTerrorismInfo()) {
      ss << "terrorism_info: {" << m_terrorism_info.to_string() << "}" << std::endl;
    }
    if (HasSubLabel()) {
      ss << "sub_label: " << m_sub_label << std::endl;
    }
    if (HasUserInfo()) {
      ss << "user_info: " << m_user_info.to_string() << std::endl;
    }
    if (HasForbidState()) {
      ss << "forbid_state: " << std::to_string(m_forbid_state) << std::endl;
    }
    if (HasListInfo()) {
      ss << "list_info: " << m_list_info.to_string() << std::endl;
    }    
    return ss.str();
  }

  private:
  std::string m_object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string m_url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  int m_compression_result;                         // 图片是否被压缩处理，值为 0（未压缩），1（正常压缩）
  std::string m_text;                               // 图片中检测的文字内容（OCR），当审核策略开启文本内容检测时返回
  std::string m_label;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int m_result;                                     // 检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  std::string m_category;                           // Label字段子集，表示审核命中的具体审核类别。例如 Sexy，表示色情标签中的性感类别
  std::string m_sub_label;                          // 命中的二级标签结果
  SceneResultInfo m_porn_info;                      // 审核场景为涉黄的审核结果信息
  SceneResultInfo m_ads_info;                       // 审核场景为广告引导的审核结果信息
  SceneResultInfo m_illegal_info;                   // 审核场景为违法的审核结果信息
  SceneResultInfo m_abuse_info;                     // 审核场景为谩骂的审核结果信息
  SceneResultInfo m_politics_info;                  // 审核场景为违法的审核结果信息
  SceneResultInfo m_terrorism_info;                 // 审核场景为谩骂的审核结果信息
  SceneResultInfo m_quality_info;                   // 审核场景为低质量的审核结果信息
  UserInfo m_user_info;                             // 用户业务字段。创建任务未设置 UserInfo 时无此字段。
  int m_forbid_state;                               // 若设置了自动冻结，该字段表示图片文件的冻结状态。0：未冻结，1：已被冻结，2：已转移文件
  ListInfo m_list_info;                             // 账号黑白名单结果。  
};

class VideoAuditingJobsDetail : public AuditingJobsDetail {
  public:
  VideoAuditingJobsDetail() : AuditingJobsDetail() {}
  virtual ~VideoAuditingJobsDetail() {}

  void SetObject(const std::string& object) {
    m_mask |= 0x00000080u;
    m_object = object;
  }

  void SetUrl(const std::string& url) {
    m_mask |= 0x00000100u;
    m_url = url;
  }

  void SetSnapShotCount(const std::string& snap_shot_count) {
    m_mask |= 0x00000200u;
    m_snap_shot_count = snap_shot_count;
  }

  void SetLabel(const std::string& label) {
    m_mask |= 0x00000400u;
    m_label = label;
  }

  void SetResult(int result) {
    m_mask |= 0x00000800u;
    m_result = result;
  }

  void SetSnapShot(const std::vector<SegmentResult>& snap_shot) {
    m_mask |= 0x00002000u;
    m_snap_shot = snap_shot;
  }

  void SetAudioSection(const std::vector<SegmentResult>& audio_section) {
    m_mask |= 0x00004000u;
    m_audio_section = audio_section;
  }

  void SetPornInfo(const SceneResultInfo& porn_info) {
    m_mask |= 0x00008000u;
    m_porn_info = porn_info;
  }

  void SetAdsInfo(const SceneResultInfo& ads_info) {
    m_mask |= 0x00010000u;
    m_ads_info = ads_info;
  }

  void SetIllegalInfo(const SceneResultInfo& illegal_info) {
    m_mask |= 0x00020000u;
    m_illegal_info = illegal_info;
  }

  void SetAbuseInfo(const SceneResultInfo& abuse_info) {
    m_mask |= 0x00040000u;
    m_abuse_info = abuse_info;
  }

  void SetPoliticsInfo(const SceneResultInfo& politics_info) {
    m_mask |= 0x00080000u;
    m_politics_info = politics_info;
  }

  void SetTerrorismInfo(const SceneResultInfo& terrorism_info) {
    m_mask |= 0x00100000u;
    m_terrorism_info = terrorism_info;
  }

  void AddSnapShot(const SegmentResult& snap_shot) {
    m_mask |= 0x00200000u;
    m_snap_shot.push_back(snap_shot);
  }

  void AddAudioSection(const SegmentResult audio_section) {
    m_mask |= 0x00400000u;
    m_audio_section.push_back(audio_section);
  }

  void SetUserInfo(const UserInfo& user_info) {
    m_mask |= 0x00800000u;
    m_user_info = user_info;
  }

  void SetForbitState(const int forbit_state) {
    m_mask |= 0x01000000u;
    m_forbid_state = forbit_state;
  }

  void SetListInfo(const ListInfo& list_info) {
    m_mask |= 0x02000000u;
    m_list_info = list_info;
  }

  std::string GetObject() const { return m_object; }

  std::string GetUrl() const { return m_url; }

  std::string GetSnapShotCount() const { return m_snap_shot_count; }

  std::string GetLabel() const { return m_label; }

  int GetResult() const { return m_result; }

  std::vector<SegmentResult> GetSnapShot() const { return m_snap_shot; }

  std::vector<SegmentResult> GetAudioSection() const { return m_audio_section; }

  SceneResultInfo GetPornInfo() const { return m_porn_info; }

  SceneResultInfo GetAdsInfo() const { return m_ads_info; }

  SceneResultInfo GetIllegalInfo() const { return m_illegal_info; }

  SceneResultInfo GetAbuseInfo() const { return m_abuse_info; }

  SceneResultInfo GetPoliticsInfo() const { return m_politics_info; }

  SceneResultInfo GetTerrorismInfo() const { return m_terrorism_info; }

  UserInfo GetUserInfo() const { return m_user_info; }

  int GetForbidState() const { return m_forbid_state; }

  ListInfo GetListInfo() const { return m_list_info; }

  bool HasObject() const { return (m_mask & 0x00000080u) != 0; }

  bool HasUrl() const { return (m_mask & 0x00000100u) != 0; }

  bool HasSnapShotCount() const { return (m_mask & 0x00000200u) != 0; }

  bool HasLabel() const { return (m_mask & 0x00000400u) != 0; }

  bool HasResult() const { return (m_mask & 0x00000800u) != 0; }

  bool HasSnapShot() const { return (m_mask & 0x00002000u) != 0; }

  bool HasAudioSection() const { return (m_mask & 0x00004000u) != 0; }

  bool HasPornInfo() const { return (m_mask & 0x00008000u) != 0; }

  bool HasAdsInfo() const { return (m_mask & 0x00010000u) != 0; }

  bool HasIllegalInfo() const { return (m_mask & 0x00020000u) != 0; }

  bool HasAbuseInfo() const { return (m_mask & 0x00040000u) != 0; }

  bool HasPoliticsInfo() const { return (m_mask & 0x00080000u) != 0; }

  bool HasTerrorismInfo() const { return (m_mask & 0x00100000u) != 0; }

  bool HasUserInfo() const { return (m_mask & 0x00200000u) != 0; }

  bool HasForbidState() const { return (m_mask & 0x00400000u) != 0; }

  bool HasListInfo() const { return (m_mask & 0x00800000u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    ss << AuditingJobsDetail::to_string();
    if (HasObject()) {
      ss << "object:" << m_object << std::endl;
    }
    if (HasUrl()) {
      ss << "url: " << m_url << std::endl;
    }
    if (HasSnapShotCount()) {
      ss <<  "SnapShotCount: " << m_snap_shot_count << std::endl;
    }
    if (HasLabel()) {
      ss << "label: " << m_label << std::endl;
    }
    if (HasResult()) {
      ss << "Result: " << std::to_string(m_result) << std::endl;
    }
    if (HasPornInfo()) {
      ss << "porn_info: {" << m_porn_info.to_string() << "}" << std::endl;
    }
    if (HasAdsInfo()) {
      ss << "ads_info: {" << m_ads_info.to_string() << "}" << std::endl;
    }
    if (HasIllegalInfo()) {
      ss << "illegal_info: {" << m_illegal_info.to_string() << "}" << std::endl;
    }
    if (HasAbuseInfo()) {
      ss << "abuse_info: {" << m_abuse_info.to_string() << "}" << std::endl;
    }
    if (HasPoliticsInfo()) {
      ss << "politics_info: {" << m_politics_info.to_string() << "}" << std::endl;
    }
    if (HasTerrorismInfo()) {
      ss << "terrorism_info: {" << m_terrorism_info.to_string() << "}" << std::endl;
    }
    if (HasSnapShot()) {
      for (std::vector<SegmentResult>::const_iterator it = m_snap_shot.begin();
           it != m_snap_shot.end(); ++it) {
        ss << "snap_shot: {" << it->to_string() << "}" << std::endl;
      }
    }
    if (HasAudioSection()) {
      for (std::vector<SegmentResult>::const_iterator it = m_audio_section.begin();
           it != m_audio_section.end(); ++it) {
        ss << "audio_section: " << it->to_string() << std::endl;
      }
    }
    if (HasUserInfo()) {
      ss << "user_info: " << m_user_info.to_string() << std::endl;
    }
    if (HasForbidState()) {
      ss << "forbid_state: " << std::to_string(m_forbid_state) << std::endl;
    }
    if (HasListInfo()) {
      ss << "list_info: " << m_list_info.to_string() << std::endl;
    }    
    return ss.str();
  }

  private:
  std::string m_object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string m_url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  std::string m_snap_shot_count;                    // 视频截图总数量
  std::string m_label;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int m_result;                                     // 检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  SceneResultInfo m_porn_info;                      // 审核场景为涉黄的审核结果信息
  SceneResultInfo m_ads_info;                       // 审核场景为广告引导的审核结果信息
  SceneResultInfo m_illegal_info;                   // 审核场景为违法的审核结果信息
  SceneResultInfo m_abuse_info;                     // 审核场景为谩骂的审核结果信息
  SceneResultInfo m_politics_info;                  // 审核场景为违法的审核结果信息
  SceneResultInfo m_terrorism_info;                 // 审核场景为谩骂的审核结果信息
  std::vector<SegmentResult> m_snap_shot;           // 视频画面截图的审核结果
  std::vector<SegmentResult> m_audio_section;       // 视频中声音的审核结果
  UserInfo m_user_info;                             // 用户业务字段。创建任务未设置 UserInfo 时无此字段。
  int m_forbid_state;                               // 若设置了自动冻结，该字段表示视频文件的冻结状态。0：未冻结，1：已被冻结，2：已转移文件
  ListInfo m_list_info;                             // 账号黑白名单结果。
};

class AudioAuditingJobsDetail : public AuditingJobsDetail {
 public:
  AudioAuditingJobsDetail() {}
  virtual ~AudioAuditingJobsDetail() {}

  void SetObject(const std::string& object) {
    m_mask |= 0x00000080u;
    m_object = object;
  }

  void SetUrl(const std::string& url) {
    m_mask |= 0x00000100u;
    m_url = url;
  }

  void SetLabel(const std::string& label) {
    m_mask |= 0x00000200u;
    m_label = label;
  }

  void SetResult(int result) {
    m_mask |= 0x00000400u;
    m_result = result;
  }

  void SetAudioText(const std::string& audio_text) {
    m_mask |= 0x00000800u;
    m_audio_text = audio_text;
  }

  void SetPornInfo(const SceneResultInfo& porn_info) {
    m_mask |= 0x00001000u;
    m_porn_info = porn_info;
  }

  void SetAdsInfo(const SceneResultInfo& ads_info) {
    m_mask |= 0x00002000u;
    m_ads_info = ads_info;
  }

  void SetIllegalInfo(const SceneResultInfo& illegal_info) {
    m_mask |= 0x00004000u;
    m_illegal_info = illegal_info;
  }

  void SetAbuseInfo(const SceneResultInfo& abuse_info) {
    m_mask |= 0x00008000u;
    m_abuse_info = abuse_info;
  }

  void SetPoliticsInfo(const SceneResultInfo& politics_info) {
    m_mask |= 0x00010000u;
    m_politics_info = politics_info;
  }

  void SetTerrorismInfo(const SceneResultInfo& terrorism_info) {
    m_mask |= 0x00020000u;
    m_terrorism_info = terrorism_info;
  }

  void SetSection(const std::vector<SegmentResult>& section) {
    m_mask |= 0x00040000u;
    m_section = section;
  }

  void AddSection(const SegmentResult& section) {
    m_mask |= 0x00040000u;
    m_section.push_back(section);
  }

  void SetTeenagerInfo(const SceneResultInfo& teenager_info) {
    m_mask |= 0x00080000u;
    m_teenager_info = teenager_info;
  }

  void SetSubLabel(const std::string& label) {
    m_mask |= 0x00100000u;
    m_sub_label = label;
  }
  
  void SetUserInfo(const UserInfo& user_info) {
    m_mask |= 0x00200000u;
    m_user_info = user_info;
  }

  void SetForbitState(const int forbit_state) {
    m_mask |= 0x00400000u;
    m_forbid_state = forbit_state;
  }

  void SetListInfo(const ListInfo& list_info) {
    m_mask |= 0x00800000u;
    m_list_info = list_info;
  }

  std::string GetObject() const { return m_object; }

  std::string GetUrl() const { return m_url; }

  std::string GetLabel() const { return m_label; }

  int GetResult() const { return m_result; }

  std::string GetAudioText() const { return m_audio_text; }

  SceneResultInfo GetPornInfo() const { return m_porn_info; }

  SceneResultInfo GetAdsInfo() const { return m_ads_info; }

  SceneResultInfo GetIllegalInfo() const { return m_illegal_info; }

  SceneResultInfo GetAbuseInfo() const { return m_abuse_info; }

  SceneResultInfo GetPoliticsInfo() const { return m_politics_info; }

  SceneResultInfo GetTerrorismInfo() const { return m_terrorism_info; }

  std::vector<SegmentResult> GetSection() const { return m_section; }

  SceneResultInfo GetTeenagerInfo() const { return m_teenager_info; } 

  std::string GetSubLabel() const { return m_sub_label; }

  UserInfo GetUserInfo() const { return m_user_info; }

  int GetForbidState() const { return m_forbid_state; }

  ListInfo GetListInfo() const { return m_list_info; }

  bool HasObject() const { return (m_mask & 0x00000080u) != 0; }

  bool HasUrl() const { return (m_mask & 0x00000100u) != 0; }

  bool HasLabel() const { return (m_mask & 0x00000200u) != 0; }

  bool HasResult() const { return (m_mask & 0x00000400u) != 0; }

  bool HasAudioText() const { return (m_mask & 0x00000800u) != 0; }

  bool HasPornInfo() const { return (m_mask & 0x00001000u) != 0; }

  bool HasAdsInfo() const { return (m_mask & 0x00002000u) != 0; }

  bool HasIllegalInfo() const { return (m_mask & 0x00004000u) != 0; }

  bool HasAbuseInfo() const { return (m_mask & 0x00008000u) != 0; }

  bool HasPoliticsInfo() const { return (m_mask & 0x00010000u) != 0; }

  bool HasTerrorismInfo() const { return (m_mask & 0x00020000u) != 0; }

  bool HasSection() const { return (m_mask & 0x00040000u) != 0; }

  bool HasTeenagerInfo() const { return (m_mask & 0x00080000u) != 0; }

  bool HasSubLabel() const { return (m_mask & 0x00100000u) != 0; }

  bool HasUserInfo() const { return (m_mask & 0x00200000u) != 0; }

  bool HasForbidState() const { return (m_mask & 0x00400000u) != 0; }

  bool HasListInfo() const { return (m_mask & 0x00800000u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    ss << AuditingJobsDetail::to_string();
    if (HasObject()) {
      ss << "object:" << m_object << std::endl;
    }
    if (HasUrl()) {
      ss << "url: " << m_url << std::endl;
    }
    if (HasLabel()) {
      ss << "label: " << m_label << std::endl;
    }
    if (HasResult()) {
      ss << "result: " << std::to_string(m_result) << std::endl;
    }
    if (HasAudioText()) {
      ss << "audio_text: " << m_audio_text << std::endl;
    }
    if (HasPornInfo()) {
      ss << "porn_info: {" << m_porn_info.to_string() << "}" << std::endl;
    }
    if (HasAdsInfo()) {
      ss << "ads_info: {" << m_ads_info.to_string() << "}" << std::endl;
    }
    if (HasIllegalInfo()) {
      ss << "illegal_info: {" << m_illegal_info.to_string() << "}" << std::endl;
    }
    if (HasAbuseInfo()) {
      ss << "abuse_info: {" << m_abuse_info.to_string() << "}" << std::endl;
    }
    if (HasPoliticsInfo()) {
      ss << "politics_info: {" << m_politics_info.to_string() << "}" << std::endl;
    }
    if (HasTerrorismInfo()) {
      ss << "terrorism_info: {" << m_terrorism_info.to_string() << "}" << std::endl;
    }
    if (HasTeenagerInfo()) {
      ss << "teenager_info: {" << m_teenager_info.to_string() << "}" << std::endl;
    }
    if (HasSection()) {
      for (std::vector<SegmentResult>::const_iterator it = m_section.begin();
           it != m_section.end(); ++it) {
        ss << "section: {" << it->to_string() << "}" << std::endl;
      }
    }
    if (HasSubLabel()) {
      ss << "sub_label: " << m_sub_label << std::endl;
    }
    if (HasUserInfo()) {
      ss << "user_info: " << m_user_info.to_string() << std::endl;
    }
    if (HasForbidState()) {
      ss << "forbid_state: " << std::to_string(m_forbid_state) << std::endl;
    }
    if (HasListInfo()) {
      ss << "list_info: " << m_list_info.to_string() << std::endl;
    }
    return ss.str();
  }

 private:
  std::string m_object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string m_url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  std::string m_label;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int m_result;                                     // 检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  std::string m_audio_text;                         // 音频文件中已识别的对应文本内容
  SceneResultInfo m_porn_info;                      // 审核场景为涉黄的审核结果信息
  SceneResultInfo m_ads_info;                       // 审核场景为广告引导的审核结果信息
  SceneResultInfo m_illegal_info;                   // 审核场景为违法的审核结果信息
  SceneResultInfo m_abuse_info;                     // 审核场景为谩骂的审核结果信息
  SceneResultInfo m_politics_info;                  // 审核场景为违法的审核结果信息
  SceneResultInfo m_terrorism_info;                 // 审核场景为谩骂的审核结果信息
  SceneResultInfo m_teenager_info;                  // 审核场景为未成年的审核结果信息
  std::vector<SegmentResult> m_section;             // 音频（文本）审核中对音频（文本）片段的审核结果
  std::string m_sub_label;                          // 该字段表示审核命中的具体子标签。注意：该字段可能返回空
  UserInfo m_user_info;                             // 用户业务字段。创建任务未设置 UserInfo 时无此字段。
  int m_forbid_state;                               // 若设置了自动冻结，该字段表示音频文件的冻结状态。0：未冻结，1：已被冻结，2：已转移文件
  ListInfo m_list_info;                             // 账号黑白名单结果。
};

class TextAuditingJobsDetail : public AuditingJobsDetail{
 public:
  TextAuditingJobsDetail() {}
  virtual ~TextAuditingJobsDetail() {}

  void SetObject(const std::string& object) {
    m_mask |= 0x00000080u;
    m_object = object;
  }

  void SetContent(const std::string& content) {
    m_mask |= 0x00000100u;
    m_content = content;
  }

  void SetSectionCount(const int section_count) {
    m_mask |= 0x00000200u;
    m_section_count = section_count;
  }

  void SetLabel(const std::string& label) {
    m_mask |= 0x00000400u;
    m_label = label;
  }

  void SetResult(int result) {
    m_mask |= 0x00000800u;
    m_result = result;
  }

  void SetPornInfo(const SceneResultInfo& porn_info) {
    m_mask |= 0x00001000u;
    m_porn_info = porn_info;
  }

  void SetAdsInfo(const SceneResultInfo& ads_info) {
    m_mask |= 0x00002000u;
    m_ads_info = ads_info;
  }

  void SetIllegalInfo(const SceneResultInfo& illegal_info) {
    m_mask |= 0x00004000u;
    m_illegal_info = illegal_info;
  }

  void SetAbuseInfo(const SceneResultInfo& abuse_info) {
    m_mask |= 0x00008000u;
    m_abuse_info = abuse_info;
  }

  void SetPoliticsInfo(const SceneResultInfo& politics_info) {
    m_mask |= 0x00010000u;
    m_politics_info = politics_info;
  }

  void SetTerrorismInfo(const SceneResultInfo& terrorism_info) {
    m_mask |= 0x00020000u;
    m_terrorism_info = terrorism_info;
  }

  void SetSection(const std::vector<SegmentResult>& section) {
    m_mask |= 0x00040000u;
    m_section = section;
  }

  void AddSection(const SegmentResult& section) {
    m_mask |= 0x00040000u;
    m_section.push_back(section);
  }

  void SetUserInfo(const UserInfo& user_info) {
    m_mask |= 0x00080000u;
    m_user_info = user_info;
  }

  void SetForbitState(const int forbit_state) {
    m_mask |= 0x00100000u;
    m_forbid_state = forbit_state;
  }

  void SetListInfo(const ListInfo& list_info) {
    m_mask |= 0x00200000u;
    m_list_info = list_info;
  }  

  std::string GetObject() const { return m_object; }

  std::string GetContent() const { return m_content; }

  int GetSectionContent() const { return m_section_count; }

  std::string GetLabel() const { return m_label; }

  int GetResult() const { return m_result; }

  SceneResultInfo GetPornInfo() const { return m_porn_info; }

  SceneResultInfo GetAdsInfo() const { return m_ads_info; }

  SceneResultInfo GetIllegalInfo() const { return m_illegal_info; }

  SceneResultInfo GetAbuseInfo() const { return m_abuse_info; }

  SceneResultInfo GetPoliticsInfo() const { return m_politics_info; }

  SceneResultInfo GetTerrorismInfo() const { return m_terrorism_info; }

  std::vector<SegmentResult> GetSection() const { return m_section; }

  UserInfo GetUserInfo() const { return m_user_info; }

  int GetForbidState() const { return m_forbid_state; }

  ListInfo GetListInfo() const { return m_list_info; }

  bool HasObject() const { return (m_mask & 0x00000080u) != 0; }

  bool HasContent() const { return (m_mask & 0x00000100u) != 0; }

  bool HasSectionCount() const { return (m_mask & 0x00000200u) != 0; }

  bool HasLabel() const { return (m_mask & 0x00000400u) != 0; }

  bool HasResult() const { return (m_mask & 0x00000800u) != 0; }

  bool HasPornInfo() const { return (m_mask & 0x00001000u) != 0; }

  bool HasAdsInfo() const { return (m_mask & 0x00002000u) != 0; }

  bool HasIllegalInfo() const { return (m_mask & 0x00004000u) != 0; }

  bool HasAbuseInfo() const { return (m_mask & 0x00008000u) != 0; }

  bool HasPoliticsInfo() const { return (m_mask & 0x00010000u) != 0; }

  bool HasTerrorismInfo() const { return (m_mask & 0x00020000u) != 0; }

  bool HasSection() const { return (m_mask & 0x00040000u) != 0; }

  bool HasUserInfo() const { return (m_mask & 0x00800000u) != 0; }

  bool HasForbidState() const { return (m_mask & 0x01000000u) != 0; }

  bool HasListInfo() const { return (m_mask & 0x02000000u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    ss << AuditingJobsDetail::to_string();
    if (HasObject()) {
      ss << "object:" << m_object << std::endl;
    }
    if (HasContent()) {
      ss << "content: " << m_content << std::endl;
    }
    if (HasLabel()) {
      ss << "label: " << m_label << std::endl;
    }
    if (HasResult()) {
      ss << "result: " << std::to_string(m_result) << std::endl;
    }
    if (HasSectionCount()) {
      ss << "section_count: " << std::to_string(m_section_count) << std::endl;
    }
    if (HasPornInfo()) {
      ss << "porn_info: {" << m_porn_info.to_string() << "}" << std::endl;
    }
    if (HasAdsInfo()) {
      ss << "ads_info: {" << m_ads_info.to_string() << "}" << std::endl;
    }
    if (HasIllegalInfo()) {
      ss << "illegal_info: {" << m_illegal_info.to_string() << "}" << std::endl;
    }
    if (HasAbuseInfo()) {
      ss << "abuse_info: {" << m_abuse_info.to_string() << "}" << std::endl;
    }
    if (HasPoliticsInfo()) {
      ss << "politics_info: {" << m_politics_info.to_string() << "}" << std::endl;
    }
    if (HasTerrorismInfo()) {
      ss << "terrorism_info: {" << m_terrorism_info.to_string() << "}" << std::endl;
    }
    if (HasSection()) {
      for (std::vector<SegmentResult>::const_iterator it = m_section.begin();
           it != m_section.end(); ++it) {
        ss << "section: {" << it->to_string() << "}" << std::endl;
      }
    }
    if (HasUserInfo()) {
      ss << "user_info: " << m_user_info.to_string() << std::endl;
    }
    if (HasForbidState()) {
      ss << "forbid_state: " << std::to_string(m_forbid_state) << std::endl;
    }
    if (HasListInfo()) {
      ss << "list_info: " << m_list_info.to_string() << std::endl;
    }    
    return ss.str();
  }

 private:
  std::string m_object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string m_content;                            // 文本内容的Base64编码，创建任务使用Content时返回
  int m_section_count;                              // 审核文本的分片数，默认为1
  std::string m_label;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int m_result;                                     // 检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  SceneResultInfo m_porn_info;                      // 审核场景为涉黄的审核结果信息
  SceneResultInfo m_ads_info;                       // 审核场景为广告引导的审核结果信息
  SceneResultInfo m_illegal_info;                   // 审核场景为违法的审核结果信息
  SceneResultInfo m_abuse_info;                     // 审核场景为谩骂的审核结果信息
  SceneResultInfo m_politics_info;                  // 审核场景为违法的审核结果信息
  SceneResultInfo m_terrorism_info;                 // 审核场景为谩骂的审核结果信息
  std::vector<SegmentResult> m_section;             // 文本审核中对文本片段的审核结果
  UserInfo m_user_info;                             // 用户业务字段。创建任务未设置 UserInfo 时无此字段。
  int m_forbid_state;                               // 若设置了自动冻结，该字段表示文件的冻结状态。0：未冻结，1：已被冻结，2：已转移文件
  ListInfo m_list_info;                             // 账号黑白名单结果。
};

class DocumentAuditingJobsDetail : public AuditingJobsDetail {
 public:
  DocumentAuditingJobsDetail() {}
  virtual ~DocumentAuditingJobsDetail() {}

  void SetObject(const std::string& object) {
    m_mask |= 0x00000080u;
    m_object = object;
  }

  void SetUrl(const std::string& url) {
    m_mask |= 0x00000100u;
    m_url = url;
  }

  void SetLabel(const std::string& label) {
    m_mask |= 0x00000200u;
    m_label = label;
  }

  void SetSuggestion(int suggestion) {
    m_mask |= 0x00000400u;
    m_suggestion = suggestion;
  }

  void SetPageCount(const int page_count) {
    m_mask |= 0x00000800u;
    m_page_count = page_count;
  }

  void SetLabels(const Labels& labels) {
    m_mask |= 0x00001000u;
    m_labels = labels;
  }

  void SetPageSegment(const PageSegment& page_segment) {
    m_mask |= 0x00002000u;
    m_page_segment = page_segment;
  }

  void SetUserInfo(const UserInfo& user_info) {
    m_mask |= 0x00004000u;
    m_user_info = user_info;
  }

  void SetForbitState(const int forbit_state) {
    m_mask |= 0x00008000u;
    m_forbid_state = forbit_state;
  }

  void SetListInfo(const ListInfo& list_info) {
    m_mask |= 0x00010000u;
    m_list_info = list_info;
  }  

  std::string GetObject() const { return m_object; }

  std::string GetUrl() const { return m_url; }

  std::string GetLabel() const { return m_label; }

  int GetSuggestion() const { return m_suggestion; }

  int GetPageCount() const { return m_page_count; }

  Labels GetLabels() const { return m_labels; }

  PageSegment GetPageSegment() const { return m_page_segment; }

  UserInfo GetUserInfo() const { return m_user_info; }

  int GetForbidState() const { return m_forbid_state; }

  ListInfo GetListInfo() const { return m_list_info; } 

  bool HasObject() const { return (m_mask & 0x00000080u) != 0; }

  bool HasUrl() const { return (m_mask & 0x00000100u) != 0; }

  bool HasLabel() const { return (m_mask & 0x00000200u) != 0; }

  bool HasSuggestion() const { return (m_mask & 0x00000400u) != 0; }

  bool HasPageCount() const { return (m_mask & 0x00000800u) != 0; }

  bool HasLabels() const { return (m_mask & 0x00001000u) != 0; }

  bool HasPageSegment() const { return (m_mask & 0x00002000u) != 0; }

  bool HasUserInfo() const { return (m_mask & 0x00004000u) != 0; }

  bool HasForbidState() const { return (m_mask & 0x00080000u) != 0; }

  bool HasListInfo() const { return (m_mask & 0x00100000u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    ss << AuditingJobsDetail::to_string();
    if (HasObject()) {
      ss << "object:" << m_object << std::endl;
    }
    if (HasUrl()) {
      ss << "url: " << m_url << std::endl;
    }
    if (HasLabel()) {
      ss << "label: " << m_label << std::endl;
    }
    if (HasSuggestion()) {
      ss << "suggestion: " << std::to_string(m_suggestion) << std::endl;
    }
    if (HasPageCount()) {
      ss << "page_count: " << std::to_string(m_page_count) << std::endl;
    }
    if (HasLabels()) {
      ss << "labels: {" << m_labels.to_string() << "}" << std::endl;
    }
    if (HasPageSegment()) {
      ss << "page_segment: {" << m_page_segment.to_string() << "}" << std::endl;
    }
    if (HasUserInfo()) {
      ss << "user_info: " << m_user_info.to_string() << std::endl;
    }
    if (HasForbidState()) {
      ss << "forbid_state: " << std::to_string(m_forbid_state) << std::endl;
    }
    if (HasListInfo()) {
      ss << "list_info: " << m_list_info.to_string() << std::endl;
    }        
    return ss.str();
  }

 private:
  std::string m_object;                             // 本次审核的文件名称，创建任务使用Object时返回
  std::string m_url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  std::string m_label;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int m_suggestion;                                 // 文档审核的检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  int m_page_count;                                 // 文档审核会将文档转图片进行审核，该字段表示文档转换图片数
  Labels m_labels;                                  // 命中的审核场景及对应的结果
  PageSegment m_page_segment;                       // 文档转换为图片后，具体每张图片的审核结果信息
  UserInfo m_user_info;                             // 用户业务字段。创建任务未设置 UserInfo 时无此字段。
  int m_forbid_state;                               // 若设置了自动冻结，该字段表示文件的冻结状态。0：未冻结，1：已被冻结，2：已转移文件
  ListInfo m_list_info;                             // 账号黑白名单结果。  
};

class WebPageAuditingJobsDetail : public AuditingJobsDetail{
 public:
  WebPageAuditingJobsDetail() {}
  virtual ~WebPageAuditingJobsDetail() {}

  void SetUrl(const std::string& url) {
    m_mask |= 0x00000080u;
    m_url = url;
  }

  void SetLabel(const std::string& label) {
    m_mask |= 0x00000100u;
    m_label = label;
  }

  void SetSuggestion(int suggestion) {
    m_mask |= 0x00000200u;
    m_suggestion = suggestion;
  }

  void SetPageCount(const int page_count) {
    m_mask |= 0x00000400u;
    m_page_count = page_count;
  }

  void SetImageResults(const PageSegment& image_results) {
    m_mask |= 0x00000800u;
    m_image_results = image_results;
  }

  void SetTextResults(const PageSegment& text_results) {
    m_mask |= 0x00001000u;
    m_text_results = text_results;
  }

  void SetHighLightHtml(const std::string& high_light_html) {
    m_mask |= 0x00002000u;
    m_high_light_html = high_light_html;
  }

  void SetLabels(const Labels& labels) {
    m_mask |= 0x00004000u;
    m_labels = labels;
  }

  void SetUserInfo(const UserInfo& user_info) {
    m_mask |= 0x00008000u;
    m_user_info = user_info;
  }

  void SetListInfo(const ListInfo& list_info) {
    m_mask |= 0x00010000u;
    m_list_info = list_info;
  }  

  std::string GetUrl() const { return m_url; }

  std::string GetLabel() const { return m_label; }

  int GetSuggestion() const { return m_suggestion; }

  int GetPageCount() const { return m_page_count; }

  Labels GetLabels() const { return m_labels; }

  PageSegment GetImageResults() const { return m_image_results; }

  PageSegment GetTextResults() const { return m_text_results; }

  std::string GetHighLightHtml() const { return m_high_light_html; }

  UserInfo GetUserInfo() const { return m_user_info; }

  ListInfo GetListInfo() const { return m_list_info; }

  bool HasUrl() const { return (m_mask & 0x00000080u) != 0; }

  bool HasLabel() const { return (m_mask & 0x00000100u) != 0; }

  bool HasSuggestion() const { return (m_mask & 0x00000200u) != 0; }

  bool HasPageCount() const { return (m_mask & 0x00000400u) != 0; }

  bool HasImageResults() const { return (m_mask & 0x00000800u) != 0; }

  bool HasTextResults() const { return (m_mask & 0x00001000u) != 0; }

  bool HasHighLightHtml() const { return (m_mask & 0x00002000u) != 0; }

  bool HasLabels() const { return (m_mask & 0x00004000u) != 0; }

  bool HasUserInfo() const { return (m_mask & 0x00080000u) != 0; }

  bool HasListInfo() const { return (m_mask & 0x00100000u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    ss << AuditingJobsDetail::to_string();
    if (HasUrl()) {
      ss << "url: " << m_url << std::endl;
    }
    if (HasLabel()) {
      ss << "label: " << m_label << std::endl;
    }
    if (HasSuggestion()) {
      ss << "suggestion: " << std::to_string(m_suggestion) << std::endl;
    }
    if (HasPageCount()) {
      ss << "page_count: " << std::to_string(m_page_count) << std::endl;
    }
    if (HasImageResults()) {
      ss << " image_results: {" << m_image_results.to_string() << "}" << std::endl;
    }
    if (HasTextResults()) {
      ss << " text_results: {" << m_text_results.to_string() << "}" << std::endl;
    }
    if (HasHighLightHtml()) {
      ss << " high_light_html: " << m_high_light_html << std::endl;
    }

    if (HasLabels()) {
      ss << "labels: {" << m_labels.to_string() << "}" << std::endl;
    }
    if (HasUserInfo()) {
      ss << "user_info: " << m_user_info.to_string() << std::endl;
    }
    if (HasListInfo()) {
      ss << "list_info: " << m_list_info.to_string() << std::endl;
    }    

    return ss.str();
  }


 private:
  std::string m_url;                                // 本次审核的文件链接，创建任务使用 Url 时返回
  std::string m_label;                              // 检测结果中所对应的优先级最高的恶意标签，返回值：Normal：正常，Porn：色情，Ads：广告等
  int m_suggestion;                                 // 网页审核的检测结果，有效值：0（审核正常），1 （判定为违规敏感文件），2（疑似敏感，建议人工复核）
  int m_page_count;                                 // 网页审核会将网页中的图片链接和文本分开送审，该字段表示送审的链接和文本总数量。
  PageSegment m_image_results;                      // 网页内图片的审核结果
  PageSegment m_text_results;                       // 网页内文本的审核结果
  std::string m_high_light_html;                    // 对违规关键字高亮处理的 Html 网页内容，请求内容指定 ReturnHighlightHtml 时返回
  Labels m_labels;                                  // 命中的审核场景及对应的结果
  UserInfo m_user_info;                             // 用户业务字段。创建任务未设置 UserInfo 时无此字段。
  ListInfo m_list_info;                             // 账号黑白名单结果。
};


class AuditingInput {
 public:
  AuditingInput() : m_mask(0x00000000u) {}
  
  void SetObject(const std::string& object) { 
    m_mask |= 0x00000001u;
    m_object = object; 
  }

  void SetUrl(const std::string& url) { 
    m_mask |= 0x00000002u;
    m_url = url; 
  }

  void SetInterval(const int interval) {
    m_mask |= 0x00000004u;
    m_interval = interval; 
  }

  void SetMaxFrames(const int max_frames) {
    m_mask |= 0x00000008u;
    m_max_frames = max_frames; 
  }

  void SetDataId(const std::string& data_id) {
    m_mask |= 0x00000010u;
    m_data_id = data_id; 
  }

  void SetLargeImageDetect(const int large_image_detect) {
    m_mask |= 0x00000020u;
    m_large_image_detect = large_image_detect; 
  }

  void SetUserInfo(const UserInfo& user_info) {
    m_mask |= 0x00000040u;
    m_user_info = user_info; 
  }

  void SetContent(const std::string& content) { 
    m_mask |= 0x00000080u;
    m_content = content; 
  }

  void SetType(const std::string& type) {
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

class SnapShotConf {
 public:
  SnapShotConf() : m_mask(0x00000000u) {}

  void SetMode(const std::string& mode) {
    m_mask |= 0x00000001u;
    m_mode = mode;
  }

  void SetCount(const int count) {
    m_mask |= 0x00000002u;
    m_count = count;
  }

  void SetTimeIterval(const float time_interval) {
    m_mask = 0x00000004u;
    m_time_interval = time_interval;
  }

  std::string GetMode() const { return m_mode; }

  int GetCount() const { return m_count; }

  float GetTimeInterval() const { return m_time_interval; }

  bool HasMode() const { return (m_mask & 0x00000001u) != 0; }

  bool HasCount() const { return (m_mask & 0x00000002u) != 0; }

  bool HasTimeInterval() const { return (m_mask & 0x00000004u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    if (HasMode()) { ss << "mode: " << m_mode; }
    if (HasCount()) { ss << " count: " << std::to_string(m_count); }
    if (HasTimeInterval()) { ss << " time_interval: " << std::to_string(m_time_interval); }
    return ss.str();
  };
 private:
  uint64_t m_mask;
  std::string m_mode;                // 截帧模式, 默认为Interval，取值为Interval、Average、Fps，// 详见https://cloud.tencent.com/document/product/436/47316
  int m_count;                       // 视频截帧数量
  float m_time_interval;             // 视频截帧频率，单位为秒，支持精准到毫秒
};

class Freeze {
  public:
  Freeze() : m_mask(0x00000000u) {}

  void SetPornScore(const int porn_score) { 
    m_mask |= 0x00000001u;
    m_porn_score = porn_score; 
  }

  void SetAdsScore(const int ads_score) {
    m_mask |= 0x00000002u;
    m_ads_score = ads_score;
  }  

  int GetPornSocre() const { return m_porn_score; }

  int GetAdsSocre() const { return m_ads_score; }

  bool HasPornScore() const { return (m_mask & 0x00000001u) != 0; }

  bool HasAdsScore() const { return (m_mask & 0x00000002u) != 0;}

  std::string to_string() const {
    std::stringstream ss;
    if (HasPornScore()) { ss << "porn_score: " << std::to_string(m_porn_score); }
    if (HasAdsScore()) { ss << " ads_score: " << std::to_string(m_ads_score); }
    return ss.str();    
  }

 private:
  uint64_t m_mask;  
  int m_porn_score;   // 取值为[0,100]，表示当色情审核结果大于或等于该分数时，自动进行冻结操作。不填写则表示不自动冻结，默认值为空。
  int m_ads_score;    // 取值为[0,100]，表示当广告审核结果大于或等于该分数时，自动进行冻结操作。不填写则表示不自动冻结，默认值为空。
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

  void SetDetectContent(const int detect_content) {
    m_mask = m_mask | 0x00000020u;
    m_detect_content = detect_content;
  }

  void SetReturnHighlightHtml(const bool return_highlight_html) {
    m_mask = m_mask | 0x00000040u;
    m_return_highlight_html = return_highlight_html;
  }	

  void SetAsync(const int async) {
    m_mask = m_mask | 0x00000080u;
    m_async = async;    
  }

  void SetFreeze(const Freeze& freeze) {
    m_mask = m_mask | 0x00000200u;
    m_freeze = freeze;
  }
   
  void SetTimeout(const int timeout) {
    m_mask = m_mask | 0x00000400u;
    m_timeout = timeout;
  }

  std::string GetBizType() const { return m_biz_type;}

  std::string GetDetectType() const { return m_biz_type; }

  SnapShotConf GetSnapShot() const { return m_snap_shot; }

  std::string GetCallBack() const { return m_callback; }

  std::string GetCallBackVersion() const { return m_callback_version; }

  int GetDetectContent() const { return m_detect_content; }

  bool GetReturnHighlightHtml() const { return m_return_highlight_html; }

  int GetAsync() const { return m_async; }

  Freeze GetFreeze() const { return m_freeze; }

  int GetTimeout() const { return m_timeout; }

  bool HasBizType() const { return (m_mask & 0x00000001u) != 0; }

  bool HasDetectType() const { return (m_mask & 0x00000002u) != 0;}

  bool HasSnapShot() const { return (m_mask & 0x00000004u) != 0; }

  bool HasCallback() const { return (m_mask & 0x00000008u) != 0; }

  bool HasCallbackVersion() const { return (m_mask & 0x00000010u) != 0; }

  bool HasDetectContent() const { return (m_mask & 0x00000020u) != 0; }

  bool HasReturnHighlightHtml() const { return (m_mask & 0x00000040u) != 0; }

  bool HasAsync() const { return (m_mask & 0x00000080u) != 0; }

  bool HasFreeze() const { return (m_mask & 0x00000200u) != 0; }

  bool HasTimeout() const { return (m_mask & 0x00000400u) != 0; }

  std::string to_string() const {
    std::stringstream ss;
    ss << "biz_type: " << m_biz_type
		   << ", detect_type: " << m_detect_type
       << ", snap_shot: " << m_snap_shot.to_string() 
       << ", callback: " << m_callback
       << ", callbcak_version: " << m_callback_version
       << ", detect_content: " << m_detect_content
       << ", return_highlight_html: " << m_return_highlight_html
       << ", async: " << m_async
       << ", freeze: " << m_freeze.to_string()
       << ", timeout: " << m_timeout;
    return ss.str();
  }

 private:
  uint64_t m_mask;
  std::string m_biz_type;           // 审核策略的唯一标识
  std::string m_detect_type;        // 审核的场景类型 有效值：Porn（涉黄）、Ads（广告）等，可以传入多种类型
  SnapShotConf m_snap_shot;         // 审核视频截帧配置
  std::string m_callback;           // 回调规则
  std::string m_callback_version;   // 回调内容结构，有效值为Simple、Detail，默认Simple
  int m_detect_content;             // 指定是否审核视频声音，0 指审核视频截图，1 审核视频截图和声音，默认为0
  bool m_return_highlight_html;     // 网页审核时，是否需要高亮网页耶的违规文本
  int m_async;                      // 是否异步进行审核，0：同步返回结果，1：异步进行审核。默认值为 0。
  Freeze m_freeze;             // 可通过该字段，设置根据审核结果给出的不同分值，对图片进行自动冻结，仅当input中审核的图片为object时有效
  int m_timeout;               // 通过该参数可设置请求的超时时间。参数有效值为大于等于0的整数，单位为毫秒，0表示不限制时间，默认值为0
};

class GetImageAuditingReq : public ObjectReq {
 public:
  GetImageAuditingReq(const std::string& bucket_name)
      : ObjectReq(bucket_name, "") {
    AddParam("ci-process", "sensitive-content-recognition");
    SetMethod("GET");
    SetHttps();
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

  // 审核场景类型，有效值：porn（涉黄）、ads（广告）等，可以传入多种类型，不同类型以逗号分隔
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

  // 异步审核参数，取值 0：同步返回结果，1：异步进行审核，默认为0。
  void SetAsync(int async) {
    AddParam("async", std::to_string(async)); 
  }

  // 审核结果（Detail版本）以回调形式发送至您的回调地址，异步审核时生效，支持以 http:// 或者 https:// 开头的地址，例如： http://www.callback.com。
  void SetCallback(const std::string callback) {
    AddParam("callback", callback); 
  }
};

class BatchImageAuditingReq : public BucketReq {
 public:
  BatchImageAuditingReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("POST");
    SetPath("/image/auditing");
    SetHttps();
    AddHeader("Content-Type", "application/xml");
  }

  virtual ~BatchImageAuditingReq() {}

  void SetConf(const Conf& conf) { m_conf = conf; }
  void SetBizType(const std::string& biz_type) { m_conf.SetBizType(biz_type); }
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void AddInput(const AuditingInput& input) { m_inputs.push_back(input); }
  void SetInputs(const std::vector<AuditingInput>& inputs) { m_inputs = inputs; }
  virtual bool GenerateRequestBody(std::string* body) const;

 private:
  std::vector<AuditingInput> m_inputs; // 需要审核的内容，传入多个Input则审核多个内容
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

class CreateAuditingJobReq : public BucketReq {
 public:
  CreateAuditingJobReq(const std::string& bucket_name)
      : BucketReq(bucket_name) {
    SetMethod("POST");
    SetHttps();
    AddHeader("Content-Type", "application/xml");
  }
  virtual ~CreateAuditingJobReq() {}
  
  void SetInput(const AuditingInput& input) { m_input = input; }
  void SetConf(const Conf& conf) { m_conf = conf; }
  virtual bool GenerateRequestBody(std::string* body) const ;
 
 protected:
  AuditingInput m_input;       // 需要审核的内容
  Conf m_conf;         // 审核配置规则
};

class CreateVideoAuditingJobReq : public CreateAuditingJobReq {
 public:
  CreateVideoAuditingJobReq(const std::string& bucket_name)
      : CreateAuditingJobReq(bucket_name) {
    SetPath("/video/auditing");
  }
  virtual ~CreateVideoAuditingJobReq() {}

  void SetBizType(const std::string& biz_type) { m_conf.SetBizType(biz_type); }
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void SetSnapShot(const SnapShotConf& snap_shot) { m_conf.SetSnapShot(snap_shot); }
  void SetCallBack(const std::string& callback) { m_conf.SetCallBack(callback); }
  void SetCallBackVersion(const std::string& callback_version) { m_conf.SetCallBackVersion(callback_version); }
  void SetDetectContent(const int detect_content) { m_conf.SetDetectContent(detect_content); }

  void SetObject(const std::string& object) { m_input.SetObject(object); }
  void SetUrl(const std::string& url) { m_input.SetUrl(url); } 
  void SetDataId(const std::string& data_id) { m_input.SetDataId(data_id); }
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

class CreateAudioAuditingJobReq : public CreateAuditingJobReq {
 public:
  CreateAudioAuditingJobReq(const std::string& bucket_name)
      : CreateAuditingJobReq(bucket_name) {
    SetPath("/audio/auditing");
  }
  virtual ~CreateAudioAuditingJobReq() {}


  void SetBizType(const std::string& biz_type) { m_conf.SetBizType(biz_type); }
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void SetCallBack(const std::string& callback) { m_conf.SetCallBack(callback); }
  void SetCallBackVersion(const std::string& callback_version) { m_conf.SetCallBackVersion(callback_version); }

  void SetObject(const std::string& object) { m_input.SetObject(object); }
  void SetUrl(const std::string& url) { m_input.SetUrl(url); } 
  void SetDataId(const std::string& data_id) { m_input.SetDataId(data_id); }
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

class CreateTextAuditingJobReq : public CreateAuditingJobReq {
 public:
  CreateTextAuditingJobReq(const std::string& bucket_name)
      : CreateAuditingJobReq(bucket_name) {
    SetPath("/text/auditing");
  }
  virtual ~CreateTextAuditingJobReq() {}

  
  void SetBizType(const std::string& biz_type) { m_conf.SetBizType(biz_type); }
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void SetCallBack(const std::string& callback) { m_conf.SetCallBack(callback); }
  void SetCallBackVersion(const std::string& callback_version) { m_conf.SetCallBackVersion(callback_version); }

  void SetObject(const std::string& object) { m_input.SetObject(object); }
  void SetUrl(const std::string& url) { m_input.SetUrl(url); } 
  void SetDataId(const std::string& data_id) { m_input.SetDataId(data_id); }
  void SetContent(const std::string& content) { m_input.SetContent(content); }
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

class CreateDocumentAuditingJobReq : public CreateAuditingJobReq {
 public:
  CreateDocumentAuditingJobReq(const std::string& bucket_name)
      : CreateAuditingJobReq(bucket_name) {
    SetPath("/document/auditing");
  }
  virtual ~CreateDocumentAuditingJobReq() {}

  void SetBizType(const std::string& biz_type) { m_conf.SetBizType(biz_type); }
  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void SetCallBack(const std::string& callback) { m_conf.SetCallBack(callback); }

  void SetObject(const std::string& object) { m_input.SetObject(object); }
  void SetUrl(const std::string& url) { m_input.SetUrl(url); } 
  void SetDataId(const std::string& data_id) { m_input.SetDataId(data_id); }
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

class CreateWebPageAuditingJobReq : public CreateAuditingJobReq {
 public:
  CreateWebPageAuditingJobReq(const std::string& bucket_name)
      : CreateAuditingJobReq(bucket_name) {
    SetPath("/webpage/auditing");
  }
  virtual ~CreateWebPageAuditingJobReq() {}


  void SetDetectType(const std::string& detect_type) { m_conf.SetDetectType(detect_type); }
  void SetCallBack(const std::string& callback) { m_conf.SetCallBack(callback); }
  void SetReturnHighligthHtml(const bool return_highlight_html) {
    m_conf.SetReturnHighlightHtml(return_highlight_html);
  }

  void SetObject(const std::string& object) { m_input.SetObject(object); }
  void SetDataId(const std::string& data_id) { m_input.SetDataId(data_id); }
  void SetUserInfo(const UserInfo& user_info) { m_input.SetUserInfo(user_info); }
  void SetUrl(const std::string& url) { m_input.SetUrl(url); }
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

