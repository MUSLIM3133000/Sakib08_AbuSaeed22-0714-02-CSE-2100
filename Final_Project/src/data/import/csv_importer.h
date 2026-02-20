#ifndef CSV_IMPORTER_H
#define CSV_IMPORTER_H

#include "core/types/event_record.h"

int load_csv(const char *filename, EventRecord **events);

#endif
