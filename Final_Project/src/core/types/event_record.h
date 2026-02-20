/**
 * @file core/types/event_record.h
 * @brief Core data types for the Windows Event Viewer
 *
 * Defines the fundamental data structures used throughout the application:
 * EventRecord (single event), EventStatistics (aggregated counts),
 * and event level constants.
 *
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#ifndef EVENT_RECORD_H
#define EVENT_RECORD_H

#include <windows.h>

/* ---- Event level constants ---- */
#define EVENT_LEVEL_CRITICAL     1
#define EVENT_LEVEL_ERROR        2
#define EVENT_LEVEL_WARNING      3
#define EVENT_LEVEL_INFORMATION  4
#define EVENT_LEVEL_VERBOSE      5

/**
 * @brief Represents a single Windows event log entry
 */
typedef struct {
    DWORD event_id;       /**< Event ID number */
    DWORD level;          /**< Severity level (use EVENT_LEVEL_* constants) */
    char *source;         /**< Provider/source name (heap-allocated, must free) */
    char *message;        /**< Event message text (heap-allocated, must free) */
    char *timestamp;      /**< Formatted local timestamp (heap-allocated, must free) */
    char *computer;       /**< Computer name, may be NULL */
    char *user;           /**< User account, may be NULL */
    DWORD task_category;  /**< Task category code */
    char *keywords;       /**< Event keywords, may be NULL */
} EventRecord;

/**
 * @brief Aggregated event counts grouped by severity level
 */
typedef struct {
    int critical_count;       /**< Number of Critical events */
    int error_count;          /**< Number of Error events */
    int warning_count;        /**< Number of Warning events */
    int information_count;    /**< Number of Informational events */
    int audit_success_count;  /**< Number of Audit Success events */
    int audit_failure_count;  /**< Number of Audit Failure events */
} EventStatistics;

/**
 * @brief Converts a numeric event level to its string representation
 * @param level Numeric level (use EVENT_LEVEL_* constants)
 * @return Static string: "Critical", "Error", "Warning", "Information", "Verbose", or "Audit Success"
 */
const char *EventRecord_GetLevelString(int level);

/**
 * @brief Frees all heap-allocated fields inside an EventRecord
 * @param record Pointer to the record to free (the struct itself is NOT freed)
 * @note After calling, all char* fields are invalid. Do not free the struct pointer itself.
 */
void EventRecord_Free(EventRecord *record);

#endif /* EVENT_RECORD_H */
