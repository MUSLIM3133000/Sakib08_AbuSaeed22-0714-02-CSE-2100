/**
 * @file utils/time/time_formatter.h
 * @brief FILETIME → readable string utility (C++17)
 *
 * C improvement:
 *  - TimeFormatter_FiletimeToString() returned heap char* (caller must free)
 *  - Now returns std::string — no manual free, no leak.
 */

#pragma once

#include <string>
#include <cstdint>

namespace EventViewer::TimeFormatter {

/**
 * @brief Converts a Windows FILETIME (as uint64_t) to a local-time string.
 * @return "MM/DD/YYYY HH:MM:SS" or "N/A" if ftVal is 0.
 *
 * C equivalent: char* TimeFormatter_FiletimeToString(ULONGLONG ftVal);
 */
std::string fileTimeToString(uint64_t ftVal);

} // namespace EventViewer::TimeFormatter
