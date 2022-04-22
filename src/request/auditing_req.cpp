#include "request/auditing_req.h"

#include "cos_sys_config.h"
#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

namespace qcloud_cos {

bool BatchImageAuditingReq::GenerateRequestBody(std::string* body) const {
  if (m_inputs.size() = 0) {
    SDK_LOG_ERR("Input is empty.");
    return false;
  }
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* root_node = doc.allocate_node(
      rapidxml::node_element, doc.allocate_string("Request"), NULL);
  doc.append_node(root_node);

  for (std::vector<Input>::const_iterator c_itr = m_inputs.begin();
       c_itr != m_inputs.end(); ++c_itr) {
    const Input& input = *c_itr;
    rapidxml::xml_node<>* input_node =
        doc.allocate_node(rapidxml::node_element, "Input", NULL);
    if (input.HasObject()) {
      input_node->append_node(rapidxml::node_element, "Object",
                              doc.allocate_string(input.GetObject().c_str()));
    }
    if (input.HasUrl()) {
      input_node->append_node(rapidxml::node_element, "Url",
                              doc.allocate_string(input.GetUrl().c_str()));
    }
    if (input.HasDataId()) {
      input_node->append_node(rapidxml::node_element, "DataId",
                              doc.allocate_string(input.GetDataId().c_str()));
    }
    if (input.HasInterval()) {
      input_node->append_node(rapidxml::node_element, "Interval",
                              doc.allocate_string(StringUtil::IntToString(input.GetInterval()).c_str()));
    }
    if (input.HasMaxFrames()) {
      input_node->append_node(rapidxml::node_element, "MaxFrames",
                              doc.allocate_string(StringUtil::IntToString(input.GetMaxFrames()).c_str()));
    }
    if (input.HasLargeImageDetect()) {
      input_node->append_node(rapidxml::node_element, "LargeImageDetect",
                              doc.allocate_string(StringUtil::IntToString(input.GetLargeImageDetect()).c_str()));
    }

    if (input.HasUserInfo()) {
      rapidxml::xml_node<>* user_info_node = doc.allocate_node(
          rapidxml::node_element, "UserInfo", NULL);
      const UserInfo user_info = input.GetUserInfo();
      if (!user_info.token_id.empty()) {
        user_info_node->append_node(rapidxml::node_element, "TokenId",
                                    doc.allocate_string(user_info.token_id.c_str()));        
      }
      if (!user_info.nick_name.empty()) {
        user_info_node->append_node(rapidxml::node_element, "Nickname",
                                    doc.allocate_string(user_info.nick_name.c_str()));        
      }
      if (!user_info.device_id.empty()) {
        user_info_node->append_node(rapidxml::node_element, "DeviceId",
                                    doc.allocate_string(user_info.device_id.c_str()));        
      }      
      if (!user_info.app_id.empty()) {
        user_info_node->append_node(rapidxml::node_element, "AppId",
                                    doc.allocate_string(user_info.app_id.c_str()));        
      }
      if (!user_info.room.empty()) {
        user_info_node->append_node(rapidxml::node_element, "Room",
                                    doc.allocate_string(user_info.room.c_str()));        
      }      
      if (!user_info.ip.empty()) {
        user_info_node->append_node(rapidxml::node_element, "IP",
                                    doc.allocate_string(user_info.ip.c_str()));        
      }
      if (!user_info.type.empty()) {
        user_info_node->append_node(rapidxml::node_element, "Type",
                                    doc.allocate_string(user_info.type.c_str()));        
      }  
      input_node->append_node(user_info_node);          
    }
    root_node->append_node(input_node);
  }

  rapidxml::xml_node<>* conf_node = doc.allocate_node(
      rapidxml::node_element, doc.allocate_string("Conf"), NULL);  
  if (m_conf.HasBizType()) {
    conf_node->append_node(rapidxml::node_element, "BizType",
                           doc.allocate_string(m_conf.GetBizType().c_str()));     
  }
  if (m_conf.HasDetectType()) {
    conf_node->append_node(rapidxml::node_element, "DetectType",
                           doc.allocate_string(m_conf.GetDetectType().c_str()));     
  }
  root_node->append_node(conf_node);
  
  rapidxml::print(std::back_inserter(*body), doc, 0);
  doc.clear();

  return true;
}

bool CreateAudtingJobReq::GenerateRequestBody(std::string* body) const {
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* root_node = doc.allocate_node(
      rapidxml::node_element, doc.allocate_string("Request"), NULL);
  doc.append_node(root_node);

  rapidxml::xml_node<>* input_node =
      doc.allocate_node(rapidxml::node_element, "Input", NULL);
  if (input.HasObject()) {
    input_node->append_node(rapidxml::node_element, "Object",
                            doc.allocate_string(input.GetObject().c_str()));
  }
  if (input.HasUrl()) {
    input_node->append_node(rapidxml::node_element, "Url",
                            doc.allocate_string(input.GetUrl().c_str()));
  }
  if (input.HasDataId()) {
    input_node->append_node(rapidxml::node_element, "DataId",
                            doc.allocate_string(input.GetDataId().c_str()));
  }
  if (input.HasContent()) {
    input_node->append_node(rapidxml::node_element, "Content",
                              doc.allocate_string(input.GetContent().c_str()));
  }
  if (input.HasType()) {
    input_node->append_node(rapidxml::node_element, "Type",
                            doc.allocate_string(input.GetType().c_str()));
  }
  if (input.HasUserInfo()) {
    rapidxml::xml_node<>* user_info_node = doc.allocate_node(
        rapidxml::node_element, "UserInfo", NULL);
    const UserInfo user_info = input.GetUserInfo();
    if (!user_info.token_id.empty()) {
      user_info_node->append_node(rapidxml::node_element, "TokenId",
                                  doc.allocate_string(user_info.token_id.c_str()));        
    }
    if (!user_info.nick_name.empty()) {
      user_info_node->append_node(rapidxml::node_element, "Nickname",
                                  doc.allocate_string(user_info.nick_name.c_str()));        
    }
    if (!user_info.device_id.empty()) {
      user_info_node->append_node(rapidxml::node_element, "DeviceId",
                                  doc.allocate_string(user_info.device_id.c_str()));        
    }      
    if (!user_info.app_id.empty()) {
      user_info_node->append_node(rapidxml::node_element, "AppId",
                                  doc.allocate_string(user_info.app_id.c_str()));        
    }
    if (!user_info.room.empty()) {
      user_info_node->append_node(rapidxml::node_element, "Room",
                                  doc.allocate_string(user_info.room.c_str()));        
    }      
    if (!user_info.ip.empty()) {
      user_info_node->append_node(rapidxml::node_element, "IP",
                                  doc.allocate_string(user_info.ip.c_str()));        
    }
    if (!user_info.type.empty()) {
      user_info_node->append_node(rapidxml::node_element, "Type",
                                  doc.allocate_string(user_info.type.c_str()));
    }  
    input_node->append_node(user_info_node);          
  }
  root_node->append_node(input_node);

  rapidxml::xml_node<>* conf_node = doc.allocate_node(
      rapidxml::node_element, doc.allocate_string("Conf"), NULL);  
  if (m_conf.HasBizType()) {
    conf_node->append_node(rapidxml::node_element, "BizType",
                           doc.allocate_string(m_conf.GetBizType().c_str()));     
  }
  if (m_conf.HasDetectType()) {
    conf_node->append_node(rapidxml::node_element, "DetectType",
                           doc.allocate_string(m_conf.GetDetectType().c_str()));     
  }
  if (m_conf.HasSnapShot()) {
    rapidxml::xml_node<>* snap_shot_node = doc.allocate_node(
        rapidxml::node_element, doc.allocate_string("SnapShot"), NULL);  
    if (!m_conf.GetSnapShot().mode.empty()) {
      snap_shot_node->append_node(rapidxml::node_element, "Mode",
                                  doc.allocate_string(m_conf.GetSnapShot().mode.c_str())); 
    }
    if (m_conf.GetSnapShot().count > 0) {
      snap_shot_node->append_node(rapidxml::node_element, "Count",
                                  doc.allocate_string(m_conf.GetSnapShot().mode.c_str())); 
    }
    if (m_conf.GetSnapShot().time_interval > 0) {
      snap_shot_node->append_node(rapidxml::node_element, "TimeInterval",
                                  doc.allocate_string(m_conf.GetSnapShot().time_interval.c_str())); 
    }
    conf_node->append_node(snap_shot_node);
  }
  if (m_conf.HasCallback()) {
    conf_node->append_node(rapidxml::node_element, "Callbcak",
                           doc.allocate_string(m_conf.GetCallBack().c_str()));         
  }
  if (m_conf.HasCallbackVersion()) {
    conf_node->append_node(rapidxml::node_element, "CallbcakVersion",
                           doc.allocate_string(m_conf.GetCallBackVersion().c_str()));         
  }
  if (m_conf.HasDetectContent()) {
    conf_node->append_node(rapidxml::node_element, "DetectContent",
                           doc.allocate_string(m_conf.GetDetectContent().c_str()));         
  }
  root_node->append_node(conf_node);
  
  rapidxml::print(std::back_inserter(*body), doc, 0);
  doc.clear();

  return true;
}

}
