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

bool CreateFileBucketResp::ParseFromXmlString(const std::string& body) {
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
    } else if ("FileBucket" == node_name) {
      BucketInfo bucket_info;
      DescribeDocProcessBucketsResp::ParseBucketInfo(node, bucket_info);
      m_result.file_bucket = bucket_info;
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool DescribeFileBucketsResp::ParseFromXmlString(const std::string& body) {
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
    } else if ("FileBucketList" == node_name) {
      BucketInfo bucket_info;
      DescribeDocProcessBucketsResp::ParseBucketInfo(node, bucket_info);
      m_result.file_bucket_list.push_back(bucket_info);
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
    } else if (node_name == "ColorPrimaries") {
      video_info.color_primaries = node->value();      
    } else if (node_name == "ColorRange") {
      video_info.color_range = node->value();
    } else if (node_name == "ColorTransfer") {
      video_info.color_transfer = node->value();            
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

bool DataProcessJobBase::ParseAudioMix(rapidxml::xml_node<>* root, AudioMix& audio_mix) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("AudioSource" ==  node_name) {
      audio_mix.audio_source = node->value();
    } else if ("MixMode" ==  node_name) {
      audio_mix.mix_mode = node->value();
    } else if ("Replace" ==  node_name) {
      audio_mix.replace = node->value();
    } else if ("EffectConfig" ==  node_name) {
      rapidxml::xml_node<>* effect_config_node = node->first_node();
      for (; effect_config_node != NULL; effect_config_node = effect_config_node->next_sibling()) {
        const std::string& effect_config_node_name = effect_config_node->name();
        if ("EnableStartFadein" == effect_config_node_name) {
          audio_mix.effect_config.enable_start_fade_in = effect_config_node->value();
        } else if ("StartFadeinTime" == effect_config_node_name) {
          audio_mix.effect_config.start_fade_in_time = effect_config_node->value();
        } else if ("EnableEndFadeout" == effect_config_node_name) {
          audio_mix.effect_config.enable_end_fade_out = effect_config_node->value();
        } else if ("EndFadeoutTime" == effect_config_node_name) {
          audio_mix.effect_config.end_fade_out_time = effect_config_node->value();
        } else if ("EnableBgmFade" == effect_config_node_name) {
          audio_mix.effect_config.enable_bgm_fade = effect_config_node->value();
        } else if ("BgmFadeTime" == effect_config_node_name) {
          audio_mix.effect_config.bgm_fade_time = effect_config_node->value();
        }
      }
    } 
  }
  return true;
}

bool DataProcessJobBase::ParseVideo(rapidxml::xml_node<>* root, Video& video) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Codec" == node_name) {
      video.codec = node->value();
    } else if ("Width" == node_name) {
      video.width = node->value();
    } else if ("Height" == node_name) {
      video.height = node->value();
    } else if ("Fps" == node_name) {
      video.fps = node->value();
    } else if ("Remove" == node_name) {
      video.remove = node->value();
    } else if ("Profile" == node_name) {
      video.profile = node->value();
    } else if ("Bitrate" == node_name) {
      video.bit_rate = node->value();    
    } else if ("Crt" == node_name) {
      video.crf = node->value();    
    } else if ("Gop" == node_name) {
      video.gop = node->value();    
    } else if ("Preset" == node_name) {
      video.preset = node->value();    
    } else if ("Bufsize" == node_name) {
      video.buf_size = node->value();    
    } else if ("Maxrate" == node_name) {
      video.max_rate = node->value();    
    } else if ("Pixfmt" == node_name) {
      video.pixfmt = node->value();    
    } else if ("LongShortMode" == node_name) {
      video.long_short_mode = node->value();    
    } else if ("Rotate" == node_name) {
      video.rotate = node->value();    
    } else if ("Roi" == node_name) {
      video.roi = node->value();    
    } else if ("Crop" == node_name) {
      video.crop = node->value();                                  
    } else if ("Interlaced" == node_name) {
      video.interlaced = node->value();                                  
    } else if ("AnimateOnlyKeepKeyFrame" == node_name) {
      video.animate_only_keep_key_frame = node->value();
    } else if ("AnimateTimeIntervalOfFrame" == node_name) {
      video.animate_time_interval_of_frame = node->value();
    } else if ("AnimateFramesPerSecond" == node_name) {
      video.animate_frames_per_second = node->value();
    } else if ("Quality" == node_name) {
      video.quality = node->value();
    }
  }
  return true;
}

