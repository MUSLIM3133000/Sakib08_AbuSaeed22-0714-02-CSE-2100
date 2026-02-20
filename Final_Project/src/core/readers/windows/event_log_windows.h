#ifndef EVENT_LOG_WINDOWS_H
#define EVENT_LOG_WINDOWS_H

#include <windows.h>
#include <winevt.h>
#include "core/types/event_record.h"
#include "core/readers/log_repository.h"

void *open_log(const wchar_t *log_name, int hours_back);
int read_events(void *handle, EventRecord **events, int max_events);
EventStatistics get_stats(const wchar_t *log_name, int hours_back);
void close_log(void *handle);

extern EventLogRepository g_windowsRepository;

#endif
