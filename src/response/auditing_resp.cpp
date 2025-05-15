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
      segment_result.SetUrl(node->value());
    } else if ("SnapShotTime" == node_name) {
      segment_result.SetSnapShotTime(StringUtil::StringToInt(node->value()));
    } else if ("Text" == node_name) {
      segment_result.SetText(node->value());
    } else if ("OffsetTime" == node_name) {
      segment_result.SetOffsetTime(StringUtil::StringToInt(node->value()));
    } else if ("Duration" == node_name) {
      segment_result.SetDuration(StringUtil::StringToInt(node->value()));
    } else if ("Label" == node_name) {
      segment_result.SetLabel(node->value());
    } else if ("Result" == node_name) {
      segment_result.SetResult(StringUtil::StringToInt(node->value()));
    } else if ("StartByte" == node_name) {
      segment_result.SetStartByte(StringUtil::StringToInt(node->value()));
    } else if ("PornInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      segment_result.SetPornInfo(info);
    } else if ("AdsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      segment_result.SetAdsInfo(info);
    } else if ("IllegalInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      segment_result.SetIllegalInfo(info);
    } else if ("AbuseInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      segment_result.SetAbuseInfo(info);
    } else if ("PoliticsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      segment_result.SetPoliticsInfo(info);
    } else if ("TerrorismInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      segment_result.SetTerrorismInfo(info);
    } else if ("TeenagerInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      segment_result.SetTeenagerInfo(info);
    } else if ("LanguageResults" == node_name) {
      RecognitionResult info = RecognitionResult();
      if (!ParseRecognitionResult(node, info)) {
        return false;
      }
      segment_result.AddLanguageResults(info);
    }
  }
  return true;
}

bool AuditingResp::ParseUserInfo(rapidxml::xml_node<>* root, UserInfo& user_info) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("TokenId" == node_name) {
      user_info.SetTokenId(node->value());
    } else if ("Nickname" == node_name) {
      user_info.SetNickName(node->value());
    } else if ("DeviceId" == node_name) {
      user_info.SetDeviceId(node->value());
    } else if ("AppId" == node_name) {
      user_info.SetAppId(node->value());
    } else if ("Room" == node_name) {
      user_info.SetRoom(node->value());
    } else if ("IP" == node_name) {
      user_info.SetIp(node->value());
    } else if ("Type" == node_name) {
      user_info.SetType(node->value());
    } else if ("ReceiveTokenId" == node_name) {
      user_info.SetReceiveTokenId(node->value());
    } else if ("Gender" == node_name) {
      user_info.SetGender(node->value());
    } else if ("Level" == node_name) {
      user_info.SetLevel(node->value());
    } else if ("Role" == node_name) {
      user_info.SetRole(node->value());
    }
  }
  return true;
}

bool AuditingResp::ParseLocation(rapidxml::xml_node<>* root, Location& location) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("X" == node_name) {
      location.SetX(StringUtil::StringToFloat(node->value()));
    } else if ("Y" == node_name) {
      location.SetY(StringUtil::StringToFloat(node->value()));
    } else if ("Height" == node_name) {
      location.SetHeight(StringUtil::StringToFloat(node->value()));
    } else if ("Width" == node_name) {
      location.SetWidth(StringUtil::StringToFloat(node->value()));
    } else if ("Rotate" == node_name) {
      location.SetRotate(StringUtil::StringToFloat(node->value()));
    }
  }
  return true;
}

bool AuditingResp::ParseObjectResults(rapidxml::xml_node<>* root, ObjectResults& object_results) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Name" == node_name) {
      object_results.SetName(node->value());
    } else if ("Location" == node_name) {
      Location location = Location();
      if (!ParseLocation(node, location)) {
        return false;
      }
      object_results.SetLocation(location);
    }
  }
  return true;
}