bool DataProcessJobBase::ParseTimeInterval(rapidxml::xml_node<>* root, TimeInterval& time_interval) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Start" == node_name) {
      time_interval.start = node->value();
    } else if ("Duration" == node_name) {
      time_interval.duration = node->value();
    }
  }
  return true;
}

bool DataProcessJobBase::ParseContainer(rapidxml::xml_node<>* root, Container& container) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Format" == node_name) {
      container.format = node->value();
    } else if ("ClipConfig" == node_name) {
      rapidxml::xml_node<>* clip_config_node = node->first_node();
      for (; clip_config_node != NULL; clip_config_node = clip_config_node->next_sibling()) {
        const std::string& clip_config_node_name = clip_config_node->name();
        if ("Durtion" == clip_config_node_name) {
          container.clip_config.duration = clip_config_node->value();
        }
      }
    }
  }
  return true;
}

bool DataProcessJobBase::ParseAudio(rapidxml::xml_node<>* root, Audio& audio) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Codec" ==  node_name) {
      audio.codec = node->value();
    } else if ("Samplerate" ==  node_name) {
      audio.sample_rate = node->value();
    } else if ("Channels" ==  node_name) {
      audio.channels = node->value();
    } else if ("SampleFormat" ==  node_name) {
      audio.sample_format = node->value();
    } else if ("Remove" ==  node_name) {
      audio.remove = node->value();
    } else if ("Bitrate" ==  node_name) {
      audio.bit_rate = node->value();    
    }
  }
  return true;
}

bool DataProcessJobBase::ParseTransConfig(rapidxml::xml_node<>* root, TransConfig& transconfig) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("AdjDarMethod" ==  node_name) {
      transconfig.adj_dar_method = node->value();
    } else if ("IsCheckReso" ==  node_name) {
      transconfig.is_check_reso = node->value();
    } else if ("ResoAdjMethod" ==  node_name) {
      transconfig.reso_adj_method = node->value();
    } else if ("IsCheckVideoBitrate" ==  node_name) {
      transconfig.is_check_video_bit_rate = node->value();
    } else if ("VideoBitrateAdjMethod" ==  node_name) {
      transconfig.video_bit_rate_adj_method = node->value();
    } else if ("IsCheckAudioBitrate" ==  node_name) {
      transconfig.is_check_audio_bit_rate = node->value();
    } else if ("AudioBitrateAdjMethod" ==  node_name) {
      transconfig.audio_bit_rate_adj_method = node->value();    
    } else if ("IsCheckVideoFps" ==  node_name) {
      transconfig.is_check_video_fps = node->value();    
    } else if ("VideoFpsAdjMethod" ==  node_name) {
      transconfig.video_fps_adj_method = node->value();    
    } else if ("DeleteMetadata" ==  node_name) {
      transconfig.delete_meta_data = node->value();    
    } else if ("IsHdr2Sdr" ==  node_name) {
      transconfig.is_hdr_2_sdr = node->value();    
    } else if ("TranscodeIndex" ==  node_name) {
      transconfig.transcode_index = node->value();    
    } else if ("HlsEncrypt" ==  node_name) {
      rapidxml::xml_node<>* hls_encrypt_node = node->first_node();
      for (; hls_encrypt_node != NULL; hls_encrypt_node = hls_encrypt_node->next_sibling()) { 
        const std::string& hls_encrypt_node_name = hls_encrypt_node->name();
        if ("IsHlsEncrypt" == hls_encrypt_node_name) {
          transconfig.hls_encrypt.is_hls_encrypt = hls_encrypt_node->name();
        } else if ("UriKey" == hls_encrypt_node_name) {
          transconfig.hls_encrypt.url_key = hls_encrypt_node->name();
        }
      }
    } else if ("DashEncrypt" ==  node_name) {
      rapidxml::xml_node<>* dash_encrypt_node = node->first_node();
      for (; dash_encrypt_node != NULL; dash_encrypt_node = dash_encrypt_node->next_sibling()) { 
        const std::string& dash_encrypt_node_name = dash_encrypt_node->name();
        if ("IsEncrypt" == dash_encrypt_node_name) {
          transconfig.dash_encrypt.is_encrypt = dash_encrypt_node->name();
        } else if ("UriKey" == dash_encrypt_node_name) {
          transconfig.dash_encrypt.url_key = dash_encrypt_node->name();
        }
      }  
    }    
  }
  return true;
}

