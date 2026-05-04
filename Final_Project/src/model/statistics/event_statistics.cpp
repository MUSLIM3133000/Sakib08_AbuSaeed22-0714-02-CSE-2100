/**
 * @file model/statistics/event_statistics.cpp
 * @brief EventStatisticsCalculator implementation (C++17)
 *
 * MVC Role: MODEL — business logic for computing event statistics.
 *
 * Uses the ILogRepository interface (DIP) to read events,
 * then aggregates counts by severity level.
 */

#include "event_statistics.h"
#include "model/repository/log_repository.h"
#include <vector>

namespace EventViewer {

EventStatistics EventStatisticsCalculator::calculate(
    const wchar_t* logName, int hoursBack) const
{
    EventStatistics stats{};
    if (!logName) return stats;

    // DIP: use the abstract ILogRepository, not the concrete Windows class
    auto repo    = ILogRepository::create();
    auto records = repo->read(logName, hoursBack, 5000);

    for (const auto& r : records) {
        switch (r.level()) {
            case EventLevel::Critical:    ++stats.criticalCount;    break;
            case EventLevel::Error:       ++stats.errorCount;       break;
            case EventLevel::Warning:     ++stats.warningCount;     break;
            case EventLevel::Information: ++stats.informationCount; break;
            default: break;
        }
    }
    return stats;
}

} // namespace EventViewer
