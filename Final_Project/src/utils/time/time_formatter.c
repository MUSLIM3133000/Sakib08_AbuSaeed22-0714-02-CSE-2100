/**
 * @file utils/time/time_formatter.c
 * @brief FILETIME-to-string conversion implementation
 *
 * Converts raw Windows FILETIME values to readable timestamps by:
 * 1. Splitting the ULONGLONG into high/low DWORD parts
 * 2. Converting UTC FILETIME → SYSTEMTIME via FileTimeToSystemTime
 * 3. Converting UTC → local time via SystemTimeToTzSpecificLocalTime
 * 4. Formatting with sprintf
 *
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#include "time_formatter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *TimeFormatter_FiletimeToString(ULONGLONG ftVal) {
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
