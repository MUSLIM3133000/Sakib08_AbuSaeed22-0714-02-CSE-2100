#ifndef EVENT_RECORD_H
#define EVENT_RECORD_H

#include <windows.h>

#define EVENT_LEVEL_CRITICAL     1
#define EVENT_LEVEL_ERROR        2
#define EVENT_LEVEL_WARNING      3
#define EVENT_LEVEL_INFORMATION  4
#define EVENT_LEVEL_VERBOSE      5

typedef struct {
    DWORD event_id;
    DWORD level;
    char *source;
    char *message;
    char *timestamp;
    char *computer;
    char *user;
    DWORD task_category;
    char *keywords;
} EventRecord;

typedef struct {
    int critical_count;
    int error_count;
    int warning_count;
    int information_count;
    int audit_success_count;
    int audit_failure_count;
} EventStatistics;

const char *get_level_string(int level);
void free_event_record(EventRecord *rec);

#endif
