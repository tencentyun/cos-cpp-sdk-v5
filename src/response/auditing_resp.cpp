#include "response/auditing_resp.h"

#include "cos_params.h"
#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

bool AuditingResp::ParseSegmentResult(rapidxml::xml_node<>* root, SegmentResult& segment_result) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Url" == node_name) {
      segment_result.url = node->value();
    } else if ("SnapShotTime" == node_name) {
      segment_result.snap_shot_time = StringUtil::StringToInt(node->value());
    } else if ("Text" == node_name) {
      segment_result.text = node->value();
    } else if ("OffsetTime" == node_name) {
      segment_result.offset_time = StringUtil::StringToInt(node->value());
    } else if ("Duration" == node_name) {
      segment_result.duration = StringUtil::StringToInt(node->value());
    } else if ("Lable" == node_name) {
      segment_result.lable = node->value();
    } else if ("Result" == node_name) {
      segment_result.result = StringUtil::StringToInt(node->value());
    } else if ("PornInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(segment_result.porn_info))) {
        return false;
      }
    } else if ("AdsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(segment_result.ads_info))) {
        return false;
      }
    } else if ("IllegalInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(segment_result.illegal_info))) {
        return false;
      }
    } else if ("AbuseInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(segment_result.abuse_info))) {
        return false;
      } 
    } else if ("PoliticsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(segment_result.politics_info))) {
        return false;
      }
    } else if ("TerrorismInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(segment_result.terrorism_info))) {
        return false;
      } 
    }
  }
  return true;
}

bool AuditingResp::ParseUserInfo(rapidxml::xml_node<>* root, UserInfo& user_info) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("TokenId" == node_name) {
      user_info.token_id = node->value();
    } else if ("Nickname" == node_name) {
      user_info.nick_name = node->value();
    } else if ("DeviceId" == node_name) {
      user_info.device_id = node->value();
    } else if ("AppId" == node_name) {
      user_info.app_id = node->value();
    } else if ("Room" == node_name) {
      user_info.room = node->value();
    } else if ("Ip" == node_name) {
      user_info.ip = node->value();
    } else if ("Type" == node_name) {
      user_info.type = node->value();
    }
  }
  return true;
}

bool AuditingResp::ParseLocation(rapidxml::xml_node<>* root, Location& location) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("X" == node_name) {
      location.x = StringUtil::StringToFloat(node->value());
    } else if ("Y" == node_name) {
      location.y = StringUtil::StringToFloat(node->value());
    } else if ("Height" == node_name) {
      location.height = StringUtil::StringToFloat(node->value());
    } else if ("Width" == node_name) {
      location.width = StringUtil::StringToFloat(node->value());
    } else if ("Rotate" == node_name) {
      location.rotate = StringUtil::StringToFloat(node->value());
    }
  }
  return true;
}

bool AuditingResp::ParseOcrResultInfo(rapidxml::xml_node<>* root, OcrResult& ocr_results) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Text" == node_name) {
      ocr_results.text = node_name->value();
    } else if ("Keywords" == node_name) {
      ocr_results.keywords.push_back(node_name->value());
    } else if ("Location" == node_name) {
      if (!ParseLocation(node, &(ocr_results.location))) {
        return false;
      }
    }
  }
  return true;
}

bool AuditingResp::ParseSceneResultInfo(rapidxml::xml_node<>* root, SceneResultInfo& scene_result_info) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" == node_name) {
      scene_result_info.code = StringUtil::StringToInt(node->value());
    } else if ("Msg" == node_name) {
      scene_result_info.msg = node->value();
    } else if ("Count" == node_name) {
      scene_result_info.count = StringUtil::StringToInt(node->value());
    } else if ("HitFlag" == node_name) {
      scene_result_info.hit_flag = StringUtil::StringToInt(node->value());
    } else if ("Score" == node_name) {
      scene_result_info.score = node->value();
    } else if ("Lable" == node_name) {
      scene_result_info.lable = node->value();
    } else if ("SubLable" == node_name) {
      scene_result_info.sub_lable = node->value();
    } else if ("Category" == node_name) {
      scene_result_info.category = node->value();
    } else if ("OcrResults" == node_name) {
      if (!ParseOcrResultInfo(node, &(scene_result_info.ocr_results))) {
        return false;
      }
    }
  }
  return true;
}

