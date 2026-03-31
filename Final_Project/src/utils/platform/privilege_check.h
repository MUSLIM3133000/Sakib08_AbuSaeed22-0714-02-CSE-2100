/**
 * @file utils/platform/privilege_check.h
 * @brief Windows administrator privilege detection
 *
 * Isolated here so that all Windows security API usage is confined
 * to utils/platform/ — no other module needs to know about SIDs.
 *
 * @platform Windows (uses AllocateAndInitializeSid, CheckTokenMembership)
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#ifndef PRIVILEGE_CHECK_H
#define PRIVILEGE_CHECK_H

#include <windows.h>

/**
 * @brief Checks whether the current process is running with administrator rights
 * @return TRUE if the process token belongs to the built-in Administrators group
 * @return FALSE otherwise (or if the check itself fails)
 */
BOOL PlatformUtils_IsRunningAsAdmin(void);

#endif /* PRIVILEGE_CHECK_H */
