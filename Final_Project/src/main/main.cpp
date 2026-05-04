/**
 * @file main/main.cpp
 * @brief Application entry point — Composition Root (C++17)
 *
 * MVC Role: ENTRY POINT — wires Model, View, and Controller together.
 *
 * This is the ONLY file that instantiates concrete infrastructure
 * classes (WindowsEventLogRepository, CsvExporter).
 * Every other file depends on interfaces (ILogRepository, IEventExporter).
 *
 * main() only wires dependencies and starts GTK — no business logic.
 */

#include <gtk/gtk.h>
#include <memory>

// Model — concrete types (only seen in this file)
#include "model/repository/log_repository.h"
#include "model/export/csv_exporter.h"

// View
#include "view/windows/main_window.h"
#include "view/context/event_viewer_context.h"

// Controller
#include "controller/action_handlers.h"

using namespace EventViewer;

int main(int argc, char** argv) {

    // ── Step 1: Build concrete Model infrastructure ──────────────────────
    auto repository = ILogRepository::create();
    auto exporter   = std::make_unique<CsvExporter>();

    // ── Step 2: Build context, inject Model dependencies ─────────────────
    EventViewerContext ctx;
    ctx.logRepository = repository.get();   // ILogRepository*
    ctx.eventExporter = exporter.get();     // IEventExporter*

    // ── Step 3: Wire GTK application (View + Controller) ─────────────────
    GtkApplication* app = gtk_application_new(
        "com.example.EventViewer",
        G_APPLICATION_DEFAULT_FLAGS);

    // View: activate signal creates the main window
    g_signal_connect(app, "activate",
                     G_CALLBACK(MainWindow::activate), &ctx);

    // Controller: GAction table maps menu/toolbar actions to handlers
    const GActionEntry appEntries[] = {
        {"quit",        ActionHandlers::onQuit,        nullptr, nullptr, nullptr, {0}},
        {"about",       ActionHandlers::onAbout,       nullptr, nullptr, nullptr, {0}},
        {"refresh",     ActionHandlers::onRefresh,     nullptr, nullptr, nullptr, {0}},
        {"open_log",    ActionHandlers::onOpenLog,     nullptr, nullptr, nullptr, {0}},
        {"save_log",    ActionHandlers::onSaveLog,     nullptr, nullptr, nullptr, {0}},
        {"create_view", ActionHandlers::onCreateView,  nullptr, nullptr, nullptr, {0}},
        {"import_view", ActionHandlers::onImportView,  nullptr, nullptr, nullptr, {0}}
    };

    g_action_map_add_action_entries(G_ACTION_MAP(app),
                                    appEntries,
                                    G_N_ELEMENTS(appEntries),
                                    app);

    // ── Step 4: Run ───────────────────────────────────────────────────────
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
