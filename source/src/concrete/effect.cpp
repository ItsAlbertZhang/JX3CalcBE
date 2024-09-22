#include "concrete.h"

using namespace jx3calc;

auto concrete::createEffect(const std::string &type, const std::string &jsonstr) -> std::shared_ptr<frame::Effect> {
    const std::unordered_map<modules::config::ClientType, std::function<std::shared_ptr<frame::Effect>(const std::string &, const std::string &)>> funcmap {
        {modules::config::ClientType::jx3_hd,
         [](const std::string &type, const std::string &jsonstr) {
             return createEffect<modules::config::ClientType::jx3_hd>(type, jsonstr);
         }},
        {modules::config::ClientType::jx3_exp,
         [](const std::string &type, const std::string &jsonstr) {
             return createEffect<modules::config::ClientType::jx3_exp>(type, jsonstr);
         }},
    };
    return funcmap.at(modules::config::clientType)(type, jsonstr);
}