bool AuditingResp::ParseOcrResultInfo(rapidxml::xml_node<>* root, OcrResult& ocr_results) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Text" == node_name) {
      ocr_results.SetText(node->value());
    } else if ("Keywords" == node_name) {
      ocr_results.AddKeyWord(node->value());
    } else if ("Location" == node_name) {
      Location location = Location();
      if (!ParseLocation(node, location)) {
        return false;
      }
      ocr_results.SetLocation(location);
    }
  }
  return true;
}

bool AuditingResp::ParseRecognitionResult(rapidxml::xml_node<>* root, RecognitionResult& result) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Label" == node_name) {
      result.SetLable(node->value());
    } else if ("Score" == node_name) {
      result.SetScore(StringUtil::StringToInt(node->value()));
    } else if ("StartTime" == node_name) {
      result.SetStartTime(StringUtil::StringToInt(node->value()));
    } else if ("EndTime" == node_name) {
      result.SetEndTime(StringUtil::StringToInt(node->value()));
    }
  }
  return true;
}

bool AuditingResp::ParseSceneResultInfo(rapidxml::xml_node<>* root, SceneResultInfo& scene_result_info) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" == node_name) {
      scene_result_info.SetCode(StringUtil::StringToInt(node->value()));
    } else if ("Msg" == node_name) {
      scene_result_info.SetMsg(node->value());
    } else if ("Count" == node_name) {
      scene_result_info.SetCount(StringUtil::StringToInt(node->value()));
    } else if ("HitFlag" == node_name) {
      scene_result_info.SetHitFlag(StringUtil::StringToInt(node->value()));
    } else if ("Score" == node_name) {
      scene_result_info.SetScore(StringUtil::StringToInt(node->value()));
    } else if ("Label" == node_name) {
      scene_result_info.SetLabel(node->value());
    } else if ("SubLabel" == node_name) {
      scene_result_info.SetSubLabel(node->value());
    } else if ("Category" == node_name) {
      scene_result_info.SetCategory(node->value());
    } else if ("OcrResults" == node_name) {
      OcrResult ocr_results = OcrResult();
      if (!ParseOcrResultInfo(node, ocr_results)) {
        return false;
      }
      scene_result_info.AddOcrResults(ocr_results);
    } else if ("ObjectResults" == node_name) {
      ObjectResults object_results = ObjectResults();
      if (!ParseObjectResults(node, object_results)) {
        return false;
      }
      scene_result_info.SetObjectResults(object_results);
    } else if ("Keywords" == node_name) {
      scene_result_info.AddKeyWord(node->value());
    } else if ("LibResults" == node_name) {
      rapidxml::xml_node<>* lib_results_node = node->first_node();
      LibResults lib_result = LibResults();
      for (; lib_results_node != NULL; lib_results_node = lib_results_node->next_sibling()) {
        const std::string& lib_results_node_name = lib_results_node->name();
        if ("LibType" == lib_results_node_name) {
          lib_result.SetLibType(StringUtil::StringToInt(lib_results_node->value()));
        } else if ("LibName" == lib_results_node_name) {
          lib_result.SetLibName(lib_results_node->value());
        } else if ("Keywords" == node_name) {
          lib_result.AddKeyWord(lib_results_node->value());
        }
      }
      scene_result_info.AddLibResult(lib_result);
    } else if ("SpeakerResults" == node_name) {
      rapidxml::xml_node<>* results_node = root->first_node();
      RecognitionResult result = RecognitionResult();
      if (!ParseRecognitionResult(node, result)) {
        return false;
      }
      scene_result_info.AddSpeakerResult(result);
    } else if ("RecognitionResults" == node_name) {
      RecognitionResult result = RecognitionResult();
      if (!ParseRecognitionResult(node, result)) {
        return false;
      }
      scene_result_info.AddRecognitionResult(result);
    }
  }
  return true;
}

