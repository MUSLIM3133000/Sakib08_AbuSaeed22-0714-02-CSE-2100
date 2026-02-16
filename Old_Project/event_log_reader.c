#include "event_log_reader.h"
#include <windows.h>
#include <winevt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *wchar_to_utf8(const wchar_t *wstr) {
    if (!wstr) return _strdup("");
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char *str = (char *)malloc(size_needed);
    if (!str) return _strdup("");
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, size_needed, NULL, NULL);
    return str;
}

static char *filetime_to_string(ULONGLONG ftVal) {
    if (ftVal == 0) return _strdup("N/A");

    FILETIME ft;
    ft.dwLowDateTime  = (DWORD)ftVal;
    ft.dwHighDateTime = (DWORD)(ftVal >> 32);

    SYSTEMTIME stUTC, stLocal;
    FileTimeToSystemTime(&ft, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    char buf[64];
    sprintf(buf, "%02d/%02d/%04d %02d:%02d:%02d",
        stLocal.wMonth, stLocal.wDay, stLocal.wYear,
        stLocal.wHour, stLocal.wMinute, stLocal.wSecond);

    return _strdup(buf);
}


int read_events(EVT_HANDLE query_handle, EventRecord **events, int max_events) {
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
            DWORD bufferUsed = 0;
            DWORD propertyCount = 0;

            EvtRender(context, evtArray[i], EvtRenderEventValues,
                      0, NULL, &bufferUsed, &propertyCount);

            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                PEVT_VARIANT values = (PEVT_VARIANT)malloc(bufferUsed);
                if (values && EvtRender(context, evtArray[i], EvtRenderEventValues,
                                        bufferUsed, values, &bufferUsed, &propertyCount)) {

                    arr[count].event_id  = values[EvtSystemEventID].UInt16Val;
                    arr[count].level     = values[EvtSystemLevel].ByteVal;
                    arr[count].source    = wchar_to_utf8(values[EvtSystemProviderName].StringVal);
                    arr[count].timestamp = filetime_to_string(values[EvtSystemTimeCreated].FileTimeVal);
                    arr[count].message   = _strdup("No message text");

                    arr[count].computer      = NULL;
                    arr[count].user          = NULL;
                    arr[count].task_category = 0;
                    arr[count].keywords      = NULL;

                    EVT_HANDLE pubMeta = EvtOpenPublisherMetadata(NULL,
                                                                  values[EvtSystemProviderName].StringVal,
                                                                  NULL, 0, 0);
                    if (pubMeta) {
                        WCHAR msgBuffer[1024];
                        DWORD msgUsed = 0;
                        if (EvtFormatMessage(pubMeta, evtArray[i], 0, 0, NULL,
                                             EvtFormatMessageEvent, 1024, msgBuffer, &msgUsed)) {
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

// ---- Extra functions ----

EVT_HANDLE open_event_log(const wchar_t *log_name, int hours_back) {
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
        wprintf(L"EvtQuery failed for log %ls, error %lu\n", log_name, GetLastError());
    }

    return hQuery;
}

void close_event_log(EVT_HANDLE h) {
    if (h) EvtClose(h);
}

void free_event_record(EventRecord *rec) {
    if (!rec) return;
    if (rec->source) free(rec->source);
    if (rec->message) free(rec->message);
    if (rec->timestamp) free(rec->timestamp);
    if (rec->computer) free(rec->computer);
    if (rec->user) free(rec->user);
    if (rec->keywords) free(rec->keywords);
}

const char *get_event_level_string(int level) {
    switch (level) {
        case 1: return "Critical";
        case 2: return "Error";
        case 3: return "Warning";
        case 4: return "Information";
        case 5: return "Verbose";
        default: return "Audit Success";
    }
}

EventStatistics get_event_statistics(const wchar_t *log_name, int hours_back) {
    EventStatistics stats = {0};

    EVT_HANDLE handle = open_event_log(log_name, hours_back);
    if (!handle) return stats;

    EventRecord *records = NULL;
    int count = read_events(handle, &records, 5000);

    for (int i = 0; i < count; i++) {
        switch (records[i].level) {
            case EVENT_LEVEL_CRITICAL:    stats.critical_count++; break;
            case EVENT_LEVEL_ERROR:       stats.error_count++; break;
            case EVENT_LEVEL_WARNING:     stats.warning_count++; break;
            case EVENT_LEVEL_INFORMATION: stats.information_count++; break;
            default: break;
        }
        free_event_record(&records[i]);
    }

    free(records);
    close_event_log(handle);
    return stats;
}



int save_events_to_csv(const char *filename, EventRecord *events, int count) {
    FILE *f = fopen(filename, "w");
    if (!f) return 0;

    fprintf(f, "EventID,Level,Source,Timestamp,Message\n");

    for (int i = 0; i < count; i++) {
        fprintf(f, "%lu,%s,\"%s\",\"%s\",\"%s\"\n",
            events[i].event_id,
            get_event_level_string(events[i].level),
            events[i].source ? events[i].source : "",
            events[i].timestamp ? events[i].timestamp : "",
            events[i].message ? events[i].message : "");
    }

    fclose(f);
    return 1;
}

int load_events_from_csv(const char *filename, EventRecord **events) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;

    char line[4096];
    int count = 0;
    EventRecord *arr = NULL;

    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        return 0;
    }

    while (fgets(line, sizeof(line), f)) {
        arr = (EventRecord *)realloc(arr, sizeof(EventRecord) * (count + 1));
        EventRecord *e = &arr[count];

        e->source = e->message = e->timestamp = e->computer = e->user = e->keywords = NULL;
        e->task_category = 0;

        char level_str[32], source[512], timestamp[128], message[2048];
        if (sscanf(line, "%lu,%31[^,],%511[^,],%127[^,],%2047[^\n]",
                   &e->event_id, level_str, source, timestamp, message) == 5) {
            e->level = 0;
            if (strcmp(level_str, "Critical") == 0) e->level = EVENT_LEVEL_CRITICAL;
            else if (strcmp(level_str, "Error") == 0) e->level = EVENT_LEVEL_ERROR;
            else if (strcmp(level_str, "Warning") == 0) e->level = EVENT_LEVEL_WARNING;
            else if (strcmp(level_str, "Information") == 0) e->level = EVENT_LEVEL_INFORMATION;
            else if (strcmp(level_str, "Verbose") == 0) e->level = EVENT_LEVEL_VERBOSE;

            e->source    = _strdup(source);
            e->timestamp = _strdup(timestamp);
            e->message   = _strdup(message);
        }

        count++;
    }

    fclose(f);
    *events = arr;
    return count;
}