bool AuditingResp::ParseResults(rapidxml::xml_node<>* root, Result* result) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Url" == node_name) {
      result.url = node->value();
    } else if ("PageNumber" == node_name) {
      result->page_number = StringUtil::StringToInt(node->value());
    } else if ("Text" == node_name) {
      result.text = node->value();
    } else if ("SheetNumber" == node_name) {
      result->sheet_number = StringUtil::StringToInt(node->value());
    } else if ("Lable" == node_name) {
      result.lable = node->value();
    } else if ("Seggestion" == node_name) {
      result->suggestion = StringUtil::StringToInt(node->value());
    } else if ("PornInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(result.porn_info))) {
        return false;
      }
    } else if ("AdsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(result.ads_info))) {
        return false;
      }
    } else if ("AbuseInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(result.abuse_info))) {
        return false;
      }
    } else if ("IllegalInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(result.illegal_info))) {
        return false;
      }    
    } else if ("PoliticsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(result.politics_info))) {
        return false;
      }
    } else if ("TerrorismInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(result.terrorism_info))) {
        return false;
      } 
    }
  }
  return true;
}

bool ImageAuditingResp::ParseJobsDetail(
     rapidxml::xml_node<>* root, ImageAuditingJobsDetail& jobs_detail) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" ==  node_name) {
      jobs_detail.code = node->value();
    } else if ("Message" ==  node_name) {
      jobs_detail.message = node->value();
    } else if ("JobId" == node_name) {
      jobs_detail.job_id = node->value();
    } else if ("State" == node_name) {
      jobs_detail.state = node->value();      
    } else if ("CreationTime" == node_name) {
      jobs_detail.creation_time = node->value();       
    } else if ("CompressionResult" == node_name) {
      jobs_detail.compression_result = StringUtil::StringToInt(node->value());
    } else if ("Result" == node_name) {
      jobs_detail.result = StringUtil::StringToInt(node->value());
    } else if ("Lable" == node_name) {
      jobs_detail.lable = node->value();
    } else if ("Category" == node_name) {
      jobs_detail.category = node->value();
    } else if ("SubLable" == node_name) {
      jobs_detail.sub_lable = node->value();
    } else if ("Text" == node_name) {
      jobs_detail.text = node->value();   
    } else if ("PornInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(jobs_detail.porn_info))) {
        return false;
      }
    } else if ("AdsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(jobs_detail.ads_info))) {
        return false;
      }     
    } else if ("AbuseInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(jobs_detail.abuse_info))) {
        return false;
      }
    } else if ("IllegalInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(jobs_detail.illegal_info))) {
        return false;
      }    
    } else if ("PoliticsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(jobs_detail.politics_info))) {
        return false;
      }
    } else if ("TerrorismInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(jobs_detail.terrorism_info))) {
        return false;
      }                
    } else if ("DataId" ==  node_name) {
      jobs_detail.data_id = node->value();
    } else if ("UserInfo" ==  node_name) {
      if (!ParseUserInfo(node, &(jobs_detail.user_info))) {
        return false;
      }
    } else if ("Object" ==  node_name) {
      jobs_detail.object = node->value();
    } else if ("Url" ==  node_name) {
      jobs_detail.url = node->value();
    }
  }
  return true;  
}

