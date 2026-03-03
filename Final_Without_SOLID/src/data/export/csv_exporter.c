/**
 * @file data/export/csv_exporter.c
 * @brief CSV export implementation
 *
 * Direct CSV export without strategy pattern abstraction.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#include "csv_exporter.h"
#include <stdio.h>
#include <stdlib.h>

int CsvExporter_Export(const char *filename, EventRecord *events, int count) {
    FILE *f = fopen(filename, "w");
    if (!f) return 0;

    fprintf(f, "EventID,Level,Source,Timestamp,Message\n");

    for (int i = 0; i < count; i++) {
        fprintf(f, "%lu,%s,\"%s\",\"%s\",\"%s\"\n",
            events[i].event_id,
            EventRecord_GetLevelString(events[i].level),
            events[i].source    ? events[i].source    : "",
            events[i].timestamp ? events[i].timestamp : "",
            events[i].message   ? events[i].message   : "");
    }

    fclose(f);
    return 1;
}
