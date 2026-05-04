/**
 * @file model/repository/windows/event_log_windows.h
 * @brief Windows Event Log reader — concrete ILogRepository (C++17)
 *
 * MVC Role: MODEL — platform-specific data access implementation.
 *
 * SOLID principles retained:
 *  - SRP : This class ONLY reads Windows Event Log. No stats, no export, no UI.
 *  - LSP : Fully satisfies ILogRepository contract — substitutable anywhere.
 *  - DIP : Callers hold ILogRepository* — they never see this class by name.
 */

#pragma once

#ifdef _WIN32
#include <windows.h>
#include <winevt.h>
#endif

#include "model/repository/log_repository.h"

namespace EventViewer {

class WindowsEventLogRepository final : public ILogRepository {
public:
    std::vector<EventRecord>  read(const wchar_t* logName,
                                   int   hoursBack,
                                   int   maxEvents) const override;

    std::vector<std::wstring> availableLogs() const override;

private:
#ifdef _WIN32
    static std::wstring buildXPathQuery(int hoursBack);
    static EventRecord  parseEvent(EVT_HANDLE hEvent, EVT_HANDLE hCtx);
#endif
};

} // namespace EventViewer
