/**
 * @file core/readers/windows/event_log_windows.h
 * @brief Windows Event Log reader — concrete ILogRepository (C++17)
 *
 * SOLID changes vs C version:
 *  - SRP : This class ONLY reads Windows Event Log. No stats, no export, no UI.
 *  - LSP : Fully satisfies ILogRepository contract — substitutable anywhere.
 *  - DIP : Callers hold ILogRepository* — they never see this class by name.
 *
 * C improvement:
 *  - void* handle pattern (EVT_HANDLE cast) replaced with RAII wrapper.
 *  - Separate Open/Read/Close functions collapsed into one safe read() method.
 *  - Raw EventRecord** output pointer replaced with std::vector<EventRecord>.
 */

#pragma once

#ifdef _WIN32
#include <windows.h>
#include <winevt.h>
#endif

#include "core/readers/log_repository.h"

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
