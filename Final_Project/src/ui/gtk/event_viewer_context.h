/**
 * @file ui/gtk/event_viewer_context.h
 * @brief Shared application context (C++17)
 *
 * SOLID changes vs C version:
 *  - SRP : Context is a pure data holder — no logic inside.
 *  - DIP : Holds ILogRepository and IEventExporter by interface pointer,
 *          not concrete Windows/CSV types.
 *
 * C improvement:
 *  - Raw struct with char* current_log_name (manual strdup/free)
 *    → std::string (automatic cleanup)
 *  - EVT_HANDLE current_log_handle (manual EvtClose)
 *    → managed inside ILogRepository — context no longer owns it
 *  - g_malloc0(sizeof(EventViewerContext)) in C
 *    → stack/unique_ptr construction in C++
 */

#pragma once

#include <gtk/gtk.h>
#include <string>
#include <memory>
#include "core/readers/log_repository.h"
#include "data/export/csv_exporter.h"

namespace EventViewer {

struct EventViewerContext {
    // ── GTK application & main window ────────────────────────────────────
    GtkApplication* app    = nullptr;
    GtkWidget*      window = nullptr;
    GtkWidget*      notebook = nullptr;

    // ── Sidebar tree ──────────────────────────────────────────────────────
    GtkWidget*    treeView  = nullptr;
    GtkTreeStore* treeStore = nullptr;

    // ── Overview tab tables ───────────────────────────────────────────────
    GtkWidget*    adminTreeView = nullptr;
    GtkListStore* adminStore    = nullptr;

    GtkWidget*    recentTreeView = nullptr;
    GtkListStore* recentStore    = nullptr;

    GtkWidget*    logTreeView = nullptr;
    GtkListStore* logStore    = nullptr;

    // ── Events tab ────────────────────────────────────────────────────────
    GtkWidget*    eventDetailsView  = nullptr;
    GtkListStore* eventDetailsStore = nullptr;

    // ── Application state ─────────────────────────────────────────────────
    std::string currentLogName;   // replaces char* + strdup/free

    // ── Injected dependencies (DIP) ───────────────────────────────────────
    // Held by raw observer pointer — lifetime owned by main().
    // Keeps GTK C-style callbacks simple (no shared_ptr overhead in gpointer).
    ILogRepository* logRepository = nullptr;   // replaces direct Windows API calls
    IEventExporter* eventExporter = nullptr;   // replaces hard-coded CsvExporter_Export
};

} // namespace EventViewer
