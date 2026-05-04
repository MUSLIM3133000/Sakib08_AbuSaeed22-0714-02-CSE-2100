/**
 * @file view/windows/main_window.cpp
 * @brief Main window implementation (C++17)
 *
 * MVC Role: VIEW — builds the GTK window layout and delegates data
 * population to the EventController (Controller layer).
 */

#include "main_window.h"
#include "view/components/menu_bar.h"
#include "view/components/tool_bar.h"
#include "view/components/sidebar.h"
#include "controller/event_controller.h"
#include "model/statistics/event_statistics.h"
#include <ctime>
#include <cstdio>

namespace EventViewer {

// ── Forward declarations of private helpers ───────────────────────────────────
static GtkWidget* createContentArea      (EventViewerContext* ctx);
static GtkWidget* createAdminEventsTable (EventViewerContext* ctx);
static GtkWidget* createRecentNodesTable (EventViewerContext* ctx);
static GtkWidget* createLogSummaryTable  (EventViewerContext* ctx);
static GtkWidget* createEventDetailsView (EventViewerContext* ctx);

// ─────────────────────────────────────────────────────────────────────────────
// MainWindow::activate
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::activate(GtkApplication* app, gpointer userData) {
    auto* ctx = static_cast<EventViewerContext*>(userData);
    ctx->app  = app;

    ctx->window = gtk_application_window_new(app);
    gtk_window_set_title       (GTK_WINDOW(ctx->window), "Event Viewer");
    gtk_window_set_default_size(GTK_WINDOW(ctx->window), 1200, 800);

    GtkWidget* mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(ctx->window), mainBox);

    gtk_box_append(GTK_BOX(mainBox), MenuBar::create(ctx));
    gtk_box_append(GTK_BOX(mainBox), ToolBar::create(ctx));

    GtkWidget* paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_paned_set_shrink_start_child(GTK_PANED(paned), FALSE);
    gtk_paned_set_shrink_end_child  (GTK_PANED(paned), FALSE);
    gtk_paned_set_position          (GTK_PANED(paned), 250);
    gtk_box_append(GTK_BOX(mainBox), paned);

    gtk_paned_set_start_child(GTK_PANED(paned), Sidebar::create(ctx));
    gtk_paned_set_end_child  (GTK_PANED(paned), createContentArea(ctx));

    // Controller: populate sidebar tree
    EventController::populateTree(ctx);

    // Controller: fetch stats and populate tables
    EventStatisticsCalculator calc;
    auto stats1h  = ctx->logRepository
                    ? calc.calculate(L"Application", 1)   : EventStatistics{};
    auto stats24h = ctx->logRepository
                    ? calc.calculate(L"Application", 24)  : EventStatistics{};
    auto stats7d  = ctx->logRepository
                    ? calc.calculate(L"Application", 168) : EventStatistics{};

    EventController::populateTables(ctx, stats1h, stats24h, stats7d);

    gtk_window_present(GTK_WINDOW(ctx->window));

    // Attach context to window for retrieval in action handlers
    g_object_set_data(G_OBJECT(ctx->window), "event_viewer_context", ctx);
}

// ─────────────────────────────────────────────────────────────────────────────
// Private helpers — pure GTK widget creation (View responsibility)
// ─────────────────────────────────────────────────────────────────────────────
static GtkWidget* createContentArea(EventViewerContext* ctx) {
    ctx->notebook = gtk_notebook_new();
    gtk_widget_set_vexpand(ctx->notebook, TRUE);

    // ── Overview tab ──────────────────────────────────────────────────────
    GtkWidget* overviewBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start  (overviewBox, 10);
    gtk_widget_set_margin_end    (overviewBox, 10);
    gtk_widget_set_margin_top    (overviewBox, 10);
    gtk_widget_set_margin_bottom (overviewBox, 10);

    GtkWidget* headerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_bottom(headerBox, 10);

    GtkWidget* titleLabel = gtk_label_new(nullptr);
    gtk_label_set_markup(GTK_LABEL(titleLabel), "<b>Overview and Summary</b>");
    gtk_widget_set_halign(titleLabel, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(headerBox), titleLabel);

    char timeBuf[80];
    time_t rawTime = time(nullptr);
    struct tm* ti  = localtime(&rawTime);
    strftime(timeBuf, sizeof(timeBuf),
             "Last refreshed: %m/%d/%Y %I:%M:%S %p", ti);

    GtkWidget* refreshLabel = gtk_label_new(timeBuf);
    gtk_widget_set_halign (refreshLabel, GTK_ALIGN_END);
    gtk_widget_set_hexpand(refreshLabel, TRUE);
    gtk_box_append(GTK_BOX(headerBox), refreshLabel);
    gtk_box_append(GTK_BOX(overviewBox), headerBox);

    GtkWidget* descBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_bottom(descBox, 20);
    gtk_box_append(GTK_BOX(descBox),
                   gtk_image_new_from_icon_name("dialog-information"));

    GtkWidget* descLabel = gtk_label_new(
        "To view events that have occurred on your computer, select the "
        "appropriate source, log or custom view node in the console tree.");
    gtk_label_set_wrap(GTK_LABEL(descLabel), TRUE);
    gtk_widget_set_margin_start(descLabel, 10);
    gtk_box_append(GTK_BOX(descBox), descLabel);
    gtk_box_append(GTK_BOX(overviewBox), descBox);

    gtk_box_append(GTK_BOX(overviewBox), createAdminEventsTable(ctx));
    gtk_box_append(GTK_BOX(overviewBox), createRecentNodesTable(ctx));
    gtk_box_append(GTK_BOX(overviewBox), createLogSummaryTable(ctx));

    gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook),
                             overviewBox, gtk_label_new("Overview"));

    // ── Events tab ────────────────────────────────────────────────────────
    gtk_notebook_append_page(GTK_NOTEBOOK(ctx->notebook),
                             createEventDetailsView(ctx),
                             gtk_label_new("Events"));

    return ctx->notebook;
}

