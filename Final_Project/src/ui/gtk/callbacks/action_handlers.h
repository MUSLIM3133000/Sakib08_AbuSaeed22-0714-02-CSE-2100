/**
 * @file ui/gtk/callbacks/action_handlers.h
 * @brief GTK4 action and signal callbacks — C++17 refactor
 *
 * SOLID changes vs C version:
 *  - SRP : ActionHandlers only wires GTK signals → service calls.
 *          No CSV parsing, no Windows API, no file I/O inline.
 *  - DIP : File open/save uses ctx->eventExporter (IEventExporter*).
 *          The hard-coded CsvExporter_Export() call is gone.
 *  - OCP : Swapping the exporter (CSV → JSON) requires zero changes here —
 *          just set ctx->eventExporter to a different IEventExporter.
 *
 * C improvement:
 *  - Global free functions (C linkage)  →  static methods of ActionHandlers
 *  - File-reading CSV parsing inline in OnOpenLogResponse
 *    →  delegates to CsvImporter::load() (SRP)
 *  - Hard-coded CsvExporter_Export() in OnSaveLogResponse
 *    →  delegates to ctx->eventExporter->exportEvents() (DIP)
 */

#pragma once

#include <gtk/gtk.h>
#include "ui/gtk/event_viewer_context.h"

namespace EventViewer {

class ActionHandlers {
public:
    // ── GAction callbacks ─────────────────────────────────────────────────
    static void onQuit       (GSimpleAction*, GVariant*, gpointer userData);
    static void onAbout      (GSimpleAction*, GVariant*, gpointer userData);
    static void onRefresh    (GSimpleAction*, GVariant*, gpointer userData);
    static void onOpenLog    (GSimpleAction*, GVariant*, gpointer userData);
    static void onSaveLog    (GSimpleAction*, GVariant*, gpointer userData);
    static void onCreateView (GSimpleAction*, GVariant*, gpointer userData);
    static void onImportView (GSimpleAction*, GVariant*, gpointer userData);

    // ── GTK signal callbacks ──────────────────────────────────────────────
    static void onTreeSelectionChanged(GtkTreeSelection* selection,
                                       gpointer          userData);

    // ── File-dialog response callbacks ────────────────────────────────────
    static void onOpenLogResponse(GtkDialog* dialog, int responseId,
                                  gpointer   userData);
    static void onSaveLogResponse(GtkDialog* dialog, int responseId,
                                  gpointer   userData);
};

} // namespace EventViewer
