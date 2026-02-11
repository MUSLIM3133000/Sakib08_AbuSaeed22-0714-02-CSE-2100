#ifndef EVENT_LOG_READER_H
#define EVENT_LOG_READER_H

#include <windows.h>
#include <winevt.h>

// ---- Event level constants ----
#define EVENT_LEVEL_CRITICAL     1
#define EVENT_LEVEL_ERROR        2
#define EVENT_LEVEL_WARNING      3
#define EVENT_LEVEL_INFORMATION  4
#define EVENT_LEVEL_VERBOSE      5

// ---- Event record structure ----
typedef struct {
    DWORD event_id;       // Event ID
    DWORD level;          // Level (Critical/Error/Warning/Information/Verbose)
    char *source;         // Provider / Source
    char *message;        // Event message text
    char *timestamp;      // Timestamp (local time string)
    char *computer;       // Computer name (optional)
    char *user;           // User (optional)
    DWORD task_category;  // Task category (optional)
    char *keywords;       // Keywords (optional)
} EventRecord;

typedef struct {
    int critical_count;
    int error_count;
    int warning_count;
    int information_count;
    int audit_success_count;
    int audit_failure_count;
} EventStatistics;

// Open a log, with optional filter by hours_back
EVT_HANDLE open_event_log(const wchar_t *log_name, int hours_back);

// Close an event log handle
void close_event_log(EVT_HANDLE handle);

// Read events from query handle
int read_events(EVT_HANDLE query_handle, EventRecord **events, int max_events);

// Free memory inside one event record
void free_event_record(EventRecord *event);

// Convert level → string ("Error", "Warning", etc.)
const char *get_event_level_string(int level);

// Get statistics for a log within hours_back timeframe
EventStatistics get_event_statistics(const wchar_t *log_name, int hours_back);

// Save events to CSV file
int save_events_to_csv(const char *filename, EventRecord *events, int count);

int load_events_from_csv(const char *filename, EventRecord **events);

#endif 
