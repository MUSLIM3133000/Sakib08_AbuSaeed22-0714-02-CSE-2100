#include "string_utils.h"
#include <stdlib.h>
#include <string.h>

char *wchar_to_utf8(const wchar_t *wstr) {
    if (!wstr) return _strdup("");

    int size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char *str = (char *)malloc((size_t)size);
    if (!str) return _strdup("");

    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, size, NULL, NULL);
    return str;
}
