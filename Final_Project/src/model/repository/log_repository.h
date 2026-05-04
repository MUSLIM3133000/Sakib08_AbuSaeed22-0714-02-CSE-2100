/**
 * @file model/repository/log_repository.h
 * @brief Abstract interface for reading event log data (C++17)
 *
 * MVC Role: MODEL — data access abstraction for event logs.
 *
 * SOLID principles retained:
 *  - DIP : High-level modules depend on THIS interface,
 *          not on event_log_windows.h or any platform-specific header.
 *  - OCP : Add a Linux/syslog reader by implementing ILogRepository — zero
 *          changes to any existing code.
 *  - LSP : Any concrete ILogRepository can be substituted transparently.
 *  - ISP : Interface is focused — only read operations. Export is separate.
 */

#pragma once

#include <vector>
#include <string>
#include <memory>
#include "model/types/event_record.h"

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