bool DataProcessJobBase::ParseTranscode(rapidxml::xml_node<>* root, Transcode& transcode) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("TimeInterval" == node_name) {
      ParseTimeInterval(node, transcode.time_interval);
    } else if ("Container" == node_name) {
      ParseContainer(node, transcode.container);
    } else if ("Video" == node_name) {
      ParseVideo(node, transcode.video);  
    } else if ("Audio" == node_name) {
      ParseAudio(node, transcode.audio);
    } else if ("TransConfig" == node_name) {
      ParseTransConfig(node, transcode.trans_config);
    } else if ("AudioMix" == node_name) {
      ParseAudioMix(node, transcode.audio_mix);  
    } else if ("AudioMixArray" == node_name) {
      AudioMix audio_mix = AudioMix();
      ParseAudioMix(node, audio_mix);  
      transcode.audio_mix_array.push_back(audio_mix);
    } else {
      SDK_LOG_WARN("Unknown field in Transcode, field_name=%s", node_name.c_str());
    }
  }
  return true;
}

bool DataProcessJobBase::ParseSnapshot(rapidxml::xml_node<>* root, Snapshot& snapshot) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Mode" == node_name) {
      snapshot.mode = node->value();
    } else if ("Start" == node_name) {
      snapshot.start = node->value();
    } else if ("TimeInterval" == node_name) {
      snapshot.time_interval = node->value();
    } else if ("Count" == node_name) {
      snapshot.count = node->value();
    } else if ("Width" == node_name) {
      snapshot.width = node->value();
    } else if ("Height" == node_name) {
      snapshot.height = node->value();
    } else if ("CIParam" == node_name) {
      snapshot.ci_param = node->value();
    } else if ("IsCheckCount" == node_name) {
      snapshot.is_check_count = node->value();
    } else if ("IsCheckBlack" == node_name) {
      snapshot.is_check_black = node->value();
    } else if ("BlackLevel" == node_name) {
      snapshot.black_level = node->value();
    } else if ("PixelBlackThreshold" == node_name) {
      snapshot.pixel_black_threshold = node->value();
    } else if ("SnapshotOutMode" == node_name) {
      snapshot.snap_shot_out_mode = node->value();
    } else if ("SpriteSnapshotConfig" == node_name) {
      rapidxml::xml_node<>* sprite_snapshot_config_node = node->first_node();
      for (;sprite_snapshot_config_node != NULL;
        sprite_snapshot_config_node = sprite_snapshot_config_node->next_sibling()) {
        const std::string& sprite_snapshot_config_node_name = sprite_snapshot_config_node->name();
        if ("CellWidth" == sprite_snapshot_config_node_name) {
          snapshot.sprite_snapshot_config.cell_width = sprite_snapshot_config_node->value();
        } else if ("CellHeight" == sprite_snapshot_config_node_name) {
          snapshot.sprite_snapshot_config.cell_height = sprite_snapshot_config_node->value();
        } else if ("Padding" == sprite_snapshot_config_node_name) {
          snapshot.sprite_snapshot_config.padding = sprite_snapshot_config_node->value();
        } else if ("Margin" == sprite_snapshot_config_node_name) {
          snapshot.sprite_snapshot_config.margin = sprite_snapshot_config_node->value();
        } else if ("Color" == sprite_snapshot_config_node_name) {
          snapshot.sprite_snapshot_config.color = sprite_snapshot_config_node->value();
        } else if ("Columns" == sprite_snapshot_config_node_name) {
          snapshot.sprite_snapshot_config.columns = sprite_snapshot_config_node->value();
        } else if ("Lines" == sprite_snapshot_config_node_name) {
          snapshot.sprite_snapshot_config.lines = sprite_snapshot_config_node->value();
        } else if ("ScaleMethod" == sprite_snapshot_config_node_name) {
          snapshot.sprite_snapshot_config.scale_method = sprite_snapshot_config_node->value();
        }
      }
    }    
  }  
  return true;
}

