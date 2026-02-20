/**
 * @file core/statistics/event_statistics.c
 * @brief Event statistics calculation implementation
 *
 * Counts events by severity level by directly calling Windows Event Log API.
 *
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#include "event_statistics.h"
#include "core/readers/windows/event_log_windows.h"
#include <stdlib.h>

EventStatistics EventStatistics_Calculate(const wchar_t *log_name, int hours_back) {
    EventStatistics stats = {0};
    if (!log_name) return stats;

    void *handle = EventLog_Windows_Open(log_name, hours_back);
    if (!handle) return stats;

    EventRecord *records = NULL;
    int count = EventLog_Windows_Read(handle, &records, 5000);

    for (int i = 0; i < count; i++) {
        switch (records[i].level) {
            case EVENT_LEVEL_CRITICAL:    stats.critical_count++;    break;
            case EVENT_LEVEL_ERROR:       stats.error_count++;       break;
            case EVENT_LEVEL_WARNING:     stats.warning_count++;     break;
            case EVENT_LEVEL_INFORMATION: stats.information_count++; break;
            default:                                                  break;
        }
        EventRecord_Free(&records[i]);
    }

    free(records);
    EventLog_Windows_Close(handle);
    return stats;
}
