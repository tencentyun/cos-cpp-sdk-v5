#include "request/data_process_req.h"

#include "cos_sys_config.h"
#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

namespace qcloud_cos {

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

bool UpdateDocProcessQueueReq::GenerateRequestBody(std::string* body) const {
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

}  // namespace qcloud_cos
