#include "csv_importer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_csv(const char *filename, EventRecord **events) {
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
        arr = (EventRecord *)realloc(arr, sizeof(EventRecord) * (size_t)(count + 1));
        EventRecord *e = &arr[count];

        e->source = e->message = e->timestamp =
        e->computer = e->user = e->keywords = NULL;
        e->task_category = 0;
        e->level = 0;

        char levelStr[32], source[512], timestamp[128], message[2048];

        if (sscanf(line, "%lu,%31[^,],%511[^,],%127[^,],%2047[^\n]",
                   &e->event_id, levelStr, source, timestamp, message) == 5) {

            if      (strcmp(levelStr, "Critical")    == 0) e->level = EVENT_LEVEL_CRITICAL;
            else if (strcmp(levelStr, "Error")        == 0) e->level = EVENT_LEVEL_ERROR;
            else if (strcmp(levelStr, "Warning")      == 0) e->level = EVENT_LEVEL_WARNING;
            else if (strcmp(levelStr, "Information")  == 0) e->level = EVENT_LEVEL_INFORMATION;
            else if (strcmp(levelStr, "Verbose")      == 0) e->level = EVENT_LEVEL_VERBOSE;

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
