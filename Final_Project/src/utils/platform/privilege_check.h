/**
 * @file utils/platform/privilege_check.h
 * @brief Windows admin privilege check (C++17)
 *
 * C improvement: returns bool instead of BOOL (portable, type-safe).
 */

#pragma once

namespace EventViewer::PlatformUtils {

/**
 * @brief Checks whether the current process has administrator rights.
 * @return true if running as admin, false otherwise.
 *
 * C equivalent: BOOL PlatformUtils_IsRunningAsAdmin(void);
 */
bool isRunningAsAdmin() noexcept;

} // namespace EventViewer::PlatformUtils
