#include "action_handlers.h"
#include "ui/models/event_models.h"
#include "data/export/csv_exporter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void on_quit(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    (void)action; (void)parameter;
    g_application_quit(G_APPLICATION(user_data));
}

void on_about(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    (void)action; (void)parameter;
    GtkApplication *app    = GTK_APPLICATION(user_data);
    GtkWindow      *parent = gtk_application_get_active_window(app);

    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Event Viewer");
    gtk_about_dialog_set_version     (GTK_ABOUT_DIALOG(dialog), "2.0");
    gtk_about_dialog_set_comments    (GTK_ABOUT_DIALOG(dialog),
                                      "A GTK4 implementation of Windows Event Viewer");
    gtk_about_dialog_set_website     (GTK_ABOUT_DIALOG(dialog), "https://example.com");
    gtk_window_set_transient_for     (GTK_WINDOW(dialog), parent);
    gtk_window_present               (GTK_WINDOW(dialog));
}

void on_refresh(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    (void)action; (void)parameter; (void)user_data;
}

void on_open_log_response(GtkDialog *dialog, int response_id, gpointer user_data) {
    AppData *data = (AppData *)user_data;

    if (response_id == GTK_RESPONSE_ACCEPT) {
        GFile *file     = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        char  *filename = g_file_get_path(file);
        g_object_unref(file);

        gtk_list_store_clear(data->models.event_details_store);

        FILE *fp = fopen(filename, "r");
        if (fp) {
            char line[4096];
            int  lineNo = 0;
            while (fgets(line, sizeof(line), fp)) {
                lineNo++;
                if (lineNo == 1) continue;

                char levelStr[128], datetimeStr[128], sourceStr[256], taskStr[128];
                int  eventId = 0;

                if (sscanf(line,
                           "\"%127[^\"]\",\"%127[^\"]\",\"%255[^\"]\",%d,\"%127[^\"]\"",
                           levelStr, datetimeStr, sourceStr, &eventId, taskStr) == 5) {
                    GtkTreeIter iter;
                    gtk_list_store_append(data->models.event_details_store, &iter);
                    gtk_list_store_set(data->models.event_details_store, &iter,
                        0, levelStr, 1, datetimeStr,
                        2, sourceStr, 3, eventId,
                        4, taskStr, -1);
                }
            }
            fclose(fp);
        }

        g_free(filename);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(data->window.notebook), 1);
    }

    gtk_window_destroy(GTK_WINDOW(dialog));
}

void on_open_log(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    (void)action; (void)parameter;
    GtkApplication     *app = GTK_APPLICATION(user_data);
    AppData *data = g_object_get_data(
                                  G_OBJECT(gtk_application_get_active_window(app)),
                                  "event_viewer_data");
    GtkWindow *parent = GTK_WINDOW(data->window.window);

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Open Log File", parent, GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Open",   GTK_RESPONSE_ACCEPT, NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "CSV Files");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    g_signal_connect(dialog, "response",
                     G_CALLBACK(on_open_log_response), data);
    gtk_window_present(GTK_WINDOW(dialog));
}

void on_save_log_response(GtkDialog *dialog, int response_id, gpointer user_data) {
    AppData *data = (AppData *)user_data;

    if (response_id == GTK_RESPONSE_ACCEPT) {
        GFile *file     = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        char  *filename = g_file_get_path(file);
        g_object_unref(file);

        FILE *fp = fopen(filename, "w");
        if (fp) {
            fprintf(fp, "Level,Date/Time,Source,Event ID,Task Category\n");

            GtkTreeIter iter;
            gboolean valid = gtk_tree_model_get_iter_first(
                                 GTK_TREE_MODEL(data->models.event_details_store), &iter);
            while (valid) {
                char *level, *datetime, *source, *task;
                int   eventId;

                gtk_tree_model_get(GTK_TREE_MODEL(data->models.event_details_store), &iter,
                    0, &level, 1, &datetime,
                    2, &source, 3, &eventId,
                    4, &task, -1);

                fprintf(fp, "\"%s\",\"%s\",\"%s\",%d,\"%s\"\n",
                        level, datetime, source, eventId, task);

                g_free(level); g_free(datetime);
                g_free(source); g_free(task);

                valid = gtk_tree_model_iter_next(
                            GTK_TREE_MODEL(data->models.event_details_store), &iter);
            }
            fclose(fp);
        }

        g_free(filename);
    }

    gtk_window_destroy(GTK_WINDOW(dialog));
}

void on_save_log(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    (void)action; (void)parameter;
    GtkApplication     *app = GTK_APPLICATION(user_data);
    AppData *data = g_object_get_data(
                                  G_OBJECT(gtk_application_get_active_window(app)),
                                  "event_viewer_data");
    GtkWindow *parent = GTK_WINDOW(data->window.window);

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Save Log File", parent, GTK_FILE_CHOOSER_ACTION_SAVE,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Save",   GTK_RESPONSE_ACCEPT, NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "CSV Files");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    g_signal_connect(dialog, "response",
                     G_CALLBACK(on_save_log_response), data);
    gtk_window_present(GTK_WINDOW(dialog));
}

void on_create_view(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    (void)action; (void)parameter;
    GtkApplication *app    = GTK_APPLICATION(user_data);
    GtkWindow      *parent = gtk_application_get_active_window(app);

    GtkWidget *dialog = gtk_message_dialog_new(parent,
        GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "Create Custom View");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
        "Custom view creation will be implemented in a future version.");
    gtk_window_present(GTK_WINDOW(dialog));
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
}

void on_import_view(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
    (void)action; (void)parameter;
    GtkApplication *app    = GTK_APPLICATION(user_data);
    GtkWindow      *parent = gtk_application_get_active_window(app);

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Import Custom View", parent, GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Import", GTK_RESPONSE_ACCEPT, NULL);

    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Custom View Files");
    gtk_file_filter_add_pattern(filter, "*.xml");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    gtk_window_present(GTK_WINDOW(dialog));
}

void on_tree_selection_changed(GtkTreeSelection *selection, gpointer user_data) {
    AppData *data = (AppData *)user_data;
    GtkTreeModel *model;
    GtkTreeIter   iter;
    gchar        *name;

    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return;

    gtk_tree_model_get(model, &iter, 0, &name, -1);
    g_print("Selected: %s\n", name);

    if (strcmp(name, "Application") == 0 ||
        strcmp(name, "System")      == 0 ||
        strcmp(name, "Security")    == 0 ||
        strcmp(name, "Setup")       == 0) {

        gtk_notebook_set_current_page(GTK_NOTEBOOK(data->window.notebook), 1);
        load_log_events(data, name);
    } else {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(data->window.notebook), 0);
    }

    g_free(name);
}
