/**
 * @file core/types/event_record.c
 * @brief Implementation of core EventRecord operations
 *
 * Provides the level-to-string conversion and memory cleanup
 * for EventRecord structs. Pure logic with no platform dependencies.
 *
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#include "event_record.h"
#include <stdlib.h>

const char *EventRecord_GetLevelString(int level) {
    switch (level) {
        case EVENT_LEVEL_CRITICAL:    return "Critical";
        case EVENT_LEVEL_ERROR:       return "Error";
        case EVENT_LEVEL_WARNING:     return "Warning";
        case EVENT_LEVEL_INFORMATION: return "Information";
        case EVENT_LEVEL_VERBOSE:     return "Verbose";
        default:                      return "Audit Success";
    }
}

void EventRecord_Free(EventRecord *record) {
    if (!record) return;
    free(record->source);
    free(record->message);
    free(record->timestamp);
    free(record->computer);
    free(record->user);
    free(record->keywords);

    /* Null out freed pointers to prevent double-free */
    record->source    = NULL;
    record->message   = NULL;
    record->timestamp = NULL;
    record->computer  = NULL;
    record->user      = NULL;
    record->keywords  = NULL;
}
