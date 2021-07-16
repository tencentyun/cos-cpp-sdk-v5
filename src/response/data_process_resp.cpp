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
      DocBucketList bucket_list;
      ParseDocBucketList(result_node, bucket_list);
      m_result.doc_bucket_list.push_back(bucket_list);
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool DescribeDocProcessBucketsResp::ParseDocBucketList(
    rapidxml::xml_node<>* root, DocBucketList& bucket_list) {
  rapidxml::xml_node<>* node = root->first_node();
  for (; node != NULL; node = node->next_sibling()) {
    const std::string& node_name = node->name();
    if (node_name == "BucketId") {
      bucket_list.bucket_id = node->value();
    } else if (node_name == "Name") {
      bucket_list.name = node->value();
    } else if (node_name == "Region") {
      bucket_list.region = node->value();
    } else if (node_name == "CreateTime") {
      bucket_list.create_time = node->value();
    } else if (node_name == "AliasBucketId") {
      bucket_list.alias_bucket_id = node->value();
    } else {
      SDK_LOG_WARN("Unknown field in DocBucketList, field_name=%s",
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
    m_x_total_page = StringUtil::StringToUint64(itr->second);
  }

  itr = headers.find(kRespHeaderXErrNo);
  if (headers.end() != itr) {
    m_x_errno = itr->second;
  }

  itr = headers.find(kRespHeaderXTotalSheet);
  if (headers.end() != itr) {
    m_x_total_sheet = StringUtil::StringToUint64(itr->second);
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
    } else {
      SDK_LOG_WARN("Unknown field in JobsDetail, field_name=%s",
                   node_name.c_str());
      return false;
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
    } else {
      SDK_LOG_WARN("Unknown field in Operation, field_name=%s",
                   node_name.c_str());
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
    } else {
      SDK_LOG_WARN("Unknown field in DocProcess, field_name=%s",
                   doc_process_node_name.c_str());
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
    } else {
      SDK_LOG_WARN("Unknown field in DocProcessResult, field_name=%s",
                   doc_process_result_node_name.c_str());
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

bool DocProcessQueueBase::ParseNonExistPIDs(rapidxml::xml_node<>* root,
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

bool DocProcessQueueBase::ParseQueueList(rapidxml::xml_node<>* root,
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
    } else {
      SDK_LOG_WARN("Unknown field in QueueList, field_name=%s",
                   queue_list_node_name.c_str());
      return false;
    }
  }
  return true;
}

bool DescribeDocProcessQueuesResp::ParseFromXmlString(const std::string& body) {
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
    } else {
      SDK_LOG_WARN("Unknown field in Response, field_name=%s",
                   node_name.c_str());
      return false;
    }
  }
  return true;
}

bool UpdateDocProcessQueueResp::ParseFromXmlString(const std::string& body) {
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

}  // namespace qcloud_cos
