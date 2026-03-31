/**
 * @file utils/conversion/string_utils.c
 * @brief String encoding conversion implementation
 *
 * Uses WideCharToMultiByte to convert wchar_t* to UTF-8 char*.
 * This adapter is necessary because Windows Event Log API returns
 * wide strings while GTK4 widgets expect UTF-8.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#include "string_utils.h"
#include <stdlib.h>
#include <string.h>

char *StringUtils_WcharToUtf8(const wchar_t *wstr) {
    if (!wstr) return _strdup("");

    /* First call: calculate the required buffer size */
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr, -1,
                                          NULL, 0, NULL, NULL);
    char *str = (char *)malloc((size_t)sizeNeeded);
    if (!str) return _strdup("");

    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, sizeNeeded, NULL, NULL);
    return str;
}
