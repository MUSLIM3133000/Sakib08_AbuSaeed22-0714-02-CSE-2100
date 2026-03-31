/**
 * @file utils/conversion/string_utils.h
 * @brief String encoding conversion utilities
 *
 * Provides helpers for converting between Windows wide-character
 * strings (wchar_t) and UTF-8 strings required by GTK4.
 *
 * @platform Windows (uses WideCharToMultiByte)
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <windows.h>

/**
 * @brief Converts a wide-character string to a heap-allocated UTF-8 string
 * @param wstr Source wide-character string; may be NULL
 * @return Heap-allocated UTF-8 string; caller must free(). Never returns NULL.
 * @note Returns a duplicate of "" when wstr is NULL
 */
char *StringUtils_WcharToUtf8(const wchar_t *wstr);

#endif /* STRING_UTILS_H */
