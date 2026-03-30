/**
 * @file main/main.cpp
 * @brief Application entry point — Composition Root (C++17)
 *
 * SOLID changes vs C version:
 *  - DIP : This is the ONLY file that instantiates concrete infrastructure
 *          classes (WindowsEventLogRepository, CsvExporter).
 *          Every other file depends on interfaces (ILogRepository, IEventExporter).
 *  - SRP : main() only wires dependencies and starts GTK — no business logic.
 *
 * C improvement:
 *  - main.c registered GAction entries with raw C function pointers
 *    → now uses static methods of ActionHandlers class (namespaced, grouped)
 *  - Context was g_malloc0'd inside MainWindow_Activate (hidden allocation)
 *    → context built here, injected via userData (explicit, traceable)
 *  - No dependency injection — everything hard-coded inside modules
 *    → all concrete types built here, passed in as interface pointers
 */

#include <gtk/gtk.h>
#include <memory>

// Infrastructure — concrete types (only seen in this file)
#include "core/readers/log_repository.h"
#include "data/export/csv_exporter.h"

// UI
#include "ui/gtk/windows/main_window.h"
#include "ui/gtk/callbacks/action_handlers.h"
#include "ui/gtk/event_viewer_context.h"

using namespace EventViewer;

int main(int argc, char** argv) {

    // ── Step 1: Build concrete infrastructure ────────────────────────────
    // ILogRepository::create() returns platform-appropriate implementation.
    // On Windows → WindowsEventLogRepository.
    // On other platforms → stub with empty results.
    auto repository = ILogRepository::create();
    auto exporter   = std::make_unique<CsvExporter>();

    // ── Step 2: Build context, inject dependencies ────────────────────────
    // DIP: context holds interface pointers — never concrete class names.
    EventViewerContext ctx;
    ctx.logRepository = repository.get();   // ILogRepository*
    ctx.eventExporter = exporter.get();     // IEventExporter*

    // ── Step 3: Wire GTK application ─────────────────────────────────────
    GtkApplication* app = gtk_application_new(
        "com.example.EventViewer",
        G_APPLICATION_DEFAULT_FLAGS);

    // Activate signal carries ctx so MainWindow never builds infrastructure
    g_signal_connect(app, "activate",
                     G_CALLBACK(MainWindow::activate), &ctx);

    // GAction table — static methods replace global C callback functions
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
