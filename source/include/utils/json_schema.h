#ifndef UTILS_JSON_SCHEMA_H_
#define UTILS_JSON_SCHEMA_H_

#include <nlohmann/json.hpp>

namespace ns_utils {
namespace json_schema {

bool validate(const nlohmann::json &schema, const nlohmann::json &json);

}; // namespace json_schema
}; // namespace ns_utils

#endif // UTILS_JSON_SCHEMA_H_