bool GetImageAuditingResp::ParseFromXmlString(const std::string& body) {
  std::string tmp_body = body;
  rapidxml::xml_document<> doc;

  if (!StringUtil::StringToXml(&tmp_body[0], &doc)) {
    SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* root = doc.first_node("RecognitionResult");
  if (NULL == root) {
    SDK_LOG_ERR("Missing root node RecognitionResult, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* node = root->first_node();
  if (!ParseJobsDetail(node, m_jobs_detail)) {
    return false;
  }
  return true;    
}

bool BatchImageAuditingResp::ParseFromXmlString(const std::string& body) {
  std::string tmp_body = body;
  rapidxml::xml_document<> doc;

  if (!StringUtil::StringToXml(&tmp_body[0], &doc)) {
    SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* root = doc.first_node("Response");
  if (NULL == root) {
    SDK_LOG_ERR("Missing root node Response, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = result_node->name();
    if ("RequestId" == node_name) {
      m_request_id = node->value();
    } else if ("JobsDetail" == node_name) {
      ImageAuditingJobsDetail jobs_detail;
      if (!ParseImageAuditingJobsDetail(node, &jobs_detail)) {
        return false;
      }
      m_jobs_detail.push_back(jobs_detail);
    }
  }
  return true;   
}

bool DescribeImageAuditingJobResp::ParseFromXmlString(const std::string& body) {
  std::string tmp_body = body;
  rapidxml::xml_document<> doc;

  if (!StringUtil::StringToXml(&tmp_body[0], &doc)) {
    SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* root = doc.first_node("Response");
  if (NULL == root) {
    SDK_LOG_ERR("Missing root node Response, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = result_node->name();
    if ("RequestId" == node_name) {
      m_request_id = node->value();
    } else if ("JobsDetail" == node_name) {
      ImageAuditingJobsDetail jobs_detail;
      if (!ParseImageAuditingJobsDetail(node, &m_jobs_detail)) {
        return false;
      }
    }
  }
  return true;   
}


bool  VideoAuditingResp::ParseJobsDetail(rapidxml::xml_node<>* root) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" ==  node_name) {
      m_jobs_detail.code = node->value();
    } else if ("Message" ==  node_name) {
      m_jobs_detail.message = node->value();
    } else if ("JobId" == node_name) {
      m_jobs_detail.job_id = node->value();
    } else if ("State" == node_name) {
      m_jobs_detail.state = node->value();      
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.creation_time = node->value();       
    } else if ("Result" == node_name) {
      m_jobs_detail.result = StringUtil::StringToInt(node->value());
    } else if ("Lable" == node_name) {
      m_jobs_detail.lable = node->value();
    } else if ("AudioText" == node_name) {
      m_jobs_detail.audio_text = node->value();      
    } else if ("PornInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.porn_info))) {
        return false;
      }
    } else if ("AdsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.ads_info))) {
        return false;
      }  
    } else if ("AbuseInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.abuse_info))) {
        return false;
      }
    } else if ("IllegalInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.illegal_info))) {
        return false;
      }    
    } else if ("PoliticsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.politics_info))) {
        return false;
      }
    } else if ("TerrorismInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.terrorism_info))) {
        return false;
      }         
    } else if ("DataId" ==  node_name) {
      m_jobs_detail.data_id = node->value();
    } else if ("UserInfo" ==  node_name) {
      if (!ParseUserInfo(node, &(m_jobs_detail.user_info))) {
        return false;
      }
    } else if ("Object" ==  node_name) {
      m_jobs_detail.object = node->value();
    } else if ("Url" ==  node_name) {
      m_jobs_detail.url = node->value();
    } else if ("SnapShotCount" == node_name ) {
      m_jobs_detail.snap_shot_count = node->value();
    } else if ("Snapshot" == node_name) {
      SegmentResult snap_shot;
      if (!ParseSegmentResult(node, &(snap_shot))) {
        return false;
      }
      m_jobs_detail.snap_shot.push_back(snap_shot);
    } else if ("AudioSection" == node_name) {
      SegmentResult audio_section;
      if (!ParseSegmentResult(node, &(audio_section))) {
        return false;
      }
      m_jobs_detail.audio_section.push_back(audio_section);
    }
  }
  return true;
}


bool AudioAuditingResp::ParseJobsDetail(rapidxml::xml_node<>* root) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" ==  node_name) {
      m_jobs_detail.code = node->value();
    } else if ("Message" ==  node_name) {
      m_jobs_detail.message = node->value();
    } else if ("JobId" == node_name) {
      m_jobs_detail.job_id = node->value();
    } else if ("State" == node_name) {
      m_jobs_detail.state = node->value();      
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.creation_time = node->value();       
    } else if ("Result" == node_name) {
      m_jobs_detail.result = StringUtil::StringToInt(node->value());
    } else if ("Lable" == node_name) {
      m_jobs_detail.lable = node->value();
    } else if ("Text" == node_name) {
      m_jobs_detail.text = node->value();
    } else if ("AudioText" == node_name) {
      m_jobs_detail.audio_text = node->value();      
    } else if ("PornInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.porn_info))) {
        return false;
      }
    } else if ("AdsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.ads_info))) {
        return false;
      }   
    } else if ("AbuseInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.abuse_info))) {
        return false;
      }
    } else if ("IllegalInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.illegal_info))) {
        return false;
      }    
    } else if ("PoliticsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.politics_info))) {
        return false;
      }
    } else if ("TerrorismInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.terrorism_info))) {
        return false;
      }         
    } else if ("DataId" ==  node_name) {
      m_jobs_detail.data_id = node->value();
    } else if ("UserInfo" ==  node_name) {
      if (!ParseUserInfo(node, &(m_jobs_detail.user_info))) {
        return false;
      }
    } else if ("Object" ==  node_name) {
      m_jobs_detail.object = node->value();
    } else if ("Url" ==  node_name) {
      m_jobs_detail.url = node->value();
    } else if ("Section" == node_name) {
      SegmentResult section;
      if (!ParseSegmentResult(node, &(section))) {
        return false;
      }
      m_jobs_detail.section.push_back(section);
    }
  }
  return true;
}

