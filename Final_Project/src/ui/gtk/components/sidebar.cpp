/**
 * @file ui/gtk/components/sidebar.cpp
 * @brief Sidebar implementation (C++17)
 *
 * C improvement:
 *  - NULL → nullptr
 *  - ActionHandlers_OnTreeSelectionChanged → ActionHandlers::onTreeSelectionChanged
 */

#include "sidebar.h"
#include "ui/gtk/callbacks/action_handlers.h"

namespace EventViewer {

GtkWidget* Sidebar::create(EventViewerContext* ctx) {
    GtkWidget* scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled, 250, -1);

    // Two columns: name (string) + isCategory (bool for future styling)
    ctx->treeStore = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_BOOLEAN);
    ctx->treeView  = gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx->treeStore));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(ctx->treeView), FALSE);

    GtkCellRenderer*   renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* col      = gtk_tree_view_column_new_with_attributes(
                                      "Log", renderer, "text", 0, nullptr);
    gtk_tree_view_append_column(GTK_TREE_VIEW(ctx->treeView), col);

    // Wire selection change → ActionHandlers::onTreeSelectionChanged (DIP)
    GtkTreeSelection* sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(ctx->treeView));
    g_signal_connect(sel, "changed",
                     G_CALLBACK(ActionHandlers::onTreeSelectionChanged), ctx);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), ctx->treeView);
    return scrolled;
}

} // namespace EventViewer
