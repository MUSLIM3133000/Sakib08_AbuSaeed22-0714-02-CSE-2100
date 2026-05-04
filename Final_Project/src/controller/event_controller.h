/**
 * @file controller/event_controller.h
 * @brief MVC Controller — mediates between Model and View (C++17)
 *
 * MVC Role: CONTROLLER — bridges data from the Model layer (EventRecord,
 * ILogRepository, EventStatistics) into the View layer (GTK list/tree stores).
 *
 * Renamed from EventModels to EventController to reflect MVC role.
 *
 * SOLID principles retained:
 *  - SRP : Only bridges EventRecord → GTK stores. No Windows API.
 *  - DIP : Receives data as std::vector<EventRecord> — never calls Windows
 *          API directly.
 *  - ISP : Separate static methods per concern.
 */

#pragma once

#include <vector>
#include "view/context/event_viewer_context.h"
#include "model/types/event_record.h"
#include "model/statistics/event_statistics.h"

namespace EventViewer {

class EventController {
public:
    /**
     * @brief Populates the sidebar tree store with the standard log hierarchy.
     */
    static void populateTree(EventViewerContext* ctx);

    /**
     * @brief Populates the overview tab tables with pre-computed statistics.
     */
    static void populateTables(EventViewerContext*    ctx,
                                const EventStatistics& stats1h,
                                const EventStatistics& stats24h,
                                const EventStatistics& stats7d);

    /**
     * @brief Fills the event details list store from a vector of records.
     */
    static void populateEventDetails(EventViewerContext*            ctx,
                                     const std::vector<EventRecord>& events);

    /**
     * @brief Loads events for the named log and updates the details view.
     * Uses ctx->logRepository (ILogRepository) — never calls Windows API directly.
     */
    static void loadLogEvents(EventViewerContext* ctx,
                               const std::string&  logName);
};

} // namespace EventViewer
