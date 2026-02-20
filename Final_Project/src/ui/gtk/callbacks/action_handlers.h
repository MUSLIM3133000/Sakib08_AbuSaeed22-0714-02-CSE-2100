#ifndef ACTION_HANDLERS_H
#define ACTION_HANDLERS_H

#include <gtk/gtk.h>
#include "ui/gtk/event_viewer_context.h"

void on_quit(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void on_about(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void on_refresh(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void on_open_log(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void on_save_log(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void on_create_view(GSimpleAction *action, GVariant *parameter, gpointer user_data);
void on_import_view(GSimpleAction *action, GVariant *parameter, gpointer user_data);

void on_tree_selection_changed(GtkTreeSelection *selection, gpointer user_data);

void on_open_log_response(GtkDialog *dialog, int response_id, gpointer user_data);
void on_save_log_response(GtkDialog *dialog, int response_id, gpointer user_data);

#endif
