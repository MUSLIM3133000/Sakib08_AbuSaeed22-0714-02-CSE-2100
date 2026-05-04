/**
 * @file model/repository/windows/event_log_windows.cpp
 * @brief Windows Event Log reader implementation (C++17)
 *
 * MVC Role: MODEL — reads event data from Windows Event Log API.
 *
 * C improvements:
 *  - EVT_HANDLE cast to void* and back  →  typed EVT_HANDLE with RAII lambda guard
 *  - calloc(max_events) upfront         →  std::vector grown on demand
 *  - _strdup / wcstombs                 →  StringUtils::wcharToUtf8 returning std::string
 *  - Manual EvtClose in every path      →  unique_ptr custom deleter (guaranteed close)
 *  - EventLog_Windows_Open/Read/Close   →  single read() method, no leaks possible
 */

#include "event_log_windows.h"
#include "utils/conversion/string_utils.h"
#include "utils/time/time_formatter.h"
#include <algorithm>

namespace EventViewer {

// ── ILogRepository factory (defined here — only this TU knows the concrete type)
std::unique_ptr<ILogRepository> ILogRepository::create() {
    return std::make_unique<WindowsEventLogRepository>();
}

// ─────────────────────────────────────────────────────────────────────────────
// WindowsEventLogRepository::read
// ─────────────────────────────────────────────────────────────────────────────
std::vector<EventRecord> WindowsEventLogRepository::read(
    const wchar_t* logName, int hoursBack, int maxEvents) const
{
    std::vector<EventRecord> results;

#ifdef _WIN32
    if (!logName || maxEvents <= 0) return results;

    std::wstring xpath = buildXPathQuery(hoursBack);

    // RAII: EVT_HANDLE auto-closed when hQuery goes out of scope
    auto evtDeleter = [](EVT_HANDLE h){ if (h) EvtClose(h); };
    using EvtHandleGuard = std::unique_ptr<void, decltype(evtDeleter)>;

    EvtHandleGuard hQuery(
        EvtQuery(nullptr, logName,
                 xpath.empty() ? L"*" : xpath.c_str(),
                 EvtQueryChannelPath | EvtQueryForwardDirection),
        evtDeleter);

    if (!hQuery) return results;

    // Render context (system fields only)
    EvtHandleGuard hCtx(
        EvtCreateRenderContext(0, nullptr, EvtRenderContextSystem),
        evtDeleter);

    if (!hCtx) return results;

    results.reserve(std::min(maxEvents, 1000));

    constexpr DWORD BATCH = 16;
    EVT_HANDLE batch[BATCH];
    DWORD returned = 0;

    while (static_cast<int>(results.size()) < maxEvents) {
        if (!EvtNext(hQuery.get(), BATCH, batch, INFINITE, 0, &returned)) {
            if (GetLastError() == ERROR_NO_MORE_ITEMS) break;
            break;
        }
        for (DWORD i = 0; i < returned; ++i) {
            EvtHandleGuard hEvt(batch[i], evtDeleter);
            if (static_cast<int>(results.size()) < maxEvents)
                results.push_back(parseEvent(hEvt.get(), hCtx.get()));
        }
    }
#else
    (void)logName; (void)hoursBack; (void)maxEvents;
#endif

    return results;
}

std::vector<std::wstring> WindowsEventLogRepository::availableLogs() const {
    return { L"Application", L"System", L"Security", L"Setup" };
}

// ─────────────────────────────────────────────────────────────────────────────
// Private helpers
// ─────────────────────────────────────────────────────────────────────────────
#ifdef _WIN32
std::wstring WindowsEventLogRepository::buildXPathQuery(int hoursBack) {
    if (hoursBack <= 0) return {};
    wchar_t buf[256];
    long long ms = static_cast<long long>(hoursBack) * 3600LL * 1000LL;
    swprintf_s(buf, L"*[System[TimeCreated[timediff(@SystemTime) <= %lld]]]", ms);
    return buf;
}

EventRecord WindowsEventLogRepository::parseEvent(EVT_HANDLE hEvent,
                                                   EVT_HANDLE hCtx)
{
    DWORD bufUsed = 0, propCount = 0;
    EvtRender(hCtx, hEvent, EvtRenderEventValues, 0, nullptr, &bufUsed, &propCount);

    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) return {};

    std::vector<BYTE> buf(bufUsed);
    auto* vals = reinterpret_cast<PEVT_VARIANT>(buf.data());

    if (!EvtRender(hCtx, hEvent, EvtRenderEventValues,
                   bufUsed, vals, &bufUsed, &propCount))
        return {};

    uint32_t   eventId  = vals[EvtSystemEventID].UInt16Val;
    auto       level    = static_cast<EventLevel>(vals[EvtSystemLevel].ByteVal);
    uint32_t   taskCat  = vals[EvtSystemTask].UInt16Val;
    ULONGLONG  ft       = (vals[EvtSystemTimeCreated].Type == EvtVarTypeFileTime)
                          ? vals[EvtSystemTimeCreated].FileTimeVal : 0;

    const wchar_t* providerW = (vals[EvtSystemProviderName].Type == EvtVarTypeString)
                                ? vals[EvtSystemProviderName].StringVal : L"";
    const wchar_t* computerW = (vals[EvtSystemComputer].Type == EvtVarTypeString)
                                ? vals[EvtSystemComputer].StringVal : L"";

    // Fetch rendered message
    std::wstring msgW;
    DWORD msgSize = 0;
    EvtFormatMessage(nullptr, hEvent, 0, 0, nullptr,
                     EvtFormatMessageEvent, 0, nullptr, &msgSize);
    if (msgSize > 0) {
        msgW.resize(msgSize);
        EvtFormatMessage(nullptr, hEvent, 0, 0, nullptr,
                         EvtFormatMessageEvent,
                         static_cast<DWORD>(msgW.size()),
                         msgW.data(), &msgSize);
    }

    return EventRecord{
        eventId,
        level,
        StringUtils::wcharToUtf8(providerW),
        msgW.empty() ? "No message" : StringUtils::wcharToUtf8(msgW.c_str()),
        TimeFormatter::fileTimeToString(ft),
        StringUtils::wcharToUtf8(computerW),
        {},
        taskCat
    };
}
#endif

} // namespace EventViewer