bool DataProcessJobBase::ParseWatermark(rapidxml::xml_node<>* root, Watermark& watermark) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Type" == node_name) {
      watermark.type = node->value();
    } else if ("Pos" == node_name) {
      watermark.pos = node->value();
    } else if ("LocMode" == node_name) {
      watermark.loc_mode = node->value();
    } else if ("Dx" == node_name) {
      watermark.dx = node->value();
    } else if ("Dy" == node_name) {
      watermark.dy = node->value();
    } else if ("StartTime" == node_name) {
      watermark.start_time = node->value();
    } else if ("EndTime" == node_name) {
      watermark.end_time = node->value();
    } else if ("SlidConfig" == node_name) {
      rapidxml::xml_node<>* slide_config_node = node->first_node();
      for (;slide_config_node != NULL; 
        slide_config_node = slide_config_node->next_sibling()) {
        const std::string slide_config_node_name = slide_config_node->name();
        if ("SlideMode" == slide_config_node_name) {
          watermark.slide_config.slide_mode = slide_config_node->value();
        } else if ("XSlideSpeed" == slide_config_node_name) {
          watermark.slide_config.x_slide_speed = slide_config_node->value();
        } else if ("YSlideSpeed" == slide_config_node_name) {
          watermark.slide_config.y_slide_speed = slide_config_node->value();
        }
      }
    } else if ("Image" == node_name) {
      rapidxml::xml_node<>* image_node = node->first_node();
      for (;image_node != NULL; 
        image_node = image_node->next_sibling()) {
        const std::string image_node_name = image_node->name();
        if ("Url" == image_node_name) {
          watermark.image.url = image_node->value();
        } else if ("Mode" == image_node_name) {
          watermark.image.mode = image_node->value();
        } else if ("Width" == image_node_name) {
          watermark.image.width = image_node->value();
        } else if ("Height" == image_node_name) {
          watermark.image.height = image_node->value();
        } else if ("Transparency" == image_node_name) {
          watermark.image.transparency = image_node->value();
        } else if ("Backgroud" ==  image_node_name) {
          watermark.image.backgroud = image_node->value();
        }
      }
    } else if ("Text" == node_name) {
      rapidxml::xml_node<>* text_node = node->first_node();
      for (;text_node != NULL; 
        text_node = text_node->next_sibling()) {
        const std::string text_node_name = text_node->name();
        if ("FontSize" == text_node_name) {
          watermark.text.font_size = text_node->value();
        } else if ("FontType" == text_node_name) {
          watermark.text.font_type = text_node->value();
        } else if ("FontColor" == text_node_name) {
          watermark.text.font_color = text_node->value();
        } else if ("Transparency" == text_node_name) {
          watermark.text.transparency = text_node->value();
        } else if ("Text" == text_node_name) {
          watermark.text.text = text_node->value();
        }
      }
    }     
  }
  return true;
}

bool DataProcessJobBase::ParseMediaResult(rapidxml::xml_node<>* root, MediaResult& media_result) {
  rapidxml::xml_node<>* node = root->first_node();
  if (node->first_node() != NULL) {
    rapidxml::xml_node<>* output_file_node =
     node->first_node()->first_node("OutputFile");
    for (; output_file_node != NULL;
     output_file_node = output_file_node->next_sibling()) {
      const std::string& output_file_node_name = output_file_node->name();
      if ("Bucket" == output_file_node_name) {
        media_result.output_file.bucket = output_file_node->value();
      } else if ("Region" == output_file_node_name) {
        media_result.output_file.region = output_file_node->value();
      } else if ("ObjectName" == output_file_node_name) {
        media_result.output_file.object_name = output_file_node->value();
      } else if ("Md5Info" == output_file_node_name) {
        rapidxml::xml_node<>* md5_info_node = output_file_node->first_node();
        for (; md5_info_node != NULL;
          md5_info_node = md5_info_node->next_sibling()) {
          const std::string& md5_info_node_name = md5_info_node->name();
          if ("ObjectName" == md5_info_node_name) {
            media_result.output_file.md5_info.object_name = md5_info_node->value();
          } else if ("Md5" == md5_info_node_name) {
            media_result.output_file.md5_info.md5 = md5_info_node->value();
          }
        }
      }
    }
  }  
  return true;  
}

