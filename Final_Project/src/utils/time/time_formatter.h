/**
 * @file utils/time/time_formatter.h
 * @brief FILETIME-to-string conversion utilities
 *
 * Converts Windows FILETIME values to human-readable local-time strings
 * for display in the event viewer UI.
 *
 * @platform Windows (uses FileTimeToSystemTime, SystemTimeToTzSpecificLocalTime)
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#ifndef TIME_FORMATTER_H
#define TIME_FORMATTER_H

#include <windows.h>

/**
 * @brief Converts a FILETIME value to a formatted local-time string
 * @param ftVal FILETIME encoded as a ULONGLONG (100-nanosecond intervals since 1601)
 * @return Heap-allocated string in "MM/DD/YYYY HH:MM:SS" format; caller must free()
 * @note Returns a duplicate of "N/A" when ftVal is 0
 */
char *TimeFormatter_FiletimeToString(ULONGLONG ftVal);

#endif /* TIME_FORMATTER_H */
