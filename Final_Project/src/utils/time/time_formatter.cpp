/**
 * @file utils/time/time_formatter.cpp
 * @brief TimeFormatter implementation (C++17)
 */

#include "time_formatter.h"
#include <cstdio>
#include <ctime>

#ifdef _WIN32
#  include <windows.h>
#endif

namespace EventViewer::TimeFormatter {

std::string fileTimeToString(uint64_t ftVal) {
    if (ftVal == 0) return "N/A";

#ifdef _WIN32
    FILETIME ft;
    ft.dwLowDateTime  = static_cast<DWORD>(ftVal & 0xFFFFFFFF);
    ft.dwHighDateTime = static_cast<DWORD>(ftVal >> 32);

    SYSTEMTIME stUTC{}, stLocal{};
    FileTimeToSystemTime(&ft, &stUTC);
    SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal);

    char buf[32];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d %02d:%02d:%02d",
             stLocal.wMonth, stLocal.wDay, stLocal.wYear,
             stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
    return buf;
#else
    // Portable: convert 100-ns intervals from 1601 to Unix epoch
    constexpr uint64_t EPOCH_DIFF = 116444736000000000ULL;
    time_t unix = static_cast<time_t>((ftVal - EPOCH_DIFF) / 10000000ULL);
    struct tm* t = localtime(&unix);
    char buf[32];
    strftime(buf, sizeof(buf), "%m/%d/%Y %H:%M:%S", t);
    return buf;
#endif
}

} // namespace EventViewer::TimeFormatter
