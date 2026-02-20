/**
 * @file main/main.c
 * @brief Application entry point
 *
 * Creates the GtkApplication, registers all GAction entries, and
 * starts the GTK main loop. All other logic is handled by the
 * respective modules under ui/, core/, data/, and utils/.
 *
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#include <gtk/gtk.h>
#include "ui/gtk/windows/main_window.h"
#include "ui/gtk/callbacks/action_handlers.h"

/**
 * @brief GAction entry table — maps action name strings to handler functions
 */
static const GActionEntry g_appEntries[] = {
    {"quit",        ActionHandlers_OnQuit,        NULL, NULL, NULL, {0}},
    {"about",       ActionHandlers_OnAbout,       NULL, NULL, NULL, {0}},
    {"refresh",     ActionHandlers_OnRefresh,     NULL, NULL, NULL, {0}},
    {"open_log",    ActionHandlers_OnOpenLog,     NULL, NULL, NULL, {0}},
    {"save_log",    ActionHandlers_OnSaveLog,     NULL, NULL, NULL, {0}},
    {"create_view", ActionHandlers_OnCreateView,  NULL, NULL, NULL, {0}},
    {"import_view", ActionHandlers_OnImportView,  NULL, NULL, NULL, {0}}
};

/**
 * @brief Application entry point
 * @param argc Argument count
 * @param argv Argument vector
 * @return GTK application exit status
 */
int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new(
        "com.example.EventViewer",
        G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(MainWindow_Activate), NULL);

    g_action_map_add_action_entries(
        G_ACTION_MAP(app),
        g_appEntries,
        G_N_ELEMENTS(g_appEntries),
        app);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
