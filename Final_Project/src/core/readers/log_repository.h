/**
 * @file core/readers/log_repository.h
 * @brief Direct event log reading function declarations
 *
 * Provides direct access to Windows Event Log reading functionality.
 * No abstraction layer - direct implementation.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#ifndef LOG_REPOSITORY_H
#define LOG_REPOSITORY_H

#include "core/types/event_record.h"

/**
 * @brief Opens a Windows Event Log for reading
 * @param log_name Wide-character log name (e.g., L"Application", L"System")
 * @param hours_back Hours of history to query (0 = all events)
 * @return Handle to the opened log, or NULL on failure
 */
void *EventLog_Open(const wchar_t *log_name, int hours_back);

/**
 * @brief Reads events from an open log handle
 * @param handle Handle returned from EventLog_Open()
 * @param events Output pointer to event array (caller must free)
 * @param max_events Maximum number of events to read
 * @return Number of events read, or 0 on error
 */
int EventLog_Read(void *handle, EventRecord **events, int max_events);

/**
 * @brief Gets statistics for a log
 * @param log_name Wide-character log name
 * @param hours_back Hours of history to analyze
 * @return Event statistics structure
 */
EventStatistics EventLog_GetStatistics(const wchar_t *log_name, int hours_back);

/**
 * @brief Closes an open log handle
 * @param handle Handle to close
 */
void EventLog_Close(void *handle);

#endif /* LOG_REPOSITORY_H */
