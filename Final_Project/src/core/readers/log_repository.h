#ifndef LOG_REPOSITORY_H
#define LOG_REPOSITORY_H

#include "core/types/event_record.h"

typedef struct {
    const char *name;
    void *(*open)(const wchar_t *identifier, int hours_back);
    int (*read)(void *handle, EventRecord **events, int max_events);
    EventStatistics (*get_statistics)(const wchar_t *identifier, int hours_back);
    void (*close)(void *handle);
} EventLogRepository;

#endif
