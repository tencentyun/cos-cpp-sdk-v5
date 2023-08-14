#include "response/data_process_resp.h"

#include "cos_params.h"
#include "cos_sys_config.h"
#include "util/string_util.h"

namespace qcloud_cos {

bool ImageRespBase::ParseFromXmlString(const std::string& body) {
  std::string tmp_body = body;
  rapidxml::xml_document<> doc;

  if (!StringUtil::StringToXml(&tmp_body[0], &doc)) {
    SDK_LOG_ERR("Parse string to xml doc error, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* root = doc.first_node("UploadResult");
  if (NULL == root) {
    SDK_LOG_ERR("Missing root node UploadResult, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* result_node = root->first_node();
  for (; result_node != NULL; result_node = result_node->next_sibling()) {
    const std::string& node_name = result_node->name();
    if ("OriginalInfo" == node_name) {
      ParseOriginalInfo(result_node);
    } else if ("ProcessResults" == node_name) {
      rapidxml::xml_node<>* process_result_node = result_node->first_node();
      for (; process_result_node != NULL;
           process_result_node = process_result_node->next_sibling()) {
        const std::string& process_result_node_name =
            process_result_node->name();
        if (process_result_node_name == "Object") {
          Object object;
          ParseObject(process_result_node, object);
          m_upload_result.process_result.objects.push_back(object);
        }
      }
    } else {
      SDK_LOG_WARN("Unknown field in UploadResult, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool ImageRespBase::ParseOriginalInfo(rapidxml::xml_node<>* root) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if (node_name == "Key") {
      m_upload_result.original_info.key = node->value();
    } else if (node_name == "Location") {
      m_upload_result.original_info.location = node->value();
    } else if (node_name == "ETag") {
      m_upload_result.original_info.etag = node->value();
    } else if (node_name == "ImageInfo") {
      rapidxml::xml_node<>* image_info_node = node->first_node();
      for (; image_info_node != NULL;
           image_info_node = image_info_node->next_sibling()) {
        const std::string& image_info_node_name = image_info_node->name();
        if (image_info_node_name == "Format") {
          m_upload_result.original_info.image_info.format =
              image_info_node->value();
        } else if (image_info_node_name == "Width") {
          m_upload_result.original_info.image_info.width =
              StringUtil::StringToInt(image_info_node->value());
        } else if (image_info_node_name == "Height") {
          m_upload_result.original_info.image_info.height =
              StringUtil::StringToInt(image_info_node->value());
        } else if (image_info_node_name == "Quality") {
          m_upload_result.original_info.image_info.quality =
              StringUtil::StringToInt(image_info_node->value());
        } else if (image_info_node_name == "Ave") {
          m_upload_result.original_info.image_info.ave =
              image_info_node->value();
        } else if (image_info_node_name == "Orientation") {
          m_upload_result.original_info.image_info.orientation =
              StringUtil::StringToInt(image_info_node->value());
        } else {
          SDK_LOG_WARN("Unknown field in ImageInfo, field_name=%s",
                       image_info_node_name.c_str());
        }
      }
    } else {
      SDK_LOG_WARN("Unknown field in OriginalInfo, field_name=%s",
                   node_name.c_str());
    }
  }
  return true;
}

bool ImageRespBase::ParseObject(rapidxml::xml_node<>* root, Object& object) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if (node_name == "Key") {
      object.key = node->value();
    } else if (node_name == "Location") {
      object.location = node->value();
    } else if (node_name == "Format") {
      object.format = node->value();
    } else if (node_name == "Width") {
      object.width = StringUtil::StringToInt(node->value());
    } else if (node_name == "Height") {
      object.height = StringUtil::StringToInt(node->value());
    } else if (node_name == "Size") {
      object.size = StringUtil::StringToInt(node->value());
    } else if (node_name == "Quality") {
      object.quality = StringUtil::StringToInt(node->value());
    } else if (node_name == "ETag") {
      object.etag = node->value();
    } else if (node_name == "CodeStatus") {
      object.code_status = StringUtil::StringToInt(node->value());
    } else if (node_name == "WatermarkStatus") {
      object.watermark_status = StringUtil::StringToInt(node->value());
    } else if (node_name == "QRcodeInfo") {
      QRcodeInfo qr_code;
      ParseQRcodeInfo(node, qr_code);
      object.qr_code_info.push_back(qr_code);
    } else {
      SDK_LOG_WARN("Unknown field in Object, field_name=%s", node_name.c_str());
    }
  }
  return true;
}

bool ImageRespBase::ParseQRcodeInfo(rapidxml::xml_node<>* root,
                                    QRcodeInfo& qr_code) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if (node_name == "CodeUrl") {
      qr_code.code_url = node->value();
    } else if (node_name == "CodeLocation") {
      rapidxml::xml_node<>* code_location_node = node->first_node();
      for (; code_location_node != NULL;
           code_location_node = code_location_node->next_sibling()) {
        const std::string& code_location_node_name = code_location_node->name();
        if (code_location_node_name == "Point") {
          qr_code.code_location.points.push_back(code_location_node->value());
        } else {
          SDK_LOG_WARN("Unknown field in CodeLocation, field_name=%s",
                       code_location_node_name.c_str());
        }
      }
    } else {
      SDK_LOG_WARN("Unknown field in ParseQRcodeInfo, field_name=%s",
                   node_name.c_str());
    }
  }
  return true;
}

bool GetQRcodeResp::ParseFromXmlString(const std::string& body) {
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

  rapidxml::xml_node<>* result_node = root->first_node();
  for (; result_node != NULL; result_node = result_node->next_sibling()) {
    const std::string& node_name = result_node->name();
    if ("CodeStatus" == node_name) {
      m_result.code_status = StringUtil::StringToInt(result_node->value());
    } else if ("QRcodeInfo" == node_name) {
      QRcodeInfo qr_info;
      ImageRespBase::ParseQRcodeInfo(result_node, qr_info);
      m_result.qr_code_info.push_back(qr_info);

    } else if ("ResultImage" == node_name) {
      m_result.result_image = result_node->value();
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool DescribeDocProcessBucketsResp::ParseFromXmlString(
    const std::string& body) {
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

  rapidxml::xml_node<>* result_node = root->first_node();
  for (; result_node != NULL; result_node = result_node->next_sibling()) {
    const std::string& node_name = result_node->name();
    if ("RequestId" == node_name) {
      m_result.request_id = result_node->value();
    } else if ("TotalCount" == node_name) {
      m_result.total_count = StringUtil::StringToInt(result_node->value());
    } else if ("PageNumber" == node_name) {
      m_result.page_number = StringUtil::StringToInt(result_node->value());
    } else if ("PageSize" == node_name) {
      m_result.page_size = StringUtil::StringToInt(result_node->value());
    } else if ("DocBucketList" == node_name) {
      BucketInfo bucket_info;
      ParseBucketInfo(result_node, bucket_info);
      m_result.doc_bucket_list.push_back(bucket_info);
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool CreateDocBucketResp::ParseFromXmlString(const std::string& body) {
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
      m_result.request_id = node->value();
    } else if ("DocBucket" == node_name) {
      BucketInfo bucket_info;
      DescribeDocProcessBucketsResp::ParseBucketInfo(node, bucket_info);
      m_result.doc_bucket = bucket_info;
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool DescribeDocProcessBucketsResp::ParseBucketInfo(rapidxml::xml_node<>* root,
                                                    BucketInfo& bucket_info) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if (node_name == "BucketId") {
      bucket_info.bucket_id = node->value();
    } else if (node_name == "Name") {
      bucket_info.name = node->value();
    } else if (node_name == "Region") {
      bucket_info.region = node->value();
    } else if (node_name == "CreateTime") {
      bucket_info.create_time = node->value();
    } else if (node_name == "AliasBucketId") {
      bucket_info.alias_bucket_id = node->value();
    } else {
      SDK_LOG_WARN("Unknown field in BucketInfo, field_name=%s",
                   node_name.c_str());
    }
  }
  return true;
}

void DocPreviewResp::ParseFromHeaders(
    const std::map<std::string, std::string>& headers) {
  BaseResp::ParseFromHeaders(headers);
  std::map<std::string, std::string>::const_iterator itr;
  itr = headers.find(kRespHeaderXTotalPage);
  if (headers.end() != itr) {
    m_x_total_page = StringUtil::StringToUint32(itr->second);
  }

  itr = headers.find(kRespHeaderXErrNo);
  if (headers.end() != itr) {
    m_x_errno = itr->second;
  }

  itr = headers.find(kRespHeaderXTotalSheet);
  if (headers.end() != itr) {
    m_x_total_sheet = StringUtil::StringToUint32(itr->second);
  }

  itr = headers.find(kRespHeaderXSheetName);
  if (headers.end() != itr) {
    m_sheet_name = itr->second;
  }
}

bool DocProcessJobBase::ParseFromXmlString(const std::string& body) {
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

  rapidxml::xml_node<>* jobs_detail_root = root->first_node("JobsDetail");
  if (NULL == jobs_detail_root) {
    SDK_LOG_ERR("Missing node JobsDetail, xml_body=%s", body.c_str());
    return false;
  }

  ParseJobsDetail(jobs_detail_root, m_jobs_detail);

  return true;
}

bool DocProcessJobBase::ParseJobsDetail(rapidxml::xml_node<>* root,
                                        JobsDetail& jobs_detail) {
  rapidxml::xml_node<>* jobs_detail_node = root->first_node();
  for (; jobs_detail_node != NULL;
       jobs_detail_node = jobs_detail_node->next_sibling()) {
    const std::string& node_name = jobs_detail_node->name();
    if ("Code" == node_name) {
      jobs_detail.code = jobs_detail_node->value();
    } else if ("Message" == node_name) {
      jobs_detail.message = jobs_detail_node->value();
    } else if ("JobId" == node_name) {
      jobs_detail.job_id = jobs_detail_node->value();
    } else if ("Tag" == node_name) {
      jobs_detail.tag = jobs_detail_node->value();
    } else if ("State" == node_name) {
      jobs_detail.state = jobs_detail_node->value();
    } else if ("CreationTime" == node_name) {
      jobs_detail.create_time = jobs_detail_node->value();
    } else if ("EndTime" == node_name) {
      jobs_detail.end_time = jobs_detail_node->value();
    } else if ("QueueId" == node_name) {
      jobs_detail.queue_id = jobs_detail_node->value();
    } else if ("Input" == node_name) {
      rapidxml::xml_node<>* object_node =
          jobs_detail_node->first_node("Object");
      if (NULL == object_node) {
        SDK_LOG_ERR("Missing node Object, node_name=%s", node_name.c_str());
        continue;
      }
      jobs_detail.input.object = object_node->value();
    } else if ("Operation" == node_name) {
      ParseOperation(jobs_detail_node, jobs_detail.operation);
    }
  }
  return true;
}

bool DocProcessJobBase::ParseOperation(rapidxml::xml_node<>* root,
                                       Operation& operation) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if (node_name == "DocProcess") {
      ParseDocProcess(node, operation.doc_process);
    } else if (node_name == "DocProcessResult") {
      ParseDocProcessResult(node, operation.doc_process_result);
    } else if (node_name == "Output") {
      rapidxml::xml_node<>* output_node = node->first_node();
      for (; output_node != NULL; output_node = output_node->next_sibling()) {
        const std::string& output_node_name = output_node->name();
        if (output_node_name == "Region") {
          operation.output.region = output_node->value();
        } else if (output_node_name == "Object") {
          operation.output.object = output_node->value();
        } else if (output_node_name == "Bucket") {
          operation.output.bucket = output_node->value();
        }
      }
    }
  }
  return true;
}

bool DocProcessJobBase::ParseDocProcess(rapidxml::xml_node<>* root,
                                        DocProcess& doc_process) {
  rapidxml::xml_node<>* doc_process_node = root->first_node();
  for (; doc_process_node != NULL;
       doc_process_node = doc_process_node->next_sibling()) {
    const std::string& doc_process_node_name = doc_process_node->name();
    if (doc_process_node_name == "SrcType") {
      doc_process.src_type = doc_process_node->value();
    } else if (doc_process_node_name == "TgtType") {
      doc_process.tgt_type = doc_process_node->value();
    } else if (doc_process_node_name == "SheetId") {
      doc_process.sheet_id = StringUtil::StringToInt(doc_process_node->value());
    } else if (doc_process_node_name == "StartPage") {
      doc_process.start_page =
          StringUtil::StringToInt(doc_process_node->value());
    } else if (doc_process_node_name == "EndPage") {
      doc_process.end_page = StringUtil::StringToInt(doc_process_node->value());
    } else if (doc_process_node_name == "PaperSize") {
      doc_process.page_size =
          StringUtil::StringToInt(doc_process_node->value());
    } else if (doc_process_node_name == "ImageParams") {
      doc_process.image_params = doc_process_node->value();
    } else if (doc_process_node_name == "DocPassword") {
      doc_process.doc_passwd = doc_process_node->value();
    } else if (doc_process_node_name == "Comments") {
      doc_process.comments = StringUtil::StringToInt(doc_process_node->value());
    } else if (doc_process_node_name == "PaperDirection") {
      doc_process.paper_direction =
          StringUtil::StringToInt(doc_process_node->value());
    } else if (doc_process_node_name == "Quality") {
      doc_process.quality = StringUtil::StringToInt(doc_process_node->value());
    } else if (doc_process_node_name == "Zoom") {
      doc_process.zoom = StringUtil::StringToInt(doc_process_node->value());
    }
  }
  return true;
}

bool DocProcessJobBase::ParseDocProcessResult(
    rapidxml::xml_node<>* root, DocProcessResult& doc_process_result) {
  rapidxml::xml_node<>* doc_process_result_node = root->first_node();
  for (; doc_process_result_node != NULL;
       doc_process_result_node = doc_process_result_node->next_sibling()) {
    const std::string& doc_process_result_node_name =
        doc_process_result_node->name();
    if (doc_process_result_node_name == "PageInfo") {
      rapidxml::xml_node<>* page_info_node =
          doc_process_result_node->first_node();
      PageInfo page_info;
      for (; page_info_node != NULL;
           page_info_node = page_info_node->next_sibling()) {
        const std::string& page_info_node_name = page_info_node->name();
        if (page_info_node_name == "PageNo") {
          page_info.page_no = StringUtil::StringToInt(page_info_node->value());
        } else if (page_info_node_name == "TgtUri") {
          page_info.tgt_uri = page_info_node->value();
        } else if (page_info_node_name == "X-SheetPics") {
          page_info.x_sheet_pics =
              StringUtil::StringToInt(page_info_node->value());
        } else if (page_info_node_name == "PicIndex") {
          page_info.pic_index =
              StringUtil::StringToInt(page_info_node->value());
        } else if (page_info_node_name == "PicNum") {
          page_info.pic_num = StringUtil::StringToInt(page_info_node->value());
        }
      }
      doc_process_result.page_infos.push_back(page_info);
    } else if (doc_process_result_node_name == "TgtType") {
      doc_process_result.tgt_type = doc_process_result_node->value();
    } else if (doc_process_result_node_name == "TaskId") {
      doc_process_result.task_id = doc_process_result_node->value();
    } else if (doc_process_result_node_name == "TotalPageCount") {
      doc_process_result.total_pageount =
          StringUtil::StringToInt(doc_process_result_node->value());
    } else if (doc_process_result_node_name == "SuccPageCount") {
      doc_process_result.succ_pagecount =
          StringUtil::StringToInt(doc_process_result_node->value());
    } else if (doc_process_result_node_name == "FailPageCount") {
      doc_process_result.fail_pagecount =
          StringUtil::StringToInt(doc_process_result_node->value());
    } else if (doc_process_result_node_name == "TotalSheetCount") {
      doc_process_result.total_sheetcount =
          StringUtil::StringToInt(doc_process_result_node->value());
    }
  }
  return true;
}

bool DescribeDocProcessJobsResp::ParseFromXmlString(const std::string& body) {
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
    if ("JobsDetail" == node_name) {
      JobsDetail jobs_detail;
      ParseJobsDetail(node, jobs_detail);
      m_jobs_details.push_back(jobs_detail);
    } else if ("NextToken" == node_name) {
      m_next_token = node->value();
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool QueuesBase::ParseNonExistPIDs(rapidxml::xml_node<>* root,
                                            NonExistPIDs& non_exist_pids) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("QueueId" == node_name) {
      non_exist_pids.queue_id.push_back(node->value());
    }
  }
  return true;
}

bool QueuesBase::ParseQueueList(rapidxml::xml_node<>* root,
                                         QueueList& queue_list) {
  rapidxml::xml_node<>* queue_list_node = root->first_node();
  for (; queue_list_node != NULL;
       queue_list_node = queue_list_node->next_sibling()) {
    const std::string& queue_list_node_name = queue_list_node->name();
    if ("QueueId" == queue_list_node_name) {
      queue_list.queue_id = queue_list_node->value();
    } else if ("Name" == queue_list_node_name) {
      queue_list.name = queue_list_node->value();
    } else if ("State" == queue_list_node_name) {
      queue_list.state = queue_list_node->value();
    } else if ("NotifyConfig" == queue_list_node_name) {
      rapidxml::xml_node<>* notify_config_node = queue_list_node->first_node();
      for (; notify_config_node != NULL;
           notify_config_node = notify_config_node->next_sibling()) {
        const std::string& notify_config_node_name = notify_config_node->name();
        if ("Url" == notify_config_node_name) {
          queue_list.notify_config.url = notify_config_node->value();
        } else if ("State" == notify_config_node_name) {
          queue_list.notify_config.state = notify_config_node->value();
        } else if ("Type" == notify_config_node_name) {
          queue_list.notify_config.type = notify_config_node->value();
        } else if ("Event" == notify_config_node_name) {
          queue_list.notify_config.event = notify_config_node->value();
        }
      }
    } else if ("MaxSize" == queue_list_node_name) {
      queue_list.max_size = StringUtil::StringToInt(queue_list_node->value());
    } else if ("MaxConcurrent" == queue_list_node_name) {
      queue_list.max_concurrent =
          StringUtil::StringToInt(queue_list_node->value());
    } else if ("UpdateTime" == queue_list_node_name) {
      queue_list.update_time = queue_list_node->value();
    } else if ("CreateTime" == queue_list_node_name) {
      queue_list.create_time = queue_list_node->value();
    } else if ("BucketId" == queue_list_node_name) {
      queue_list.bucket_id = queue_list_node->value();
    } else if ("Category" == queue_list_node_name) {
      queue_list.category = queue_list_node->value();
    }
  }
  return true;
}

bool DescribeQueuesResp::ParseFromXmlString(const std::string& body) {
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
    if ("TotalCount" == node_name) {
      m_total_count = StringUtil::StringToInt(node->value());
    } else if ("RequestId" == node_name) {
      m_request_id = node->value();
    } else if ("PageNumber" == node_name) {
      m_page_number = StringUtil::StringToInt(node->value());
    } else if ("PageSize" == node_name) {
      m_page_size = StringUtil::StringToInt(node->value());
    } else if ("QueueList" == node_name) {
      ParseQueueList(node, m_queue_list);
    } else if ("NonExistPIDs" == node_name) {
      ParseNonExistPIDs(node, m_non_exist_pids);
    }
  }
  return true;
}

bool UpdateQueueResp::ParseFromXmlString(const std::string& body) {
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
    } else if ("Queue" == node_name) {
      ParseQueueList(node, m_queue);
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool DescribeMediaBucketsResp::ParseFromXmlString(const std::string& body) {
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
      m_result.request_id = node->value();
    } else if ("TotalCount" == node_name) {
      m_result.total_count = StringUtil::StringToInt(node->value());
    } else if ("PageNumber" == node_name) {
      m_result.page_number = StringUtil::StringToInt(node->value());
    } else if ("PageSize" == node_name) {
      m_result.page_size = StringUtil::StringToInt(node->value());
    } else if ("MediaBucketList" == node_name) {
      BucketInfo bucket_info;
      DescribeDocProcessBucketsResp::ParseBucketInfo(node, bucket_info);
      m_result.media_bucket_list.push_back(bucket_info);
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool CreateMediaBucketResp::ParseFromXmlString(const std::string& body) {
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
      m_result.request_id = node->value();
    } else if ("MediaBucket" == node_name) {
      BucketInfo bucket_info;
      DescribeDocProcessBucketsResp::ParseBucketInfo(node, bucket_info);
      m_result.media_bucket = bucket_info;
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool GetMediaInfoResp::ParseVideo(rapidxml::xml_node<>* root,
                                  VideoInfo& video_info) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if (node_name == "Index") {
      video_info.index = StringUtil::StringToInt(node->value());
    } else if (node_name == "CodecName") {
      video_info.codec_name = node->value();
    } else if (node_name == "CodecLongName") {
      video_info.codec_long_name = node->value();
    } else if (node_name == "CodecTimeBase") {
      video_info.codec_time_base = node->value();
    } else if (node_name == "CodecTagString") {
      video_info.codec_tag_string = node->value();
    } else if (node_name == "CodecTag") {
      video_info.codec_tag = node->value();
    } else if (node_name == "Profile") {
      video_info.profile = node->value();
    } else if (node_name == "Height") {
      video_info.height = StringUtil::StringToInt(node->value());
    } else if (node_name == "Width") {
      video_info.width = StringUtil::StringToInt(node->value());
    } else if (node_name == "HasBFrame") {
      video_info.has_bframe = StringUtil::StringToInt(node->value());
    } else if (node_name == "RefFrames") {
      video_info.ref_frames = StringUtil::StringToInt(node->value());
    } else if (node_name == "Sar") {
      video_info.sar = node->value();
    } else if (node_name == "Dar") {
      video_info.dar = node->value();
    } else if (node_name == "PixFormat") {
      video_info.pix_format = node->value();
    } else if (node_name == "FieldOrder") {
      video_info.field_order = node->value();
    } else if (node_name == "Level") {
      video_info.level = StringUtil::StringToInt(node->value());
    } else if (node_name == "Fps") {
      video_info.fps = StringUtil::StringToInt(node->value());
    } else if (node_name == "AvgFps") {
      video_info.avg_fps = node->value();
    } else if (node_name == "Timebase") {
      video_info.time_base = node->value();
    } else if (node_name == "StartTime") {
      video_info.start_time = StringUtil::StringToFloat(node->value());
    } else if (node_name == "Duration") {
      video_info.duration = StringUtil::StringToFloat(node->value());
    } else if (node_name == "Bitrate") {
      video_info.bitrate = StringUtil::StringToFloat(node->value());
    } else if (node_name == "NumFrames") {
      video_info.num_frames = StringUtil::StringToInt(node->value());
    } else if (node_name == "Language") {
      video_info.language = node->value();
    } else {
      SDK_LOG_WARN("Unknown field in Video, field_name=%s", node_name.c_str());
    }
  }
  return true;
}

bool GetMediaInfoResp::ParseAudio(rapidxml::xml_node<>* root,
                                  AudioInfo& audio_info) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if (node_name == "Index") {
      audio_info.index = StringUtil::StringToInt(node->value());
    } else if (node_name == "CodecName") {
      audio_info.codec_name = node->value();
    } else if (node_name == "CodecLongName") {
      audio_info.codec_long_name = node->value();
    } else if (node_name == "CodecTimeBase") {
      audio_info.codec_time_base = node->value();
    } else if (node_name == "CodecTagString") {
      audio_info.codec_tag_string = node->value();
    } else if (node_name == "CodecTag") {
      audio_info.codec_tag = node->value();
    } else if (node_name == "SampleFmt") {
      audio_info.sample_fmt = node->value();
    } else if (node_name == "SampleRate") {
      audio_info.sample_rate = StringUtil::StringToInt(node->value());
    } else if (node_name == "Channel") {
      audio_info.channel = StringUtil::StringToInt(node->value());
    } else if (node_name == "ChannelLayout") {
      audio_info.channel_layout = node->value();
    } else if (node_name == "Timebase") {
      audio_info.time_base = node->value();
    } else if (node_name == "StartTime") {
      audio_info.start_time = StringUtil::StringToFloat(node->value());
    } else if (node_name == "Duration") {
      audio_info.duration = StringUtil::StringToFloat(node->value());
    } else if (node_name == "Bitrate") {
      audio_info.bitrate = StringUtil::StringToFloat(node->value());
    } else if (node_name == "Language") {
      audio_info.language = node->value();
    } else {
      SDK_LOG_WARN("Unknown field in Audio, field_name=%s", node_name.c_str());
    }
  }
  return true;
}

bool GetMediaInfoResp::ParseSubtitle(rapidxml::xml_node<>* root,
                                     SubtitleInfo& subtitle_info) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if (node_name == "Index") {
      subtitle_info.index = StringUtil::StringToInt(node->value());
    } else if (node_name == "Language") {
      subtitle_info.language = node->value();
    } else {
      SDK_LOG_WARN("Unknown field in Subtitle, field_name=%s",
                   node_name.c_str());
    }
  }  
  return true;
}
bool GetMediaInfoResp::ParseFormat(rapidxml::xml_node<>* root,
                                   FormatInfo& format_info) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if (node_name == "NumStream") {
      format_info.num_stream = StringUtil::StringToInt(node->value());
    } else if (node_name == "NumProgram") {
      format_info.num_program = StringUtil::StringToInt(node->value());
    } else if (node_name == "FormatName") {
      format_info.format_name = node->value();
    } else if (node_name == "FormatLongName") {
      format_info.format_long_name = node->value();
    } else if (node_name == "StartTime") {
      format_info.start_time = StringUtil::StringToFloat(node->value());
    } else if (node_name == "Duration") {
      format_info.duration = StringUtil::StringToFloat(node->value());
    } else if (node_name == "Bitrate") {
      format_info.bitrate = StringUtil::StringToFloat(node->value());
    } else if (node_name == "Size") {
      format_info.size = StringUtil::StringToInt(node->value());
    } else {
      SDK_LOG_WARN("Unknown field in Format, field_name=%s", node_name.c_str());
    }
  }
  return true;
}

