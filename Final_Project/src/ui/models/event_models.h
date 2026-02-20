#ifndef EVENT_MODELS_H
#define EVENT_MODELS_H

#include "ui/gtk/event_viewer_context.h"
#include "core/types/event_record.h"

void fill_tree(AppData *data);
void fill_tables(AppData *data);
void load_log_events(AppData *data, const char *log_name);
void fill_event_details(AppData *data, EventRecord *events, int count);

#endif
