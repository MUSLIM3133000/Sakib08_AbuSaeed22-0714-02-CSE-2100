/**
 * @file controller/action_handlers.cpp
 * @brief GTK4 action handler implementations (C++17)
 *
 * MVC Role: CONTROLLER — receives user input from the View,
 * interacts with the Model (CsvImporter, IEventExporter),
 * and updates the View through EventController.
 */

#include "action_handlers.h"
#include "controller/event_controller.h"
#include "model/import/csv_importer.h"
#include <string>
#include <vector>

namespace EventViewer {

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────
static EventViewerContext* contextFromApp(gpointer userData) {
    auto* app = GTK_APPLICATION(userData);
    GtkWindow* win = gtk_application_get_active_window(app);
    return static_cast<EventViewerContext*>(
               g_object_get_data(G_OBJECT(win), "event_viewer_context"));
}

// ─────────────────────────────────────────────────────────────────────────────
// GAction callbacks
// ─────────────────────────────────────────────────────────────────────────────
void ActionHandlers::onQuit(GSimpleAction*, GVariant*, gpointer userData) {
    g_application_quit(G_APPLICATION(userData));
}

void ActionHandlers::onAbout(GSimpleAction*, GVariant*, gpointer userData) {
    auto* app    = GTK_APPLICATION(userData);
    auto* parent = gtk_application_get_active_window(app);

    GtkWidget* dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Event Viewer");
    gtk_about_dialog_set_version     (GTK_ABOUT_DIALOG(dialog), "2.0");
    gtk_about_dialog_set_comments    (GTK_ABOUT_DIALOG(dialog),
                                      "A GTK4 C++17 implementation of Windows Event Viewer");
    gtk_about_dialog_set_website     (GTK_ABOUT_DIALOG(dialog), "https://example.com");
    gtk_window_set_transient_for     (GTK_WINDOW(dialog), parent);
    gtk_window_present               (GTK_WINDOW(dialog));
}

void ActionHandlers::onRefresh(GSimpleAction*, GVariant*, gpointer) {
    // Future: re-fetch currentLogName via ctx->logRepository
}

// ─────────────────────────────────────────────────────────────────────────────
// Open Log
// ─────────────────────────────────────────────────────────────────────────────
void ActionHandlers::onOpenLogResponse(GtkDialog* dialog, int responseId,
                                        gpointer   userData)
{
    auto* ctx = static_cast<EventViewerContext*>(userData);

    if (responseId == GTK_RESPONSE_ACCEPT) {
        GFile* file     = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        gchar* filename = g_file_get_path(file);
        g_object_unref(file);

        // Controller delegates to Model for CSV parsing
        CsvImporter importer;
        auto records = importer.load(std::string(filename));
        g_free(filename);

        gtk_list_store_clear(ctx->eventDetailsStore);
        EventController::populateEventDetails(ctx, records);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(ctx->notebook), 1);
    }

    gtk_window_destroy(GTK_WINDOW(dialog));
}

void ActionHandlers::onOpenLog(GSimpleAction*, GVariant*, gpointer userData) {
    auto* ctx    = contextFromApp(userData);
    auto* parent = GTK_WINDOW(ctx->window);

    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Open Log File", parent, GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Open",   GTK_RESPONSE_ACCEPT, nullptr);

    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "CSV Files");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    g_signal_connect(dialog, "response",
                     G_CALLBACK(ActionHandlers::onOpenLogResponse), ctx);
    gtk_window_present(GTK_WINDOW(dialog));
}

