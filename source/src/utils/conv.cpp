#include "utils/conv.h"
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#else
#include <iconv.h>
#endif

#define CP_GBK 936

using namespace jx3calc;

std::string utils::utf82gbk(const std::string &utf8) {
#ifdef _WIN32
    int len = 0; // 使用 Windows API 进行转换.

    len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0); // 获取 UTF-16 编码格式所需内存大小.
    if (0 == len)
        return utf8;
    std::vector<wchar_t> v1;
    v1.resize(len + 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, v1.data(), len); // 转换为 UTF-16 编码格式.
    v1[len] = L'\0';

    len = WideCharToMultiByte(CP_GBK, 0, v1.data(), -1, nullptr, 0, nullptr, nullptr); // 获取 GBK 编码格式所需内存大小.
    if (0 == len)
        return utf8;
    std::vector<char> v2;
    v2.resize(len + 1);
    WideCharToMultiByte(CP_GBK, 0, v1.data(), -1, v2.data(), len, nullptr, nullptr); // 转换为 GBK 编码格式.
    v2[len] = '\0';
    return v2.data();
#else
    iconv_t cd = iconv_open("GBK", "UTF-8"); // 使用 iconv 进行转换.

    if (cd == (iconv_t)-1)
        return utf8;

    size_t            len = utf8.length();
    std::vector<char> v;
    v.resize(len * 2 + 1);
    char  *inbuf        = utf8.c_str();
    char  *outbuf       = v.data();
    size_t inbytesleft  = len;
    size_t outbytesleft = len * 2;
    size_t ret          = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    *outbuf             = '\0';
    iconv_close(cd);
    if (ret != (size_t)-1)
        return utf8;
    return v.data();
#endif
}

std::string utils::gbk2utf8(const std::string &gbk) {
#ifdef _WIN32
    int len = 0; // 使用 Windows API 进行转换.

    len = MultiByteToWideChar(CP_GBK, 0, gbk.c_str(), -1, nullptr, 0); // 获取 UTF-16 编码格式所需内存大小.
    if (0 == len)
        return gbk;
    std::vector<wchar_t> v1;
    v1.resize(len + 1);
    MultiByteToWideChar(CP_GBK, 0, gbk.c_str(), -1, v1.data(), len); // 转换为 UTF-16 编码格式.
    v1[len] = L'\0';

    len = WideCharToMultiByte(CP_UTF8, 0, v1.data(), -1, nullptr, 0, nullptr, nullptr); // 获取 UTF-8 编码格式所需内存大小.
    if (0 == len)
        return gbk;
    std::vector<char> v2;
    v2.resize(len + 1);
    WideCharToMultiByte(CP_UTF8, 0, v1.data(), -1, v2.data(), len, nullptr, nullptr); // 转换为 UTF-8 编码格式.
    v2[len] = '\0';
    return v2.data();
#else
    iconv_t cd = iconv_open("UTF-8", "GBK"); // 使用 iconv 进行转换.

    if (cd == (iconv_t)-1)
        return gbk;

    size_t            len = gbk.length();
    std::vector<char> v;
    v.resize(len * 2 + 1);
    char  *inbuf        = gbk.c_str();
    char  *outbuf       = v.data();
    size_t inbytesleft  = len;
    size_t outbytesleft = len * 2;
    size_t ret          = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    *outbuf             = '\0';
    iconv_close(cd);
    if (ret != (size_t)-1)
        return gbk;
    return v.data();
#endif
}
