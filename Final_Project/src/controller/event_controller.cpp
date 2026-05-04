/**
 * @file controller/event_controller.cpp
 * @brief EventController implementation (C++17)
 *
 * MVC Role: CONTROLLER — bridges Model data into View (GTK stores).
 * Renamed from EventModels to EventController for MVC clarity.
 */

#include "event_controller.h"
#include "utils/platform/privilege_check.h"
#include <cstdio>

namespace EventViewer {

// ─────────────────────────────────────────────────────────────────────────────
// populateTree — populates the sidebar tree (View) with log hierarchy
// ─────────────────────────────────────────────────────────────────────────────
void EventController::populateTree(EventViewerContext* ctx) {
    GtkTreeIter root, child, subchild;

    gtk_tree_store_append(ctx->treeStore, &root, nullptr);
    gtk_tree_store_set(ctx->treeStore, &root,
                       0, "Event Viewer (Local)", 1, TRUE, -1);

    // Custom Views
    gtk_tree_store_append(ctx->treeStore, &child, &root);
    gtk_tree_store_set(ctx->treeStore, &child, 0, "Custom Views", 1, TRUE, -1);

    gtk_tree_store_append(ctx->treeStore, &subchild, &child);
    gtk_tree_store_set(ctx->treeStore, &subchild, 0, "Server Roles", 1, FALSE, -1);

    gtk_tree_store_append(ctx->treeStore, &subchild, &child);
    gtk_tree_store_set(ctx->treeStore, &subchild, 0, "Administrative Events", 1, FALSE, -1);

    // Windows Logs
    gtk_tree_store_append(ctx->treeStore, &child, &root);
    gtk_tree_store_set(ctx->treeStore, &child, 0, "Windows Logs", 1, TRUE, -1);

    const char* windowsLogs[] = {
        "Application", "Security", "Setup", "System", "Forwarded Events"
    };
    for (const auto* name : windowsLogs) {
        gtk_tree_store_append(ctx->treeStore, &subchild, &child);
        gtk_tree_store_set(ctx->treeStore, &subchild, 0, name, 1, FALSE, -1);
    }

    // Applications and Services Logs
    gtk_tree_store_append(ctx->treeStore, &child, &root);
    gtk_tree_store_set(ctx->treeStore, &child,
                       0, "Applications and Services Logs", 1, TRUE, -1);

    gtk_tree_store_append(ctx->treeStore, &subchild, &child);
    gtk_tree_store_set(ctx->treeStore, &subchild, 0, "Hardware Events", 1, FALSE, -1);

    GtkTreePath* path = gtk_tree_path_new_from_string("0");
    gtk_tree_view_expand_row(GTK_TREE_VIEW(ctx->treeView), path, FALSE);
    gtk_tree_path_free(path);
}

// ─────────────────────────────────────────────────────────────────────────────
// populateTables — pushes Model statistics data into View tables
// ─────────────────────────────────────────────────────────────────────────────
void EventController::populateTables(EventViewerContext*    ctx,
                                      const EventStatistics& stats1h,
                                      const EventStatistics& stats24h,
                                      const EventStatistics& stats7d)
{
    GtkTreeIter iter;

    struct Row { const char* type; uint32_t h1, h24, d7; };
    const Row rows[] = {
        {"Critical",      stats1h.criticalCount,    stats24h.criticalCount,    stats7d.criticalCount},
        {"Error",         stats1h.errorCount,       stats24h.errorCount,       stats7d.errorCount},
        {"Warning",       stats1h.warningCount,     stats24h.warningCount,     stats7d.warningCount},
        {"Information",   stats1h.informationCount, stats24h.informationCount, stats7d.informationCount},
        {"Audit Success", stats1h.auditSuccessCount, stats24h.auditSuccessCount, stats7d.auditSuccessCount},
        {"Audit Failure", stats1h.auditFailureCount, stats24h.auditFailureCount, stats7d.auditFailureCount}
    };

    for (const auto& r : rows) {
        gtk_list_store_append(ctx->adminStore, &iter);
        gtk_list_store_set(ctx->adminStore, &iter,
            0, r.type,
            1, "-", 2, "-", 3, "-",
            4, static_cast<int>(r.h1),
            5, static_cast<int>(r.h24),
            6, static_cast<int>(r.d7),
            -1);
    }

    struct LogRow { const char *name, *size, *modified, *enabled, *retention; };
    const LogRow logData[] = {
        {"Windows PowerShell", "15.00 MB/...", "8/20/2025 1:27:55 AM", "Enabled", "Overwrite events as nece..."},
        {"Visual Studio",      "68 KB/1.00...", "8/9/2025 1:21:39 PM",  "Enabled", "Overwrite events as nece..."},
        {"System",             "20.00 MB/...", "8/20/2025 1:27:47 AM", "Enabled", "Overwrite events as nece..."},
        {"Security",           "20.00 MB/...", "8/20/2025 1:24:04 AM", "Enabled", "Overwrite events as nece..."},
        {"OneApp_IGCC",        "1.00 MB/1...",  "8/20/2025 1:23:25 AM", "Enabled", "Overwrite events as nece..."}
    };

    for (const auto& l : logData) {
        gtk_list_store_append(ctx->logStore, &iter);
        gtk_list_store_set(ctx->logStore, &iter,
            0, l.name, 1, l.size,
            2, l.modified, 3, l.enabled,
            4, l.retention, -1);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// populateEventDetails — pushes Model EventRecords into View list store
// ─────────────────────────────────────────────────────────────────────────────
void EventController::populateEventDetails(EventViewerContext*             ctx,
                                            const std::vector<EventRecord>& events)
{
    GtkTreeIter iter;
    for (const auto& e : events) {
        gtk_list_store_append(ctx->eventDetailsStore, &iter);
        gtk_list_store_set(ctx->eventDetailsStore, &iter,
            0, e.levelString(),
            1, e.timestamp().empty() ? "N/A" : e.timestamp().c_str(),
            2, e.source().empty()    ? "N/A" : e.source().c_str(),
            3, static_cast<int>(e.eventId()),
            4, "General",
            -1);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// loadLogEvents — reads Model data and updates View
// ─────────────────────────────────────────────────────────────────────────────
void EventController::loadLogEvents(EventViewerContext* ctx,
                                     const std::string&  logName)
{
    gtk_list_store_clear(ctx->eventDetailsStore);

    const wchar_t* wname = nullptr;

    if      (logName == "Application") wname = L"Application";
    else if (logName == "System")      wname = L"System";
    else if (logName == "Security")    wname = L"Security";
    else if (logName == "Setup")       wname = L"Setup";
    else {
        g_print("Unknown log: %s\n", logName.c_str());
        return;
    }

    if (!ctx->logRepository) return;

    auto events = ctx->logRepository->read(wname, 24, 1000);
    g_print("Loaded %zu events from %s\n", events.size(), logName.c_str());

    if (!events.empty()) {
        populateEventDetails(ctx, events);
        ctx->currentLogName = logName;
    } else {
        g_print("Failed to load events from: %s\n", logName.c_str());

        GtkWidget* dialog = gtk_message_dialog_new(
            GTK_WINDOW(ctx->window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Failed to open event log: %s", logName.c_str());

        if (logName == "Security" && !PlatformUtils::isRunningAsAdmin()) {
            gtk_message_dialog_format_secondary_text(
                GTK_MESSAGE_DIALOG(dialog),
                "Administrator privileges are required to access the Security log.");
        }

        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), nullptr);
    }
}

} // namespace EventViewer
