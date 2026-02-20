#ifndef CSV_EXPORTER_H
#define CSV_EXPORTER_H

#include "core/types/event_record.h"

typedef enum {
    EXPORT_FORMAT_CSV  = 0,
    EXPORT_FORMAT_XML  = 1,
    EXPORT_FORMAT_JSON = 2
} ExportFormat;

typedef struct {
    const char *name;
    const char *extension;
    int (*export)(const char *filename, EventRecord *events, int count);
} ExportStrategy;

int export_csv(const char *filename, EventRecord *events, int count);
extern ExportStrategy g_csvExportStrategy;
ExportStrategy *ExportStrategyFactory_Create(ExportFormat format);

#endif