bool AuditingResp::ParseListInfo(rapidxml::xml_node<>* root, ListInfo& list_info) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("ListResults" == node_name) {
      rapidxml::xml_node<>* result_node = node->first_node();
      ListResult result = ListResult();
      for (; result_node != NULL; result_node = result_node->next_sibling()) {
        const std::string& result_node_name = result_node->name();
        if ("ListType" == result_node_name) {
          result.SetListType(StringUtil::StringToInt(result_node->value()));
        } else if ("ListName" == result_node_name) {
          result.SetListName(result_node->value());
        } else if ("Entity" == result_node_name) {
          result.SetEntity(result_node->value());
        }
      }
      list_info.AddListResult(result);
    }
  }
  return true;
}


bool AuditingResp::ParseResults(rapidxml::xml_node<>* root, Result& result) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Url" == node_name) {
      result.SetUrl(node->value());
    } else if ("PageNumber" == node_name) {
      result.SetPageNumber(StringUtil::StringToInt(node->value()));
    } else if ("Text" == node_name) {
      result.SetText(node->value());
    } else if ("SheetNumber" == node_name) {
      result.SetSheetNumber(StringUtil::StringToInt(node->value()));
    } else if ("Label" == node_name) {
      result.SetLabel(node->value());
    } else if ("Suggestion" == node_name) {
      result.SetSuggestion(StringUtil::StringToInt(node->value()));
    } else if ("PornInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      result.SetPornInfo(info);
    } else if ("AdsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      result.SetAdsInfo(info);
    } else if ("AbuseInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      result.SetAbuseInfo(info);
    } else if ("IllegalInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      result.SetIllegalInfo(info);
    } else if ("PoliticsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      result.SetPoliticsInfo(info);
    } else if ("TerrorismInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      result.SetTerrorismInfo(info);
    }
  }
  return true;
}

