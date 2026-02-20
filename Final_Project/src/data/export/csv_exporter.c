#include "csv_exporter.h"
#include <stdio.h>
#include <stdlib.h>

int export_csv(const char *filename, EventRecord *events, int count) {
    FILE *f = fopen(filename, "w");
    if (!f) return 0;

    fprintf(f, "EventID,Level,Source,Timestamp,Message\n");

    for (int i = 0; i < count; i++) {
        fprintf(f, "%lu,%s,\"%s\",\"%s\",\"%s\"\n",
            events[i].event_id,
            get_level_string(events[i].level),
            events[i].source    ? events[i].source    : "",
            events[i].timestamp ? events[i].timestamp : "",
            events[i].message   ? events[i].message   : "");
    }

    fclose(f);
    return 1;
}

ExportStrategy g_csvExportStrategy = {
    .name      = "CSV (Comma-Separated Values)",
    .extension = ".csv",
    .export    = export_csv
};

ExportStrategy *ExportStrategyFactory_Create(ExportFormat format) {
    switch (format) {
        case EXPORT_FORMAT_CSV:  return &g_csvExportStrategy;
        default: return NULL;
    }
}