static GtkWidget* createAdminEventsTable(EventViewerContext* ctx) {
    GtkWidget* frame    = gtk_frame_new("Summary of Administrative Events");
    GtkWidget* scrolled = gtk_scrolled_window_new();
    gtk_widget_set_margin_bottom(frame, 20);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 150);

    ctx->adminStore =
        gtk_list_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
    ctx->adminTreeView =
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx->adminStore));

    const char* cols[] = {
        "Event Type", "Event ID", "Source", "Log",
        "Last hour",  "24 hours", "7 days"
    };
    for (int i = 0; i < 7; ++i) {
        GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
        if (i >= 4) g_object_set(renderer, "xalign", 1.0, nullptr);
        GtkTreeViewColumn* col = gtk_tree_view_column_new_with_attributes(
                                     cols[i], renderer, "text", i, nullptr);
        gtk_tree_view_column_set_resizable(col, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(ctx->adminTreeView), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  ctx->adminTreeView);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled);
    return frame;
}

static GtkWidget* createRecentNodesTable(EventViewerContext* ctx) {
    GtkWidget* frame    = gtk_frame_new("Recently Viewed Nodes");
    GtkWidget* scrolled = gtk_scrolled_window_new();
    gtk_widget_set_margin_bottom(frame, 20);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 100);

    ctx->recentStore =
        gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING);
    ctx->recentTreeView =
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx->recentStore));

    const char* cols[] = {"Name", "Description", "Modified", "Created"};
    for (int i = 0; i < 4; ++i) {
        GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn* col = gtk_tree_view_column_new_with_attributes(
                                     cols[i], renderer, "text", i, nullptr);
        gtk_tree_view_column_set_resizable(col, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(ctx->recentTreeView), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  ctx->recentTreeView);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled);
    return frame;
}

static GtkWidget* createLogSummaryTable(EventViewerContext* ctx) {
    GtkWidget* frame    = gtk_frame_new("Log Summary");
    GtkWidget* scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 200);

    ctx->logStore =
        gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_STRING);
    ctx->logTreeView =
        gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx->logStore));

    const char* cols[] = {
        "Log Name", "Size (Current/Maximum)",
        "Modified", "Enabled", "Retention Policy"
    };
    for (int i = 0; i < 5; ++i) {
        GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn* col = gtk_tree_view_column_new_with_attributes(
                                     cols[i], renderer, "text", i, nullptr);
        gtk_tree_view_column_set_resizable(col, TRUE);
        gtk_tree_view_append_column(GTK_TREE_VIEW(ctx->logTreeView), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  ctx->logTreeView);
    gtk_frame_set_child(GTK_FRAME(frame), scrolled);
    return frame;
}

static GtkWidget* createEventDetailsView(EventViewerContext* ctx) {
    GtkWidget* scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);

    ctx->eventDetailsStore =
        gtk_list_store_new(5, G_TYPE_STRING, G_TYPE_STRING,
                           G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);
    ctx->eventDetailsView =
        gtk_tree_view_new_with_model(
            GTK_TREE_MODEL(ctx->eventDetailsStore));

    const char* cols[] = {
        "Level", "Date and Time", "Source", "Event ID", "Task Category"
    };
    for (int i = 0; i < 5; ++i) {
        GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
        if (i == 3) g_object_set(renderer, "xalign", 1.0, nullptr);
        GtkTreeViewColumn* col = gtk_tree_view_column_new_with_attributes(
                                     cols[i], renderer, "text", i, nullptr);
        gtk_tree_view_column_set_resizable    (col, TRUE);
        gtk_tree_view_column_set_sort_column_id(col, i);
        gtk_tree_view_append_column(
            GTK_TREE_VIEW(ctx->eventDetailsView), col);
    }

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  ctx->eventDetailsView);
    return scrolled;
}

} // namespace EventViewer