bool DataProcessJobBase::ParseOutput(rapidxml::xml_node<>* root, Output& output) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Region" == node_name) {
      output.region = node->value();
    } else if ("Bucket" == node_name) {
      output.bucket = node->value();
    } else if ("Object" == node_name) {
      output.object = node->value();
    } else if ("SpriteObject" == node_name) {
      output.sprite_object = node->value();
    } else if ("AuObject" == node_name) {
      output.au_object = node->value();
    } else if ("BassObject" == node_name) {
      output.bass_object = node->value();
    } else if ("Drum" == node_name) {
      output.drum_object = node->value();
    }
  }
  return true;
}

bool DataProcessJobBase::ParseRemoveWatermark(rapidxml::xml_node<>* root, RemoveWatermark& remove_watermark) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Dx" == node_name) {
      remove_watermark.dx = node->value();
    } else if ("Dy" == node_name) {
      remove_watermark.dy = node->value();
    } else if ("Width" == node_name) {
      remove_watermark.width = node->value();
    } else if ("Height" == node_name) {
      remove_watermark.height = node->value();
    }
  }
  return true;  
}

bool DataProcessJobBase::ParseSubtitles(rapidxml::xml_node<>* root, Subtitles& subtitles) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Subtitle" == node_name) {
      rapidxml::xml_node<>* subtitle_node = node->first_node();
      Subtitle subtitle = Subtitle();
      for (;subtitle_node != NULL; 
        subtitle_node = subtitle_node->next_sibling()) { 
        const std::string subtitle_node_name = subtitle_node->name();
        if ("Url" == subtitle_node_name) {
          subtitle.url = subtitle_node->value();
        }
      }
      subtitles.subtitle.push_back(subtitle);
    }
  }
  return true;  
}

bool DataProcessJobBase::ParseFileUncompressConfig(rapidxml::xml_node<>* root, FileUncompressConfig& file_uncompress_config) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Prefix" == node_name) {
      file_uncompress_config.prefix = node->value();
    } else if ("UnCompressKey" == node_name) {
      file_uncompress_config.un_compress_key = node->value();
    } else if ("PrefixReplaced" == node_name) {
      file_uncompress_config.prefix_replaced = node->value();
    }
  }
  return true;  
}

bool DataProcessJobBase::ParseFileUncompressResult(rapidxml::xml_node<>* root, FileUncompressResult& file_uncompress_result) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Bucket" == node_name) {
      file_uncompress_result.bucket = node->value();
    } else if ("Region" == node_name) {
      file_uncompress_result.region = node->value();
    } else if ("FileCount" == node_name) {
      file_uncompress_result.file_count = node->value();
    }
  }
  return true;  
}

bool DataProcessJobBase::ParseAnimation(rapidxml::xml_node<>* root, Animation& animation) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("TimeInterval" == node_name) {
      ParseTimeInterval(node, animation.time_interval);
    } else if ("Container" == node_name) {
      ParseContainer(node, animation.container);
    } else if ("Video" == node_name) {
      ParseVideo(node, animation.video);  
    }
  }
  return true;    
}

bool DataProcessJobBase::ParseInput(rapidxml::xml_node<>* root, Input& input) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Region" == node_name) {
      input.region = node->value();
    } else if ("Bucket" == node_name) {
      input.bucket = node->value();
    } else if ("Object" == node_name) {
      input.object = node->value();
    } else if ("Url" == node_name) {
      input.url = node->value();
    } 
  }
  return true;    
}

bool DataProcessJobBase::ParseSmartCover(rapidxml::xml_node<>* root, SmartCover& smartcover) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Format" == node_name) {
      smartcover.format = node->value();
    } else if ("Width" == node_name) {
      smartcover.width = node->value();
    } else if ("Height" == node_name) {
      smartcover.height = node->value();
    } else if ("Count" == node_name) {
      smartcover.count = node->value();
    } else if ("DeleteDuplicates" == node_name) {
      smartcover.delete_duplicates = node->value();
    }  
  }
  return true; 
}

