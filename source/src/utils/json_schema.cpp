#include "utils/json_schema.h"
#include <string>
#include <unordered_map>

enum class JsonType {
    object,
    array,
    string,
    integer,
    boolean,
    number,
    null,
};
const std::unordered_map<std::string, JsonType> JsonTypeMap = {
    {"object",  JsonType::object },
    {"array",   JsonType::array  },
    {"string",  JsonType::string },
    {"integer", JsonType::integer},
    {"boolean", JsonType::boolean},
    {"number",  JsonType::number },
    {"null",    JsonType::null   },
};

bool ns_utils::json_schema::validate(const nlohmann::json &schema, const nlohmann::json &json) {
    if (!JsonTypeMap.contains(schema["type"].get<std::string>())) [[unlikely]]
        return false;
    switch (JsonTypeMap.at(schema["type"].get<std::string>())) {
    case JsonType::object:
        if (!json.is_object()) [[unlikely]]
            return false;
        if (schema.contains("properties")) {
            for (auto &x : schema["properties"].items()) {
                if (!json.contains(x.key())) [[unlikely]] {
                    if (schema["required"].contains(x.key())) [[unlikely]]
                        return false;
                } else if (!validate(x.value(), json[x.key()])) [[unlikely]]
                    return false;
            }
        } else if (schema.contains("anyOf")) {
            bool flag = false;
            for (auto &x : schema["anyOf"].items()) {
                if (validate(x.value(), json)) {
                    flag = true;
                    break;
                }
            }
            if (!flag) [[unlikely]]
                return false;
        } else [[unlikely]] {
            return false;
        }
        break;
    case JsonType::array:
        if (!json.is_array()) [[unlikely]]
            return false;
        for (auto &x : json.items()) {
            if (!validate(schema["items"], x.value())) [[unlikely]]
                return false;
        }
        break;
    case JsonType::string:
        if (!json.is_string()) [[unlikely]]
            return false;
        if (schema.contains("const")) {
            if (json.get<std::string>() != schema["const"].get<std::string>()) [[unlikely]]
                return false;
        }
        if (schema.contains("enum")) {
            bool flag = false;
            for (auto &x : schema["enum"].items()) {
                if (json.get<std::string>() == x.value().get<std::string>()) {
                    flag = true;
                    break;
                }
            }
            if (!flag) [[unlikely]]
                return false;
        }
        break;
    case JsonType::integer:
        if (!json.is_number_integer()) [[unlikely]]
            return false;
        if (schema.contains("const")) {
            if (json.get<int>() != schema["const"].get<int>()) [[unlikely]]
                return false;
        }
        if (schema.contains("minimum")) {
            if (json.get<int>() < schema["minimum"].get<int>()) [[unlikely]]
                return false;
        }
        if (schema.contains("maximum")) {
            if (json.get<int>() > schema["maximum"].get<int>()) [[unlikely]]
                return false;
        }
        if (schema.contains("enum")) {
            bool flag = false;
            for (auto &x : schema["enum"].items()) {
                if (json.get<int>() == x.value().get<int>()) {
                    flag = true;
                    break;
                }
            }
            if (!flag) [[unlikely]]
                return false;
        }
        break;
    case JsonType::boolean:
        if (schema.contains("const")) {
            if (json.get<bool>() != schema["const"].get<bool>()) [[unlikely]]
                return false;
        }
        if (!json.is_boolean()) [[unlikely]]
            return false;
        break;
    case JsonType::number:
        if (!json.is_number()) [[unlikely]]
            return false;
        break;
    case JsonType::null:
        if (!json.is_null()) [[unlikely]]
            return false;
        break;
    }
    return true;
}