bool TextAuditingResp::ParseJobsDetail(rapidxml::xml_node<>* root) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" ==  node_name) {
      m_jobs_detail.code = node->value();
    } else if ("Message" ==  node_name) {
      m_jobs_detail.message = node->value();
    } else if ("JobId" == node_name) {
      m_jobs_detail.job_id = node->value();
    } else if ("State" == node_name) {
      m_jobs_detail.state = node->value();      
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.creation_time = node->value();       
    } else if ("Result" == node_name) {
      m_jobs_detail.result = StringUtil::StringToInt(node->value());
    } else if ("Lable" == node_name) {
      m_jobs_detail.lable = node->value();   
    } else if ("PornInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.porn_info))) {
        return false;
      }
    } else if ("AdsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.ads_info))) {
        return false;
      }
    } else if ("AbuseInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.abuse_info))) {
        return false;
      }
    } else if ("IllegalInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.illegal_info))) {
        return false;
      }    
    } else if ("PoliticsInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.politics_info))) {
        return false;
      }
    } else if ("TerrorismInfo" == node_name) {
      if (!ParseSceneResultInfo(node, &(m_jobs_detail.terrorism_info))) {
        return false;
      }   
    } else if ("DataId" ==  node_name) {
      m_jobs_detail.data_id = node->value();
    } else if ("UserInfo" ==  node_name) {
      if (!ParseUserInfo(node, &(m_jobs_detail.user_info))) {
        return false;
      }
    } else if ("Object" ==  node_name) {
      m_jobs_detail.object = node->value();
    } else if ("Section" == node_name) {
      SegmentResult section;
      if (!ParseSegmentResult(node, &(section))) {
        return false;
      }
      m_jobs_detail.section.push_back(section);
    }
  }
  return true;
}


bool DocumentAuditingResp::ParseJobsDetail(rapidxml::xml_node<>* root) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" ==  node_name) {
      m_jobs_detail.code = node->value();
    } else if ("Message" ==  node_name) {
      m_jobs_detail.message = node->value();
    } else if ("JobId" == node_name) {
      m_jobs_detail.job_id = node->value();
    } else if ("State" == node_name) {
      m_jobs_detail.state = node->value();      
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.creation_time = node->value();       
    } else if ("Lable" == node_name) {
      m_jobs_detail.lable = node->value();
    } else if ("DataId" ==  node_name) {
      m_jobs_detail.data_id = node->value();
    } else if ("UserInfo" ==  node_name) {
      if (!ParseUserInfo(node, &(m_jobs_detail.user_info))) {
        return false;
      }
    } else if ("Object" ==  node_name) {
      m_jobs_detail.object = node->value();
    } else if ("Url" ==  node_name) {
      m_jobs_detail.url = node->value();
    } else if ("Suggestion" ==  node_name) {
      m_jobs_detail.suggestion = StringUtil::StringToInt(node->value());
    } else if ("PageCount" ==  node_name) {
      m_jobs_detail.page_count = StringUtil::StringToInt(node->value());
    } else if ("Labels" ==  node_name) {
      rapidxml::xml_node<>* child_node = node->first_node();
      for (; child_node != NULL; child_node = child_node->next_sibling()) {
        const std::string& child_node_name = node->name();
        if ("PornInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.porn_info))) {
            return false;
          }
        } else if ("AdsInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.ads_info))) {
            return false;
          }
        } else if ("AbuseInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.abuse_info))) {
            return false;
          }
        } else if ("IllegalInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.illegal_info))) {
            return false;
          }    
        } else if ("PoliticsInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.politics_info))) {
            return false;
          }
        } else if ("TerrorismInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.terrorism_info))) {
          return false;
          }           
        } 
      }
    } else if ("PageSegment" == node_name) {
      rapidxml::xml_node<>* child_node = node->first_node();
      for (; child_node != NULL; child_node = child_node->next_sibling()) {
        const std::string& child_node_name = node->name();
        if ("Results" == child_node_name) {
          Result* result = new Results();
          if (!ParseResults(child_node, &(result))) {
            return false;
          }
          m_jobs_detail.page_segment.results.push_back(result);
        }
      }
    }
  }
  return true;
}

