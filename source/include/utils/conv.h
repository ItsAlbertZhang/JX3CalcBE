#pragma once
#include <string>

namespace jx3calc {
namespace utils {

std::string utf82gbk(const std::string &utf8); // 这个函数在 Unix 下不应被使用.
std::string gbk2utf8(const std::string &gbk);  // 在本地数据为 utf-8 编码的情况下, 这个函数在 Unix 下不应被使用.

} // namespace utils
} // namespace jx3calc