bool ImageAuditingResp::ParseImageAuditingJobsDetail(rapidxml::xml_node<> *root, ImageAuditingJobsDetail &jobs_detail) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    SDK_LOG_INFO("node_name {%s} node_value {%s}", node_name.c_str(), node->value());
    if ("Code" ==  node_name) {
      jobs_detail.SetCode(node->value());
    } else if ("Message" ==  node_name) {
      jobs_detail.SetMessage(node->value());
    } else if ("JobId" == node_name) {
      jobs_detail.SetJobId(node->value());
    } else if ("State" == node_name) {
      jobs_detail.SetState(node->value());
    } else if ("CreationTime" == node_name) {
      jobs_detail.SetCreationTime(node->value());
    } else if ("CompressionResult" == node_name) {
      jobs_detail.SetCompressionResult(StringUtil::StringToInt(node->value()));
    } else if ("Result" == node_name) {
      jobs_detail.SetResult(StringUtil::StringToInt(node->value()));
    } else if ("Label" == node_name) {
      jobs_detail.SetLabel(node->value());
    } else if ("Category" == node_name) {
      jobs_detail.SetCategory(node->value());
    } else if ("SubLabel" == node_name) {
      jobs_detail.SetSubLabel(node->value());
    } else if ("Text" == node_name) {
      jobs_detail.SetText(node->value());
    } else if ("PornInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      jobs_detail.SetPornInfo(info);
    } else if ("AdsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      jobs_detail.SetAdsInfo(info);
    } else if ("AbuseInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      jobs_detail.SetAbuseInfo(info);
    } else if ("IllegalInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      jobs_detail.SetIllegalInfo(info);
    } else if ("PoliticsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      jobs_detail.SetPoliticsInfo(info);
    } else if ("TerrorismInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      jobs_detail.SetTerrorismInfo(info);
    } else if ("QualityInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      jobs_detail.SetTerrorismInfo(info);   
    } else if ("DataId" ==  node_name) {
      jobs_detail.SetDataId(node->value());
    } else if ("UserInfo" ==  node_name) {
      UserInfo user_info = UserInfo();
      if (!ParseUserInfo(node, user_info)) {
        return false;
      }
      jobs_detail.SetUserInfo(user_info);
    } else if ("Object" ==  node_name) {
      jobs_detail.SetObject(node->value());
    } else if ("ForbidState" ==  node_name) {
      jobs_detail.SetForbitState(StringUtil::StringToInt(node->value()));     
    } else if ("ListInfo" ==  node_name) {
      ListInfo info = ListInfo();
      ParseListInfo(node, info);
      jobs_detail.SetListInfo(info);     
    } else if ("Url" ==  node_name) {
      jobs_detail.SetUrl(node->value());
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

  if (!ParseImageAuditingJobsDetail(root, m_jobs_detail)) {
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
    const std::string& node_name = node->name();
    if ("RequestId" == node_name) {
      m_request_id = node->value();
    } else if ("JobsDetail" == node_name) {
      ImageAuditingJobsDetail jobs_detail;
      if (!ParseImageAuditingJobsDetail(node, jobs_detail)) {
        return false;
      }
      m_jobs_details.push_back(jobs_detail);
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
    const std::string& node_name = node->name();
    if ("RequestId" == node_name) {
      m_request_id = node->value();
    } else if ("JobsDetail" == node_name) {
      ImageAuditingJobsDetail jobs_detail;
      if (!ParseImageAuditingJobsDetail(node, m_jobs_detail)) {
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
      m_jobs_detail.SetCode(node->value());
    } else if ("Message" ==  node_name) {
      m_jobs_detail.SetMessage(node->value());
    } else if ("JobId" == node_name) {
      m_jobs_detail.SetJobId(node->value());
    } else if ("State" == node_name) {
      m_jobs_detail.SetState(node->value());
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.SetCreationTime(node->value());
    } else if ("Result" == node_name) {
      m_jobs_detail.SetResult(StringUtil::StringToInt(node->value()));
    } else if ("Label" == node_name) {
      m_jobs_detail.SetLabel(node->value());
    } else if ("PornInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetPornInfo(info);
    } else if ("AdsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetAdsInfo(info);
    } else if ("AbuseInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetAbuseInfo(info);
    } else if ("IllegalInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetIllegalInfo(info);
    } else if ("PoliticsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetPoliticsInfo(info);
    } else if ("TerrorismInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetTerrorismInfo(info);
    } else if ("DataId" ==  node_name) {
      m_jobs_detail.SetDataId(node->value());
    } else if ("UserInfo" ==  node_name) {
      UserInfo user_info = UserInfo();
      if (!ParseUserInfo(node, user_info)) {
        return false;
      }
      m_jobs_detail.SetUserInfo(user_info);
    } else if ("Object" ==  node_name) {
      m_jobs_detail.SetObject(node->value());
    } else if ("Url" ==  node_name) {
      m_jobs_detail.SetUrl(node->value());
    } else if ("SnapshotCount" == node_name ) {
      m_jobs_detail.SetSnapShotCount(node->value());
    } else if ("Snapshot" == node_name) {
      SegmentResult snap_shot;
      if (!ParseSegmentResult(node, snap_shot)) {
        return false;
      }
      m_jobs_detail.AddSnapShot(snap_shot);
    } else if ("AudioSection" == node_name) {
      SegmentResult audio_section;
      if (!ParseSegmentResult(node, audio_section)) {
        return false;
      }
      m_jobs_detail.AddAudioSection(audio_section);
    } else if ("ForbidState" ==  node_name) {
      m_jobs_detail.SetForbitState(StringUtil::StringToInt(node->value()));    
    } else if ("ListInfo" ==  node_name) {
      ListInfo info = ListInfo();
      ParseListInfo(node, info);
      m_jobs_detail.SetListInfo(info);
    } else if ("UserInfo" ==  node_name) {
      UserInfo info = UserInfo();
      ParseUserInfo(node, info);
      m_jobs_detail.SetUserInfo(info);
    }
  }
  return true;
}


bool AudioAuditingResp::ParseJobsDetail(rapidxml::xml_node<>* root) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" ==  node_name) {
      m_jobs_detail.SetCode(node->value());
    } else if ("Message" ==  node_name) {
      m_jobs_detail.SetMessage(node->value());
    } else if ("JobId" == node_name) {
      m_jobs_detail.SetJobId(node->value());
    } else if ("State" == node_name) {
      m_jobs_detail.SetState(node->value());
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.SetCreationTime(node->value());
    } else if ("Result" == node_name) {
      m_jobs_detail.SetResult(StringUtil::StringToInt(node->value()));
    } else if ("Label" == node_name) {
      m_jobs_detail.SetLabel(node->value());
    } else if ("AudioText" == node_name) {
      m_jobs_detail.SetAudioText(node->value());
    } else if ("PornInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetPornInfo(info);
    } else if ("AdsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetAdsInfo(info);
    } else if ("AbuseInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetAbuseInfo(info);
    } else if ("IllegalInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetIllegalInfo(info);
    } else if ("PoliticsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetPoliticsInfo(info);
    } else if ("TerrorismInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetTerrorismInfo(info);
    } else if ("TeenagerInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetTeenagerInfo(info);   
    } else if ("DataId" ==  node_name) {
      m_jobs_detail.SetDataId(node->value());
    } else if ("UserInfo" ==  node_name) {
      UserInfo user_info = UserInfo();
      if (!ParseUserInfo(node, user_info)) {
        return false;
      }
      m_jobs_detail.SetUserInfo(user_info);
    } else if ("Object" ==  node_name) {
      m_jobs_detail.SetObject(node->value());
    } else if ("Url" ==  node_name) {
      m_jobs_detail.SetUrl(node->value());
    } else if ("SubLabel" ==  node_name) {
      m_jobs_detail.SetSubLabel(node->value());   
    } else if ("Section" == node_name) {
      SegmentResult section;
      if (!ParseSegmentResult(node, section)) {
        return false;
      }
      m_jobs_detail.AddSection(section);
    } else if ("ForbidState" ==  node_name) {
      m_jobs_detail.SetForbitState(StringUtil::StringToInt(node->value()));    
    } else if ("ListInfo" ==  node_name) {
      ListInfo info = ListInfo();
      ParseListInfo(node, info);
      m_jobs_detail.SetListInfo(info);
    } else if ("UserInfo" ==  node_name) {
      UserInfo info = UserInfo();
      ParseUserInfo(node, info);
      m_jobs_detail.SetUserInfo(info);
    }
  }
  return true;
}

