#include "request/data_process_req.h"

#include "cos_sys_config.h"
#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

namespace qcloud_cos {

void PutImageByFileReq:: CheckCoverOriginImage() {
  std::vector<PicRules> pic_rules = m_pic_operation.GetRules();
  std::string object_name = GetObjectName();
  std::string object_dir = object_name.substr(0, object_name.find_last_of("/") + 1);
  for (std::vector<PicRules>::const_iterator c_itr = pic_rules.begin();
       c_itr != pic_rules.end(); ++c_itr) {
    const PicRules& pic_rule = *c_itr;
    std::string pic_absolute_key;
    if (StringUtil::StringStartsWith(pic_rule.fileid, "/")) {
      // fileid 以“/”开头为绝对路径
      pic_absolute_key = StringUtil::StringRemovePrefix(pic_rule.fileid, "/");
    } else {
      // fileid 不以“/”开头为相对路径
      pic_absolute_key = object_dir + pic_rule.fileid;
    }
    // 如果图片处理的效果图存放路径与上传文件路径相同，则会覆盖上传的原文件
    // 覆盖上传原文件后Etag与本地文件不相同，此时不比较Etag
    if (pic_absolute_key == object_name) {
      TurnOffCheckETag();
      TurnOffComputeConentMd5();
      break;
    }
  }
}

bool CreateDocProcessJobsReq::GenerateRequestBody(std::string* body) const {
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* root_node = doc.allocate_node(
      rapidxml::node_element, doc.allocate_string("Request"), NULL);
  doc.append_node(root_node);

  root_node->append_node(doc.allocate_node(rapidxml::node_element, "Tag",
                                           doc.allocate_string(m_tag.c_str())));

  rapidxml::xml_node<>* input_node =
      doc.allocate_node(rapidxml::node_element, "Input", NULL);
  input_node->append_node(
      doc.allocate_node(rapidxml::node_element, "Object",
                        doc.allocate_string(m_input.object.c_str())));
  root_node->append_node(input_node);

  rapidxml::xml_node<>* operation_node =
      doc.allocate_node(rapidxml::node_element, "Operation", NULL);

  rapidxml::xml_node<>* operation_output_node =
      doc.allocate_node(rapidxml::node_element, "Output", NULL);
  operation_output_node->append_node(doc.allocate_node(
      rapidxml::node_element, "Region",
      doc.allocate_string(m_operation.output.region.c_str())));
  operation_output_node->append_node(doc.allocate_node(
      rapidxml::node_element, "Bucket",
      doc.allocate_string(m_operation.output.bucket.c_str())));
  operation_output_node->append_node(doc.allocate_node(
      rapidxml::node_element, "Object",
      doc.allocate_string(m_operation.output.object.c_str())));
  operation_node->append_node(operation_output_node);

  rapidxml::xml_node<>* operation_doc_process_node =
      doc.allocate_node(rapidxml::node_element, "DocProcess", NULL);
  if (!m_operation.doc_process.src_type.empty()) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "SrcType",
        doc.allocate_string(m_operation.doc_process.src_type.c_str())));
  }
  if (!m_operation.doc_process.tgt_type.empty()) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "TgtType",
        doc.allocate_string(m_operation.doc_process.tgt_type.c_str())));
  }
  if (m_operation.doc_process.sheet_id > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "SheetId",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.sheet_id)
                .c_str())));
  }
  if (m_operation.doc_process.start_page > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "StartPage",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.start_page)
                .c_str())));
  }
  if (m_operation.doc_process.end_page > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "EndPage",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.end_page)
                .c_str())));
  }
  if (!m_operation.doc_process.image_params.empty()) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "ImageParams",
        doc.allocate_string(m_operation.doc_process.image_params.c_str())));
  }
  if (!m_operation.doc_process.doc_passwd.empty()) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "DocPassword",
        doc.allocate_string(m_operation.doc_process.doc_passwd.c_str())));
  }
  if (m_operation.doc_process.comments > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "Comments",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.comments)
                .c_str())));
  }
  if (m_operation.doc_process.paper_direction > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "PaperDirection",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.paper_direction)
                .c_str())));
  }
  if (m_operation.doc_process.quality > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "Quality",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.quality).c_str())));
  }
  if (m_operation.doc_process.zoom > 0) {
    operation_doc_process_node->append_node(doc.allocate_node(
        rapidxml::node_element, "Zoom",
        doc.allocate_string(
            StringUtil::IntToString(m_operation.doc_process.zoom).c_str())));
  }
  operation_node->append_node(operation_doc_process_node);

  root_node->append_node(operation_node);

  root_node->append_node(
      doc.allocate_node(rapidxml::node_element, "QueueId",
                        doc.allocate_string(m_queue_id.c_str())));

  rapidxml::print(std::back_inserter(*body), doc, 0);
  doc.clear();

  return true;
}

