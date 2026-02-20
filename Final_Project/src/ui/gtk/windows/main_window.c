#include "main_window.h"
#include "ui/gtk/components/menu_bar.h"
#include "ui/gtk/components/tool_bar.h"
#include "ui/gtk/components/sidebar.h"
#include "ui/models/event_models.h"
#include <time.h>
#include <stdio.h>

static GtkWidget *create_content_area(AppData *data);
static GtkWidget *create_admin_events_table(AppData *data);
static GtkWidget *create_recent_nodes_table(AppData *data);
static GtkWidget *create_log_summary_table(AppData *data);
static GtkWidget *create_event_details_view(AppData *data);

void activate_window(GtkApplication *app, gpointer user_data) {
    (void)user_data;

    AppData *data = g_malloc0(sizeof(AppData));
    data->window.app             = app;
    data->state.current_log_name = NULL;
#ifdef _WIN32
    data->state.current_log_handle = NULL;
#endif

    data->window.window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(data->window.window), "Event Viewer");
    gtk_window_set_default_size(GTK_WINDOW(data->window.window), 1200, 800);

    GtkWidget *mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(data->window.window), mainBox);

    gtk_box_append(GTK_BOX(mainBox), create_menu(data));
    gtk_box_append(GTK_BOX(mainBox), create_toolbar(data));

    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_shrink_start_child(GTK_PANED(paned), FALSE);
    gtk_paned_set_shrink_end_child  (GTK_PANED(paned), FALSE);
    gtk_paned_set_position          (GTK_PANED(paned), 250);
    gtk_box_append(GTK_BOX(mainBox), paned);

    gtk_paned_set_start_child(GTK_PANED(paned), create_sidebar(data));
    gtk_paned_set_end_child  (GTK_PANED(paned), create_content_area(data));

    fill_tree(data);
    fill_tables(data);

    gtk_window_present(GTK_WINDOW(data->window.window));

    g_object_set_data_full(G_OBJECT(data->window.window),
                           "event_viewer_data", data, g_free);
}

static GtkWidget *create_content_area(AppData *data) {
    data->window.notebook = gtk_notebook_new();
    gtk_widget_set_vexpand(data->window.notebook, TRUE);

    GtkWidget *overviewBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start  (overviewBox, 10);
    gtk_widget_set_margin_end    (overviewBox, 10);
    gtk_widget_set_margin_top    (overviewBox, 10);
    gtk_widget_set_margin_bottom (overviewBox, 10);

    GtkWidget *headerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_bottom(headerBox, 10);

    GtkWidget *titleLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titleLabel), "<b>Overview and Summary</b>");
    gtk_widget_set_halign(titleLabel, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(headerBox), titleLabel);

    char timeBuf[80];
    time_t rawTime  = time(NULL);
    struct tm *ti   = localtime(&rawTime);
    strftime(timeBuf, sizeof(timeBuf),
             "Last refreshed: %m/%d/%Y %I:%M:%S %p", ti);

    GtkWidget *refreshLabel = gtk_label_new(timeBuf);
    gtk_widget_set_halign  (refreshLabel, GTK_ALIGN_END);
    gtk_widget_set_hexpand (refreshLabel, TRUE);
    gtk_box_append(GTK_BOX(headerBox), refreshLabel);
    gtk_box_append(GTK_BOX(overviewBox), headerBox);

    GtkWidget *descBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_bottom(descBox, 20);

    GtkWidget *infoIcon = gtk_image_new_from_icon_name("dialog-information");
    gtk_box_append(GTK_BOX(descBox), infoIcon);

    GtkWidget *descLabel = gtk_label_new(
        "To view events that have occurred on your computer, select the "
        "appropriate source, log or custom view node in the console tree.");
    gtk_label_set_wrap(GTK_LABEL(descLabel), TRUE);
    gtk_widget_set_margin_start(descLabel, 10);
    gtk_box_append(GTK_BOX(descBox), descLabel);
    gtk_box_append(GTK_BOX(overviewBox), descBox);

    gtk_box_append(GTK_BOX(overviewBox), create_admin_events_table(data));
    gtk_box_append(GTK_BOX(overviewBox), create_recent_nodes_table(data));
    gtk_box_append(GTK_BOX(overviewBox), create_log_summary_table(data));

    gtk_notebook_append_page(GTK_NOTEBOOK(data->window.notebook),
                             overviewBox, gtk_label_new("Overview"));

    gtk_notebook_append_page(GTK_NOTEBOOK(data->window.notebook),
                             create_event_details_view(data),
                             gtk_label_new("Events"));

    return data->window.notebook;
}

static GtkWidget *create_admin_events_table(AppData *data) {
    GtkWidget *frame    = gtk_frame_new("Summary of Administrative Events");
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_margin_bottom(frame, 20);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 150);

    data->models.admin_store =
        gtk_list_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
    data->models.admin_tree_view =
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(data->models.admin_store));

    const char *cols[] = {
        "Event Type", "Event ID", "Source", "Log",
        "Last hour", "24 hours", "7 days"
    };
    for (int i = 0; i < 7; i++) {
        GtkCellRenderer   *renderer = gtk_cell_renderer_text_new();
        if (i >= 4) g_object_set(renderer, "xalign", 1.0, NULL);
        GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(
                                     cols[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_resizable(col, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(data->models.admin_tree_view), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  data->models.admin_tree_view);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled);
    return frame;
}

static GtkWidget *create_recent_nodes_table(AppData *data) {
    GtkWidget *frame    = gtk_frame_new("Recently Viewed Nodes");
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_margin_bottom(frame, 20);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 100);

    data->models.recent_store =
        gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING);
    data->models.recent_tree_view =
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(data->models.recent_store));

    const char *cols[] = {"Name", "Description", "Modified", "Created"};
    for (int i = 0; i < 4; i++) {
        GtkCellRenderer   *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(
                                     cols[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_resizable(col, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(data->models.recent_tree_view), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  data->models.recent_tree_view);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled);
    return frame;
}

static GtkWidget *create_log_summary_table(AppData *data) {
    GtkWidget *frame    = gtk_frame_new("Log Summary");
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 200);

    data->models.log_store =
        gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING);
    data->models.log_tree_view =
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(data->models.log_store));

    const char *cols[] = {
        "Log Name", "Size (Current/Maximum)", "Modified", "Enabled", "Retention Policy"
    };
    for (int i = 0; i < 5; i++) {
        GtkCellRenderer   *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(
                                     cols[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_resizable(col, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(data->models.log_tree_view), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  data->models.log_tree_view);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled);
    return frame;
}

static GtkWidget *create_event_details_view(AppData *data) {
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);

    data->models.event_details_store =
        gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);

    data->models.event_details_view =
        gtk_tree_view_new_with_model(
            GTK_TREE_MODEL(data->models.event_details_store));

    const char *cols[] = {
        "Level", "Date and Time", "Source", "Event ID", "Task Category"
    };
    for (int i = 0; i < 5; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        if (i == 3) g_object_set(renderer, "xalign", 1.0, NULL);

        GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(
                                     cols[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_resizable    (col, TRUE);
        gtk_tree_view_column_set_sort_column_id(col, i);
        gtk_tree_view_append_column(
            GTK_TREE_VIEW(data->models.event_details_view), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  data->models.event_details_view);
    return scrolled;
}
