#ifndef COS_CPP_SDK_V5_INCLUDE_UTIL_JSON_UTIL_H_
#define COS_CPP_SDK_V5_INCLUDE_UTIL_JSON_UTIL_H_

#include <string>

#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"

namespace qcloud_cos {

class JsonUtil {
 public:
  /**
   * @brief 从 JSON 对象中提取字符串字段值（静默方式）
   *
   * @param json_object JSON 对象指针
   * @param key 字段键名
   * @param value 输出参数，存储提取的字符串值
   * @return true 字段存在且为字符串类型，提取成功
   * @return false 字段不存在或不是字符串类型
   */
  static bool GetStringValue(const Poco::JSON::Object::Ptr& json_object,
                             const std::string& key,
                             std::string* value);

  /**
   * @brief 从 JSON 对象中提取整数字段值（静默方式）
   *
   * @param json_object JSON 对象指针
   * @param key 字段键名
   * @param value 输出参数，存储提取的整数值
   * @return true 字段存在且为整数类型，提取成功
   * @return false 字段不存在或不是整数类型
   */
  static bool GetIntegerValue(const Poco::JSON::Object::Ptr& json_object,
                              const std::string& key,
                              uint64_t* value);

  /**
   * @brief 从 JSON 对象中提取布尔字段值（静默方式）
   *
   * @param json_object JSON 对象指针
   * @param key 字段键名
   * @param value 输出参数，存储提取的布尔值
   * @return true 字段存在且为布尔类型，提取成功
   * @return false 字段不存在或不是布尔类型
   */
  static bool GetBoolValue(const Poco::JSON::Object::Ptr& json_object,
                           const std::string& key,
                           bool* value);
};

}  // namespace qcloud_cos

#endif  // COS_CPP_SDK_V5_INCLUDE_UTIL_JSON_UTIL_H_
