#include "util/string_util.h"

#include <stdio.h>
#include <string.h>

#include <iostream>

namespace qcloud_cos {

std::string& StringUtil::Trim(std::string &s) {
    if (s.empty()) {
        return s;
    }

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

std::string StringUtil::XmlToString(const rapidxml::xml_document<>& xml_doc) {
    std::string s;
    print(std::back_inserter(s), xml_doc, 0);
    return s;
}

bool StringUtil::StringToXml(const std::string& xml_str, rapidxml::xml_document<>* doc) {
    try {
        doc->parse<0>(const_cast<char *>(xml_str.c_str()));
    } catch(rapidxml::parse_error e) {
        return false;
    }
    return true;
}

std::string StringUtil::Uint64ToString(uint64_t num) {
    char buf[65];
#if __WORDSIZE == 64
    snprintf(buf, sizeof(buf), "%lu", num);
#else
    snprintf(buf, sizeof(buf), "%llu", num);
#endif
    std::string str(buf);
    return str;
}

std::string StringUtil::IntToString(int num) {
    char buf[65];
    snprintf(buf, sizeof(buf), "%d", num);
    std::string str(buf);
    return str;
}


void StringUtil::StringToUpper(std::string* s) {
    std::string::iterator end = s->end();
    for (std::string::iterator i = s->begin(); i != end; ++i)
        *i = toupper(static_cast<unsigned char>(*i));
}

std::string StringUtil::StringToUpper(const std::string& s) {
    std::string temp = s;
    StringUtil::StringToUpper(&temp);
    return temp;
}

void StringUtil::StringToLower(std::string* s) {
    std::string::iterator end = s->end();
    for (std::string::iterator i = s->begin(); i != end; ++i)
        *i = tolower(static_cast<unsigned char>(*i));
}

std::string StringUtil::StringToLower(const std::string& s) {
    std::string temp = s;
    StringUtil::StringToLower(&temp);
    return temp;
}

std::string StringUtil::JoinStrings(const std::vector<std::string>& str_vec,
                                    const std::string& delimiter) {
    std::string ret;
    for (std::vector<std::string>::const_iterator c_itr = str_vec.begin();
         c_itr != str_vec.end(); ++c_itr) {
        if (c_itr + 1 == str_vec.end()) {
            ret = ret + *c_itr;
        } else {
            ret = ret + *c_itr + delimiter;
        }
    }
    return ret;
}

uint64_t StringUtil::StringToUint64(const std::string& str) {
    unsigned long long temp = strtoull(str.c_str(), NULL, 10);
    return temp;
}

bool StringUtil::StringStartsWith(const std::string& str, const std::string& prefix) {
    return (str.size() >= prefix.size()) &&
        strncmp(str.c_str(), prefix.c_str(), prefix.size()) == 0;
}

bool StringUtil::StringStartsWithIgnoreCase(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() &&
        strncasecmp(str.c_str(), prefix.c_str(), prefix.size()) == 0;
}

std::string StringUtil::StringRemovePrefix(const std::string& str, const std::string& prefix) {
    if (StringStartsWith(str, prefix)) {
        return str.substr(prefix.size());
    }
    return str;
}

} // namespace qcloud_cos
