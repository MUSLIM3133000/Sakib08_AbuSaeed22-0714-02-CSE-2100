/**
 * @file utils/conversion/string_utils.cpp
 * @brief String conversion implementation (C++17)
 */

#include "string_utils.h"

#ifdef _WIN32
#  include <windows.h>
#endif

namespace EventViewer::StringUtils {

std::string wcharToUtf8(const wchar_t* wstr) {
    if (!wstr || wstr[0] == L'\0') return {};
#ifdef _WIN32
    int n = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    if (n <= 0) return {};
    std::string result(static_cast<size_t>(n - 1), '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, result.data(), n, nullptr, nullptr);
    return result;
#else
    // Portable fallback: assume ASCII-range wchar_t
    return std::string(wstr, wstr + wcslen(wstr));
#endif
}

std::wstring utf8ToWchar(const std::string& str) {
    if (str.empty()) return {};
#ifdef _WIN32
    int n = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    if (n <= 0) return {};
    std::wstring result(static_cast<size_t>(n - 1), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, result.data(), n);
    return result;
#else
    return std::wstring(str.begin(), str.end());
#endif
}

} // namespace EventViewer::StringUtils
