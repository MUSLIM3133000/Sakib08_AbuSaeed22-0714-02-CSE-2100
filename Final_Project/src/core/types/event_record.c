#include "event_record.h"
#include <stdlib.h>

const char *get_level_string(int level) {
    switch (level) {
        case EVENT_LEVEL_CRITICAL:    return "Critical";
        case EVENT_LEVEL_ERROR:       return "Error";
        case EVENT_LEVEL_WARNING:     return "Warning";
        case EVENT_LEVEL_INFORMATION: return "Information";
        case EVENT_LEVEL_VERBOSE:     return "Verbose";
        default:                      return "Audit Success";
    }
}

void free_event_record(EventRecord *rec) {
    if (!rec) return;
    free(rec->source);
    free(rec->message);
    free(rec->timestamp);
    free(rec->computer);
    free(rec->user);
    free(rec->keywords);

    rec->source    = NULL;
    rec->message   = NULL;
    rec->timestamp = NULL;
    rec->computer  = NULL;
    rec->user      = NULL;
    rec->keywords  = NULL;
}