bool UpdateQueueReq::GenerateRequestBody(std::string* body) const {
  rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* root_node = doc.allocate_node(
      rapidxml::node_element, doc.allocate_string("Request"), NULL);
  doc.append_node(root_node);

  root_node->append_node(doc.allocate_node(
      rapidxml::node_element, "Name", doc.allocate_string(m_name.c_str())));
  root_node->append_node(
      doc.allocate_node(rapidxml::node_element, "QueueID",
                        doc.allocate_string(m_queue_id.c_str())));
  root_node->append_node(doc.allocate_node(
      rapidxml::node_element, "State", doc.allocate_string(m_state.c_str())));

  rapidxml::xml_node<>* notify_config_node =
      doc.allocate_node(rapidxml::node_element, "NotifyConfig", NULL);
  notify_config_node->append_node(
      doc.allocate_node(rapidxml::node_element, "Url",
                        doc.allocate_string(m_notify_config.url.c_str())));
  notify_config_node->append_node(
      doc.allocate_node(rapidxml::node_element, "Type",
                        doc.allocate_string(m_notify_config.type.c_str())));
  notify_config_node->append_node(
      doc.allocate_node(rapidxml::node_element, "Event",
                        doc.allocate_string(m_notify_config.event.c_str())));
  notify_config_node->append_node(
      doc.allocate_node(rapidxml::node_element, "State",
                        doc.allocate_string(m_notify_config.state.c_str())));

  root_node->append_node(notify_config_node);

  rapidxml::print(std::back_inserter(*body), doc, 0);
  doc.clear();

  return true;
}

