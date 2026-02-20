/**
 * @file ui/gtk/callbacks/action_handlers.h
 * @brief GTK4 action and signal callback declarations
 *
 * All on_* signal handlers and GAction callbacks are declared here.
 * Following GTK naming conventions: on_<object>_<signal>.
 *
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#ifndef ACTION_HANDLERS_H
#define ACTION_HANDLERS_H

#include <gtk/gtk.h>
#include "ui/gtk/event_viewer_context.h"

/* ---- GAction callbacks (wired to GtkApplication via app_entries) ---- */

void ActionHandlers_OnQuit        (GSimpleAction *action, GVariant *parameter, gpointer user_data);
void ActionHandlers_OnAbout       (GSimpleAction *action, GVariant *parameter, gpointer user_data);
void ActionHandlers_OnRefresh     (GSimpleAction *action, GVariant *parameter, gpointer user_data);
void ActionHandlers_OnOpenLog     (GSimpleAction *action, GVariant *parameter, gpointer user_data);
void ActionHandlers_OnSaveLog     (GSimpleAction *action, GVariant *parameter, gpointer user_data);
void ActionHandlers_OnCreateView  (GSimpleAction *action, GVariant *parameter, gpointer user_data);
void ActionHandlers_OnImportView  (GSimpleAction *action, GVariant *parameter, gpointer user_data);

/* ---- GTK signal callbacks ---- */

/**
 * @brief Fires when the sidebar tree selection changes
 * @param selection The GtkTreeSelection that changed
 * @param user_data Pointer to EventViewerContext
 */
void ActionHandlers_OnTreeSelectionChanged(GtkTreeSelection *selection, gpointer user_data);

/* ---- File-dialog response callbacks ---- */

void ActionHandlers_OnOpenLogResponse(GtkDialog *dialog, int response_id, gpointer user_data);
void ActionHandlers_OnSaveLogResponse(GtkDialog *dialog, int response_id, gpointer user_data);

#endif /* ACTION_HANDLERS_H */
