#include "event_statistics.h"
#include <stdlib.h>

EventStatistics calc_stats(EventLogRepository *repo, const wchar_t *log_name, int hours_back) {
    EventStatistics stats = {0};
    if (!repo || !log_name) return stats;

    void *handle = repo->open(log_name, hours_back);
    if (!handle) return stats;

    EventRecord *records = NULL;
    int count = repo->read(handle, &records, 5000);

    for (int i = 0; i < count; i++) {
        switch (records[i].level) {
            case EVENT_LEVEL_CRITICAL:    stats.critical_count++;    break;
            case EVENT_LEVEL_ERROR:       stats.error_count++;       break;
            case EVENT_LEVEL_WARNING:     stats.warning_count++;     break;
            case EVENT_LEVEL_INFORMATION: stats.information_count++; break;
            default:                                                  break;
        }
        free_event_record(&records[i]);
    }

    free(records);
    repo->close(handle);
    return stats;
}