bool WebPageAuditingResp::ParseJobsDetail(rapidxml::xml_node<>* root) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" ==  node_name) {
      m_jobs_detail.code = node->value();
    } else if ("Message" ==  node_name) {
      m_jobs_detail.message = node->value();
    } else if ("JobId" == node_name) {
      m_jobs_detail.job_id = node->value();
    } else if ("State" == node_name) {
      m_jobs_detail.state = node->value();      
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.creation_time = node->value();       
    } else if ("Seggestion" == node_name) {
      m_jobs_detail.suggestion = StringUtil::StringToInt(node->value());
    } else if ("Lable" == node_name) {
      m_jobs_detail.lable = node->value();   
    } else if ("DataId" ==  node_name) {
      m_jobs_detail.data_id = node->value();
    } else if ("UserInfo" ==  node_name) {
      if (!ParseUserInfo(node, &(m_jobs_detail.user_info))) {
        return false;
      }
    } else if ("Url" ==  node_name) {
      m_jobs_detail.url = node->value();
    } else if ("PageCount" ==  node_name) {
      m_jobs_detail.page_count = StringUtil::StringToInt(node->value());
    } else if ("Labels" ==  node_name) {
      rapidxml::xml_node<>* child_node = node->first_node();
      for (; child_node != NULL; child_node = child_node->next_sibling()) {
        const std::string& child_node_name = node->name();
        if ("PornInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.porn_info))) {
            return false;
          }
        } else if ("AdsInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.ads_info))) {
            return false;
          }
        } else if ("AbuseInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.abuse_info))) {
            return false;
          }
        } else if ("IllegalInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.illegal_info))) {
            return false;
          }    
        } else if ("PoliticsInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.politics_info))) {
            return false;
          }
        } else if ("TerrorismInfo" == child_node_name) {
          if (!ParseSceneResultInfo(child_node, &(m_jobs_detail.lables.terrorism_info))) {
          return false;
          }           
        } 
    } else if ("ImageResults" == node_name) {
      rapidxml::xml_node<>* child_node = node->first_node();
      for (; child_node != NULL; child_node = child_node->next_sibling()) {
        const std::string& child_node_name = node->name();
        if ("Results" == child_node_name) {
          Result* result = new Results();
          if (!ParseResults(child_node, &(result))) {
            return false;
          }
          m_jobs_detail.image_results.results.push_back(result);
        }
      }
    } else if ("TextResults" == node_name) {
      rapidxml::xml_node<>* child_node = node->first_node();
      for (; child_node != NULL; child_node = child_node->next_sibling()) {
        const std::string& child_node_name = node->name();
        if ("Results" == child_node_name) {
          Result* result = new Results();
          if (!ParseResults(child_node, &(result))) {
            return false;
          }
          m_jobs_detail.text_results.results.push_back(result);
        }
      }
    } else if ("HighlightHtml" == node_name) {
      m_jobs_detail.high_light_html = node->value();
    }
  }
  return true;
}



bool AuditingJobResp::ParseFromXmlString(const std::string& body) {
  std::string tmp_body = body;
  rapidxml::xml_document<> doc;

  if (!StringUtil::StringToXml(&tmp_body[0], &doc)) {
    SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* root = doc.first_node("Response");
  if (NULL == root) {
    SDK_LOG_ERR("Missing root node Response, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = result_node->name();
    if ("RequestId" == node_name) {
      m_request_id = node->value();
    } else if ("JobsDetail" == node_name) {
      if (!ParseJobsDetail(node)) {
        return false;
      }
    }
  }
  return true;  
}


}