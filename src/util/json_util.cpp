#include "util/json_util.h"

#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"

namespace qcloud_cos {

bool JsonUtil::GetStringValue(const Poco::JSON::Object::Ptr& json_object,
                              const std::string& key,
                              std::string* value) {
  if (!json_object || !value) return false;
  if (json_object->has(key)) {
    Poco::Dynamic::Var value_get = json_object->get(key);
    if (value_get.isString()) {
      *value = value_get.convert<std::string>();
      return true;
    }
  }
  return false;
}

bool JsonUtil::GetIntegerValue(const Poco::JSON::Object::Ptr& json_object,
                               const std::string& key,
                               uint64_t* value) {
  if (!json_object || !value) return false;
  if (json_object->has(key)) {
    Poco::Dynamic::Var value_get = json_object->get(key);
    if (value_get.isInteger()) {
      int64_t tmp = value_get.convert<int64_t>();
      if (tmp < 0) return false;
      *value = static_cast<uint64_t>(tmp);
      return true;
    }
  }
  return false;
}

bool JsonUtil::GetBoolValue(const Poco::JSON::Object::Ptr& json_object,
                            const std::string& key,
                            bool* value) {
  if (!json_object || !value) return false;
  if (json_object->has(key)) {
    Poco::Dynamic::Var value_get = json_object->get(key);
    if (value_get.isBoolean()) {
      *value = value_get.convert<bool>();
      return true;
    }
  }
  return false;
}

}  // namespace qcloud_cos
