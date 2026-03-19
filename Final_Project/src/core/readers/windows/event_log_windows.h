/**
 * @file core/readers/windows/event_log_windows.h
 * @brief Windows Event Log API reader declarations
 *
 * Direct implementation for reading Windows Event Logs using winevt.h API.
 *
 * @platform Windows Vista and later (requires wevtapi.dll)
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#ifndef EVENT_LOG_WINDOWS_H
#define EVENT_LOG_WINDOWS_H

#include <windows.h>
#include <winevt.h>
#include "core/types/event_record.h"

/**
 * @brief Opens a Windows Event Log channel for querying
 * @param log_name Wide-string channel name (e.g., L"Application", L"System")
 * @param hours_back Restrict query to events in last N hours; 0 = all events
 * @return EVT_HANDLE query handle cast to void*, or NULL on failure
 * @note Caller must pass handle to EventLog_Windows_Close() when done
 * @warning Security log requires administrator privileges
 */
void *EventLog_Windows_Open(const wchar_t *log_name, int hours_back);

/**
 * @brief Reads events from a Windows Event Log query handle
 * @param handle Opaque EVT_HANDLE returned from EventLog_Windows_Open()
 * @param events Output: allocated array of EventRecord; caller must free each + the array
 * @param max_events Upper limit of events to return
 * @return Count of events populated in *events, or 0 on error
 */
int EventLog_Windows_Read(void *handle, EventRecord **events, int max_events);

/**
 * @brief Computes event statistics for a Windows Event Log channel
 * @param log_name Wide-string channel name
 * @param hours_back Hours of history to include
 * @return Populated EventStatistics struct (zeroed on failure)
 */
EventStatistics EventLog_Windows_GetStatistics(const wchar_t *log_name, int hours_back);

/**
 * @brief Closes a Windows Event Log query handle
 * @param handle Opaque EVT_HANDLE to close
 */
void EventLog_Windows_Close(void *handle);

#endif /* EVENT_LOG_WINDOWS_H */
