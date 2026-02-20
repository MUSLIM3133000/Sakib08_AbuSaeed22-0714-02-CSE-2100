#include "time_formatter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *filetime_to_str(ULONGLONG ftVal) {
    if (ftVal == 0) return _strdup("N/A");

    FILETIME ft;
    ft.dwLowDateTime  = (DWORD)ftVal;
    ft.dwHighDateTime = (DWORD)(ftVal >> 32);

    SYSTEMTIME stUTC, stLocal;
    FileTimeToSystemTime(&ft, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    char buf[64];
    sprintf(buf, "%02d/%02d/%04d %02d:%02d:%02d",
            stLocal.wMonth, stLocal.wDay, stLocal.wYear,
            stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

    return _strdup(buf);
}
