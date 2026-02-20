/**
 * @file core/readers/windows/event_log_windows.c
 * @brief Windows Event Log API implementation
 *
 * Direct implementation using EvtQuery / EvtNext / EvtRender from winevt.h.
 *
 * @platform Windows Vista and later
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#include "event_log_windows.h"
#include "utils/conversion/string_utils.h"
#include "utils/time/time_formatter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *EventLog_Windows_Open(const wchar_t *log_name, int hours_back) {
    EVT_HANDLE hQuery = NULL;
    wchar_t query[256];

    if (hours_back > 0) {
        long long ms_back = (long long)hours_back * 60LL * 60LL * 1000LL;
        swprintf(query, 256,
            L"*[System[TimeCreated[timediff(@SystemTime) <= %lld]]]",
            ms_back);

        hQuery = EvtQuery(NULL, log_name, query,
                          EvtQueryChannelPath | EvtQueryForwardDirection);
    } else {
        hQuery = EvtQuery(NULL, log_name, L"*",
                          EvtQueryChannelPath | EvtQueryForwardDirection);
    }

    if (!hQuery) {
        wprintf(L"EvtQuery failed for log '%ls', error %lu\n",
                log_name, GetLastError());
    }

    return (void *)hQuery;
}

int EventLog_Windows_Read(void *handle, EventRecord **events, int max_events) {
    EVT_HANDLE query_handle = (EVT_HANDLE)handle;

    if (!query_handle || !events || max_events <= 0) return 0;

    EventRecord *arr = (EventRecord *)calloc((size_t)max_events, sizeof(EventRecord));
    if (!arr) return 0;

    int count = 0;
    EVT_HANDLE evtArray[16];
    DWORD returned = 0;

    EVT_HANDLE context = EvtCreateRenderContext(0, NULL, EvtRenderContextSystem);
    if (!context) {
        wprintf(L"EvtCreateRenderContext failed: %lu\n", GetLastError());
        free(arr);
        return 0;
    }

    while (count < max_events) {
        BOOL ok = EvtNext(query_handle, 16, evtArray, INFINITE, 0, &returned);
        if (!ok) {
            DWORD err = GetLastError();
            if (err == ERROR_NO_MORE_ITEMS) break;
            wprintf(L"EvtNext failed: %lu\n", err);
            break;
        }

        for (DWORD i = 0; i < returned && count < max_events; i++) {
            DWORD bufferUsed    = 0;
            DWORD propertyCount = 0;

            EvtRender(context, evtArray[i], EvtRenderEventValues,
                      0, NULL, &bufferUsed, &propertyCount);

            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                PEVT_VARIANT values = (PEVT_VARIANT)malloc(bufferUsed);

                if (values && EvtRender(context, evtArray[i], EvtRenderEventValues,
                                        bufferUsed, values, &bufferUsed, &propertyCount)) {

                    arr[count].event_id  = values[EvtSystemEventID].UInt16Val;
                    arr[count].level     = values[EvtSystemLevel].ByteVal;
                    arr[count].source    = StringUtils_WcharToUtf8(
                                              values[EvtSystemProviderName].StringVal);
                    arr[count].timestamp = TimeFormatter_FiletimeToString(
                                              values[EvtSystemTimeCreated].FileTimeVal);
                    arr[count].message   = _strdup("No message text");

                    arr[count].computer      = NULL;
                    arr[count].user          = NULL;
                    arr[count].task_category = 0;
                    arr[count].keywords      = NULL;

                    EVT_HANDLE pubMeta = EvtOpenPublisherMetadata(
                                            NULL,
                                            values[EvtSystemProviderName].StringVal,
                                            NULL, 0, 0);
                    if (pubMeta) {
                        WCHAR msgBuffer[1024];
                        DWORD msgUsed = 0;
                        if (EvtFormatMessage(pubMeta, evtArray[i], 0, 0, NULL,
                                             EvtFormatMessageEvent,
                                             1024, msgBuffer, &msgUsed)) {
                            char temp[2048];
                            wcstombs(temp, msgBuffer, sizeof(temp));
                            free(arr[count].message);
                            arr[count].message = _strdup(temp);
                        }
                        EvtClose(pubMeta);
                    }
                }
                free(values);
            }

            EvtClose(evtArray[i]);
            count++;
        }
    }

    EvtClose(context);
    *events = arr;
    return count;
}

EventStatistics EventLog_Windows_GetStatistics(const wchar_t *log_name, int hours_back) {
    EventStatistics stats = {0};

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
            default: break;
        }
        EventRecord_Free(&records[i]);
    }

    free(records);
    EventLog_Windows_Close(handle);
    return stats;
}

void EventLog_Windows_Close(void *handle) {
    if (handle) EvtClose((EVT_HANDLE)handle);
}
