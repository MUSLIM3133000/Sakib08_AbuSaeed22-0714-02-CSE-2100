/**
 * @file ui/gtk/windows/main_window.c
 * @brief Main application window implementation
 *
 * Creates and displays the main window with all UI components.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#include "main_window.h"
#include "ui/gtk/components/menu_bar.h"
#include "ui/gtk/components/tool_bar.h"
#include "ui/gtk/components/sidebar.h"
#include "ui/models/event_models.h"
#include <time.h>
#include <stdio.h>

static GtkWidget *MainWindow_CreateContentArea(EventViewerContext *ctx);
static GtkWidget *MainWindow_CreateAdminEventsTable(EventViewerContext *ctx);
static GtkWidget *MainWindow_CreateRecentNodesTable(EventViewerContext *ctx);
static GtkWidget *MainWindow_CreateLogSummaryTable(EventViewerContext *ctx);
static GtkWidget *MainWindow_CreateEventDetailsView(EventViewerContext *ctx);

void MainWindow_Activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;

    EventViewerContext *ctx = g_malloc0(sizeof(EventViewerContext));
    ctx->app             = app;
    ctx->current_log_name = NULL;
#ifdef _WIN32
    ctx->current_log_handle = NULL;
#endif

    ctx->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(ctx->window), "Event Viewer");
    gtk_window_set_default_size(GTK_WINDOW(ctx->window), 1200, 800);

    GtkWidget *mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(ctx->window), mainBox);

    gtk_box_append(GTK_BOX(mainBox), MenuBar_Create(ctx));
    gtk_box_append(GTK_BOX(mainBox), ToolBar_Create(ctx));

    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_shrink_start_child(GTK_PANED(paned), FALSE);
    gtk_paned_set_shrink_end_child  (GTK_PANED(paned), FALSE);
    gtk_paned_set_position          (GTK_PANED(paned), 250);
    gtk_box_append(GTK_BOX(mainBox), paned);

    gtk_paned_set_start_child(GTK_PANED(paned), Sidebar_Create(ctx));
    gtk_paned_set_end_child  (GTK_PANED(paned), MainWindow_CreateContentArea(ctx));

    EventModels_PopulateTree(ctx);
    EventModels_PopulateTables(ctx);

    gtk_window_present(GTK_WINDOW(ctx->window));

    g_object_set_data_full(G_OBJECT(ctx->window),
                           "event_viewer_context", ctx, g_free);
}

static GtkWidget *MainWindow_CreateContentArea(EventViewerContext *ctx) {
    ctx->notebook = gtk_notebook_new();
    gtk_widget_set_vexpand(ctx->notebook, TRUE);

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

    gtk_box_append(GTK_BOX(overviewBox), MainWindow_CreateAdminEventsTable(ctx));
    gtk_box_append(GTK_BOX(overviewBox), MainWindow_CreateRecentNodesTable(ctx));
    gtk_box_append(GTK_BOX(overviewBox), MainWindow_CreateLogSummaryTable(ctx));

    gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook),
                             overviewBox, gtk_label_new("Overview"));

    gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook),
                             MainWindow_CreateEventDetailsView(ctx),
                             gtk_label_new("Events"));

    return ctx->notebook;
}

static GtkWidget *MainWindow_CreateAdminEventsTable(EventViewerContext *ctx) {
    GtkWidget *frame    = gtk_frame_new("Summary of Administrative Events");
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_margin_bottom(frame, 20);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 150);

    ctx->admin_store =
        gtk_list_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
    ctx->admin_tree_view =
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx->admin_store));

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
        gtk_tree_view_append_column(GTK_TREE_VIEW(ctx->admin_tree_view), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  ctx->admin_tree_view);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled);
    return frame;
}

static GtkWidget *MainWindow_CreateRecentNodesTable(EventViewerContext *ctx) {
    GtkWidget *frame    = gtk_frame_new("Recently Viewed Nodes");
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_margin_bottom(frame, 20);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 100);

    ctx->recent_store =
        gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING);
    ctx->recent_tree_view =
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx->recent_store));

    const char *cols[] = {"Name", "Description", "Modified", "Created"};
    for (int i = 0; i < 4; i++) {
        GtkCellRenderer   *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(
                                     cols[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_resizable(col, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(ctx->recent_tree_view), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  ctx->recent_tree_view);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled);
    return frame;
}

static GtkWidget *MainWindow_CreateLogSummaryTable(EventViewerContext *ctx) {
    GtkWidget *frame    = gtk_frame_new("Log Summary");
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 200);

    ctx->log_store =
        gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING);
    ctx->log_tree_view =
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx->log_store));

    const char *cols[] = {
        "Log Name", "Size (Current/Maximum)", "Modified", "Enabled", "Retention Policy"
    };
    for (int i = 0; i < 5; i++) {
        GtkCellRenderer   *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(
                                     cols[i], renderer, "text", i, NULL);
        gtk_tree_view_column_set_resizable(col, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(ctx->log_tree_view), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  ctx->log_tree_view);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled);
    return frame;
}

static GtkWidget *MainWindow_CreateEventDetailsView(EventViewerContext *ctx) {
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);

    ctx->event_details_store =
        gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);

    ctx->event_details_view =
        gtk_tree_view_new_with_model(
            GTK_TREE_MODEL(ctx->event_details_store));

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
            GTK_TREE_VIEW(ctx->event_details_view), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  ctx->event_details_view);
    return scrolled;
}