bool GetMediaInfoResp::ParseFromXmlString(const std::string& body) {
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
    if ("MediaInfo" == node_name) {
      rapidxml::xml_node<>* media_node = node->first_node();
      for (; media_node != NULL; media_node = media_node->next_sibling()) {
        const std::string& media_node_name = media_node->name();
        if (media_node_name == "Stream") {
          rapidxml::xml_node<>* stream_node = media_node->first_node();
          for (; stream_node != NULL;
               stream_node = stream_node->next_sibling()) {
            const std::string& stream_node_name = stream_node->name();
            if ("Video" == stream_node_name) {
              ParseVideo(stream_node, m_result.media_info.stream.video);
            } else if ("Audio" == stream_node_name) {
              ParseAudio(stream_node, m_result.media_info.stream.audio);
            } else if ("Subtitle" == stream_node_name) {
              ParseSubtitle(stream_node, m_result.media_info.stream.subtitle);
            } else {
              SDK_LOG_WARN("Unknown field in Stream, field_name=%s",
                           stream_node_name.c_str());
            }
          }
        } else if (media_node_name == "Format") {
          ParseFormat(media_node, m_result.media_info.format);
        } else {
          SDK_LOG_WARN("Unknown field in MediaInfo, field_name=%s",
                       node_name.c_str());
        }
      }
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool DataProcessJobBase::ParseFromXmlString(const std::string& body) {
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

  rapidxml::xml_node<>* jobs_detail_root = root->first_node("JobsDetail");
  if (NULL == jobs_detail_root) {
    SDK_LOG_ERR("Missing node JobsDetail, xml_body=%s", body.c_str());
    return false;
  }

  rapidxml::xml_node<>* jobs_detail_node = jobs_detail_root->first_node();
  for (; jobs_detail_node != NULL;
       jobs_detail_node = jobs_detail_node->next_sibling()) {
    const std::string& node_name = jobs_detail_node->name();
    if ("Code" == node_name) {
      m_jobs_detail.code = jobs_detail_node->value();
    } else if ("Message" == node_name) {
      m_jobs_detail.message = jobs_detail_node->value();
    } else if ("JobId" == node_name) {
      m_jobs_detail.job_id = jobs_detail_node->value();
    } else if ("Tag" == node_name) {
      m_jobs_detail.tag = jobs_detail_node->value();
    } else if ("State" == node_name) {
      m_jobs_detail.state = jobs_detail_node->value();
    } else if ("CreationTime" == node_name) {
      m_jobs_detail.create_time = jobs_detail_node->value();
    } else if ("EndTime" == node_name) {
      m_jobs_detail.end_time = jobs_detail_node->value();
    } else if ("QueueId" == node_name) {
      m_jobs_detail.queue_id = jobs_detail_node->value();
    } else if ("QueueType" == node_name) {
      m_jobs_detail.queue_type = jobs_detail_node->value();
    } else if ("StartTime" == node_name) {
      m_jobs_detail.start_time = jobs_detail_node->value();
    } else if ("Progress" == node_name) {
      m_jobs_detail.progress = jobs_detail_node->value();                  
    } else if ("Input" == node_name) {
      rapidxml::xml_node<>* input_node = jobs_detail_node->first_node();
      for (; input_node != NULL;
        input_node = input_node->next_sibling()) {
        const std::string& input_node_name = input_node->name();
        if ("Region" == input_node_name) {
          m_jobs_detail.input.region = input_node->value();
        } else if ("Bucket" == input_node_name) {
          m_jobs_detail.input.bucket = input_node->value();
        } else if ("Object" == input_node_name) {
          m_jobs_detail.input.object = input_node->value();
        } else if ("Url" == input_node_name) {
          m_jobs_detail.input.url = input_node->value();
        }        
      }          
    } else if ("Operation" == node_name) {
      rapidxml::xml_node<>* operation_node = jobs_detail_node->first_node();
      for (; operation_node != NULL;
        operation_node = operation_node->next_sibling()) {
        const std::string& operation_node_name = operation_node->name();
        if ("TemplateId" == operation_node_name) {
          m_jobs_detail.operation.template_id = operation_node->value();
        } else if ("TemplateName" == operation_node_name) {
          m_jobs_detail.operation.template_name = operation_node->value();
        } else if ("UserData" == operation_node_name) {
          m_jobs_detail.operation.user_data = operation_node->value();
        } else if ("JobLevel" == operation_node_name) {
          m_jobs_detail.operation.job_level = std::stoi(operation_node->value());;
        } else if ("Output" == operation_node_name) {
          rapidxml::xml_node<>* output_node = operation_node->first_node();
          for (; output_node != NULL;
            output_node = output_node->next_sibling()) { 
            const std::string& output_node_name = output_node->name();
            if ("Region" == output_node_name) {
              m_jobs_detail.operation.output.region = output_node->value();
            } else if ("Bucket" == output_node_name) {
              m_jobs_detail.operation.output.bucket = output_node->value();
            } else if ("Object" == output_node_name) {
              m_jobs_detail.operation.output.object = output_node->value();
            } else if ("SpriteObject" == output_node_name) {
              m_jobs_detail.operation.output.sprite_object = output_node->value();
            }
          } 
        } else if ("MediaResult" == operation_node_name) {
          if (operation_node->first_node() != NULL) {
            rapidxml::xml_node<>* output_file_node =
             operation_node->first_node()->first_node("OutputFile");
            for (; output_file_node != NULL;
             output_file_node = output_file_node->next_sibling()) {
              const std::string& output_file_node_name = output_file_node->name();
              if ("Bucket" == output_file_node_name) {
                m_jobs_detail.operation.media_result.output_file.bucket = output_file_node->value();
              } else if ("Region" == output_file_node_name) {
                m_jobs_detail.operation.media_result.output_file.region = output_file_node->value();
              } else if ("ObjectName" == output_file_node_name) {
                m_jobs_detail.operation.media_result.output_file.object_name = output_file_node->value();
              } else if ("Md5Info" == output_file_node_name) {
                rapidxml::xml_node<>* md5_info_node = output_file_node->first_node();
                for (; md5_info_node != NULL;
                  md5_info_node = md5_info_node->next_sibling()) {
                  const std::string& md5_info_node_name = md5_info_node->name();
                  if ("ObjectName" == md5_info_node_name) {
                    m_jobs_detail.operation.media_result.output_file.md5_info.object_name = md5_info_node->value();
                  } else if ("Md5" == md5_info_node_name) {
                    m_jobs_detail.operation.media_result.output_file.md5_info.md5 = md5_info_node->value();
                  }
                }
              }
            }
          }
        } else if ("Snapshot" == operation_node_name) {
          rapidxml::xml_node<>* snapshot_node = operation_node->first_node();
          for (;snapshot_node != NULL; 
            snapshot_node = snapshot_node->next_sibling()) {
            const std::string& snapshot_node_name = snapshot_node->name();
            if ("Mode" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.mode = snapshot_node->value();
            } else if ("Start" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.start = snapshot_node->value();
            } else if ("TimeInterval" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.time_interval = snapshot_node->value();
            } else if ("Count" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.count = snapshot_node->value();
            } else if ("Width" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.width = snapshot_node->value();
            } else if ("Height" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.height = snapshot_node->value();
            } else if ("CIParam" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.ci_param = snapshot_node->value();
            } else if ("IsCheckCount" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.is_check_count = snapshot_node->value();
            } else if ("IsCheckBlack" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.is_check_black = snapshot_node->value();
            } else if ("BlackLevel" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.black_level = snapshot_node->value();
            } else if ("PixelBlackThreshold" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.pixel_black_threshold = snapshot_node->value();
            } else if ("SnapshotOutMode" == snapshot_node_name) {
              m_jobs_detail.operation.snapshot.snap_shot_out_mode = snapshot_node->value();
            } else if ("SpriteSnapshotConfig" == snapshot_node_name) {
              rapidxml::xml_node<>* sprite_snapshot_config_node = snapshot_node->first_node();
              for (;sprite_snapshot_config_node != NULL;
                sprite_snapshot_config_node = sprite_snapshot_config_node->next_sibling()) {
                const std::string& sprite_snapshot_config_node_name = sprite_snapshot_config_node->name();
                if ("CellWidth" == sprite_snapshot_config_node_name) {
                  m_jobs_detail.operation.snapshot.sprite_snapshot_config.cell_width = sprite_snapshot_config_node->value();
                } else if ("CellHeight" == sprite_snapshot_config_node_name) {
                  m_jobs_detail.operation.snapshot.sprite_snapshot_config.cell_height = sprite_snapshot_config_node->value();
                } else if ("Padding" == sprite_snapshot_config_node_name) {
                  m_jobs_detail.operation.snapshot.sprite_snapshot_config.padding = sprite_snapshot_config_node->value();
                } else if ("Margin" == sprite_snapshot_config_node_name) {
                  m_jobs_detail.operation.snapshot.sprite_snapshot_config.margin = sprite_snapshot_config_node->value();
                } else if ("Color" == sprite_snapshot_config_node_name) {
                  m_jobs_detail.operation.snapshot.sprite_snapshot_config.color = sprite_snapshot_config_node->value();
                } else if ("Columns" == sprite_snapshot_config_node_name) {
                  m_jobs_detail.operation.snapshot.sprite_snapshot_config.columns = sprite_snapshot_config_node->value();
                } else if ("Lines" == sprite_snapshot_config_node_name) {
                  m_jobs_detail.operation.snapshot.sprite_snapshot_config.lines = sprite_snapshot_config_node->value();
                } else if ("ScaleMethod" == sprite_snapshot_config_node_name) {
                  m_jobs_detail.operation.snapshot.sprite_snapshot_config.scale_method = sprite_snapshot_config_node->value();
                }
              }
            }
          } 
        } else if ("FileUncompressConfig" == operation_node_name) {
          rapidxml::xml_node<>* file_uncompress_config_node = operation_node->first_node();
          for (;file_uncompress_config_node != NULL; 
            file_uncompress_config_node = file_uncompress_config_node->next_sibling()) {
            const std::string& file_uncompress_config_node_name = file_uncompress_config_node->name();
            if ("Prefix" == file_uncompress_config_node_name) {
              m_jobs_detail.operation.file_uncompress_config.prefix = file_uncompress_config_node->value();
            } else if ("UnCompressKey" == file_uncompress_config_node_name) {
              m_jobs_detail.operation.file_uncompress_config.un_compress_key = file_uncompress_config_node->value();
            } else if ("PrefixReplaced" == file_uncompress_config_node_name) {
              m_jobs_detail.operation.file_uncompress_config.prefix_replaced = file_uncompress_config_node->value();
            }
          }
        } else if ("FileUncompressResult" == operation_node_name) {
          rapidxml::xml_node<>* file_uncompress_result_node = operation_node->first_node();
          for (;file_uncompress_result_node != NULL; 
            file_uncompress_result_node = file_uncompress_result_node->next_sibling()) {
            const std::string& file_uncompress_reult_node_name = file_uncompress_result_node->name();
            if ("Bucket" == file_uncompress_reult_node_name) {
              m_jobs_detail.operation.file_uncompress_result.bucket = file_uncompress_result_node->value();
            } else if ("Region" == file_uncompress_reult_node_name) {
              m_jobs_detail.operation.file_uncompress_result.region = file_uncompress_result_node->value();
            } else if ("FileCount" == file_uncompress_reult_node_name) {
              m_jobs_detail.operation.file_uncompress_result.file_count = file_uncompress_result_node->value();
            }
          }
        }
      }
    }
  }
  return true;
}

}  // namespace qcloud_cos
