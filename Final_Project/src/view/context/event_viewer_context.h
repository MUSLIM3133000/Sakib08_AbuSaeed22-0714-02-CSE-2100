/**
 * @file view/context/event_viewer_context.h
 * @brief Shared application context (C++17)
 *
 * MVC Role: Shared context struct passed between View and Controller.
 * Holds GTK widget references, application state, and injected dependencies.
 *
 * SOLID principles retained:
 *  - SRP : Context is a pure data holder — no logic inside.
 *  - DIP : Holds ILogRepository and IEventExporter by interface pointer.
 */

#pragma once

#include <gtk/gtk.h>
#include <string>
#include <memory>
#include "model/repository/log_repository.h"
#include "model/export/csv_exporter.h"

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
    std::string currentLogName;

    // ── Injected dependencies (DIP) ───────────────────────────────────────
    ILogRepository* logRepository = nullptr;
    IEventExporter* eventExporter = nullptr;
};

} // namespace EventViewer
