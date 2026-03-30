/**
 * @file core/statistics/event_statistics.h
 * @brief Event statistics types and calculator interface (C++17)
 *
 * SOLID changes vs C version:
 *  - SRP : EventStatistics is a plain value object — no calculation inside.
 *          IStatisticsCalculator is a separate interface for calculation.
 *  - ISP : IStatisticsCalculator has exactly one responsibility.
 *  - DIP : Callers depend on IStatisticsCalculator, not on the concrete impl.
 *
 * C improvement:
 *  - The C EventStatistics_Calculate() called Windows API directly (hard-coded).
 *    Now calculation is behind an interface — swappable, testable.
 */

#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include "core/types/event_record.h"

namespace EventViewer {

// ── Value object: aggregated counts ──────────────────────────────────────────
struct EventStatistics {
    uint32_t criticalCount    = 0;
    uint32_t errorCount       = 0;
    uint32_t warningCount     = 0;
    uint32_t informationCount = 0;
    uint32_t auditSuccessCount = 0;
    uint32_t auditFailureCount = 0;

    uint32_t total() const noexcept {
        return criticalCount + errorCount + warningCount
             + informationCount + auditSuccessCount + auditFailureCount;
    }
};

// ── Interface: calculation strategy (DIP + ISP) ───────────────────────────────
// C version had one global function tightly coupled to Windows API.
// Now any class can implement this interface (live log, CSV file, mock data).
class IStatisticsCalculator {
public:
    virtual ~IStatisticsCalculator() = default;

    /**
     * @brief Calculates event statistics for the given log channel.
     * @param logName    Wide-string log channel (e.g. L"Application")
     * @param hoursBack  Hours of history; 0 = all events
     */
    virtual EventStatistics calculate(const wchar_t* logName,
                                      int hoursBack) const = 0;
};

// ── Concrete: reads stats from Windows Event Log ──────────────────────────────
class EventStatisticsCalculator : public IStatisticsCalculator {
public:
    EventStatistics calculate(const wchar_t* logName,
                              int hoursBack) const override;
};

} // namespace EventViewer