bool TextAuditingResp::ParseJobsDetail(rapidxml::xml_node<>* root) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" ==  node_name) {
      m_jobs_detail.SetCode(node->value());
    } else if ("Message" ==  node_name) {
      m_jobs_detail.SetMessage(node->value());
    } else if ("JobId" == node_name) {
      m_jobs_detail.SetJobId(node->value());
    } else if ("State" == node_name) {
      m_jobs_detail.SetState(node->value());
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.SetCreationTime(node->value());
    } else if ("Result" == node_name) {
      m_jobs_detail.SetResult(StringUtil::StringToInt(node->value()));
    } else if ("SectionCount" == node_name) {
      m_jobs_detail.SetSectionCount(StringUtil::StringToInt(node->value()));
    } else if ("Label" == node_name) {
      m_jobs_detail.SetLabel(node->value());
    } else if ("Content" == node_name) {
      m_jobs_detail.SetContent(node->value());
    } else if ("PornInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetPornInfo(info);
    } else if ("AdsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetAdsInfo(info);
    } else if ("AbuseInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetAbuseInfo(info);
    } else if ("IllegalInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetIllegalInfo(info);
    } else if ("PoliticsInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetPoliticsInfo(info);
    } else if ("TerrorismInfo" == node_name) {
      SceneResultInfo info = SceneResultInfo();
      if (!ParseSceneResultInfo(node, info)) {
        return false;
      }
      m_jobs_detail.SetTerrorismInfo(info);
    } else if ("DataId" ==  node_name) {
      m_jobs_detail.SetDataId(node->value());
    } else if ("UserInfo" ==  node_name) {
      UserInfo user_info = UserInfo();
      if (!ParseUserInfo(node, user_info)) {
        return false;
      }
      m_jobs_detail.SetUserInfo(user_info);
    } else if ("Object" ==  node_name) {
      m_jobs_detail.SetObject(node->value());
    } else if ("Section" == node_name) {
      SegmentResult section;
      if (!ParseSegmentResult(node, section)) {
        return false;
      }
      m_jobs_detail.AddSection(section);
    } else if ("ForbidState" ==  node_name) {
      m_jobs_detail.SetForbitState(StringUtil::StringToInt(node->value()));     
    } else if ("ListInfo" ==  node_name) {
      ListInfo info = ListInfo();
      ParseListInfo(node, info);
      m_jobs_detail.SetListInfo(info);
    } else if ("UserInfo" ==  node_name) {
      UserInfo info = UserInfo();
      ParseUserInfo(node, info);
      m_jobs_detail.SetUserInfo(info);     
    }
  }
  return true;
}


