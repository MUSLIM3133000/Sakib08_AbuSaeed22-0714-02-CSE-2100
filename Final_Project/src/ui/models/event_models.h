/**
 * @file ui/models/event_models.h
 * @brief GTK data model population — C++17 refactor
 *
 * SOLID changes vs C version:
 *  - SRP : EventModels only bridges EventRecord → GTK stores. No Windows API.
 *  - DIP : Receives data as std::vector<EventRecord> — never calls Windows
 *          API directly. The C version called EventLog_Windows_Open() here.
 *  - ISP : Separate static methods per concern — populate tree, tables, details.
 *
 * C improvement:
 *  - Free C functions with EventViewerContext* param
 *    → static methods of EventModels class (grouped, namespaced)
 *  - Direct EventStatistics_Calculate() call inside PopulateTables
 *    → receives pre-computed EventStatistics (injected, testable)
 */

#pragma once

#include <vector>
#include "ui/gtk/event_viewer_context.h"
#include "core/types/event_record.h"
#include "core/statistics/event_statistics.h"

namespace EventViewer {

class EventModels {
public:
    /**
     * @brief Populates the sidebar tree store with the standard log hierarchy.
     * Pure GTK — no data dependencies.
     */
    static void populateTree(EventViewerContext* ctx);

    /**
     * @brief Populates the overview tab tables.
     * @param stats1h / stats24h / stats7d  Pre-computed statistics (DIP: no
     *        direct Windows API call here — caller fetches and passes them in).
     */
    static void populateTables(EventViewerContext*    ctx,
                                const EventStatistics& stats1h,
                                const EventStatistics& stats24h,
                                const EventStatistics& stats7d);

    /**
     * @brief Fills the event details list store from a vector of records.
     * @param events  Already-fetched records (no Windows API call here).
     *
     * C equivalent: EventModels_PopulateEventDetails(ctx, events, count)
     * Improvement: vector replaces raw pointer + int count.
     */
    static void populateEventDetails(EventViewerContext*            ctx,
                                     const std::vector<EventRecord>& events);

    /**
     * @brief Loads events for the named log and updates the details view.
     * Uses ctx->logRepository (ILogRepository) — never calls Windows API directly.
     *
     * C equivalent: EventModels_LoadLogEvents(ctx, log_name)
     */
    static void loadLogEvents(EventViewerContext* ctx,
                               const std::string&  logName);
};

} // namespace EventViewer