// ─────────────────────────────────────────────────────────────────────────────
// Save Log
// ─────────────────────────────────────────────────────────────────────────────
void ActionHandlers::onSaveLogResponse(GtkDialog* dialog, int responseId,
                                        gpointer   userData)
{
    auto* ctx = static_cast<EventViewerContext*>(userData);

    if (responseId == GTK_RESPONSE_ACCEPT) {
        GFile* file     = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(dialog));
        gchar* filename = g_file_get_path(file);
        g_object_unref(file);

        // Collect EventRecords from the GTK store
        std::vector<EventRecord> records;
        GtkTreeIter iter;
        gboolean valid = gtk_tree_model_get_iter_first(
                             GTK_TREE_MODEL(ctx->eventDetailsStore), &iter);

        while (valid) {
            gchar *level{}, *datetime{}, *source{}, *task{};
            int    eventId = 0;

            gtk_tree_model_get(GTK_TREE_MODEL(ctx->eventDetailsStore), &iter,
                0, &level, 1, &datetime,
                2, &source, 3, &eventId, 4, &task, -1);

            records.emplace_back(
                static_cast<uint32_t>(eventId),
                eventLevelFromString(level ? level : ""),
                source   ? source   : "",
                "",
                datetime ? datetime : "",
                "",
                "",
                0u,
                task ? task : ""
            );

            g_free(level); g_free(datetime);
            g_free(source); g_free(task);

            valid = gtk_tree_model_iter_next(
                        GTK_TREE_MODEL(ctx->eventDetailsStore), &iter);
        }

        // DIP: export through IEventExporter
        if (ctx->eventExporter) {
            ctx->eventExporter->exportEvents(std::string(filename), records);
        }
        g_free(filename);
    }

    gtk_window_destroy(GTK_WINDOW(dialog));
}

void ActionHandlers::onSaveLog(GSimpleAction*, GVariant*, gpointer userData) {
    auto* ctx    = contextFromApp(userData);
    auto* parent = GTK_WINDOW(ctx->window);

    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Save Log File", parent, GTK_FILE_CHOOSER_ACTION_SAVE,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Save",   GTK_RESPONSE_ACCEPT, nullptr);

    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "CSV Files");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    g_signal_connect(dialog, "response",
                     G_CALLBACK(ActionHandlers::onSaveLogResponse), ctx);
    gtk_window_present(GTK_WINDOW(dialog));
}

// ─────────────────────────────────────────────────────────────────────────────
// Create / Import View
// ─────────────────────────────────────────────────────────────────────────────
void ActionHandlers::onCreateView(GSimpleAction*, GVariant*, gpointer userData) {
    auto* app    = GTK_APPLICATION(userData);
    auto* parent = gtk_application_get_active_window(app);

    GtkWidget* dialog = gtk_message_dialog_new(parent,
        GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "Create Custom View");
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
        "Custom view creation will be implemented in a future version.");
    gtk_window_present(GTK_WINDOW(dialog));
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), nullptr);
}

void ActionHandlers::onImportView(GSimpleAction*, GVariant*, gpointer userData) {
    auto* app    = GTK_APPLICATION(userData);
    auto* parent = gtk_application_get_active_window(app);

    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        "Import Custom View", parent, GTK_FILE_CHOOSER_ACTION_OPEN,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Import", GTK_RESPONSE_ACCEPT, nullptr);

    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Custom View Files");
    gtk_file_filter_add_pattern(filter, "*.xml");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    gtk_window_present(GTK_WINDOW(dialog));
}

// ─────────────────────────────────────────────────────────────────────────────
// Tree selection changed — Controller delegates to EventController
// ─────────────────────────────────────────────────────────────────────────────
void ActionHandlers::onTreeSelectionChanged(GtkTreeSelection* selection,
                                             gpointer          userData)
{
    auto*        ctx = static_cast<EventViewerContext*>(userData);
    GtkTreeModel* model{};
    GtkTreeIter   iter{};
    gchar*        name{};

    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return;

    gtk_tree_model_get(model, &iter, 0, &name, -1);
    g_print("Selected: %s\n", name);

    const std::string logName(name);
    g_free(name);

    if (logName == "Application" || logName == "System" ||
        logName == "Security"    || logName == "Setup") {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(ctx->notebook), 1);
        EventController::loadLogEvents(ctx, logName);
    } else {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(ctx->notebook), 0);
    }
}

} // namespace EventViewer