bool DocumentAuditingResp::ParseJobsDetail(rapidxml::xml_node<>* root) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" ==  node_name) {
      m_jobs_detail.SetCode(node->value());
    } else if ("Message" ==  node_name) {
      m_jobs_detail.SetMessage(node->value());
    } else if ("JobId" == node_name) {
      m_jobs_detail.SetJobId(node->value());
    } else if ("State" == node_name) {
      m_jobs_detail.SetState(node->value());
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.SetCreationTime(node->value());
    } else if ("Label" == node_name) {
      m_jobs_detail.SetLabel(node->value());
    } else if ("DataId" ==  node_name) {
      m_jobs_detail.SetDataId(node->value());
    } else if ("UserInfo" ==  node_name) {
      UserInfo user_info;
      if (!ParseUserInfo(node, user_info)) {
        return false;
      }
      m_jobs_detail.SetUserInfo(user_info);
    } else if ("Object" ==  node_name) {
      m_jobs_detail.SetObject(node->value());
    } else if ("Url" ==  node_name) {
      m_jobs_detail.SetUrl(node->value());
    } else if ("Suggestion" ==  node_name) {
      m_jobs_detail.SetSuggestion(StringUtil::StringToInt(node->value()));
    } else if ("PageCount" ==  node_name) {
      m_jobs_detail.SetPageCount(StringUtil::StringToInt(node->value()));
    } else if ("Labels" ==  node_name) {
      Labels labels = Labels();
      rapidxml::xml_node<>* child_node = node->first_node();
      for (; child_node != NULL; child_node = child_node->next_sibling()) {
        const std::string& child_node_name = child_node->name();
        if ("PornInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetPornInfo(info);
        } else if ("AdsInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetAdsInfo(info);
        } else if ("AbuseInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetAbuseInfo(info);
        } else if ("IllegalInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetIllegalInfo(info);
        } else if ("PoliticsInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetPoliticsInfo(info);
        } else if ("TerrorismInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetTerrorismInfo(info);
        }
      }
      m_jobs_detail.SetLabels(labels);
    } else if ("PageSegment" == node_name) {
      PageSegment page_segment = PageSegment();
      rapidxml::xml_node<>* child_node = node->first_node();
      for (; child_node != NULL; child_node = child_node->next_sibling()) {
        const std::string& child_node_name = child_node->name();
        if ("Results" == child_node_name) {
          Result result = Result();
          if (!ParseResults(child_node, result)) {
            return false;
          }
          page_segment.AddResult(result);
        }
      }
      m_jobs_detail.SetPageSegment(page_segment);
    } else if ("ForbidState" ==  node_name) {
      m_jobs_detail.SetForbitState(StringUtil::StringToInt(node->value()));    
    } else if ("ListInfo" ==  node_name) {
      ListInfo info = ListInfo();
      ParseListInfo(node, info);
      m_jobs_detail.SetListInfo(info);
    } else if ("UserInfo" ==  node_name) {
      UserInfo info = UserInfo();
      ParseUserInfo(node, info);
      m_jobs_detail.SetUserInfo(info);     
    }
  }
  return true;
}

