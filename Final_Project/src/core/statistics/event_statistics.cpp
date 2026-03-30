/**
 * @file core/statistics/event_statistics.cpp
 * @brief EventStatisticsCalculator implementation (C++17)
 *
 * C improvement: The original called EventLog_Windows_Open() + Read() directly
 * and used raw pointers + malloc. Now uses the ILogRepository interface,
 * std::vector<EventRecord>, and no manual memory management.
 */

#include "event_statistics.h"
#include "core/readers/log_repository.h"
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
