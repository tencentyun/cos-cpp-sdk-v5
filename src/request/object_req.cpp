// Copyright (c) 2017, Tencent Inc.
// All rights reserved.
//
// Author: sevenyou <sevenyou@tencent.com>
// Created: 08/25/17
// Description:

#include "request/object_req.h"

#include "rapidxml/1.13/rapidxml.hpp"
#include "rapidxml/1.13/rapidxml_print.hpp"
#include "rapidxml/1.13/rapidxml_utils.hpp"

namespace qcloud_cos {

bool CompleteMultiUploadReq::GenerateRequestBody(std::string* body) const {
    const std::vector<uint64_t>& part_numbers = GetPartNumbers();
    const std::vector<std::string>& etags = GetEtags();

    // part_numbers与etags长度必须一致
    if (part_numbers.size() != etags.size()) {
        return false;
    }

    // 1.生成CompleteMultiUploadReq需要的xml字符串
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* root_node = doc.allocate_node(rapidxml::node_element,
                                                doc.allocate_string("CompleteMultipartUpload"),
                                                NULL);
    doc.append_node(root_node);

    for (int i = 0; i < part_numbers.size(); ++i) {
        rapidxml::xml_node<>* part_node = doc.allocate_node(rapidxml::node_element,
                                                            doc.allocate_string("Part"),
                                                            NULL);
        part_node->append_node(doc.allocate_node(rapidxml::node_element,
                     doc.allocate_string("PartNumber"),
                     doc.allocate_string(StringUtil::Uint64ToString(part_numbers[i]).c_str())));
        part_node->append_node(doc.allocate_node(rapidxml::node_element,
                                         doc.allocate_string("ETag"),
                                         doc.allocate_string(etags[i].c_str())));
        root_node->append_node(part_node);
    }

    // 2. 填充xml字符串
    rapidxml::print(std::back_inserter(*body), doc, 0);
    doc.clear();

    return true;
}

} // namespace qcloud_cos