bool WebPageAuditingResp::ParseJobsDetail(rapidxml::xml_node<>* root) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Code" ==  node_name) {
      m_jobs_detail.SetCode(node->value());
    } else if ("Message" ==  node_name) {
      m_jobs_detail.SetMessage(node->value());
    } else if ("JobId" == node_name) {
      m_jobs_detail.SetJobId(node->value());
    } else if ("State" == node_name) {
      m_jobs_detail.SetState(node->value());
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.SetCreationTime(node->value());
    } else if ("Suggestion" == node_name) {
      m_jobs_detail.SetSuggestion(StringUtil::StringToInt(node->value()));
    } else if ("Label" == node_name) {
      m_jobs_detail.SetLabel(node->value());
    } else if ("DataId" ==  node_name) {
      m_jobs_detail.SetDataId(node->value());
    } else if ("UserInfo" ==  node_name) {
      UserInfo user_info = UserInfo();
      if (!ParseUserInfo(node, user_info)) {
        return false;
      }
      m_jobs_detail.SetUserInfo(user_info);
    } else if ("Url" ==  node_name) {
      m_jobs_detail.SetUrl(node->value());
    } else if ("PageCount" ==  node_name) {
      m_jobs_detail.SetPageCount(StringUtil::StringToInt(node->value()));
    } else if ("Labels" ==  node_name) {
      Labels labels = Labels();
      rapidxml::xml_node<>* child_node = node->first_node();
      for (; child_node != NULL; child_node = child_node->next_sibling()) {
        const std::string &child_node_name = child_node->name();
        if ("PornInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetPornInfo(info);
        } else if ("AdsInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetAdsInfo(info);
        } else if ("AbuseInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetAbuseInfo(info);
        } else if ("IllegalInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetIllegalInfo(info);
        } else if ("PoliticsInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetPoliticsInfo(info);
        } else if ("TerrorismInfo" == child_node_name) {
          SceneResultInfo info = SceneResultInfo();
          if (!ParseSceneResultInfo(child_node, info)) {
            return false;
          }
          labels.SetTerrorismInfo(info);
        }
      }
      m_jobs_detail.SetLabels(labels);
    } else if ("ImageResults" == node_name) {
      PageSegment image_results = PageSegment();
      rapidxml::xml_node<>* child_node = node->first_node();
      for (; child_node != NULL; child_node = child_node->next_sibling()) {
        const std::string& child_node_name = child_node->name();
        if ("Results" == child_node_name) {
          Result result = Result();
          if (!ParseResults(child_node, result)) {
            return false;
          }
          image_results.AddResult(result);
        }
      }
      m_jobs_detail.SetImageResults(image_results);
    } else if ("TextResults" == node_name) {
      PageSegment text_results = PageSegment();
      rapidxml::xml_node<>* child_node = node->first_node();
      for (; child_node != NULL; child_node = child_node->next_sibling()) {
        const std::string& child_node_name = child_node->name();
        if ("Results" == child_node_name) {
          Result result = Result();
          if (!ParseResults(child_node, result)) {
            return false;
          }
          text_results.AddResult(result);
        }
      }
      m_jobs_detail.SetTextResults(text_results);
    } else if ("HighlightHtml" == node_name) {
      m_jobs_detail.SetHighLightHtml(node->value());    
    } else if ("ListInfo" ==  node_name) {
      ListInfo info = ListInfo();
      ParseListInfo(node, info);
      m_jobs_detail.SetListInfo(info);
    } else if ("UserInfo" ==  node_name) {
      UserInfo info = UserInfo();
      ParseUserInfo(node, info);
      m_jobs_detail.SetUserInfo(info);     
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
    const std::string& node_name = node->name();
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