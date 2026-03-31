/**
 * @file utils/platform/privilege_check.cpp
 * @brief Admin privilege check implementation (C++17)
 */

#include "privilege_check.h"

#ifdef _WIN32
#  include <windows.h>
#endif

namespace EventViewer::PlatformUtils {

bool isRunningAsAdmin() noexcept {
#ifdef _WIN32
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2,
                                  SECURITY_BUILTIN_DOMAIN_RID,
                                  DOMAIN_ALIAS_RID_ADMINS,
                                  0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(nullptr, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    return isAdmin != FALSE;
#else
    return false;
#endif
}

} // namespace EventViewer::PlatformUtils
