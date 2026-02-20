#ifndef EVENT_STATISTICS_H
#define EVENT_STATISTICS_H

#include "core/types/event_record.h"
#include "core/readers/log_repository.h"

EventStatistics calc_stats(EventLogRepository *repo, const wchar_t *log_name, int hours_back);

#endif
