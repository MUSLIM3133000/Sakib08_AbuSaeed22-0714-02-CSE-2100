#include "event_models.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
#include "core/readers/windows/event_log_windows.h"
#include "core/statistics/event_statistics.h"
#include "utils/platform/privilege_check.h"
#endif

void fill_tree(AppData *data) {
    GtkTreeIter root, child, subchild;

    gtk_tree_store_append(data->models.tree_store, &root, NULL);
    gtk_tree_store_set(data->models.tree_store, &root,
                       0, "Event Viewer (Local)", 1, TRUE, -1);

    gtk_tree_store_append(data->models.tree_store, &child, &root);
    gtk_tree_store_set(data->models.tree_store, &child,
                       0, "Custom Views", 1, TRUE, -1);

    gtk_tree_store_append(data->models.tree_store, &subchild, &child);
    gtk_tree_store_set(data->models.tree_store, &subchild,
                       0, "Server Roles", 1, FALSE, -1);

    gtk_tree_store_append(data->models.tree_store, &subchild, &child);
    gtk_tree_store_set(data->models.tree_store, &subchild,
                       0, "Administrative Events", 1, FALSE, -1);

    gtk_tree_store_append(data->models.tree_store, &child, &root);
    gtk_tree_store_set(data->models.tree_store, &child,
                       0, "Windows Logs", 1, TRUE, -1);

    const char *windowsLogs[] = {
        "Application", "Security", "Setup", "System", "Forwarded Events"
    };
    for (int i = 0; i < 5; i++) {
        gtk_tree_store_append(data->models.tree_store, &subchild, &child);
        gtk_tree_store_set(data->models.tree_store, &subchild,
                           0, windowsLogs[i], 1, FALSE, -1);
    }

    gtk_tree_store_append(data->models.tree_store, &child, &root);
    gtk_tree_store_set(data->models.tree_store, &child,
                       0, "Applications and Services Logs", 1, TRUE, -1);

    gtk_tree_store_append(data->models.tree_store, &subchild, &child);
    gtk_tree_store_set(data->models.tree_store, &subchild,
                       0, "Hardware Events", 1, FALSE, -1);

    GtkTreePath *path = gtk_tree_path_new_from_string("0");
    gtk_tree_view_expand_row(GTK_TREE_VIEW(data->models.tree_view), path, FALSE);
    gtk_tree_path_free(path);
}

void fill_tables(AppData *data) {
    GtkTreeIter iter;

#ifdef _WIN32
    EventLogRepository *repo = &g_windowsRepository;

    EventStatistics stats1h  = calc_stats(repo, L"Application", 1);
    EventStatistics stats24h = calc_stats(repo, L"Application", 24);
    EventStatistics stats7d  = calc_stats(repo, L"Application", 168);

    struct { const char *type; int h1, h24, d7; } eventsData[] = {
        {"Critical",      stats1h.critical_count,    stats24h.critical_count,    stats7d.critical_count},
        {"Error",         stats1h.error_count,       stats24h.error_count,       stats7d.error_count},
        {"Warning",       stats1h.warning_count,     stats24h.warning_count,     stats7d.warning_count},
        {"Information",   stats1h.information_count, stats24h.information_count, stats7d.information_count},
        {"Audit Success", stats1h.audit_success_count, stats24h.audit_success_count, stats7d.audit_success_count},
        {"Audit Failure", stats1h.audit_failure_count, stats24h.audit_failure_count, stats7d.audit_failure_count}
    };

    for (int i = 0; i < 6; i++) {
        gtk_list_store_append(data->models.admin_store, &iter);
        gtk_list_store_set(data->models.admin_store, &iter,
            0, eventsData[i].type,
            1, "-", 2, "-", 3, "-",
            4, eventsData[i].h1,
            5, eventsData[i].h24,
            6, eventsData[i].d7,
            -1);
    }
#endif

    struct { const char *name, *size, *modified, *enabled, *retention; } logData[] = {
        {"Windows PowerShell", "15.00 MB/...", "8/20/2025 1:27:55 AM", "Enabled", "Overwrite events as nece..."},
        {"Visual Studio",      "68 KB/1.00...", "8/9/2025 1:21:39 PM",  "Enabled", "Overwrite events as nece..."},
        {"System",             "20.00 MB/...", "8/20/2025 1:27:47 AM", "Enabled", "Overwrite events as nece..."},
        {"Security",           "20.00 MB/...", "8/20/2025 1:24:04 AM", "Enabled", "Overwrite events as nece..."},
        {"OneApp_IGCC",        "1.00 MB/1...",  "8/20/2025 1:23:25 AM", "Enabled", "Overwrite events as nece..."}
    };

    for (int i = 0; i < 5; i++) {
        gtk_list_store_append(data->models.log_store, &iter);
        gtk_list_store_set(data->models.log_store, &iter,
            0, logData[i].name,    1, logData[i].size,
            2, logData[i].modified, 3, logData[i].enabled,
            4, logData[i].retention, -1);
    }
}

void fill_event_details(AppData *data, EventRecord *events, int count) {
    GtkTreeIter iter;

    for (int i = 0; i < count; i++) {
        gtk_list_store_append(data->models.event_details_store, &iter);
        gtk_list_store_set(data->models.event_details_store, &iter,
            0, get_level_string(events[i].level),
            1, events[i].timestamp ? events[i].timestamp : "N/A",
            2, events[i].source    ? events[i].source    : "N/A",
            3, (int)events[i].event_id,
            4, "General",
            -1);
    }
}

void load_log_events(AppData *data, const char *log_name) {
    gtk_list_store_clear(data->models.event_details_store);

#ifdef _WIN32
    if (data->state.current_log_handle) {
        EvtClose(data->state.current_log_handle);
        data->state.current_log_handle = NULL;
    }

    wchar_t wlogName[256] = {0};
    if      (strcmp(log_name, "Application") == 0) wcscpy(wlogName, L"Application");
    else if (strcmp(log_name, "System")      == 0) wcscpy(wlogName, L"System");
    else if (strcmp(log_name, "Security")    == 0) wcscpy(wlogName, L"Security");
    else if (strcmp(log_name, "Setup")       == 0) wcscpy(wlogName, L"Setup");
    else {
        g_print("Unknown log: %s\n", log_name);
        return;
    }

    EventLogRepository *repo = &g_windowsRepository;
    void *handle = repo->open(wlogName, 24);
    data->state.current_log_handle = (EVT_HANDLE)handle;

    if (handle) {
        EventRecord *events = NULL;
        int count = repo->read(handle, &events, 1000);
        g_print("Loaded %d events from %s\n", count, log_name);

        fill_event_details(data, events, count);

        for (int i = 0; i < count; i++) free_event_record(&events[i]);
        free(events);

        free(data->state.current_log_name);
        data->state.current_log_name = strdup(log_name);

    } else {
        g_print("Failed to open log: %s\n", log_name);

        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(data->window.window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            "Failed to open event log: %s", log_name);

        if (strcmp(log_name, "Security") == 0 && !is_admin()) {
            gtk_message_dialog_format_secondary_text(
                GTK_MESSAGE_DIALOG(dialog),
                "Administrator privileges are required to access the Security log.");
        }

        gtk_window_present(GTK_WINDOW(dialog));
        g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
    }
#else
    (void)log_name;
#endif
}