bool CreateDataProcessJobsReq::GenerateRequestBody(std::string* body) const {
#define TAG_STRING_FIELD(NODE, KEY, FIELD) \
  if (!KEY.empty()) { \
    NODE->append_node(doc.allocate_node( \
      rapidxml::node_element, FIELD, \
      doc.allocate_string(KEY.c_str()))); \
  }

 rapidxml::xml_document<> doc;
  rapidxml::xml_node<>* root_node = doc.allocate_node(
      rapidxml::node_element, doc.allocate_string("Request"), NULL);
  doc.append_node(root_node);

  // tag
  root_node->append_node(doc.allocate_node(
      rapidxml::node_element, "Tag", doc.allocate_string(
        options_.tag.c_str())));

  // queueId
  if (!options_.queue_id.empty()) {
    root_node->append_node(
      doc.allocate_node(rapidxml::node_element, "QueueId",
                        doc.allocate_string(options_.queue_id.c_str())));
  }      

  // input
  rapidxml::xml_node<>* input_node =
      doc.allocate_node(rapidxml::node_element, "Input", NULL);
  input_node->append_node(
      doc.allocate_node(rapidxml::node_element, "Object",
                        doc.allocate_string(options_.input.object.c_str())));
  root_node->append_node(input_node);
  
  rapidxml::xml_node<>* operation_node =
      doc.allocate_node(rapidxml::node_element, "Operation", NULL);

  // output
  rapidxml::xml_node<>* operation_output_node =
      doc.allocate_node(rapidxml::node_element, "Output", NULL);
  TAG_STRING_FIELD(operation_output_node, options_.operation.output.region, "Region");
  TAG_STRING_FIELD(operation_output_node, options_.operation.output.bucket, "Bucket");
  TAG_STRING_FIELD(operation_output_node, options_.operation.output.object, "Object");
  TAG_STRING_FIELD(operation_output_node, options_.operation.output.sprite_object, "SpriteObject");
  operation_node->append_node(operation_output_node);

  if (options_.operation.job_level != 0) {
    operation_node->append_node(doc.allocate_node(
      rapidxml::node_element, "JobLevel",
      doc.allocate_string(std::to_string(options_.operation.job_level).c_str())));
  }

  TAG_STRING_FIELD(operation_node, options_.operation.user_data, "UserData");
  TAG_STRING_FIELD(operation_node, options_.operation.template_id, "TemplateId");

  // snapshot
  rapidxml::xml_node<>* operation_snapshot_node =
      doc.allocate_node(rapidxml::node_element, "Snapshot", NULL);
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.mode, "Mode");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.start, "Start");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.time_interval, "TimeInterval");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.count, "Count");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.width, "Width");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.height, "Height");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.ci_param, "CIParam");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.is_check_count, "IsCheckCount");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.is_check_black, "IsCheckBlack");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.black_level, "BlackLevel");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.pixel_black_threshold, "PixelBlackThreshold");
  TAG_STRING_FIELD(operation_snapshot_node, options_.operation.snapshot.snap_shot_out_mode, "SnapshotOutMode");
  
  rapidxml::xml_node<>* operation_snapshot_sprite_snapshot_config_node =
      doc.allocate_node(rapidxml::node_element, "SpriteSnapshotConfig", NULL);
  TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.cell_width, "CellWidth");
  TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.cell_height, "CellHeight");
  TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.margin, "Margin");
  TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.padding, "Padding");
  TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.color, "Color");
  TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.columns, "Columns");
  TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.lines, "Lines");
  TAG_STRING_FIELD(operation_snapshot_sprite_snapshot_config_node, options_.operation.snapshot.sprite_snapshot_config.scale_method, "ScaleMethod");
  operation_snapshot_node->append_node(operation_snapshot_sprite_snapshot_config_node);
  operation_node->append_node(operation_snapshot_node);
  TAG_STRING_FIELD(operation_output_node, options_.operation.output.region, "Region");
  TAG_STRING_FIELD(operation_output_node, options_.operation.output.bucket, "Bucket");
  TAG_STRING_FIELD(operation_output_node, options_.operation.output.object, "Object");
  operation_node->append_node(operation_output_node);

  if (options_.operation.job_level != 0) {
    operation_node->append_node(doc.allocate_node(
      rapidxml::node_element, "JobLevel",
      doc.allocate_string(std::to_string(options_.operation.job_level).c_str())));
  }

  TAG_STRING_FIELD(operation_node, options_.operation.user_data, "UserData");
  TAG_STRING_FIELD(operation_node, options_.operation.template_id, "TemplateId");

  // snapshot
  rapidxml::xml_node<>* operation_file_uncompress_config_node =
      doc.allocate_node(rapidxml::node_element, "FileUncompressConfig", NULL);
  TAG_STRING_FIELD(operation_file_uncompress_config_node, options_.operation.file_uncompress_config.prefix, "Prefix");
  TAG_STRING_FIELD(operation_file_uncompress_config_node, options_.operation.file_uncompress_config.un_compress_key, "UnCompressKey");
  TAG_STRING_FIELD(operation_file_uncompress_config_node, options_.operation.file_uncompress_config.prefix_replaced, "PrefixReplaced");
  operation_node->append_node(operation_file_uncompress_config_node);
  root_node->append_node(operation_node);


  // Callback
  TAG_STRING_FIELD(root_node, options_.callback_format, "CallBackFormat");
  TAG_STRING_FIELD(root_node, options_.callback_type, "CallBackType");
  TAG_STRING_FIELD(root_node, options_.callback, "CallBack");
  rapidxml::xml_node<>* callback_mq_config =
    doc.allocate_node(rapidxml::node_element, "CallBackMqConfig", NULL);
  TAG_STRING_FIELD(callback_mq_config, options_.callback_mq_config.mq_mode, "MqMode");
  TAG_STRING_FIELD(callback_mq_config, options_.callback_mq_config.mq_region, "MqRegion");
  TAG_STRING_FIELD(callback_mq_config, options_.callback_mq_config.mq_name, "MqName");
  root_node->append_node(callback_mq_config);

  rapidxml::print(std::back_inserter(*body), doc, 0);
  doc.clear();
  return true;
}



}  // namespace qcloud_cos