bool DataProcessJobBase::ParseDigitalWatermark(rapidxml::xml_node<>* root, DigitalWatermark& digital_watermark) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Type" == node_name) {
      digital_watermark.type = node->value();
    } else if ("Message" == node_name) {
      digital_watermark.message = node->value();
    } else if ("Version" == node_name) {
      digital_watermark.version = node->value();
    } else if ("IgnoreError" == node_name) {
      digital_watermark.ignore_error = node->value();
    } else if ("State" == node_name) {
      digital_watermark.state = node->value();
    }  
  }
  return true; 
}

bool DataProcessJobBase::ParseExtractDigitalWatermark(rapidxml::xml_node<>* root, ExtractDigitalWatermark& extract_digital_watermark) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Type" == node_name) {
      extract_digital_watermark.type = node->value();
    } else if ("Message" == node_name) {
      extract_digital_watermark.message = node->value();
    } else if ("Version" == node_name) {
      extract_digital_watermark.version = node->value();
    }
  }
  return true; 
}

bool DataProcessJobBase::ParseConcatFragment(rapidxml::xml_node<>* root, ConcatFragment& concat_fragment) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Url" == node_name) {
      concat_fragment.url = node->value();
    } else if ("FragmentIndex" == node_name) {
      concat_fragment.fragment_index = node->value();
    } else if ("StartTime" == node_name) {
      concat_fragment.start_time = node->value();
    } else if ("EndTime" == node_name) {
      concat_fragment.end_time = node->value();
    }
  }
  return true;   
}


bool DataProcessJobBase::ParseConcat(rapidxml::xml_node<>* root, Concat& concat) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("ConcatFragment" == node_name) {
      ConcatFragment fragment = ConcatFragment();
      ParseConcatFragment(node, fragment);
      concat.concat_fragment.push_back(fragment);
    } else if ("Audio" == node_name) {
      ParseAudio(node, concat.audio);
    } else if ("Video" == node_name) {
      ParseVideo(node, concat.video);
    } else if ("Container" == node_name) {
      ParseContainer(node, concat.container);
    } else if ("AudioMix" == node_name) {
      ParseAudioMix(node, concat.audio_mix);
    } else if ("AudioMixArray" == node_name) {
      AudioMix audio_mix = AudioMix();
      ParseAudioMix(node, audio_mix);  
      concat.audio_mix_array.push_back(audio_mix);
    } else if ("Index" == node_name) {
      concat.index = node->value();
    } else if ("DirectConcat" == node_name) {
      concat.direct_concat = node->value();
    } else if ("SceneChangeInfo" == node_name) {
      rapidxml::xml_node<>* scene_change_info_node = node->first_node();
      for (; scene_change_info_node != NULL;
        scene_change_info_node = scene_change_info_node->next_sibling()) {
        const std::string& scene_change_info_node_name = scene_change_info_node->name();
        if ("Mode" == scene_change_info_node_name) {
          concat.scene_change_info.mode = scene_change_info_node->value();
        } else if ("Time" == scene_change_info_node_name) {
          concat.scene_change_info.time = scene_change_info_node->value();
        }
      }
    }
  }
  return true;   
}

bool DataProcessJobBase::ParseVideoMontage(rapidxml::xml_node<>* root, VideoMontage& video_montage) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Audio" == node_name) {
      ParseAudio(node, video_montage.audio);
    } else if ("Video" == node_name) {
      ParseVideo(node, video_montage.video);
    } else if ("Container" == node_name) {
      ParseContainer(node, video_montage.container);
    } else if ("AudioMix" == node_name) {
      ParseAudioMix(node, video_montage.audio_mix);
    } else if ("AudioMixArray" == node_name) {
      AudioMix audio_mix = AudioMix();
      ParseAudioMix(node, audio_mix);  
      video_montage.audio_mix_array.push_back(audio_mix);
    } else if ("Scene" == node_name) {
      video_montage.scene = node->value();
    }
  }
  return true;   
}

bool DataProcessJobBase::ParseVoiceSeparate(rapidxml::xml_node<>* root, VoiceSeparate& voice_separate) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("AudioMode" == node_name) {
      voice_separate.audio_mode = node->value();
    } else if ("AudioConfig" == node_name) {
      rapidxml::xml_node<>* audio_config_node = node->first_node();
      for (; audio_config_node != NULL; audio_config_node = audio_config_node->next_sibling()) {
        const std::string& audio_config_node_name = audio_config_node->name();
        if ("Codec" ==  audio_config_node_name) {
          voice_separate.audio_config.codec = audio_config_node->value();
        } else if ("Samplerate" == audio_config_node_name) {
          voice_separate.audio_config.sample_rate = audio_config_node->value();
        } else if ("Bitrate" ==  audio_config_node_name) {
          voice_separate.audio_config.bit_rate = audio_config_node->value();
        } else if ("Channels" == audio_config_node_name) {
          voice_separate.audio_config.channels = audio_config_node->value();
        }
      }
    }
  }
  return true;   
}

