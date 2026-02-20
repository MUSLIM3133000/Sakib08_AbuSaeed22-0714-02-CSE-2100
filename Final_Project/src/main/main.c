#include <gtk/gtk.h>
#include "ui/gtk/windows/main_window.h"
#include "ui/gtk/callbacks/action_handlers.h"

static const GActionEntry app_actions[] = {
    {"quit",        on_quit,        NULL, NULL, NULL, {0}},
    {"about",       on_about,       NULL, NULL, NULL, {0}},
    {"refresh",     on_refresh,     NULL, NULL, NULL, {0}},
    {"open_log",    on_open_log,    NULL, NULL, NULL, {0}},
    {"save_log",    on_save_log,    NULL, NULL, NULL, {0}},
    {"create_view", on_create_view, NULL, NULL, NULL, {0}},
    {"import_view", on_import_view, NULL, NULL, NULL, {0}}
};

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new(
        "com.example.EventViewer",
        G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(activate_window), NULL);

    g_action_map_add_action_entries(
        G_ACTION_MAP(app),
        app_actions,
        G_N_ELEMENTS(app_actions),
        app);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
