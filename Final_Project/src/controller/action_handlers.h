/**
 * @file controller/action_handlers.h
 * @brief GTK4 action and signal callbacks — C++17
 *
 * MVC Role: CONTROLLER — handles user actions (menu clicks, toolbar buttons,
 * file dialogs) and delegates to the Model for data operations and the
 * EventController for updating the View.
 *
 * SOLID principles retained:
 *  - SRP : ActionHandlers only wires GTK signals → service calls.
 *  - DIP : File save uses ctx->eventExporter (IEventExporter*).
 *  - OCP : Swapping the exporter requires zero changes here.
 */

#pragma once

#include <gtk/gtk.h>
#include "view/context/event_viewer_context.h"

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
