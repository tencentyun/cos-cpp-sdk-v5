#include "util/string_util.h"

#include <stdio.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <bitset>

#if defined(WIN32)
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

namespace qcloud_cos {

std::string& StringUtil::Trim(std::string &s) {
    if (s.empty()) {
        return s;
    }

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

std::string StringUtil::Trim(const std::string& s, const std::string& trim_value) {
    std::string ret = StringRemovePrefix(s, trim_value);
    ret = StringRemoveSuffix(ret, trim_value);
    return ret;
}

std::string StringUtil::XmlToString(const rapidxml::xml_document<>& xml_doc) {
    std::string s;
    print(std::back_inserter(s), xml_doc, 0);
    return s;
}

bool StringUtil::StringToXml(char* xml_str, rapidxml::xml_document<>* doc) {
    try {
        doc->parse<0>(xml_str);
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

int StringUtil::StringToInt(const std::string& str) {
	
	std::istringstream is(str);
	int temp = 0;
	is >> temp;
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

bool StringUtil::StringEndsWith(const std::string& str, const std::string& suffix) {
    return (str.size() >= suffix.size()) &&
        strncmp(str.substr(str.size() - suffix.size()).c_str(), suffix.c_str(), suffix.size()) == 0;
}

bool StringUtil::StringEndsWithIgnoreCase(const std::string& str, const std::string& suffix) {
    return (str.size() >= suffix.size()) &&
        strncasecmp(str.substr(str.size() - suffix.size()).c_str(), suffix.c_str(), suffix.size()) == 0;
}

std::string StringUtil::StringRemoveSuffix(const std::string& str, const std::string& suffix) {
    if (StringEndsWith(str, suffix)) {
        return str.substr(0, str.size() - suffix.size());
    }
    return str;
}

void StringUtil::SplitString(const std::string& str, char delim, std::vector<std::string>* vec) {
    std::stringstream ss(str);
    std::string item;
    while(std::getline(ss, item, delim)) {
        if(!item.empty()) {
            vec->push_back(item);
        }
    }
}

void StringUtil::SplitString(const std::string& str, const std::string& sep, std::vector<std::string>* vec) {
    size_t start = 0, index = 0;

    while ((index = str.find(sep, start)) != std::string::npos) {
        if(index > start) {
            vec->push_back(str.substr(start, index - start));
        }

        start = index + sep.size();
        if(start == std::string::npos) {
            return;
        }
    }

    vec->push_back(str.substr(start));
}

std::string StringUtil::HttpMethodToString(HTTP_METHOD method) {
    switch (method) {
        case HTTP_GET:
            return "GET";
        case HTTP_HEAD:
            return "HEAD";
        case HTTP_PUT:
            return "PUT";
        case HTTP_POST:
            return "POST";
        case HTTP_DELETE:
            return "DELETE";
        case HTTP_OPTIONS:
            return "OPTIONS";
        default:
            return "UNKNOWN";
    }
}

// 判断下etag的长度, V5的Etag长度是32(MD5), V4的是40(sha)
// v4的etag则跳过校验
bool StringUtil::IsV4ETag(const std::string& etag) {
    if (etag.length() != 32) {
        return true;
    } else {
        return false;
    }
}

bool StringUtil::IsMultipartUploadETag(const std::string& etag) {
    if (etag.find("-") != std::string::npos) {
        return true;
    }

    return false;
}

uint32_t StringUtil::GetUint32FromStrWithBigEndian(const          char * str) {
    uint32_t num = 0;
    std::bitset<8> bs(str[0]);
    uint32_t tmp = bs.to_ulong();
    //std::cout << "tmp " << tmp<< std::endl;
    num |= (tmp << 24);
    bs = str[1];
    tmp = bs.to_ulong();
    //std::cout << "tmp " << tmp<< std::endl;
    num |= (tmp << 16);
    bs = str[2];
    tmp = bs.to_ulong();
    //std::cout << "tmp " << tmp<< std::endl;
    num |= (tmp << 8);
    bs = str[3];
    tmp = bs.to_ulong();
    //std::cout << "tmp " << tmp<< std::endl;
    num |= (tmp);
    return num;
}

uint16_t StringUtil::GetUint16FromStrWithBigEndian(const char * str) {
    uint16_t num = 0;
    std::bitset<8> bs(str[0]);
    uint16_t tmp = bs.to_ulong();
    //std::cout << "tmp " << tmp<< std::endl;
    num |= (tmp << 8);
    bs = str[1];
    tmp = bs.to_ulong();
    //std::cout << "tmp " << tmp<< std::endl;
    num |= tmp;
    return num;
}
} // namespace qcloud_cos