bool DataProcessJobBase::ParseSegment(rapidxml::xml_node<>* root, Segment& segment) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if ("Format" == node_name) {
      segment.format = node->value();
    } else if ("Duration" == node_name) {
      segment.duration = node->value();
    } else if ("TranscodeIndex" == node_name) {
      segment.transcode_index = node->value();
    } else if ("StartTime" == node_name) {
      segment.start_time = node->value();
    } else if ("EndTime" == node_name) {
      segment.end_time = node->value();
    } else if ("HlsEncrypt" == node_name) {
      rapidxml::xml_node<>* hls_encrypt_node = node->first_node();
      for (; hls_encrypt_node != NULL; hls_encrypt_node = hls_encrypt_node->next_sibling()) { 
        const std::string& hls_encrypt_node_name = hls_encrypt_node->name();
        if ("IsHlsEncrypt" == hls_encrypt_node_name) {
          segment.hls_encrypt.is_hls_encrypt = hls_encrypt_node->name();
        } else if ("UriKey" == hls_encrypt_node_name) {
          segment.hls_encrypt.url_key = hls_encrypt_node->name();
        }
      }
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
      ParseInput(jobs_detail_node, m_jobs_detail.input);        
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
          ParseOutput(operation_node, m_jobs_detail.operation.output);
        } else if ("MediaResult" == operation_node_name) {
          ParseMediaResult(operation_node, m_jobs_detail.operation.media_result);
        } else if ("Snapshot" == operation_node_name) {
          ParseSnapshot(operation_node, m_jobs_detail.operation.snapshot);
        } else if ("FileUncompressConfig" == operation_node_name) {
          ParseFileUncompressConfig(operation_node, m_jobs_detail.operation.file_uncompress_config);
        } else if ("FileUncompressResult" == operation_node_name) {
          ParseFileUncompressResult(operation_node, m_jobs_detail.operation.file_uncompress_result);
        } else if ("Transcode" == operation_node_name) {
          ParseTranscode(operation_node, m_jobs_detail.operation.transcode);
        } else if ("Watermark" == operation_node_name) {
          Watermark watermark = Watermark();
          ParseWatermark(operation_node, watermark);
          m_jobs_detail.operation.watermarks.push_back(watermark);
        } else if ("RemoveWatermark" == operation_node_name) {
          ParseRemoveWatermark(operation_node, m_jobs_detail.operation.remove_watermark);
        } else if ("Subtitles" == operation_node_name) {
          ParseSubtitles(operation_node, m_jobs_detail.operation.subtitles);
        } else if ("Animation" == operation_node_name) {
          ParseAnimation(operation_node, m_jobs_detail.operation.animation);
        } else if ("SmartCover" == operation_node_name) {
          ParseSmartCover(operation_node, m_jobs_detail.operation.smart_cover);
        } else if ("ConcatTemplate" == operation_node_name) {
          ParseConcat(operation_node, m_jobs_detail.operation.concat);
        } else if ("DigitalWatermark" ==  operation_node_name) {
          ParseDigitalWatermark(operation_node, m_jobs_detail.operation.digital_watermark);
        } else if ("ExtractDigitalWatermark" == operation_node_name) {
          ParseExtractDigitalWatermark(operation_node, m_jobs_detail.operation.extract_digital_watermark);
        } else if ("VideoMontage" == operation_node_name) {
          ParseVideoMontage(operation_node, m_jobs_detail.operation.video_montage);
        } else if ("VoiceSeparate" ==  operation_node_name) {
          ParseVoiceSeparate(operation_node, m_jobs_detail.operation.voice_separate);
        } else if ("Segment" ==  operation_node_name) {
          ParseSegment(operation_node, m_jobs_detail.operation.segment);
        }
      }
    }
  }
  return true;
}

}  // namespace qcloud_cos
