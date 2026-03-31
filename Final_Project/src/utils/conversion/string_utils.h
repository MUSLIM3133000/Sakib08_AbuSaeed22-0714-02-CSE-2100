/**
 * @file utils/conversion/string_utils.h
 * @brief Wide-string / UTF-8 conversion utilities (C++17)
 *
 * C improvement:
 *  - StringUtils_WcharToUtf8() returned heap char* (caller must free)
 *  - Now returns std::string — automatic cleanup, no leak possible.
 */

#pragma once

#include <string>

namespace EventViewer::StringUtils {

/** Converts wchar_t* → UTF-8 std::string. Returns "" for nullptr input. */
std::string wcharToUtf8(const wchar_t* wstr);

/** Converts std::string (UTF-8) → std::wstring. */
std::wstring utf8ToWchar(const std::string& str);

} // namespace EventViewer::StringUtils
