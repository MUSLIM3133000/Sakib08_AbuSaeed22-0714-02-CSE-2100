/**
 * @file utils/platform/privilege_check.c
 * @brief Windows administrator privilege detection implementation
 *
 * Uses AllocateAndInitializeSid to build the built-in Administrators SID,
 * then calls CheckTokenMembership to determine the process token's group
 * membership.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#include "privilege_check.h"

BOOL PlatformUtils_IsRunningAsAdmin(void) {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2,
                                  SECURITY_BUILTIN_DOMAIN_RID,
                                  DOMAIN_ALIAS_RID_ADMINS,
                                  0, 0, 0, 0, 0, 0,
                                  &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    return isAdmin;
}
