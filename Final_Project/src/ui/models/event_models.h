/**
 * @file ui/models/event_models.h
 * @brief GTK data model population for event records
 *
 * Bridges between the core EventRecord layer and GTK4 list/tree
 * stores. No Windows API calls appear here; data comes in as plain
 * EventRecord arrays.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#ifndef EVENT_MODELS_H
#define EVENT_MODELS_H

#include "ui/gtk/event_viewer_context.h"
#include "core/types/event_record.h"

/**
 * @brief Populates the sidebar tree store with the standard log hierarchy
 * @param ctx Pointer to the shared application context
 */
void EventModels_PopulateTree(EventViewerContext *ctx);

/**
 * @brief Populates the admin events and log summary tables (Overview tab)
 * @param ctx Pointer to the shared application context
 */
void EventModels_PopulateTables(EventViewerContext *ctx);

/**
 * @brief Loads and displays events from a named log in the Events tab
 * @param ctx       Pointer to the shared application context
 * @param log_name  ASCII log name ("Application", "System", "Security", "Setup")
 */
void EventModels_LoadLogEvents(EventViewerContext *ctx, const char *log_name);

/**
 * @brief Clears and repopulates the event details store from an array
 * @param ctx     Pointer to the shared application context
 * @param events  Array of EventRecord to display
 * @param count   Number of records in the array
 */
void EventModels_PopulateEventDetails(EventViewerContext *ctx,
                                      EventRecord        *events,
                                      int                 count);

#endif /* EVENT_MODELS_H */
