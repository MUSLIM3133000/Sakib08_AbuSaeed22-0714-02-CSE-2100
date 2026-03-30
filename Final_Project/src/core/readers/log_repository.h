/**
 * @file core/readers/log_repository.h
 * @brief Abstract interface for reading event log data (C++17)
 *
 * SOLID changes vs C version:
 *  - DIP : High-level modules (statistics, UI models) depend on THIS interface,
 *          not on event_log_windows.h or any platform-specific header.
 *  - OCP : Add a Linux/syslog reader by implementing ILogRepository — zero
 *          changes to any existing code.
 *  - LSP : Any concrete ILogRepository can be substituted transparently.
 *  - ISP : Interface is focused — only read operations. Export is separate.
 *
 * C improvement:
 *  - void* handle pattern replaced by a proper abstract class.
 *  - Raw EventRecord* array replaced by std::vector<EventRecord>.
 *  - Caller no longer responsible for free() on every element.
 *  - Factory method create() returns a platform-appropriate implementation.
 */

#pragma once

#include <vector>
#include <string>
#include <memory>
#include "core/types/event_record.h"

namespace EventViewer {

class ILogRepository {
public:
    virtual ~ILogRepository() = default;

    /**
     * @brief Reads events from the named log channel.
     * @param logName    Wide-string channel (e.g. L"Application")
     * @param hoursBack  Hours of history to include; 0 = all events
     * @param maxEvents  Upper bound on returned records
     * @return Vector of EventRecord (empty on failure — never throws)
     *
     * C equivalent: EventLog_Open() + EventLog_Read() + EventLog_Close()
     * collapsed into one safe call that cleans up after itself.
     */
    virtual std::vector<EventRecord> read(const wchar_t* logName,
                                          int   hoursBack,
                                          int   maxEvents) const = 0;

    /** @brief Returns the names of available log channels for the sidebar tree. */
    virtual std::vector<std::wstring> availableLogs() const = 0;

    /**
     * @brief Factory: returns a platform-appropriate concrete repository.
     * Windows build → WindowsEventLogRepository.
     * Non-Windows build → stub implementation (empty results).
     */
    static std::unique_ptr<ILogRepository> create();
};

} // namespace EventViewer
